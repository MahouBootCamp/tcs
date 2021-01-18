#ifndef DEFAULT_DISPATCHER_H
#define DEFAULT_DISPATCHER_H

#include <boost/log/trivial.hpp>

#include "tcs/dispatcher/idispatcher.h"
#include "tcs/dispatcher/phase0_check_new_order.h"
#include "tcs/dispatcher/phase1_finish_withdrawal.h"
#include "tcs/dispatcher/phase2_assign_next_drive_order.h"
#include "tcs/dispatcher/phase3_assign_reserved_order.h"
#include "tcs/dispatcher/phase4_assign_free_order.h"
#include "tcs/dispatcher/phase5_charge_vehicle.h"
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

class DefaultDispatcher : public IDispatcher {
 public:
  DefaultDispatcher(Executor* executor, OrderPool* order_pool,
                    MapService* map_service, VehicleService* vehicle_service,
                    TransportOrderService* transport_order_service,
                    IRouter* router, ControllerPool* controller_pool);

  void Dispatch() override;
  void WithdrawOrder(const TransportOrder* order, bool immediate) override;
  void WithdrawOrder(const Vehicle* vehicle, bool immediate) override;

 private:
  void DispatchTask();

  void DispatchPeriodically();

  void VehicleProcessStateChangeEventHandler(const Vehicle* vehicle,
                                             ProcessState old_state,
                                             ProcessState new_state);

  void VehicleNeedChargeEventHandler(const Vehicle* vehicle);

  Executor* executor_;
  OrderPool* order_pool_;
  MapService* map_service_;
  VehicleService* vehicle_service_;
  TransportOrderService* transport_order_service_;
  IRouter* router_;
  ControllerPool* controller_pool_;
  std::unique_ptr<ReserveOrderPool> reserve_order_pool_;
  std::unique_ptr<UniversalDispatchUtil> universal_dispatch_util_;

  Phase0CheckNewOrder phase0_;
  Phase1FinishWithdrawal phase1_;
  Phase2AssignNextDriveOrder phase2_;
  Phase3AssignReservedOrder phase3_;
  Phase4AssignFreeOrder phase4_;
  Phase5ChargeVehicle phase5_;
  Phase6ParkVehicle phase6_;
};

}  // namespace tcs

#endif /* DEFAULT_DISPATCHER_H */
