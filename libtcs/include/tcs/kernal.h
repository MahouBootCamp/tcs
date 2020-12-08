#ifndef KERNAL_H
#define KERNAL_H

#include <boost/log/trivial.hpp>
#include <future>
#include <memory>
#include <mutex>

#include "tcs/idispatcher.h"
#include "tcs/ikernal.h"
#include "tcs/irouter.h"
#include "tcs/ischeduler.h"
#include "tcs/ivehicle_controller.h"
#include "tcs/util/controller_pool.h"
#include "tcs/util/map.h"
#include "tcs/util/order_pool.h"

namespace tcs {

class Kernal : public IKernal {
 public:
  Kernal() {}
  ~Kernal();

  KernalState get_state() override { return state_; }
  void Start() override;

  void Exit() override;

  void SetVehicleController() override;

  TransportOrderID AddTransportOrder(
      std::vector<DriveOrder> drive_orders,
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
};

}  // namespace tcs

#endif /* KERNAL_H */
