#ifndef PHASE5_RECHARGE_VEHICLE_H
#define PHASE5_RECHARGE_VEHICLE_H

#include "tcs/dispatcher/phase.h"

namespace tcs {

class Phase5RechargeVehicle : public Phase {
 public:
  void Run() override;
};

}  // namespace tcs

#endif /* PHASE5_RECHARGE_VEHICLE_H */
