#ifndef IDISPATCHER_H
#define IDISPATCHER_H

#include "tcs/data/vehicle.h"
#include "tcs/order/transport_order.h"

namespace tcs {

class IDispatcher {
 public:
  virtual void Dispatch() = 0;
  virtual void WithdrawOrder(TransportOrder* order, bool immediate) = 0;
  virtual void WithdrawOrder(Vehicle* vehicle, bool immediate) = 0;
  virtual ~IDispatcher() {}
};

}  // namespace tcs

#endif /* IDISPATCHER_H */
