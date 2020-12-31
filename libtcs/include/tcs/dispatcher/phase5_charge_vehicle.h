#ifndef PHASE5_CHARGE_VEHICLE_H
#define PHASE5_CHARGE_VEHICLE_H

#include "tcs/dispatcher/assignment_candidate.h"
#include "tcs/dispatcher/phase.h"
#include "tcs/dispatcher/universal_dispatch_util.h"
#include "tcs/util/order_pool.h"

namespace tcs {

// UNDONE: We need to review this part
// NOTE: The phase would be skipped
class Phase5ChargeVehicle : public Phase {
 public:
  void Run() override;

 private:
  void CreateChargeOrder(Vehicle* vehicle);

  VehicleService* vehicle_service_;
  MapService* map_service_;
  OrderPool* order_pool_;
  IRouter* router_;
  UniversalDispatchUtil* universal_dispatch_util_;
};

}  // namespace tcs

#endif /* PHASE5_CHARGE_VEHICLE_H */
