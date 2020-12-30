#ifndef VEHICLE_SERVICE_H
#define VEHICLE_SERVICE_H

#include "tcs/data/vehicle.h"
#include "tcs/util/event.h"
#include "tcs/util/map.h"

namespace tcs {

class VehicleService {
 public:
  Event<VehicleState, VehicleState>& VehicleStateChangeEvent() {
    return vehicle_state_change_event_;
  }

  Event<ProcessState, ProcessState>& ProcessStateChangeEvent() {
    return process_state_change_event_;
  }

  std::unordered_set<Vehicle*> FilterVehiclesByState(VehicleState state);
  std::unordered_set<Vehicle*> FilterVehiclesByProcessState(ProcessState state);
  void UpdateVehicleState(MapObjectID vehicle_id, VehicleState state);
  void UpdateVehicleProcessState(MapObjectID vehicle_id, ProcessState state);
  void UpdateVehicleTransportOrder(MapObjectID vehicle_id,
                                   TransportOrderRef order_ref);

  template <class Predicate>
  std::unordered_set<Vehicle*> FilterBy(Predicate p) {
    std::unordered_set<Vehicle*> result;
    auto vehicles = map_->GetAllVehicles();
    for (auto& vehicle : vehicles) {
      if (p(vehicle)) result.insert(vehicle);
    }
    return result;
  }

 private:
  Map* map_;
  Event<VehicleState, VehicleState> vehicle_state_change_event_;
  Event<ProcessState, ProcessState> process_state_change_event_;
};

}  // namespace tcs

#endif /* VEHICLE_SERVICE_H */
