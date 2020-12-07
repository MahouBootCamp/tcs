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
 public:
  Vehicle(MapObjectID id) : MapObject(id, MapObjectType::kVehicle) {}

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
