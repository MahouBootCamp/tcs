#include "tcs/scheduler/default_scheduler.h"

namespace tcs {

void DefaultScheduler::Claim(
    IVehicleController *vehicle,
    std::vector<std::unordered_set<const MapResource *>> resource_sequence) {
  std::scoped_lock<std::mutex> lock{scheduler_mut_};
  claims_by_vehicle_.insert(std::make_pair(vehicle, resource_sequence));
}

void DefaultScheduler::Unclaim(IVehicleController *vehicle) {
  std::scoped_lock<std::mutex> lock{scheduler_mut_};
  claims_by_vehicle_.erase(vehicle);
}

void DefaultScheduler::Allocate(
    IVehicleController *vehicle,
    std::unordered_set<const MapResource *> resources) {
  executor_->Submit(&DefaultScheduler::AllocateTask, this, vehicle,
                    std::move(resources));
}

void DefaultScheduler::AllocateNow(
    IVehicleController *vehicle,
    std::unordered_set<const MapResource *> resources) {
  std::scoped_lock<std::mutex> lock{scheduler_mut_};
  if (reservation_pool_.ResourcesAvailable(vehicle, resources)) {
    for (auto &resource : resources) {
      auto &entry = reservation_pool_.GetReservationEntry(resource);
      entry.vehicle = vehicle;
      entry.count += 1;
    }
  } else
    throw std::runtime_error("Resources unavailable!");
}

void DefaultScheduler::Free(IVehicleController *vehicle,
                            std::unordered_set<const MapResource *> resources) {
  // Free resources
  {
    std::scoped_lock<std::mutex> lock{scheduler_mut_};
    reservation_pool_.Free(vehicle, resources);
  }

  // Retry allocation
  RetryTask();
}

void DefaultScheduler::FreeAll(IVehicleController *vehicle) {
  // Free resources
  {
    std::scoped_lock<std::mutex> lock{scheduler_mut_};
    reservation_pool_.FreeAll(vehicle);
  }

  // Retry allocation
  RetryTask();
}

void DefaultScheduler::AllocateTask(
    IVehicleController *vehicle,
    std::unordered_set<const MapResource *> resources) {
  if (TryAllocate(vehicle, resources)) {
    CheckTask(vehicle, std::move(resources));
  } else {
    BOOST_LOG_TRIVIAL(debug)
        << "Resources unavailable for vehicle " << vehicle->GetVehicleID()
        << ". Deferring allocation...";
    {
      std::scoped_lock<std::mutex> lock{scheduler_mut_};
      deferred_allocations_.push_back(
          std::make_pair(vehicle, std::move(resources)));
    }
  }
}

void DefaultScheduler::CheckTask(
    IVehicleController *vehicle,
    std::unordered_set<const MapResource *> resources) {
  if (!vehicle->AllocationSuccessful(resources)) {
    BOOST_LOG_TRIVIAL(warning)
        << "Resources are refused by vehicle " << vehicle->GetVehicleID()
        << ". Freeing them...";
    {
      std::scoped_lock<std::mutex> lock{scheduler_mut_};
      reservation_pool_.Free(vehicle, resources);
    }

    RetryTask();
  }
}

void DefaultScheduler::RetryTask() {
  // Take cached allocations out & reset cache
  BOOST_LOG_TRIVIAL(debug) << "Retry deferred allocations";
  std::scoped_lock<std::mutex> lock{scheduler_mut_};

  auto deferred_allocations_move = std::move(deferred_allocations_);
  deferred_allocations_ = {};

  for (auto &pair : deferred_allocations_move) {
    executor_->Submit(&DefaultScheduler::AllocateTask, this, pair.first,
                      std::move(pair.second));
  }
}

bool DefaultScheduler::TryAllocate(
    IVehicleController *vehicle,
    std::unordered_set<const MapResource *> &resources) {
  BOOST_LOG_TRIVIAL(debug) << "Try allocating for vehicle "
                           << vehicle->GetVehicleID();
  auto expanded_resources = ExpandBlocks(resources);

  std::scoped_lock<std::mutex> lock{scheduler_mut_};
  if (reservation_pool_.ResourcesAvailable(vehicle, expanded_resources)) {
    // Allocate
    BOOST_LOG_TRIVIAL(debug) << "Resources available for vehicle "
                             << vehicle->GetVehicleID() << ". Allocating...";
    for (auto &resource : resources) {
      auto &entry = reservation_pool_.GetReservationEntry(resource);
      // Runtime check
      if (entry.vehicle != nullptr && entry.vehicle != vehicle)
        throw std::runtime_error(
            "Allocating a resource allocated by another vehicle!");
      entry.vehicle = vehicle;
      entry.count += 1;
    }
    return true;
  } else
    return false;
}

std::unordered_set<const MapResource *> DefaultScheduler::ExpandBlocks(
    const std::unordered_set<const MapResource *> &resources) {
  std::unordered_set<const MapResource *> expanded_resources{
      resources};  // Copy original set
  for (auto &resource : expanded_resources) {
    auto &blocks = resource->GetBlocks();
    for (auto &block : blocks) {
      auto block_resources = map_service_->GetResourcesByID(
          map_service_->GetBlock(block)->GetResources());
      expanded_resources.insert(block_resources.begin(), block_resources.end());
    }
  }
  return expanded_resources;
}

}  // namespace tcs