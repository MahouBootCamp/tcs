#ifndef VEHICLE_SERVICE_H
#define VEHICLE_SERVICE_H

#include "tcs/data/vehicle.h"
#include "tcs/util/event.h"
#include "tcs/util/map.h"

namespace tcs {

class VehicleService {
 public:
  VehicleService(Map* map) : map_{map} {}

  Event<VehicleState, VehicleState>& VehicleStateChangeEvent() {
    return vehicle_state_change_event_;
  }

  Event<ProcessState, ProcessState>& ProcessStateChangeEvent() {
    return process_state_change_event_;
  }

  template <class Predicate>
  std::unordered_set<Vehicle*> FilterBy(Predicate p) {
    std::unordered_set<Vehicle*> result;
    auto vehicles = map_->GetAllVehicles();
    for (auto& vehicle : vehicles) {
      if (p(vehicle)) result.insert(vehicle);
    }
    return result;
  }

  Vehicle* GetVehicle(MapObjectID id) { return map_->GetVehicle(id); }

  std::unordered_set<Vehicle*> GetAllVehicles() {
    return map_->GetAllVehicles();
  }

  std::unordered_set<Vehicle*> FilterVehiclesByState(VehicleState state) {
    return FilterBy(
        [&state](Vehicle* v) { return v->GetVehicleState() == state; });
  }

  std::unordered_set<Vehicle*> FilterVehiclesByProcessState(
      ProcessState state) {
    return FilterBy(
        [&state](Vehicle* v) { return v->GetProcessState() == state; });
  }

  void UpdateVehicleState(MapObjectID vehicle_id, VehicleState state);

  void UpdateVehicleProcessState(MapObjectID vehicle_id, ProcessState state);

  void UpdateVehicleTransportOrder(MapObjectID vehicle_id,
                                   TransportOrderRef order_ref);

  void UpdateVehicleCurrentPosition(MapObjectID vehicle_id,
                                    MapObjectRef point_ref);

  void UpdateVehicleRouteProgressIndex(MapObjectID vehicle_id,
                                       std::size_t index);

  void UpdateVehicleNeedCharge(MapObjectID vehicle_id, bool need_charge);

  void UpdateVehicleFinishCharge(MapObjectID vehicle_id, bool finish_charge);

 private:
  Map* map_;
  Event<VehicleState, VehicleState> vehicle_state_change_event_;
  Event<ProcessState, ProcessState> process_state_change_event_;
};

}  // namespace tcs

#endif /* VEHICLE_SERVICE_H */
