#ifndef RESERVE_ORDER_POOL_H
#define RESERVE_ORDER_POOL_H

#include <unordered_map>

#include "tcs/service/map_service.h"

namespace tcs {

class ReserveOrderPool {
 public:
  bool IsReserved(TransportOrderID order_id) {
    return reserve_order_pool_.find(order_id) != reserve_order_pool_.end();
  }

  void AddReservation(TransportOrderID order_id, MapObjectID vehicle_id) {
    reserve_order_pool_.insert({order_id, vehicle_id});
  }

  void RemoveReservationByOrder(TransportOrderID order_id) {
    reserve_order_pool_.erase(order_id);
  }

  void RemoveReservationByVehicle(MapObjectID vehicle_id);

  std::unordered_set<TransportOrderID> GetReservationsByVehicle(
      MapObjectID vehicle_id);

 private:
  std::unordered_map<TransportOrderID, MapObjectID> reserve_order_pool_;
};

}  // namespace tcs

#endif /* RESERVE_ORDER_POOL_H */
