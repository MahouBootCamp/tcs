#include "tcs/dispatcher/phase5_recharge_vehicle.h"

namespace tcs {

void Phase5RechargeVehicle::Run() {
  return;

  auto vehicles = vehicle_service_->FilterBy([](Vehicle* v) {
    return v->get_integration_level() == IntegrationLevel::kUtilized &&
           v->get_process_state() == ProcessState::kIdle &&
           v->get_vehicle_state() == VehicleState::kIdle &&
           v->get_current_point().has_value() && v->get_need_charge();
  });

  for (auto& vehicle : vehicles) {
    CreateRechargeOrder(vehicle);
  }
}

void Phase5RechargeVehicle::CreateRechargeOrder(Vehicle* vehicle) {
  auto points = map_service_->GetAllPoints();
  std::unordered_set<Point*> charge_points;
  for (auto& point : points)
    if (point->get_type() == PointType::kCharge) charge_points.insert(point);

  if (charge_points.empty()) {
    BOOST_LOG_TRIVIAL(info) << "No charge points";
    return;
  }


}

}  // namespace tcs