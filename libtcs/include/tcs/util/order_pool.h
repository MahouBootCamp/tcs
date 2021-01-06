#ifndef ORDER_POOL_H
#define ORDER_POOL_H

#include <memory>
#include <mutex>
#include <stdexcept>
#include <unordered_map>

#include "tcs/order/transport_order.h"
#include "tcs/util/map.h"

namespace tcs {

class OrderPool {
 public:
  // Default constructor only for test.
  // TODO: DELETE it after finishing dev!
  OrderPool() = default;

  // Map* MUST NOT be nullptr
  OrderPool(Map* map) : map_{map} {}

  TransportOrder* GetOrder(TransportOrderID);
  TransportOrderID AddOrder(
      std::vector<Destination> destinations,
      std::unordered_set<TransportOrderID> dependencies = {});
  std::unordered_set<TransportOrder*> GetAllOrders();
  std::unordered_set<TransportOrder*> GetOrdersByID(
      std::unordered_set<TransportOrderID>& id_set);

 private:
  // mutable std::recursive_mutex mut_;
  TransportOrderID next_id_ = 0;
  std::unordered_map<TransportOrderID, std::unique_ptr<TransportOrder>>
      order_pool_;
  Map* map_;
};

}  // namespace tcs

#endif /* ORDER_POOL_H */
