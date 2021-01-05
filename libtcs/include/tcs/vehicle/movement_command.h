#ifndef MOVEMENT_COMMAND_H
#define MOVEMENT_COMMAND_H

#include "tcs/data/location.h"
#include "tcs/order/route.h"

namespace tcs {

struct MovementCommand {
  // Route& route;
  Step step;
  std::string operation;
  // Location* operation_location;
  bool is_last_movement;
};

inline bool operator==(const MovementCommand &cmd1,
                       const MovementCommand &cmd2) {
  return cmd1.step == cmd2.step && cmd1.operation == cmd2.operation &&
         cmd1.is_last_movement == cmd2.is_last_movement;
}

inline bool operator!=(const MovementCommand &cmd1,
                       const MovementCommand &cmd2) {
  return !(cmd1 == cmd2);
}

}  // namespace tcs

#endif /* MOVEMENT_COMMAND_H */
