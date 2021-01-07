#include "tcs/vehicle/default_controller.h"

namespace tcs {

DefaultController::DefaultController(Vehicle* vehicle, IVehicleAdapter* adapter,
                                     IScheduler* scheduler,
                                     VehicleService* vehicle_service)
    : vehicle_{vehicle},
      adapter_{adapter},
      scheduler_{scheduler},
      vehicle_service_{vehicle_service} {
  adapter_->FinishCommandEvent().Subscribe(
      std::bind(&DefaultController::FinishCommandEventHandler, this,
                std::placeholders::_1));
  adapter_->FailCommandEvent().Subscribe(
      std::bind(&DefaultController::FailCommandEventHandler, this,
                std::placeholders::_1));
  adapter_->UpdatePositionEvent().Subscribe(
      std::bind(&DefaultController::UpdatePositionEventHandler, this,
                std::placeholders::_1));
  adapter_->RequestChargeEvent().Subscribe(
      std::bind(&DefaultController::RequestChargeEventHandler, this));
  adapter_->UpdateVehicleStateEvent().Subscribe(
      std::bind(&DefaultController::UpdateVehicleStateEventHandler, this,
                std::placeholders::_1));

  adapter_->Enable();
}

void DefaultController::UpdatePositionEventHandler(MapObjectRef point_ref) {
  vehicle_service_->UpdateVehicleCurrentPosition(vehicle_->GetID(), point_ref);
}

void DefaultController::FinishCommandEventHandler(MovementCommand cmd) {
  auto cmd_sent = std::move(command_sent_.front());
  command_sent_.pop_front();
  if (cmd_sent != cmd) {
    throw std::runtime_error("Adapter executed unknown command");
  }

  auto vehicle_id = vehicle_->GetID();
  if (cmd.operation == kChargeOperation) {
    vehicle_service_->UpdateVehicleNeedCharge(vehicle_id, false);
    vehicle_service_->UpdateVehicleFinishCharge(vehicle_id, true);
  }

  vehicle_service_->UpdateVehicleRouteProgressIndex(
      vehicle_id, vehicle_->GetRouteProgressIndex() + 1);
  auto resources = std::move(allocated_resources_.front());
  allocated_resources_.pop_front();
  scheduler_->Free(this, resources);
  // Check if current drive order is finished：
  // No command unsent and vehicle has finished final step.
  if (HasFinishedOrAbortedDriveOrder()) {
    vehicle_service_->UpdateVehicleRouteProgressIndex(vehicle_id, 0);
    vehicle_service_->UpdateVehicleProcessState(
        vehicle_id,
        ProcessState::kAwaitingOrder);  // The dispatcher would notice this
                                        // state change in next phase
    current_drive_order_.reset();
  } else if (CanSendNextCommand()) {
    AllocateForNextCommand();
  }
}

void DefaultController::FailCommandEventHandler(MovementCommand cmd) {
  // UNDONE: Not implemented
}

void DefaultController::RequestChargeEventHandler() {
  vehicle_service_->UpdateVehicleNeedCharge(vehicle_->GetID(), true);
}

void DefaultController::UpdateVehicleStateEventHandler(VehicleState state) {
  vehicle_service_->UpdateVehicleState(vehicle_->GetID(), state);
}

bool DefaultController::AllocationSuccessful(
    std::unordered_set<MapResource*> resources) {
  if (pending_resources_ != resources) return false;

  if (!pending_command_.has_value()) {
    waiting_for_allocation_ = false;
    pending_resources_.clear();
    return false;
  }

  auto cmd = std::move(pending_command_.value());
  pending_command_.reset();
  pending_resources_.clear();
  allocated_resources_.push_back(resources);
  command_sent_.push_back(cmd);
  adapter_->EnqueueCommand(std::move(cmd));
  waiting_for_allocation_ = false;
  if (CanSendNextCommand()) AllocateForNextCommand();
  return true;
}

void DefaultController::AllocationFailed() {
  throw std::runtime_error("Allocation fails");
}

void DefaultController::SetDriveOrder(DriveOrder order) {
  if (current_drive_order_.has_value())
    throw std::runtime_error("vehicle " + std::to_string(vehicle_->GetID()) +
                             " already has an order");
  BOOST_LOG_TRIVIAL(debug) << "vehicle " << vehicle_->GetID()
                           << " setting driveorder";
  scheduler_->Claim(this, ExpandDriveOrder(order));
  current_drive_order_ = order;
  vehicle_service_->UpdateVehicleRouteProgressIndex(vehicle_->GetID(), 0);
  CreateFutureCommands(order);
  if (CanSendNextCommand()) {
    AllocateForNextCommand();
  }
}

void DefaultController::AbortDriveOrder(bool immediately) {
  if (immediately) {
    current_drive_order_.reset();
    waiting_for_allocation_ = false;
    pending_resources_.clear();
    vehicle_service_->UpdateVehicleRouteProgressIndex(vehicle_->GetID(), 0);

    adapter_->ClearCommandQueue();
    command_queue_.clear();
    pending_command_.reset();

    // Free all resources except current position
    auto current_resources = allocated_resources_.front();
    allocated_resources_.pop_front();
    for (auto& resources : allocated_resources_) {
      scheduler_->Free(this, resources);
    }
    allocated_resources_.clear();
    allocated_resources_.push_back(current_resources);

  } else {
    if (current_drive_order_.has_value()) command_queue_.clear();
  }
}

void DefaultController::InitPosition(MapResource* point) {
  scheduler_->AllocateNow(this, {point});
  vehicle_service_->UpdateVehicleCurrentPosition(vehicle_->GetID(),
                                                 point->GetID());
  // TODO: Set controller private status of this allocation
  // UNDONE: MARK WHERE I STOPPED...
  allocated_resources_.push_back({point});
}

std::vector<std::unordered_set<MapResource*>>
DefaultController::ExpandDriveOrder(DriveOrder& order) {
  std::vector<std::unordered_set<MapResource*>> res;
  auto& steps = order.GetRoute()->GetSteps();
  for (auto& step : steps) {
    res.push_back({step.destination, step.path});
  }
  return res;
}

void DefaultController::CreateFutureCommands(DriveOrder& order) {
  // REVIEW: command_queue_.clear() ?

  std::string operation = order.GetDestination().operation;
  auto& steps = order.GetRoute()->GetSteps();
  auto sz = steps.size();
  for (auto index = 0; index != sz; ++index) {
    bool is_last_movement = index + 1 == sz;
    command_queue_.push_back({steps[index],
                              is_last_movement ? operation : kNoOperation,
                              is_last_movement});
  }
}

void DefaultController::AllocateForNextCommand() {
  if (pending_command_.has_value())
    throw std::runtime_error("Already has a pending command");
  auto cmd = command_queue_.front();
  command_queue_.pop_front();
  pending_resources_ = ExpandMovementCommand(cmd);
  scheduler_->Allocate(this, pending_resources_);
  waiting_for_allocation_ = true;
  pending_command_ = cmd;
}

std::unordered_set<MapResource*> DefaultController::ExpandMovementCommand(
    MovementCommand& cmd) {
  return {cmd.step.destination, cmd.step.path};
}

}  // namespace tcs