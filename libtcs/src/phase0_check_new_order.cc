#include "tcs/dispatcher/phase0_check_new_order.h"

namespace tcs {

void Phase0CheckNewOrder::Run() {
  BOOST_LOG_TRIVIAL(info) << "Phase 0: Check New Order...";
  auto orders =
      transport_order_service_->FilterOrdersByState(TransportOrderState::kRaw);
  for (auto& order : orders) {
    // Check routability
    if (!router_->ChechRoutability(order)) {
      transport_order_service_->UpdateOrderState(
          order->GetID(), TransportOrderState::kUnRoutable);
      continue;
    }

    // Ready for dispatch
    transport_order_service_->UpdateOrderState(order->GetID(),
                                               TransportOrderState::kActive);

    // Check Dependencies. Dispatch if dependencies finished.
    if (transport_order_service_->HasUnfinishedDependencies(order->GetID()))
      transport_order_service_->UpdateOrderState(
          order->GetID(), TransportOrderState::kDispatchable);
  }
}

}  // namespace tcs