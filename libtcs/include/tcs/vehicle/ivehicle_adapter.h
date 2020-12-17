#ifndef IVEHICLE_ADAPTER_H
#define IVEHICLE_ADAPTER_H

#include <unordered_set>

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
  // virtual void InitPosition(MapObjectID point) = 0;
  virtual ~IVehicleAdapter() {}

  Event<MapObjectID>& UpdatePositionEvent() { return update_position_event_; }
  Event<MovementCommand>& FinishCommandEvent() { return finish_command_event_; }
  Event<>& RequestChargeEvent() { return request_charge_event_; }

 private:
  Event<MapObjectID> update_position_event_;
  Event<MovementCommand> finish_command_event_;
  Event<> request_charge_event_;
};

}  // namespace tcs

#endif /* IVEHICLE_ADAPTER_H */
