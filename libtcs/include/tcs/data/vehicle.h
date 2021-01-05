#ifndef VEHICLE_H
#define VEHICLE_H

#include "tcs/data/map_object.h"
#include "tcs/order/transport_order.h"

namespace tcs {

enum class VehicleState {
  kUnknown,      // Cannot connect to it
  kUnavailable,  // Connected but not available
  kError,        // Vehicle in ERROR
  kIdle,         // Can execute
  kExecuting,
  kCharging
};

// NOTE: Now it can only be kIgnored or kUtilized
enum class IntegrationLevel {
  kIgnored,    // System would take it as not exsisting.
  kNoticed,    // System knows its position, but would not reserve resources.
  kRespected,  // System reserves resources but would not assign orders.
  kUtilized    // Totally under system control.
};

enum class ProcessState { kIdle, kAwaitingOrder, kProcessingOrder };

class Vehicle : public MapObject {
  // REVIEW: Should vehicle be a map object?
 public:
  Vehicle(MapObjectID id) : MapObject{id, MapObjectType::kVehicle} {}

  bool GetNeedCharge() { return need_charge_; }
  void SetNeedCharge(bool need_charge) { need_charge_ = need_charge; }

  bool GetFinishCharge() { return finish_charge_; }
  void SetFinishCharge(bool finish_charge) { finish_charge_ = finish_charge; }

  VehicleState GetVehicleState() { return vehicle_state_; }
  void SetVehicleState(VehicleState vehicle_state) {
    vehicle_state_ = vehicle_state;
  }

  IntegrationLevel GetIntegrationLevel() { return integration_level_; }
  void SetIntegrationLevel(IntegrationLevel integration_level) {
    integration_level_ = integration_level;
  }

  ProcessState GetProcessState() { return process_state_; }
  void SetProcessState(ProcessState process_state) {
    process_state_ = process_state;
  }

  TransportOrderRef GetTransportOrder() { return transport_order_; }
  void SetTransportOrder(TransportOrderRef order_ref) {
    transport_order_ = order_ref;
  }

  std::size_t GetRouteProgressIndex() { return route_progress_index_; }
  void SetRouteProgressIndex(std::size_t index) {
    route_progress_index_ = index;
  }

  MapObjectRef GetCurrentPoint() { return current_point_; }
  void SetCurrentPoint(MapObjectRef point_ref) { current_point_ = point_ref; }

 private:
  bool need_charge_ = false;
  bool finish_charge_ = false;
  VehicleState vehicle_state_ = VehicleState::kIdle;
  IntegrationLevel integration_level_ = IntegrationLevel::kRespected;
  ProcessState process_state_ = ProcessState::kIdle;
  TransportOrderRef transport_order_ = std::nullopt;
  std::size_t route_progress_index_ = 0;
  MapObjectRef current_point_ = std::nullopt;
};

}  // namespace tcs

#endif /* VEHICLE_H */
