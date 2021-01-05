#ifndef TRANSPORT_ORDER_H
#define TRANSPORT_ORDER_H

#include <optional>
#include <unordered_set>

#include "tcs/data/location.h"
#include "tcs/data/map_object.h"
#include "tcs/order/drive_order.h"

namespace tcs {

using TransportOrderID = int;
using TransportOrderRef = std::optional<TransportOrderID>;

enum class TransportOrderState {
  kRaw,           // Just created
  kActive,        // Routable and waiting for dependencies to finish
  kDispatchable,  // All dependencies completed, ready to dispatch
  kBeingProcessed,
  kWithdraw,
  kFinished,
  kFailed,
  kUnRoutable
};

inline bool IsFinalState(TransportOrderState state) {
  return state == TransportOrderState::kFinished ||
         state == TransportOrderState::kFailed ||
         state == TransportOrderState::kUnRoutable;
}

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
    // System created Parking order can be dispensed
    if (destinations.back().operation == kParkOperation) dispensable_ = true;
  }

  TransportOrderID get_id() { return id_; }
  std::vector<DriveOrder>& get_drive_orders() { return drive_orders_; }

  std::vector<DriveOrder> GetFutureDriveOrders() {
    return std::vector<DriveOrder>(drive_orders_.begin() + progress_index_,
                                   drive_orders_.end());
  }

  std::unordered_set<TransportOrderID>& get_dependencies() {
    return dependencies_;
  }

  std::size_t get_progress_index() { return progress_index_; }

  TransportOrderState get_state() { return state_; }

  MapObjectRef get_vehicle() { return vehicle_; }

  bool get_dispensable() { return dispensable_; }

 private:
  TransportOrderID id_;
  std::vector<DriveOrder> drive_orders_;
  std::unordered_set<TransportOrderID> dependencies_;
  std::size_t progress_index_ = 0;
  TransportOrderState state_ = TransportOrderState::kRaw;
  MapObjectRef vehicle_ = std::nullopt;
  bool dispensable_ = false;
};

}  // namespace tcs

#endif /* TRANSPORT_ORDER_H */
