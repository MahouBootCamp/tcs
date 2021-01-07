#include "tcs/dispatcher/default_dispatcher.h"

namespace tcs {

void DefaultDispatcher::Dispatch() {
  BOOST_LOG_TRIVIAL(debug) << "Scheduling dispatch task...";
  executor_->Submit(&DefaultDispatcher::DispatchTask, this);
}

void DefaultDispatcher::WithdrawOrder(TransportOrder* order, bool immediate) {
  if (!immediate) {
    BOOST_LOG_TRIVIAL(debug) << "Scheduling withdraw task...";
    executor_->Submit(&UniversalDispatchUtil::AbortOrderByOrder,
                      universal_dispatch_util_.get(), order);
  } else {
    // UNDONE
  }
}

void DefaultDispatcher::WithdrawOrder(Vehicle* vehicle, bool immediate) {
  if (!immediate) {
    BOOST_LOG_TRIVIAL(debug) << "Scheduling withdraw task...";
    executor_->Submit(&UniversalDispatchUtil::AbortOrderByVehicle,
                      universal_dispatch_util_.get(), vehicle);
  } else {
    // UNDONE
  }
}

void DefaultDispatcher::DispatchTask() {
  phase0_.Run();
  phase1_.Run();
  phase2_.Run();
  phase3_.Run();
  phase4_.Run();
  phase5_.Run();
  phase6_.Run();
}

void DefaultDispatcher::DispatchPeriodically() {}

void DefaultDispatcher::VehicleProcessStateChangeEventHandler(
    Vehicle* vehicle, ProcessState old_state, ProcessState new_state) {
  if (vehicle->GetIntegrationLevel() == IntegrationLevel::kUtilized) {
    if (old_state != new_state && (new_state == ProcessState::kAwaitingOrder ||
                                   new_state == ProcessState::kIdle))
      Dispatch();
  }
}

void DefaultDispatcher::VehicleNeedChargeEventHandler(Vehicle* vehicle) {
  if (vehicle->GetIntegrationLevel() == IntegrationLevel::kUtilized &&
      vehicle->GetNeedCharge()) {
    Dispatch();
  }
}

}  // namespace tcs