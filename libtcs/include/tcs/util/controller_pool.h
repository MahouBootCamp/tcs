#ifndef CONTROLLER_POOL_H
#define CONTROLLER_POOL_H

#include <memory>
#include <unordered_map>

#include "tcs/scheduler/ischeduler.h"
#include "tcs/util/map.h"
#include "tcs/vehicle/default_controller.h"
#include "tcs/vehicle/ivehicle_controller.h"
#include "tcs/vehicle/sim_vehicle_adapter.h"

namespace tcs {

// Controller Pool stores a map of vehicles to controllers
class ControllerPool {
 public:
  ControllerPool(Map* map, IScheduler* scheduler)
      : map_{map}, scheduler_{scheduler} {}

  // Controller would take the management of adapter's lifecycle.
  // If adapter is null, system would generate a simulate adapter.
  void AttachVehicleController(MapObjectID vehicle,
                               IVehicleAdapter* adapter = nullptr);

  // Detach a vehicle.
  // UNDONE: Not implemented.
  void DetachVehicleController(MapObjectID vehicle);

  IVehicleController* GetController(MapObjectID vehicle);

 private:
  Map* map_;
  IScheduler* scheduler_;
  std::unordered_map<MapObjectID, std::unique_ptr<IVehicleController>>
      controller_pool_;
};

}  // namespace tcs

#endif /* CONTROLLER_POOL_H */
