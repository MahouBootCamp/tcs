#ifndef KERNAL_H
#define KERNAL_H

#include <boost/log/trivial.hpp>
#include <future>
#include <memory>
#include <mutex>

#include "tcs/dispatcher/default_dispatcher.h"
#include "tcs/dispatcher/idispatcher.h"
#include "tcs/ikernal.h"
#include "tcs/router/default_router.h"
#include "tcs/router/irouter.h"
#include "tcs/scheduler/default_scheduler.h"
#include "tcs/scheduler/ischeduler.h"
#include "tcs/service/map_service.h"
#include "tcs/service/transport_order_service.h"
#include "tcs/service/vehicle_service.h"
#include "tcs/util/controller_pool.h"
#include "tcs/util/executor.h"
#include "tcs/util/map.h"
#include "tcs/util/order_pool.h"
#include "tcs/vehicle/ivehicle_controller.h"

namespace tcs {

const std::size_t kExecutorThreads = 1;

class Kernal : public IKernal {
 public:
  Kernal(Map* map)
      : executor_{std::make_unique<Executor>(kExecutorThreads)},
        map_{map},
        order_pool_{std::make_unique<OrderPool>(map)},
        map_service_{std::make_unique<MapService>(map)},
        vehicle_service_{std::make_unique<VehicleService>(map)},
        transport_order_service_{
            std::make_unique<TransportOrderService>(order_pool_.get())},
        router_{std::make_unique<DefaultRouter>(map_service_.get())},
        scheduler_{std::make_unique<DefaultScheduler>(executor_.get(),
                                                      map_service_.get())},
        controller_pool_{std::make_unique<ControllerPool>(
            map, vehicle_service_.get(), scheduler_.get())},
        dispatcher_{std::make_unique<DefaultDispatcher>(
            executor_.get(), order_pool_.get(), map_service_.get(),
            vehicle_service_.get(), transport_order_service_.get(),
            router_.get(), controller_pool_.get())} {}
  ~Kernal();

  KernalState GetState() override { return state_; }

  void Start() override;

  void Exit() override;

  void EnableVehicle() override;

  TransportOrderID AddTransportOrder(
      std::vector<Destination> destinations,
      std::unordered_set<TransportOrderID> dependencies = {}) override;

  void WithdrawTransportOrder(TransportOrderID id, bool immediate) override;

  // NOTE: Following functions are for test only
  Executor* GetExecutor() { return executor_.get(); }
  Map* GetMap() { return map_.get(); }
  OrderPool* GetOrderPool() { return order_pool_.get(); }
  MapService* GetMapService() { return map_service_.get(); }
  VehicleService* GetVehicleService() { return vehicle_service_.get(); }
  TransportOrderService* GetTransportOrderService() {
    return transport_order_service_.get();
  }
  IRouter* GetRouter() { return router_.get(); }
  IScheduler* GetScheduler() { return scheduler_.get(); }
  ControllerPool* GetControllerPool() { return controller_pool_.get(); }
  IDispatcher* GetDispathcer() { return dispatcher_.get(); }

 private:
  std::recursive_mutex global_mutex_;

  std::unique_ptr<Executor> executor_;
  std::unique_ptr<Map> map_;
  std::unique_ptr<OrderPool> order_pool_;
  std::unique_ptr<MapService> map_service_;
  std::unique_ptr<VehicleService> vehicle_service_;
  std::unique_ptr<TransportOrderService> transport_order_service_;
  std::unique_ptr<IRouter> router_;
  std::unique_ptr<IScheduler> scheduler_;
  std::unique_ptr<ControllerPool> controller_pool_;
  std::unique_ptr<IDispatcher> dispatcher_;

  std::promise<void> on_quit_;
  std::future<void> quit_fut_;
  std::thread monitor_;
  KernalState state_ = KernalState::kIdle;
};

}  // namespace tcs

#endif /* KERNAL_H */
