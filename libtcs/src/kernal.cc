#include "tcs/kernal.h"

namespace tcs {

Kernal::~Kernal() {
  if (state_ != KernalState::kExit) Exit();
  if (monitor_.joinable()) monitor_.join();
}

void Kernal::Start() {
  BOOST_LOG_TRIVIAL(info) << "Kernal: start operating...";

  {
    std::scoped_lock<std::recursive_mutex> lock{global_mutex_};
    state_ = KernalState::kOperating;
    quit_fut_ = on_quit_.get_future();
  }

  monitor_ = std::thread([this]() {
    // if (quit_fut_.valid()) quit_fut_.wait();
    while (quit_fut_.valid()) {
      if (quit_fut_.wait_for(std::chrono::seconds(kDispatchCycle)) ==
          std::future_status::timeout) {
        dispatcher_->Dispatch();
      } else
        break;
    }
  });
}

void Kernal::Exit() {
  BOOST_LOG_TRIVIAL(info) << "Kernal: exiting...";
  std::scoped_lock<std::recursive_mutex> lock{global_mutex_};
  state_ = KernalState::kExit;
  on_quit_.set_value();
}

void Kernal::EnableVehicle(MapObjectID vehicle, MapObjectID initial_position,
                           IVehicleAdapter* adapter) {
  std::scoped_lock<std::recursive_mutex> lock{global_mutex_};
  if (state_ != KernalState::kIdle)
    throw std::runtime_error("Cannot enable vehicle while kernal running!");
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

TransportOrderID Kernal::AddTransportOrder(
    std::vector<Destination> destinations,
    std::unordered_set<TransportOrderID> dependencies) {
  std::scoped_lock<std::recursive_mutex> lock{global_mutex_};
  if (state_ != KernalState::kOperating)
    throw std::runtime_error("Cannot add order while kernal is not operating!");

  auto order_id = order_pool_->AddOrder(destinations, dependencies);
  dispatcher_->Dispatch();
  return order_id;
}

void Kernal::WithdrawTransportOrder(TransportOrderID id) {
  std::scoped_lock<std::recursive_mutex> lock{global_mutex_};
  if (state_ != KernalState::kOperating)
    throw std::runtime_error(
        "Cannot withdraw order while kernal is not operating!");

  auto order = transport_order_service_->GetTransportOrder(id);
  if (!order) throw std::runtime_error("Order invalid!");
  dispatcher_->WithdrawOrder(order, false);
}

}  // namespace tcs