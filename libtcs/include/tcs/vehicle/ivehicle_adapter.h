#ifndef IVEHICLE_ADAPTER_H
#define IVEHICLE_ADAPTER_H

#include "tcs/util/event.h"

namespace tcs {

// Vehicle adapter translates tcs commands into robot commands.
// NOTE: Users need to implement this interface.
class IVehicleAdapter {
 public:
  virtual ~IVehicleAdapter() {}
  Event<>& UpdatePositionEvent() { return update_position_event_; }

 private:
  Event<> update_position_event_;
};

}  // namespace tcs

#endif /* IVEHICLE_ADAPTER_H */
