#ifndef VEHICLE_SERVICE_H
#define VEHICLE_SERVICE_H

#include "tcs/data/vehicle.h"
#include "tcs/util/event.h"
#include "tcs/util/map.h"

namespace tcs {

class VehicleService {
 public:
  VehicleService(Map* map, std::recursive_mutex& global_mutex)
      : map_{map}, global_mutex_{global_mutex} {}

  // Event<VehicleState, VehicleState>& VehicleStateChangeEvent() {
  //   return vehicle_state_change_event_;
  // }

  Event<const Vehicle*, ProcessState, ProcessState>&
  VehicleProcessStateChangeEvent() {
    return process_state_change_event_;
  }

  Event<const Vehicle*>& VehicleNeedChangeEvent() {
    return vehicle_need_change_event_;
  }

  template <class Predicate>
  std::unordered_set<const Vehicle*> FilterBy(Predicate p) const {
    std::scoped_lock<std::recursive_mutex> lock{global_mutex_};
    std::unordered_set<const Vehicle*> result;
    auto vehicles = map_->GetAllVehicles();
    for (auto& vehicle : vehicles) {
      if (p(const_cast<Vehicle*>(vehicle))) result.insert(vehicle);
    }
    return result;
  }

  const Vehicle* GetVehicle(MapObjectID id) const {
    return map_->GetVehicle(id);
  }

  std::unordered_set<const Vehicle*> GetAllVehicles() {
    return map_->GetAllVehicles();
  }

  std::unordered_set<const Vehicle*> FilterVehiclesByState(
      VehicleState state) const {
    return FilterBy(
        [&state](Vehicle* v) { return v->GetVehicleState() == state; });
  }

  std::unordered_set<const Vehicle*> FilterVehiclesByProcessState(
      ProcessState state) const {
    return FilterBy(
        [&state](Vehicle* v) { return v->GetProcessState() == state; });
  }

  IntegrationLevel ReadVehicleIntegrationLevel(MapObjectID vehicle_id) const;
  void UpdateVehicleIntegrationLevel(MapObjectID vehicle_id,
                                     IntegrationLevel level);

  VehicleState ReadVehicleState(MapObjectID vehicle_id) const;
  void UpdateVehicleState(MapObjectID vehicle_id, VehicleState state);

  ProcessState ReadVehicleProcessState(MapObjectID vehicle_id) const;
  void UpdateVehicleProcessState(MapObjectID vehicle_id, ProcessState state);

  TransportOrderRef ReadVehicleTransportOrder(MapObjectID vehicle_id) const;
  void UpdateVehicleTransportOrder(MapObjectID vehicle_id,
                                   TransportOrderRef order_ref);

  MapObjectRef ReadVehicleCurrentPosition(MapObjectID vehicle_id) const;
  void UpdateVehicleCurrentPosition(MapObjectID vehicle_id,
                                    MapObjectRef point_ref);

  std::size_t ReadVehicleRouteProgressIndex(MapObjectID vehicle_id) const;
  void UpdateVehicleRouteProgressIndex(MapObjectID vehicle_id,
                                       std::size_t index);

  bool ReadVehicleNeedCharge(MapObjectID vehicle_id) const;
  void UpdateVehicleNeedCharge(MapObjectID vehicle_id, bool need_charge);

  bool ReadVehicleFinishCharge(MapObjectID vehicle_id) const;
  void UpdateVehicleFinishCharge(MapObjectID vehicle_id, bool finish_charge);

 private:
  Map* map_;
  std::recursive_mutex& global_mutex_;
  // Event<VehicleState, VehicleState> vehicle_state_change_event_;
  Event<const Vehicle*, ProcessState, ProcessState> process_state_change_event_;
  Event<const Vehicle*> vehicle_need_change_event_;
};

}  // namespace tcs

#endif /* VEHICLE_SERVICE_H */
