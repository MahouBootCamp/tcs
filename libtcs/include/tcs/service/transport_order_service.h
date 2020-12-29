#ifndef TRANSPORT_ORDER_SERVICE_H
#define TRANSPORT_ORDER_SERVICE_H

#include "tcs/order/transport_order.h"
#include "tcs/util/event.h"

namespace tcs {

class TransportOrderService {
 public:
  Event<TransportOrderState, TransportOrderState>& OrderStateChangeEvent() {
    return order_state_change_event_;
  }

  void UpdateOrderState(TransportOrderID order_id, TransportOrderState state);
  bool HasUnfinishedDependencies(TransportOrderID order_id);
  std::unordered_set<TransportOrder*> FilterOrdersByState(
      TransportOrderState state);
  TransportOrder* GetTransportOrder(TransportOrderID id);
  void SetTransportOrderNextDriveOrder(TransportOrderID id);
  void UpdateOrderWithDependencyFinished(TransportOrderID finished_id);
  void SetTransportOrderVehicleAndDriveOrder(
      TransportOrderID id, MapObjectRef vehicle_ref,
      std::optional<std::vector<DriveOrder>> drive_orders);

 private:
  Event<TransportOrderState, TransportOrderState> order_state_change_event_;
};

}  // namespace tcs

#endif /* TRANSPORT_ORDER_SERVICE_H */
