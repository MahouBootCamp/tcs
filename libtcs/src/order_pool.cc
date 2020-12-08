#include "tcs/util/order_pool.h"

namespace tcs {

TransportOrder* OrderPool::get_order(TransportOrderID id) {
  std::scoped_lock<std::recursive_mutex> lk{mut_};
  auto itr = order_pool_.find(id);
  if (itr == order_pool_.end())
    return nullptr;
  else
    return itr->second.get();
}

TransportOrderID OrderPool::AddOrder(
    std::vector<DriveOrder> drive_orders,
    std::unordered_set<TransportOrderID> dependencies) {
  std::scoped_lock<std::recursive_mutex> lk{mut_};
  auto ptr =
      std::make_unique<TransportOrder>(next_id_, drive_orders, dependencies);
  order_pool_.insert({next_id_, std::move(ptr)});
  auto tmp = next_id_;
  next_id_ += 1;
  return tmp;
}

}  // namespace tcs