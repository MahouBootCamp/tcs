#include "tcs/kernel.h"

namespace tcs {

Kernel::~Kernel() {
  Exit();
  executor_->Exit();
  if (monitor_.joinable()) monitor_.join();
}

void Kernel::Start() {
  BOOST_LOG_TRIVIAL(info) << "Kernel: start operating...";

  {
    std::scoped_lock<std::recursive_mutex> lock{global_mutex_};
    state_ = KernelState::kOperating;
    // quit_fut_ = on_quit_.get_future();
  }

  monitor_ = std::thread([this]() {
    // if (quit_fut_.valid()) quit_fut_.wait();
    while (true) {
      std::this_thread::sleep_for(std::chrono::seconds(kDispatchCycle));
      bool exit = false;
      {
        std::scoped_lock<std::recursive_mutex> lock{global_mutex_};
        exit = state_ == KernelState::kExit;
      }
      if (exit) {
        BOOST_LOG_TRIVIAL(info) << "Exit periodical dispatch task";
        break;
      } else {
        BOOST_LOG_TRIVIAL(info) << "Dispatch periodically";
        dispatcher_->Dispatch();
      }
    }
  });
}

void Kernel::Exit() {
  {
    std::scoped_lock<std::recursive_mutex> lock{global_mutex_};
    if (state_ != KernelState::kOperating) return;
    BOOST_LOG_TRIVIAL(info) << "Kernel: exiting...";
    state_ = KernelState::kExit;
  }
}

void Kernel::EnableVehicle(MapObjectID vehicle, MapObjectID initial_position,
                           IVehicleAdapter* adapter) {
  std::scoped_lock<std::recursive_mutex> lock{global_mutex_};
  if (state_ != KernelState::kIdle)
    throw std::runtime_error("Cannot enable vehicle while kernel running!");
  if (controller_pool_->GetController(vehicle))
    throw std::runtime_error("Vehicle already enabled!");
  controller_pool_->AttachVehicleController(vehicle, adapter);
  auto controller = controller_pool_->GetController(vehicle);
  auto initial_point = map_service_->GetPoint(initial_position);
  if (!initial_point)
    throw std::invalid_argument("Vehicle initial position invalid!");
  controller->InitPosition(initial_point);
  vehicle_service_->UpdateVehicleIntegrationLevel(vehicle,
                                                  IntegrationLevel::kUtilized);
}

TransportOrderID Kernel::AddTransportOrder(
    std::vector<Destination> destinations,
    std::unordered_set<TransportOrderID> dependencies) {
  std::scoped_lock<std::recursive_mutex> lock{global_mutex_};
  if (state_ != KernelState::kOperating)
    throw std::runtime_error("Cannot add order while kernel is not operating!");

  auto order_id = order_pool_->AddOrder(destinations, dependencies);
  BOOST_LOG_TRIVIAL(debug) << "Dispatch due to new order";
  dispatcher_->Dispatch();
  return order_id;
}

void Kernel::WithdrawTransportOrder(TransportOrderID id) {
  std::scoped_lock<std::recursive_mutex> lock{global_mutex_};
  if (state_ != KernelState::kOperating)
    throw std::runtime_error(
        "Cannot withdraw order while kernel is not operating!");

  auto order = transport_order_service_->GetTransportOrder(id);
  if (!order) throw std::runtime_error("Order invalid!");
  dispatcher_->WithdrawOrder(order, false);
}

}  // namespace tcs