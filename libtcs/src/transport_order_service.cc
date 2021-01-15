#include "tcs/service/transport_order_service.h"

namespace tcs {

bool TransportOrderService::HasUnfinishedDependencies(
    TransportOrderID order_id) {
  auto order = GetTransportOrder(order_id);
  auto& dependencies = order->GetDependencies();
  for (auto& dependency_id : dependencies) {
    if (!IsFinalState(ReadOrderState(dependency_id))) return false;
  }
  return true;
}

TransportOrderState TransportOrderService::ReadOrderState(
    TransportOrderID order_id) const {
  return order_pool_->GetOrder(order_id)->GetState();
}

void TransportOrderService::UpdateOrderState(TransportOrderID order_id,
                                             TransportOrderState state) {
  auto order = order_pool_->GetOrder(order_id);
  auto old_state = order->GetState();
  order->SetState(state);
  order_state_change_event_.Fire(order, old_state, state);
}

std::size_t TransportOrderService::ReadOrderProgressIndex(
    TransportOrderID order_id) const {
  return order_pool_->GetOrder(order_id)->GetProgressIndex();
}

std::vector<DriveOrder> TransportOrderService::ReadOrderFutureDriveOrders(
    TransportOrderID order_id) const {
  return order_pool_->GetOrder(order_id)->GetFutureDriveOrders();
}

void TransportOrderService::UpdateOrderNextDriveOrder(
    TransportOrderID order_id) {
  auto order = order_pool_->GetOrder(order_id);
  order->SetProgressIndex(order->GetProgressIndex() + 1);
}

std::vector<DriveOrder> TransportOrderService::ReadOrderDriveOrders(
    TransportOrderID order_id) const {
  return order_pool_->GetOrder(order_id)->GetDriveOrders();
}

MapObjectRef TransportOrderService::ReadOrderVehicle(
    TransportOrderID order_id) const {
  return order_pool_->GetOrder(order_id)->GetVehicle();
}

void TransportOrderService::UpdateOrderVehicleAndDriveOrder(
    TransportOrderID order_id, MapObjectRef vehicle_ref,
    std::optional<std::vector<DriveOrder> > drive_orders) {
  auto order = order_pool_->GetOrder(order_id);
  order->SetVehicle(vehicle_ref);
  if (drive_orders.has_value()) {
    order->SetDriveOrders(std::move(drive_orders.value()));
  } else  // Actually would not happen
    order->SetDriveOrders({});
}

}  // namespace tcs