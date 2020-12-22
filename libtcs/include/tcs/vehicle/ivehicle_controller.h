#ifndef IVEHICLE_CONTROLLER_H
#define IVEHICLE_CONTROLLER_H

#include <unordered_set>
#include <vector>

#include "tcs/data/map_resource.h"
#include "tcs/order/transport_order.h"
#include "tcs/vehicle/ivehicle_adapter.h"

namespace tcs {

// Vehicle controller provides interfaces for controlling vehicles
class IVehicleController {
 public:
  virtual MapObjectID GetVehicleID() = 0;
  // Called by scheduler
  virtual bool OnAllocationSuccessful(std::unordered_set<MapResource*> resources) = 0;
  // Called by scheduler
  virtual void OnAllocationFailed() = 0;
  virtual void SetDriveOrder(DriveOrder order) = 0;
  virtual void UpdateDriveOrder(DriveOrder order) = 0;
  // virtual void ClearDriveOrder() = 0;
  virtual void AbortDriveOrder(bool immediately) = 0;
  // virtual void ClearCommandQueue() = 0;
  virtual bool CanProcess(std::unordered_set<std::string> operations) = 0;
  virtual void InitPosition(MapResource* point) = 0;
  // TODO: Add a get movement command queue function

  // virtual void OnUpdatePositionEvent() = 0;
  // virtual void OnFinishCommandEvent() = 0;
  // virtual void OnRequestChargeEvent() = 0;

  virtual ~IVehicleController() {}
};

}  // namespace tcs

#endif /* IVEHICLE_CONTROLLER_H */
