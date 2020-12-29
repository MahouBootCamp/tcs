#include "tcs/dispatcher/default_dispatcher.h"

namespace tcs {

void DefaultDispatcher::Dispatch() {
  BOOST_LOG_TRIVIAL(debug) << "Scheduling dispatch task...";
  executor_->Submit(&DefaultDispatcher::DispatchTask, this);
}

void DefaultDispatcher::WithdrawOrder(TransportOrder* order, bool immediate) {}

void DefaultDispatcher::WithdrawOrder(Vehicle* order, bool immediate) {}

void DefaultDispatcher::DispatchTask() {
  phase0_.Run();
  phase1_.Run();
  phase2_.Run();
  phase3_.Run();
  phase4_.Run();
  phase5_.Run();
  phase6_.Run();
}

}  // namespace tcs