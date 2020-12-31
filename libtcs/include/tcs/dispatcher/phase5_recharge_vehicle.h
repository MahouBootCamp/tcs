#ifndef PHASE5_RECHARGE_VEHICLE_H
#define PHASE5_RECHARGE_VEHICLE_H

#include "tcs/dispatcher/phase.h"

namespace tcs {

// UNDONE: We need to review this part
// NOTE: The phase would be skipped
class Phase5RechargeVehicle : public Phase {
 public:
  void Run() override;
 private:
  void CreateRechargeOrder(Vehicle* vehicle);

  VehicleService* vehicle_service_;
  MapService* map_service_;
};

}  // namespace tcs

#endif /* PHASE5_RECHARGE_VEHICLE_H */
