#ifndef PHASE6_PARK_VEHICLE_H
#define PHASE6_PARK_VEHICLE_H

#include "tcs/dispatcher/phase.h"

namespace tcs {

class Phase6ParkVehicle : public Phase {
 public:
  void Run() override;
};

}  // namespace tcs

#endif /* PHASE6_PARK_VEHICLE_H */
