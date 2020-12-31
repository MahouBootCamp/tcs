#ifndef PHASE1_FINISH_WITHDRAWAL_H
#define PHASE1_FINISH_WITHDRAWAL_H

#include "tcs/dispatcher/phase.h"

namespace tcs {

class Phase1FinishWithdrawal : public Phase {
 public:
  // Phase1FinishWithdrawal(IRouter* router, VehicleService* vehicle_service,
  //                        TransportOrderService* transport_order_service)
  //     : router_{router},
  //       vehicle_service_{vehicle_service},
  //       transport_order_service_{transport_order_service} {}
  void Run() override;

 private:
  void FinishWithDrawal(Vehicle* vehicle, TransportOrder* order);

  IRouter* router_;
  VehicleService* vehicle_service_;
  TransportOrderService* transport_order_service_;
  MapService* map_service_;
};

}  // namespace tcs

#endif /* PHASE1_FINISH_WITHDRAWAL_H */
