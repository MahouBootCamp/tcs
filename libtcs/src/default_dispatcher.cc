#include "tcs/dispatcher/default_dispatcher.h"

namespace tcs {

void DefaultDispatcher::Dispatch() {
  BOOST_LOG_TRIVIAL(debug) << "Scheduling dispatch task...";
  executor_->Submit(&DefaultDispatcher::DispatchTask, this);
}

void DefaultDispatcher::WithdrawOrder(TransportOrder* order, bool immediate) {}

void DefaultDispatcher::WithdrawOrder(Vehicle* order, bool immediate) {}

void DefaultDispatcher::DispatchTask() {
  Phase0CheckNewOrder();
  Phase1FinishWithdrawal();
  Phase2AssignNextDriveOrder();
  Phase3AssignSequenceSuccessor();
  Phase4AssignReservedOrderPhase();
  Phase5AssignFreeOrderPhase();
  Phase6RechargeVehicle();
  Phase7ParkVehicle();
}

void DefaultDispatcher::Phase0CheckNewOrder() {
  auto orders =
      transport_order_service_->FilterOrdersByState(TransportOrderState::kRaw);
  for (auto& order : orders) {
    // Check routability
    if (!router_->ChechRoutability(order)) {
      transport_order_service_->UpdateOrderState(
          order->get_id(), TransportOrderState::kUnRoutable);
      continue;
    }

    // Ready for dispatch
    transport_order_service_->UpdateOrderState(order->get_id(),
                                               TransportOrderState::kActive);

    // Check Dependencies. Dispatch if dependencies finished.
    if (transport_order_service_->HasUnfinishedDependencies(order->get_id()))
      transport_order_service_->UpdateOrderState(
          order->get_id(), TransportOrderState::kDispatchable);
  }
}

void DefaultDispatcher::Phase1FinishWithdrawal() {
  auto vehicles = vehicle_service_->FilterVehiclesByProcessState(
      ProcessState::kAwaitingOrder);
  for (auto& vehicle : vehicles) {
    auto order = transport_order_service_->GetTransportOrder(
        vehicle->get_transport_order().value());
    if (order->get_state() == TransportOrderState::kWithdraw) {
      FinishWithDrawal(vehicle);
    }
  }
}

void DefaultDispatcher::Phase2AssignNextDriveOrder() {
  auto vehicles = vehicle_service_->FilterVehiclesByProcessState(
      ProcessState::kAwaitingOrder);
  for (auto& vehicle : vehicles) {
    CheckForNextOrder(vehicle);
  }
}

// void DefaultDispatcher::Phase4AssignReservedOrderPhase() {}

void DefaultDispatcher::Phase5AssignFreeOrderPhase() {}

void DefaultDispatcher::Phase6RechargeVehicle() {}

void DefaultDispatcher::Phase7ParkVehicle() {}

void DefaultDispatcher::FinishWithDrawal(Vehicle* vehicle) {
  transport_order_service_->UpdateOrderState(
      vehicle->get_transport_order().value(), TransportOrderState::kFailed);
  vehicle_service_->UpdateVehicleProcessState(vehicle->get_id(),
                                              ProcessState::kIdle);
  vehicle_service_->UpdateVehicleTransportOrder(vehicle->get_id(),
                                                std::nullopt);
  router_->SelectRoute(vehicle, {});
}

void DefaultDispatcher::CheckForNextOrder(Vehicle* vehicle) {
  auto vehicle_id = vehicle->get_id();
  auto order_id = vehicle->get_transport_order().value();
  transport_order_service_->SetTransportOrderNextDriveOrder(order_id);
  auto order = transport_order_service_->GetTransportOrder(order_id);
  // Check if transport order is finished
  if (order->get_progress_index() == order->get_drive_orders().size()) {
    BOOST_LOG_TRIVIAL(info)
        << "Vehicle " << vehicle_id << " finished transport order " << order_id;
    // Update order to kFinished
    transport_order_service_->UpdateOrderState(order_id,
                                               TransportOrderState::kFinished);
    // Update vehicle to kIdle. It would be dispatched in next phase
    vehicle_service_->UpdateVehicleProcessState(vehicle_id,
                                                ProcessState::kIdle);
    vehicle_service_->UpdateVehicleTransportOrder(vehicle_id, std::nullopt);
    router_->SelectRoute(vehicle, {});
    // Check orders dependent on this order if all dependencies finished. Set
    // them Dispatchable if true.
    transport_order_service_->UpdateOrderWithDependencyFinished(order_id);
  } else {
    BOOST_LOG_TRIVIAL(info)
        << "Assigning next drive order to vehicle " << vehicle_id;
    auto& drive_order =
        order->get_drive_orders().at(order->get_progress_index());

    // Check if the drive_order can bypass :no need to move and no special
    // operation
    if (drive_order.get_route()->get_steps().back().destination->get_id() ==
            vehicle->get_current_point().value() &&
        drive_order.get_destination().operation == kNoOperation) {
      // Bypass and check next driveorder
      vehicle_service_->UpdateVehicleProcessState(vehicle_id,
                                                  ProcessState::kAwaitingOrder);
      CheckForNextOrder(vehicle);
    } else {
      // Set controller next drive order
      controller_pool_->GetController(vehicle_id)->SetDriveOrder(drive_order);
      vehicle_service_->UpdateVehicleProcessState(
          vehicle_id, ProcessState::kProcessingOrder);
    }
  }
}

}  // namespace tcs