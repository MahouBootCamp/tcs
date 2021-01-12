#ifndef PHASE1_FINISH_WITHDRAWAL_H
#define PHASE1_FINISH_WITHDRAWAL_H

#include "tcs/dispatcher/phase.h"

namespace tcs {

class Phase1FinishWithdrawal : public Phase {
 public:
  Phase1FinishWithdrawal(MapService* map_service,
                         VehicleService* vehicle_service,
                         TransportOrderService* transport_order_service,
                         IRouter* router)
      : map_service_{map_service},
        vehicle_service_{vehicle_service},
        transport_order_service_{transport_order_service},
        router_{router} {}
  void Run() override;

 private:
  void FinishWithDrawal(Vehicle* vehicle, TransportOrder* order);

  MapService* map_service_;
  VehicleService* vehicle_service_;
  TransportOrderService* transport_order_service_;
  IRouter* router_;
};

}  // namespace tcs

#endif /* PHASE1_FINISH_WITHDRAWAL_H */
