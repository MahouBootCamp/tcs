#include "tcs/scheduler/reservation_pool.h"

namespace tcs {

ReservationEntry &ReservationPool::GetReservationEntry(MapResource *resource) {
  auto itr = reservation_pool_.find(resource);
  if (itr == reservation_pool_.end()) {
    // Create an empty entry for resource
    ReservationEntry entry{resource};
    auto insert_res = reservation_pool_.insert(std::make_pair(resource, entry));
    return insert_res.first->second;
  } else {
    return itr->second;
  }
}

std::unordered_set<MapResource *> ReservationPool::GetAllocatedResources(
    IVehicleController *vehicle) {
  std::unordered_set<MapResource *> res;
  for (auto &entry : reservation_pool_) {
    if (entry.second.vehicle == vehicle) res.insert(entry.first);
  }
  return res;
}

bool ReservationPool::ResourcesAvailable(
    IVehicleController *vehicle, std::unordered_set<MapResource *> &resources) {
  for (auto &resource : resources) {
    auto &entry = GetReservationEntry(resource);
    if (entry.count != 0 && entry.vehicle != vehicle) return false;
  }
  return true;
}

void ReservationPool::Free(IVehicleController *vehicle,
                           std::unordered_set<MapResource *> &resources) {
  auto freeable_resources = GetFreeableResources(vehicle, resources);
  for (auto &resource : freeable_resources) {
    auto &entry = GetReservationEntry(resource);
    if (entry.count < 0)
      throw std::runtime_error("Freeing resource has count less than 1");
    entry.count -= 1;
    if (entry.count == 0) entry.vehicle = nullptr;
  }
}

void ReservationPool::FreeAll(IVehicleController *vehicle) {
  auto resources = GetAllocatedResources(vehicle);
  for (auto &resource : resources) {
    auto &entry = GetReservationEntry(resource);
    entry.count = 0;
    entry.vehicle = nullptr;
  }
}

std::unordered_set<MapResource *>
ReservationPool::FilterCompletelyFreeResources(
    std::unordered_set<MapResource *> &resources) {
  std::unordered_set<MapResource *> res;
  for (auto &resource : resources) {
    auto &entry = GetReservationEntry(resource);
    if (entry.vehicle == nullptr && entry.count == 0) res.insert(resource);
  }
  return res;
}

std::unordered_set<MapResource *> ReservationPool::GetFreeableResources(
    IVehicleController *vehicle, std::unordered_set<MapResource *> &resources) {
  std::unordered_set<MapResource *> res;
  for (auto &resource : resources) {
    auto &entry = GetReservationEntry(resource);
    if (entry.vehicle == vehicle)
      res.insert(resource);
    else
      BOOST_LOG_TRIVIAL(warning)
          << "Freeing resource " << resource->GetID()
          << " which is not reserved for vehicle " << entry.vehicle->GetVehicleID();
  }
  return res;
}

}  // namespace tcs