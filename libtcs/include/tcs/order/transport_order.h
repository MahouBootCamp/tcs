#ifndef TRANSPORT_ORDER_H
#define TRANSPORT_ORDER_H

#include <optional>
#include <unordered_set>

#include "tcs/data/map_object.h"
#include "tcs/order/drive_order.h"

namespace tcs {

using TransportOrderID = int;
using TransportOrderRef = std::optional<TransportOrderID>;

enum class TransportOrderState {
  kRaw,           // Just created
  kActive,        // Creation completed, waiting to check dispachability
  kDispatchable,  // All dependencies completed, ready to dispatch
  kBeingProcessed,
  kWithdraw,
  kFinished,
  kFailed
};

class TransportOrder {
 public:
  // TransportOrder(TransportOrderID id, std::vector<DriveOrder> drive_orders,
  //                std::unordered_set<TransportOrderID> dependencies)
  //     : id_(id), drive_orders_(drive_orders), dependencies_(dependencies) {}

  TransportOrder(TransportOrderID id, std::vector<Destination> destinations,
                 std::unordered_set<TransportOrderID> dependencies)
      : id_{id}, dependencies_{dependencies} {
    for (auto&& dst : destinations) {
      drive_orders_.push_back(dst);
    }
  }

  TransportOrderID get_id() { return id_; }
  std::vector<DriveOrder>& get_drive_orders() { return drive_orders_; }

  std::vector<DriveOrder> GetFutureDriveOrders() {
    return std::vector<DriveOrder>(
        drive_orders_.begin() + next_drive_order_index_, drive_orders_.end());
  }

  std::unordered_set<TransportOrderID>& get_dependencies() {
    return dependencies_;
  }

 private:
  TransportOrderID id_;
  std::vector<DriveOrder> drive_orders_;
  std::unordered_set<TransportOrderID> dependencies_;
  std::size_t next_drive_order_index_ = 0;
  TransportOrderState state_ = TransportOrderState::kRaw;
  MapObjectRef vehicle_ = std::nullopt;
};

}  // namespace tcs

#endif /* TRANSPORT_ORDER_H */
