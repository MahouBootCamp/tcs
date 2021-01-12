#ifndef TRANSPORT_ORDER_SERVICE_H
#define TRANSPORT_ORDER_SERVICE_H

#include "tcs/order/transport_order.h"
#include "tcs/util/event.h"
#include "tcs/util/order_pool.h"

namespace tcs {

class TransportOrderService {
 public:
  TransportOrderService(OrderPool* order_pool) : order_pool_{order_pool} {}

  Event<TransportOrderState, TransportOrderState>& OrderStateChangeEvent() {
    return order_state_change_event_;
  }

  template <class Predicate>
  std::unordered_set<TransportOrder*> FilterBy(Predicate p) {
    std::unordered_set<TransportOrder*> result;
    auto orders = order_pool_->GetAllOrders();
    for (auto& order : orders) {
      if (p(order)) result.insert(order);
    }
    return result;
  }

  std::unordered_set<TransportOrder*> FilterOrdersByState(
      TransportOrderState state) {
    return FilterBy(
        [&state](TransportOrder* order) { return order->GetState() == state; });
  }

  TransportOrder* GetTransportOrder(TransportOrderID id) {
    return order_pool_->GetOrder(id);
  }

  std::unordered_set<TransportOrder*> GetTransportOrdersByID(
      std::unordered_set<TransportOrderID>& id_set) {
    return order_pool_->GetOrdersByID(id_set);
  }

  bool HasUnfinishedDependencies(TransportOrderID order_id);

  void UpdateOrderState(TransportOrderID order_id, TransportOrderState state);

  // void UpdateOrderWithDependencyFinished(TransportOrderID finished_id);

  void UpdateOrderNextDriveOrder(TransportOrderID id);

  // Since the vehicle and driveorder would be decided at the same time, they
  // are set in one function
  void UpdateOrderVehicleAndDriveOrder(
      TransportOrderID id, MapObjectRef vehicle_ref,
      std::optional<std::vector<DriveOrder>> drive_orders);

 private:
  Event<TransportOrderState, TransportOrderState> order_state_change_event_;
  OrderPool* order_pool_;
};

}  // namespace tcs

#endif /* TRANSPORT_ORDER_SERVICE_H */
