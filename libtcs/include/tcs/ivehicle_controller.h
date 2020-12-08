#ifndef IVEHICLE_CONTROLLER_H
#define IVEHICLE_CONTROLLER_H

#include <unordered_set>
#include <vector>

#include "tcs/data/map_resource.h"
#include "tcs/order/transport_order.h"

namespace tcs {

// Vehicle controller provides interfaces for controlling vehicles
class IVehicleController {
 public:
  // Check if these resources are required now if withdraw or reroute happened
  virtual bool AllocationIsNowRequired(
      std::unordered_set<MapResource*> resources) = 0;
  // Called when allocation fails
  virtual void AllocationFailed() = 0;
  virtual void SetDriveOrder(DriveOrder order) = 0;
  virtual void UpdateDriveOrder(DriveOrder order) = 0;
  virtual void ClearDriveOrder() = 0;
  virtual void AbortDriveOrder() = 0;
  virtual void ClearCommandQueue() = 0;
  virtual bool CanProcess() = 0;
  // TODO: Add a get movement command queue function
  virtual ~IVehicleController() {}
};

}  // namespace tcs

#endif /* IVEHICLE_CONTROLLER_H */
