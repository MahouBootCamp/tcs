#ifndef PHASE0_CHECK_NEW_ORDER_H
#define PHASE0_CHECK_NEW_ORDER_H

#include "tcs/dispatcher/phase.h"

namespace tcs {

class Phase0CheckNewOrder : public Phase {
 public:
  Phase0CheckNewOrder(TransportOrderService* transport_order_service,
                      IRouter* router)
      : transport_order_service_{transport_order_service}, router_{router} {}
  void Run() override;

 private:
  TransportOrderService* transport_order_service_;
  IRouter* router_;
};

}  // namespace tcs

#endif /* PHASE0_CHECK_NEW_ORDER_H */
