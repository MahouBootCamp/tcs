#include "tcs/dispatcher/phase5_charge_vehicle.h"

namespace tcs {

void Phase5ChargeVehicle::Run() {
  // UNDONE!
  return;

  BOOST_LOG_TRIVIAL(info) << "Phase 5: Charge low power vehicles...";

  auto vehicles = vehicle_service_->FilterBy([](Vehicle* v) {
    return v->GetIntegrationLevel() == IntegrationLevel::kUtilized &&
           v->GetProcessState() == ProcessState::kIdle &&
           v->GetVehicleState() == VehicleState::kIdle &&
           v->GetCurrentPoint().has_value() && v->GetNeedCharge();
  });

  if (vehicles.empty()) {
    BOOST_LOG_TRIVIAL(info) << "No vehicle needs charging, skipping...";
    return;
  }

  auto charge_location = map_service_->GetChargeLocation();
  for (auto& vehicle : vehicles) {
    // Check if there are available charge points on each loop
    if (charge_location->GetLinkedPoints().empty()) {
      BOOST_LOG_TRIVIAL(info) << "No available charge points, skipping...";
      return;
    }
    CreateChargeOrder(vehicle);
  }
}

void Phase5ChargeVehicle::CreateChargeOrder(Vehicle* vehicle) {
  auto charge_location = map_service_->GetChargeLocation();

  auto order_id =
      order_pool_->AddOrder({{charge_location->GetID(), kChargeOperation}});
  auto order = order_pool_->GetOrder(order_id);
  auto drive_order = router_->GetRoute(
      map_service_->GetPoint(vehicle->GetCurrentPoint().value()), order);
  if (drive_order.has_value()) {
    charge_location->ReservePoint(drive_order->back()
                                      .GetRoute()
                                      ->GetSteps()
                                      .back()
                                      .destination->GetID());
    universal_dispatch_util_->AssignOrder(vehicle, order,
                                          std::move(drive_order));
  }
}

}  // namespace tcs