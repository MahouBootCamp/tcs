#ifndef ISCHEDULER_H
#define ISCHEDULER_H

#include <unordered_set>
#include <vector>

#include "tcs/data/map_resource.h"
#include "tcs/ivehicle_controller.h"

namespace tcs {

class IScheduler {
 public:
  // Claim a sequence of resources for a vehicle
  virtual void Claim(
      IVehicleController* vehicle,
      std::vector<std::unordered_set<MapResource*>> resource_sequence) = 0;
  // Unclaim resources for a vehicle
  virtual void Unclaim(IVehicleController* vehicle) = 0;
  // Notify scheduler of a vehicle's progress. Resources in sequence before
  // index are no longer needed.
  virtual void UpdateProgressIndex(IVehicleController* vehicle,
                                   std::size_t index) = 0;
  // Allocate resources for vehicle. A MapResource can only be reserved for one
  // vehicle at the same time.
  virtual void Allocate(IVehicleController* vehicle,
                        std::unordered_set<MapResource*> resources) = 0;
  // Allocate resources for vehicle without blocking.
  // NOTE: This function is for manual control or emergency.
  virtual void AllocateNow(IVehicleController* vehicle,
                           std::unordered_set<MapResource*> resources) = 0;
  // Free specified resources
  virtual void Free(IVehicleController* vehicle,
                    std::unordered_set<MapResource*> resources) = 0;
  // Free resources allocated for vehicle
  virtual void FreeAll(IVehicleController* vehicle) = 0;
  virtual ~IScheduler() {}
};

}  // namespace tcs

#endif /* ISCHEDULER_H */
