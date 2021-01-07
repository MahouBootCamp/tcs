#ifndef IVEHICLE_ADAPTER_H
#define IVEHICLE_ADAPTER_H

#include <unordered_set>

#include "tcs/data/vehicle.h"
#include "tcs/util/event.h"
#include "tcs/vehicle/movement_command.h"

namespace tcs {

// Vehicle adapter translates tcs commands into robot commands.
// NOTE: Users need to implement this interface.
class IVehicleAdapter {
 public:
  virtual void Enable() = 0;
  virtual void Disable() = 0;
  virtual bool IsEnabled() = 0;
  virtual bool CanProcess(std::unordered_set<std::string> operations) = 0;
  virtual bool EnqueueCommand(MovementCommand command) = 0;
  virtual void ClearCommandQueue() = 0;
  virtual bool CanEnqueueCommand() = 0;
  // virtual void InitPosition(MapObjectID point) = 0;
  virtual ~IVehicleAdapter() {}

  Event<MapObjectRef>& UpdatePositionEvent() { return update_position_event_; }
  Event<MovementCommand>& FinishCommandEvent() { return finish_command_event_; }
  Event<MovementCommand>& FailCommandEvent() { return fail_command_event_; }
  Event<>& RequestChargeEvent() { return request_charge_event_; }
  Event<VehicleState>& UpdateVehicleStateEvent() {
    return update_vehicle_state_event_;
  }

 private:
  Event<MapObjectRef> update_position_event_;
  Event<MovementCommand> finish_command_event_;
  Event<MovementCommand> fail_command_event_;
  Event<> request_charge_event_;
  Event<VehicleState> update_vehicle_state_event_;
};

}  // namespace tcs

#endif /* IVEHICLE_ADAPTER_H */
