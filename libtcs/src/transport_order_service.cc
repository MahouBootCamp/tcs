#include "tcs/service/transport_order_service.h"

namespace tcs {

bool TransportOrderService::HasUnfinishedDependencies(
    TransportOrderID order_id) {
  auto order = GetTransportOrder(order_id);
  auto& dependencies = order->GetDependencies();
  for (auto& dependency_id : dependencies) {
    auto dependency = GetTransportOrder(dependency_id);
    if (!IsFinalState(dependency->GetState())) return false;
  }
  return true;
}

void TransportOrderService::UpdateOrderState(TransportOrderID order_id,
                                             TransportOrderState state) {
  auto order = GetTransportOrder(order_id);
  auto old_state = order->GetState();
  order->SetState(state);
  order_state_change_event_.Fire(old_state, state);
}

void TransportOrderService::UpdateOrderNextDriveOrder(TransportOrderID id) {
  auto order = GetTransportOrder(id);
  order->SetProgressIndex(order->GetProgressIndex() + 1);
}

void TransportOrderService::UpdateOrderVehicleAndDriveOrder(
    TransportOrderID id, MapObjectRef vehicle_ref,
    std::optional<std::vector<DriveOrder> > drive_orders) {
  auto order = GetTransportOrder(id);
  order->SetVehicle(vehicle_ref);
  if (drive_orders.has_value()) {
    order->SetDriveOrders(std::move(drive_orders.value()));
  } else  // Actually would not happen
    order->SetDriveOrders({});
}

}  // namespace tcs