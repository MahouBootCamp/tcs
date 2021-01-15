#ifndef PHASE3_ASSIGN_RESERVED_ORDER_H
#define PHASE3_ASSIGN_RESERVED_ORDER_H

#include "tcs/dispatcher/assignment_candidate.h"
#include "tcs/dispatcher/phase.h"
#include "tcs/dispatcher/universal_dispatch_util.h"

namespace tcs {

class Phase3AssignReservedOrder : public Phase {
 public:
  Phase3AssignReservedOrder(MapService* map_service,
                            VehicleService* vehicle_service,
                            TransportOrderService* transport_order_service,
                            IRouter* router,
                            ReserveOrderPool* reserve_order_pool,
                            UniversalDispatchUtil* universal_dispatch_util)
      : map_service_{map_service},
        vehicle_service_{vehicle_service},
        transport_order_service_{transport_order_service},
        router_{router},
        reserve_order_pool_{reserve_order_pool},
        universal_dispatch_util_{universal_dispatch_util} {}

  void Run() override;

 private:
  void CheckForReservedOrder(const Vehicle* vehicle);

  MapService* map_service_;
  VehicleService* vehicle_service_;
  TransportOrderService* transport_order_service_;
  IRouter* router_;
  ReserveOrderPool* reserve_order_pool_;
  UniversalDispatchUtil* universal_dispatch_util_;
};

}  // namespace tcs

#endif /* PHASE3_ASSIGN_RESERVED_ORDER_H */
