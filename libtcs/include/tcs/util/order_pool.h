#ifndef ORDER_POOL_H
#define ORDER_POOL_H

#include <memory>
#include <mutex>
#include <unordered_map>

#include "tcs/order/transport_order.h"

namespace tcs {

class OrderPool {
 public:
  TransportOrder* get_order(TransportOrderID);
  TransportOrderID AddOrder(
      std::vector<DriveOrder> drive_orders,
      std::unordered_set<TransportOrderID> dependencies = {});

 private:
  mutable std::recursive_mutex mut_;
  TransportOrderID next_id_ = 0;
  std::unordered_map<TransportOrderID, std::unique_ptr<TransportOrder>>
      order_pool_;
};

}  // namespace tcs

#endif /* ORDER_POOL_H */
