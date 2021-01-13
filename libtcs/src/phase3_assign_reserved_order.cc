#include "tcs/dispatcher/phase3_assign_reserved_order.h"

namespace tcs {

void Phase3AssignReservedOrder::Run() {
  BOOST_LOG_TRIVIAL(info) << "Phase 3: Assign Reserved Transport Order...";
  auto vehicles = vehicle_service_->FilterBy([](Vehicle* v) {
    return v->GetProcessState() == ProcessState::kIdle &&
           (v->GetVehicleState() == VehicleState::kIdle ||
            v->GetVehicleState() == VehicleState::kCharging);
  });
  for (auto& vehicle : vehicles) CheckForReservedOrder(vehicle);
}

void Phase3AssignReservedOrder::CheckForReservedOrder(Vehicle* vehicle) {
  auto reserved_orders =
      reserve_order_pool_->GetReservationsByVehicle(vehicle->GetID());
  TransportOrder* selected_order = nullptr;
  for (auto& order_id : reserved_orders) {
    auto order = transport_order_service_->GetTransportOrder(order_id);
    if (order->GetState() == TransportOrderState::kDispatchable) {
      selected_order = order;
      break;
    }
  }

  if (!selected_order) return;

  reserve_order_pool_->RemoveReservationByOrder(selected_order->GetID());
  auto current_point =
      map_service_->GetPoint(vehicle->GetCurrentPoint().value());
  auto drive_order = router_->GetRoute(current_point, selected_order);
  if (!drive_order.has_value())
    // REVIEW: Next point of vehicle may be unroutable?
    throw std::runtime_error("Reserved order unroutable!");
  // AssignmentCandidate candidate {vehicle, selected_order,
  // std::move(drive_order)};
  universal_dispatch_util_->AssignOrder(vehicle, selected_order,
                                        std::move(drive_order));
}

}  // namespace tcs