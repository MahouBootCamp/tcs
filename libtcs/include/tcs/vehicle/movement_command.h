#ifndef MOVEMENT_COMMAND_H
#define MOVEMENT_COMMAND_H

#include "tcs/data/location.h"
#include "tcs/order/route.h"

namespace tcs {

struct MovementCommand {
  Route& route;
  Step& step;
  std::string operation;
  Location* operation_location;
  bool is_last_movement;
};

}  // namespace tcs

#endif /* MOVEMENT_COMMAND_H */
