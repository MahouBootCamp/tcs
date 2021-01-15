#ifndef IDISPATCHER_H
#define IDISPATCHER_H

#include <boost/log/trivial.hpp>

#include "tcs/data/vehicle.h"
#include "tcs/dispatcher/phase0_check_new_order.h"
#include "tcs/dispatcher/phase1_finish_withdrawal.h"
#include "tcs/dispatcher/phase2_assign_next_drive_order.h"
#include "tcs/dispatcher/phase3_assign_reserved_order.h"
#include "tcs/dispatcher/phase4_assign_free_order.h"
#include "tcs/dispatcher/phase5_charge_vehicle.h"
#include "tcs/dispatcher/phase6_park_vehicle.h"
#include "tcs/dispatcher/reserve_order_pool.h"
#include "tcs/dispatcher/universal_dispatch_util.h"
#include "tcs/order/transport_order.h"
#include "tcs/router/irouter.h"
#include "tcs/service/map_service.h"
#include "tcs/service/transport_order_service.h"
#include "tcs/service/vehicle_service.h"
#include "tcs/util/controller_pool.h"
#include "tcs/util/executor.h"
#include "tcs/util/map.h"

namespace tcs {

class IDispatcher {
 public:
  virtual void Dispatch() = 0;
  virtual void WithdrawOrder(const TransportOrder* order, bool immediate) = 0;
  virtual void WithdrawOrder(const Vehicle* vehicle, bool immediate) = 0;
  virtual ~IDispatcher() {}
};

}  // namespace tcs

#endif /* IDISPATCHER_H */
