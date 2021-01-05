#include "tcs/dispatcher/phase5_charge_vehicle.h"

namespace tcs {

void Phase5ChargeVehicle::Run() {
  // UNDONE!
  return;

  BOOST_LOG_TRIVIAL(info) << "Phase 5: Charge low power vehicles...";

  auto vehicles = vehicle_service_->FilterBy([](Vehicle* v) {
    return v->get_integration_level() == IntegrationLevel::kUtilized &&
           v->get_process_state() == ProcessState::kIdle &&
           v->get_vehicle_state() == VehicleState::kIdle &&
           v->get_current_point().has_value() && v->get_need_charge();
  });

  if (vehicles.empty()) {
    BOOST_LOG_TRIVIAL(info) << "No vehicle needs charging, skipping...";
    return;
  }

  auto charge_location = map_service_->GetChargeLocation();
  for (auto& vehicle : vehicles) {
    // Check if there are available charge points on each loop
    if (charge_location->get_linked_points().empty()) {
      BOOST_LOG_TRIVIAL(info) << "No available charge points, skipping...";
      return;
    }
    CreateChargeOrder(vehicle);
  }
}

void Phase5ChargeVehicle::CreateChargeOrder(Vehicle* vehicle) {
  auto charge_location = map_service_->GetChargeLocation();

  auto order_id =
      order_pool_->AddOrder({{charge_location->get_id(), kChargeOperation}});
  auto order = order_pool_->get_order(order_id);
  auto drive_order = router_->GetRoute(
      map_service_->GetPoint(vehicle->get_current_point().value()), order);
  if (drive_order.has_value()) {
    charge_location->ReservePoint(drive_order->back()
                                      .get_route()
                                      ->get_steps()
                                      .back()
                                      .destination->get_id());
    universal_dispatch_util_->AssignOrder(vehicle, order,
                                          std::move(drive_order));
  }
}

}  // namespace tcs