#ifndef DEFAULT_DISPATCHER_H
#define DEFAULT_DISPATCHER_H

#include <boost/log/trivial.hpp>

#include "tcs/dispatcher/idispatcher.h"
#include "tcs/dispatcher/phase0_check_new_order.h"
#include "tcs/dispatcher/phase1_finish_withdrawal.h"
#include "tcs/dispatcher/phase2_assign_next_drive_order.h"
#include "tcs/dispatcher/phase3_assign_reserved_order.h"
#include "tcs/dispatcher/phase4_assign_free_order.h"
#include "tcs/dispatcher/phase5_recharge_vehicle.h"
#include "tcs/dispatcher/phase6_park_vehicle.h"
#include "tcs/dispatcher/reserve_order_pool.h"
#include "tcs/dispatcher/universal_dispatch_util.h"
#include "tcs/router/irouter.h"
#include "tcs/service/map_service.h"
#include "tcs/service/transport_order_service.h"
#include "tcs/service/vehicle_service.h"
#include "tcs/util/controller_pool.h"
#include "tcs/util/executor.h"
#include "tcs/util/map.h"

namespace tcs {

// TODO: multithread protection.
// TODO: add trigger events for dispatch.
class DefaultDispatcher : public IDispatcher {
 public:
  // TODO: Imcomplete constructor
  DefaultDispatcher(MapService* map_service, IRouter* router,
                    Executor* executor, ControllerPool* controller_pool)
      : map_service_{map_service},
        router_{router},
        executor_(executor),
        controller_pool_{controller_pool} {}
  void Dispatch() override;
  void WithdrawOrder(TransportOrder* order, bool immediate) override;
  void WithdrawOrder(Vehicle* vehicle, bool immediate) override;

 private:
  void DispatchTask();

  MapService* map_service_;
  IRouter* router_;
  Executor* executor_;
  TransportOrderService* transport_order_service_;
  VehicleService* vehicle_service_;
  ControllerPool* controller_pool_;
  std::unique_ptr<ReserveOrderPool> reserve_order_pool_;
  std::unique_ptr<UniversalDispatchUtil> universal_dispatch_util_;

  // TODO: Add constructors for all phases
  Phase0CheckNewOrder phase0_;
  Phase1FinishWithdrawal phase1_;
  Phase2AssignNextDriveOrder phase2_;
  Phase3AssignReservedOrder phase3_;
  Phase4AssignFreeOrder phase4_;
  Phase5RechargeVehicle phase5_;
  Phase6ParkVehicle phase6_;
};

}  // namespace tcs

#endif /* DEFAULT_DISPATCHER_H */
