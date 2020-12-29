#include "tcs/dispatcher/reserve_order_pool.h"

namespace tcs {

void ReserveOrderPool::RemoveReservationByVehicle(MapObjectID vehicle_id) {
  for (auto itr = reserve_order_pool_.begin();
       itr != reserve_order_pool_.end();) {
    if (itr->second == vehicle_id)
      itr = reserve_order_pool_.erase(itr);
    else
      itr++;
  }
}

std::unordered_set<TransportOrderID> ReserveOrderPool::GetReservationsByVehicle(
    MapObjectID vehicle_id) {
  std::unordered_set<TransportOrderID> result;
  for (auto& pair : reserve_order_pool_) {
    if (pair.second == vehicle_id) {
      result.insert(pair.first);
    }
  }
  return result;
}

}  // namespace tcs