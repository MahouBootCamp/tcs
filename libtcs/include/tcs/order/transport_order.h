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

  TransportOrderID GetID() const { return id_; }

  std::vector<DriveOrder> GetDriveOrders() {
    return drive_orders_;
  }

  void SetDriveOrders(std::vector<DriveOrder> drive_orders) {
    drive_orders_ = drive_orders;
  }

  std::vector<DriveOrder> GetFutureDriveOrders() {
    return std::vector<DriveOrder>(drive_orders_.begin() + progress_index_,
                                   drive_orders_.end());
  }

  const std::unordered_set<TransportOrderID>& GetDependencies() const {
    return dependencies_;
  }

  std::size_t GetProgressIndex() { return progress_index_; }
  void SetProgressIndex(std::size_t index) { progress_index_ = index; }

  TransportOrderState GetState() { return state_; }
  void SetState(TransportOrderState state) { state_ = state; }

  MapObjectRef GetVehicle() { return vehicle_; }
  void SetVehicle(MapObjectRef vehicle_ref) { vehicle_ = vehicle_ref; }

  bool GetDispensable() const { return dispensable_; }

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
