#include "tcs/dispatcher/phase2_assign_next_drive_order.h"

namespace tcs {

void Phase2AssignNextDriveOrder::Run() {
  BOOST_LOG_TRIVIAL(info) << "Phase 2: Assign Next Drive Order...";
  auto vehicles = vehicle_service_->FilterVehiclesByProcessState(
      ProcessState::kAwaitingOrder);
  for (auto& vehicle : vehicles) {
    CheckForNextOrder(vehicle);
  }
}

void Phase2AssignNextDriveOrder::CheckForNextOrder(Vehicle* vehicle) {
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

    // If charge / park order, cancel reservation of charge point
    // if (order->get_drive_orders().back().get_destination().site ==
    //     map_service_->GetChargeLocation()->get_id()) {
    //   map_service_->GetChargeLocation()->ReleasePoint(
    //       order->get_drive_orders()
    //           .back()
    //           .get_route()
    //           ->get_steps()
    //           .back()
    //           .destination->get_id());
    // }

    // if (order->get_drive_orders().back().get_destination().site ==
    //     map_service_->GetParkLocation()->get_id()) {
    //   map_service_->GetParkLocation()->ReleasePoint(order->get_drive_orders()
    //                                                     .back()
    //                                                     .get_route()
    //                                                     ->get_steps()
    //                                                     .back()
    //                                                     .destination->get_id());
    // }

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
    if (CanBypassDriveOrder(drive_order, vehicle)) {
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