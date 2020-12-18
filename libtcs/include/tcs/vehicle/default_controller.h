#ifndef DEFAULT_CONTROLLER_H
#define DEFAULT_CONTROLLER_H

#include <boost/log/trivial.hpp>
#include <functional>
#include <list>
#include <memory>
#include <optional>

#include "tcs/data/vehicle.h"
#include "tcs/scheduler/ischeduler.h"
#include "tcs/vehicle/ivehicle_controller.h"
#include "tcs/vehicle/movement_command.h"

namespace tcs {

// UNDONE: No multiprocess protection added
class DefaultController : public IVehicleController {
 public:
  DefaultController(Vehicle* vehicle, IVehicleAdapter* adapter,
                    IScheduler* scheduler);

  void OnUpdatePositionEvent(MapObjectID point);

  void OnFinishCommandEvent(MovementCommand cmd);

  void OnRequestChargeEvent();

  bool OnAllocationSuccessful(
      std::unordered_set<MapResource*> resources) override;

  void OnAllocationFailed() override;

  void SetDriveOrder(DriveOrder order) override;

  // Only called in reroute. I'm not implementing reroute so I'm not
  // implementing this.
  void UpdateDriveOrder(DriveOrder order) override {
    // NOT IMPLEMENTED
    return;
  }

  // Called when user aborts an order immediately.
  // void ClearDriveOrder() override;

  // Called when user aborts an order.
  void AbortDriveOrder(bool immediately) override;

  // Called when user aborts an order immediately.
  // void ClearCommandQueue() override {}

  bool CanProcess(std::unordered_set<std::string> operations) override {
    return adapter_->CanProcess(operations);
  }

  // User sets initial position for vehicle.
  // Would allocate a point resource for it immediately
  void InitPosition(MapResource* point) override;

 private:
  std::vector<std::unordered_set<MapResource*>> ExpandDriveOrder(
      DriveOrder& order);

  void CreateFutureCommands(DriveOrder& order);

  void AllocateForNextCommand();

  std::unordered_set<MapResource*> ExpandMovementCommand(MovementCommand& cmd);

  Vehicle* vehicle_;
  std::unique_ptr<IVehicleAdapter> adapter_;
  IScheduler* scheduler_;
  std::optional<DriveOrder> current_drive_order_ = std::nullopt;
  std::list<MovementCommand> command_queue_;
  std::optional<MovementCommand> pending_command_;
  std::unordered_set<MapResource*> pending_resources_;
  bool waiting_for_allocation_ = false;
  std::list<std::unordered_set<MapResource*>> allocated_resources_;
  mutable std::mutex mut_;
};

}  // namespace tcs

#endif /* DEFAULT_CONTROLLER_H */
