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
  order_state_change_event_.Fire(std::move(old_state), std::move(state));
}

}  // namespace tcs