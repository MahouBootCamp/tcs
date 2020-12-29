#ifndef PHASE0_CHECK_NEW_ORDER_H
#define PHASE0_CHECK_NEW_ORDER_H

#include "tcs/dispatcher/phase.h"

namespace tcs {

class Phase0CheckNewOrder : public Phase {
 public:
  // Phase0CheckNewOrder(IRouter* router,
  //                     TransportOrderService* transport_order_service)
  //     : router_{router}, transport_order_service_{transport_order_service} {}
  void Run() override;

 private:
  IRouter* router_;
  TransportOrderService* transport_order_service_;
};

}  // namespace tcs

#endif /* PHASE0_CHECK_NEW_ORDER_H */
