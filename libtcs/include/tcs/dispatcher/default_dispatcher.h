#ifndef DEFAULT_DISPATCHER_H
#define DEFAULT_DISPATCHER_H

#include <boost/log/trivial.hpp>

#include "tcs/dispatcher/idispatcher.h"
#include "tcs/router/irouter.h"
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
  DefaultDispatcher(Map* map, IRouter* router, Executor* executor,
                    ControllerPool* controller_pool)
      : map_{map},
        router_{router},
        executor_(executor),
        controller_pool_{controller_pool} {}
  void Dispatch() override;
  void WithdrawOrder(TransportOrder* order, bool immediate) override;
  void WithdrawOrder(Vehicle* vehicle, bool immediate) override;

 private:
  void DispatchTask();

  void Phase0CheckNewOrder();
  void Phase1FinishWithdrawal();
  void Phase2AssignNextDriveOrder();
  // NOTE: Not implemented because I aborted order sequence.
  void Phase3AssignSequenceSuccessor() { return; }
  // UNDONE: Not implemented. Figure out why would an order get reserved?
  void Phase4AssignReservedOrderPhase() { return; }
  void Phase5AssignFreeOrderPhase();
  void Phase6RechargeVehicle();
  void Phase7ParkVehicle();

  void FinishWithDrawal(Vehicle* vehicle);
  void CheckForNextOrder(Vehicle* vehicle);

  Map* map_;
  IRouter* router_;
  Executor* executor_;
  TransportOrderService* transport_order_service_;
  VehicleService* vehicle_service_;
  ControllerPool* controller_pool_;
};

}  // namespace tcs

#endif /* DEFAULT_DISPATCHER_H */
