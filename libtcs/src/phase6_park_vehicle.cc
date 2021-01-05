#include "tcs/dispatcher/phase6_park_vehicle.h"

namespace tcs {

void Phase6ParkVehicle::Run() {
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

  auto park_location = map_service_->GetParkLocation();
  for (auto& vehicle : vehicles) {
    // Check if there are available charge points on each loop
    if (park_location->GetLinkedPoints().empty()) {
      BOOST_LOG_TRIVIAL(info) << "No available charge points, skipping...";
      return;
    }
    CreateParkOrder(vehicle);
  }
}

void Phase6ParkVehicle::CreateParkOrder(Vehicle* vehicle) {
  auto park_location = map_service_->GetParkLocation();

  auto order_id =
      order_pool_->AddOrder({{park_location->GetID(), kParkOperation}});
  auto order = order_pool_->GetOrder(order_id);
  auto drive_order = router_->GetRoute(
      map_service_->GetPoint(vehicle->GetCurrentPoint().value()), order);
  if (drive_order.has_value()) {
    park_location->ReservePoint(drive_order->back()
                                      .GetRoute()
                                      ->GetSteps()
                                      .back()
                                      .destination->GetID());
    universal_dispatch_util_->AssignOrder(vehicle, order,
                                          std::move(drive_order));
  }
}


}