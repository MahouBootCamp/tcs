#ifndef DEFAULT_CONTROLLER_H
#define DEFAULT_CONTROLLER_H

#include <boost/log/trivial.hpp>
#include <functional>
#include <memory>
#include <optional>

#include "tcs/data/vehicle.h"
#include "tcs/scheduler/ischeduler.h"
#include "tcs/vehicle/ivehicle_controller.h"
#include "tcs/vehicle/movement_command.h"

namespace tcs {

// Undone: No multiprocess protection added
class DefaultController : public IVehicleController {
 public:
  DefaultController(Vehicle* vehicle, IVehicleAdapter* adapter,
                    IScheduler* scheduler)
      : vehicle_{vehicle}, adapter_{adapter}, scheduler_{scheduler} {
    adapter_->FinishCommandEvent().Subscribe(std::bind(
        &DefaultController::OnFinishCommandEvent, this, std::placeholders::_1));
    adapter_->UpdatePositionEvent().Subscribe(
        std::bind(&DefaultController::OnUpdatePositionEvent, this,
                  std::placeholders::_1));
    adapter_->RequestChargeEvent().Subscribe(
        std::bind(&DefaultController::OnRequestChargeEvent, this));

    adapter_->Enable();
  }

  void OnUpdatePositionEvent(MapObjectID point) {
    vehicle_->set_current_point(point);
  }

  void OnFinishCommandEvent(MovementCommand cmd) {
    if (cmd.operation == kChargeOperation) {
      vehicle_->set_finish_charge(true);
      vehicle_->set_need_charge(false);
    }
  }

  void OnRequestChargeEvent() { vehicle_->set_need_charge(true); }

  bool OnAllocationSuccessful() override { return true; }

  void OnAllocationFailed() override {}

  void SetDriveOrder(DriveOrder order) override;

  void UpdateDriveOrder(DriveOrder order) override {}

  void ClearDriveOrder() override {}

  void AbortDriveOrder() override {}

  void ClearCommandQueue() override {}

  bool CanProcess(std::unordered_set<std::string> operations) override {
    return adapter_->CanProcess(operations);
  }

  void InitPosition(MapObjectID point) override {
    // adapter_->InitPosition(point);
  }

 private:
  std::vector<std::unordered_set<MapResource*>> ExpandDriveOrder(
      DriveOrder& order);
  
  void CreateFutureCommands(DriveOrder& order);

  Vehicle* vehicle_;
  std::unique_ptr<IVehicleAdapter> adapter_;
  IScheduler* scheduler_;
  std::optional<DriveOrder> current_drive_order_ = std::nullopt;
  std::vector<MovementCommand> command_vector_;
};

}  // namespace tcs

#endif /* DEFAULT_CONTROLLER_H */
