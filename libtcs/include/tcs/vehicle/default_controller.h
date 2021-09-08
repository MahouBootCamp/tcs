#ifndef DEFAULT_CONTROLLER_H
#define DEFAULT_CONTROLLER_H

#include <boost/log/trivial.hpp>
#include <functional>
#include <list>
#include <memory>
#include <optional>

#include "tcs/data/vehicle.h"
#include "tcs/scheduler/ischeduler.h"
#include "tcs/service/vehicle_service.h"
#include "tcs/vehicle/ivehicle_controller.h"
#include "tcs/vehicle/movement_command.h"

namespace tcs {

class DefaultController : public IVehicleController {
 public:
  // Controller would take over control of Adapter's lifecycle
  DefaultController(const Vehicle* vehicle, IVehicleAdapter* adapter,
                    VehicleService* vehicle_service, IScheduler* scheduler);

  ~DefaultController();

  MapObjectID GetVehicleID() override { return vehicle_->GetID(); }

  void UpdatePositionEventHandler(MapObjectRef point_ref);

  void FinishCommandEventHandler(MovementCommand cmd);

  void FailCommandEventHandler(MovementCommand cmd);

  void RequestChargeEventHandler();

  void UpdateVehicleStateEventHandler(VehicleState state);

  bool AllocationSuccessful(
      std::unordered_set<const MapResource*> resources) override;

  void AllocationFailed() override;

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
  void InitPosition(const Point* point) override;

 private:
  std::vector<std::unordered_set<const MapResource*>> ExpandDriveOrder(
      DriveOrder& order);

  void CreateFutureCommands(DriveOrder& order);

  void AllocateForNextCommand();

  std::unordered_set<const MapResource*> ExpandMovementCommand(
      MovementCommand& cmd);

  bool CanSendNextCommand() {
    return adapter_->CanEnqueueCommand() && command_queue_.size() > 0 &&
           !waiting_for_allocation_;
  }

  bool HasFinishedOrAbortedDriveOrder() {
    return command_sent_.empty() && command_queue_.empty() &&
           !pending_command_.has_value();
  }

  const Vehicle* vehicle_;
  std::unique_ptr<IVehicleAdapter> adapter_;
  VehicleService* vehicle_service_;
  IScheduler* scheduler_;

  std::optional<DriveOrder> current_drive_order_ = std::nullopt;
  std::list<MovementCommand> command_queue_;
  std::list<MovementCommand> command_sent_;
  std::optional<MovementCommand> pending_command_;
  std::unordered_set<const MapResource*> pending_resources_;
  bool waiting_for_allocation_ = false;
  std::list<std::unordered_set<const MapResource*>> allocated_resources_;
  mutable std::mutex vehicle_mut_;
};

}  // namespace tcs

#endif /* DEFAULT_CONTROLLER_H */
