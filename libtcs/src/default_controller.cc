#include "tcs/vehicle/default_controller.h"

namespace tcs {

void DefaultController::SetDriveOrder(DriveOrder order) {
  if (current_drive_order_.has_value())
    throw std::runtime_error("vehicle " + std::to_string(vehicle_->get_id()) +
                             " already has an order");
  BOOST_LOG_TRIVIAL(debug) << "vehicle " << vehicle_->get_id()
                           << " setting driveorder";
  scheduler_->Claim(this, ExpandDriveOrder(order));

  current_drive_order_ = order;
  vehicle_->set_route_progress_index(0);
}

std::vector<std::unordered_set<MapResource*>>
DefaultController::ExpandDriveOrder(DriveOrder& order) {
  std::vector<std::unordered_set<MapResource*>> res;
  auto& steps = order.get_route()->get_steps();
  for (auto& step : steps) {
    res.push_back({step.destination, step.path});
  }
  return res;
}

void DefaultController::CreateFutureCommands(DriveOrder& order) {
  // TODO: command_vector_.clear() ?

  std::string operation = order.get_destination().operation;
  auto& steps = order.get_route()->get_steps();
  auto sz = steps.size();
  for (auto index = 0; index != sz; ++index) {
    bool is_last_movement = index + 1 == sz;
    command_vector_.push_back({steps[index],
                               is_last_movement ? operation : kNoOperation,
                               is_last_movement});
    
  }
}

}  // namespace tcs