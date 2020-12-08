#ifndef VEHICLE_H
#define VEHICLE_H

#include "tcs/data/map_object.h"
#include "tcs/order.h"

namespace tcs {

enum class VehicleState {
  kUnknown,      // Cannot connect to it
  kUnavailable,  // Connected but not available
  kError,        // Vehicle in ERROR
  kIdle,         // Can execute
  kExecuting,
  kCharging
};

enum class IntegrationLevel {
  kIgnored,    // System would take it as not exsisting.
  kNoticed,    // System knows its position, but would not reserve resources.
  kRespected,  // System reserves resources but would not assign orders.
  kUtilized    // Totally under system control.
};

enum class ProcessState { kIdle, kAwaitingOrder, kProcessingOrder };

class Vehicle : public MapObject {
  // REVIEW: Should vehicle be a map element?
 public:
  Vehicle(MapObjectID id) : MapObject(id, MapObjectType::kVehicle) {}

  bool get_need_charge() { return need_charge_; }
  void set_need_charge(bool need_charge) { need_charge_ = need_charge; }

  bool get_finish_charge() { return finish_charge_; }
  void set_finish_charge(bool finish_charge) { finish_charge_ = finish_charge; }

  VehicleState get_vehicle_state() { return vehicle_state_; }
  void set_vehicle_state(VehicleState vehicle_state) {
    vehicle_state_ = vehicle_state;
  }

  IntegrationLevel get_integration_level() { return integration_level_; }
  void set_integration_level(IntegrationLevel integration_level) {
    integration_level_ = integration_level;
  }

  ProcessState get_process_state() { return process_state_; }
  void set_process_state(ProcessState process_state) {
    process_state_ = process_state;
  }

  TransportOrderRef get_transport_order() { return transport_order_; }
  void set_transport_order(TransportOrderRef order_ref) {
    transport_order_ = order_ref;
  }

  std::size_t get_route_progress_index() { return route_progress_index_; }
  void set_route_progress_index(std::size_t index) {
    route_progress_index_ = index;
  }

  MapObjectRef get_current_point() { return current_point_; }
  void set_current_point(MapObjectRef point_ref) { current_point_ = point_ref; }

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
