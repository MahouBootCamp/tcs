#ifndef KERNAL_H
#define KERNAL_H

#include <boost/log/trivial.hpp>
#include <future>
#include <memory>
#include <mutex>

#include "tcs/dispatcher/idispatcher.h"
#include "tcs/ikernal.h"
#include "tcs/router/irouter.h"
#include "tcs/scheduler/ischeduler.h"
#include "tcs/util/controller_pool.h"
#include "tcs/util/executor.h"
#include "tcs/util/map.h"
#include "tcs/util/order_pool.h"
#include "tcs/vehicle/ivehicle_controller.h"

namespace tcs {

const std::size_t kExecutorThreads = 1;

class Kernal : public IKernal {
 public:
  Kernal() : executor_(kExecutorThreads) {}
  ~Kernal();

  KernalState get_state() override { return state_; }
  void Start() override;

  void Exit() override;

  void EnableVehicle() override;

  TransportOrderID AddTransportOrder(
      std::vector<Destination> destinations,
      std::unordered_set<TransportOrderID> dependencies = {}) override;

  void WithdrawTransportOrder(TransportOrderID id, bool immediate) override;

 private:
  std::promise<void> on_quit_;
  std::future<void> quit_fut_;
  std::thread monitor_;
  KernalState state_ = KernalState::kIdle;
  std::unique_ptr<Map> map_;
  std::unique_ptr<OrderPool> order_pool_;
  std::unique_ptr<ControllerPool> controller_pool_;
  std::unique_ptr<IRouter> router_;
  std::unique_ptr<IDispatcher> dispatcher_;
  std::unique_ptr<IScheduler> scheduler_;
  Executor executor_;
};

}  // namespace tcs

#endif /* KERNAL_H */
