#ifndef TRANSPORT_ORDER_SERVICE_H
#define TRANSPORT_ORDER_SERVICE_H

#include "tcs/order/transport_order.h"
#include "tcs/util/event.h"
#include "tcs/util/order_pool.h"

namespace tcs {

class TransportOrderService {
 public:
  TransportOrderService(OrderPool* order_pool) : order_pool_{order_pool} {}

  Event<const TransportOrder*, TransportOrderState, TransportOrderState>&
  OrderStateChangeEvent() {
    return order_state_change_event_;
  }

  template <class Predicate>
  std::unordered_set<const TransportOrder*> FilterBy(Predicate p) const {
    std::unordered_set<const TransportOrder*> result;
    auto orders = order_pool_->GetAllOrders();
    for (auto& order : orders) {
      if (p(order)) result.insert(order);
    }
    return result;
  }

  std::unordered_set<const TransportOrder*> FilterOrdersByState(
      TransportOrderState state) {
    return FilterBy(
        [&state](TransportOrder* order) { return order->GetState() == state; });
  }

  const TransportOrder* GetTransportOrder(TransportOrderID id) const {
    return order_pool_->GetOrder(id);
  }

  std::unordered_set<const TransportOrder*> GetTransportOrdersByID(
      std::unordered_set<TransportOrderID>& id_set) const {
    return order_pool_->GetOrdersByID(id_set);
  }

  bool HasUnfinishedDependencies(TransportOrderID order_id);

  TransportOrderState ReadOrderState(TransportOrderID order_id) const;
  void UpdateOrderState(TransportOrderID order_id, TransportOrderState state);

  std::size_t ReadOrderProgressIndex(TransportOrderID order_id) const;
  std::vector<DriveOrder> ReadOrderFutureDriveOrders(
      TransportOrderID order_id) const;
  void UpdateOrderNextDriveOrder(TransportOrderID order_id);

  std::vector<DriveOrder> ReadOrderDriveOrders(
      TransportOrderID order_id) const;
  MapObjectRef ReadOrderVehicle(TransportOrderID order_id) const;
  // Since the vehicle and driveorder would be decided at the same time, they
  // are set in one function
  void UpdateOrderVehicleAndDriveOrder(
      TransportOrderID order_id, MapObjectRef vehicle_ref,
      std::optional<std::vector<DriveOrder>> drive_orders);

 private:
  Event<const TransportOrder*, TransportOrderState, TransportOrderState>
      order_state_change_event_;
  OrderPool* order_pool_;
};

}  // namespace tcs

#endif /* TRANSPORT_ORDER_SERVICE_H */
