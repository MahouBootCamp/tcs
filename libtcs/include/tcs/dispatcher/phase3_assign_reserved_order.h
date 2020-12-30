#ifndef PHASE3_ASSIGN_RESERVED_ORDER_H
#define PHASE3_ASSIGN_RESERVED_ORDER_H

#include "tcs/dispatcher/phase.h"
#include "tcs/dispatcher/assignment_candidate.h"
#include "tcs/dispatcher/universal_dispatch_util.h"

namespace tcs {

class Phase3AssignReservedOrder : public Phase {
 public:
  void Run() override;

 private:
  void CheckForReservedOrder(Vehicle* vehicle);

  VehicleService* vehicle_service_;
  ReserveOrderPool* reserve_order_pool_;
  TransportOrderService* transport_order_service_;
  IRouter* router_;
  MapService* map_service_;
  UniversalDispatchUtil* universal_dispatch_util_;
};

}  // namespace tcs

#endif /* PHASE3_ASSIGN_RESERVED_ORDER_H */
