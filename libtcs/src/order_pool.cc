#include "tcs/util/order_pool.h"

namespace tcs {

TransportOrder* OrderPool::get_order(TransportOrderID id) {
  // std::scoped_lock<std::recursive_mutex> lk{mut_};
  auto itr = order_pool_.find(id);
  if (itr == order_pool_.end())
    return nullptr;
  else
    return itr->second.get();
}

TransportOrderID OrderPool::AddOrder(
    std::vector<DriveOrder> drive_orders,
    std::unordered_set<TransportOrderID> dependencies) {
  // Check validity
  for (auto& order : drive_orders) {
    auto site = map_->get_resource(order.get_destination().site);
    if (!site || site->get_type() == MapObjectType::kPath)
      throw std::invalid_argument("Destination invalid");
  }

  for (auto& transport_order : dependencies) {
    if (!get_order(transport_order))
      throw std::invalid_argument("Dependency invalid");
  }

  // std::scoped_lock<std::recursive_mutex> lk{mut_};
  auto ptr =
      std::make_unique<TransportOrder>(next_id_, drive_orders, dependencies);
  order_pool_.insert({next_id_, std::move(ptr)});
  auto tmp = next_id_;
  next_id_ += 1;
  return tmp;
}

}  // namespace tcs