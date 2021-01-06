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
  auto vehicle_id = vehicle->GetID();
  auto order_id = vehicle->GetTransportOrder().value();
  transport_order_service_->UpdateOrderNextDriveOrder(order_id);
  auto order = transport_order_service_->GetTransportOrder(order_id);
  // Check if transport order is finished
  if (order->GetProgressIndex() == order->GetDriveOrders().size()) {
    BOOST_LOG_TRIVIAL(info)
        << "Vehicle " << vehicle_id << " finished transport order " << order_id;
    // Update order to kFinished
    transport_order_service_->UpdateOrderState(order_id,
                                               TransportOrderState::kFinished);

    SetOrderFinished(vehicle, order);

    // If charge / park order, cancel reservation of charge point
    // if (order->GetDriveOrders().back().GetDestination().site ==
    //     map_service_->GetChargeLocation()->GetID()) {
    //   map_service_->GetChargeLocation()->ReleasePoint(
    //       order->GetDriveOrders()
    //           .back()
    //           .GetRoute()
    //           ->GetSteps()
    //           .back()
    //           .destination->GetID());
    // }

    // if (order->GetDriveOrders().back().GetDestination().site ==
    //     map_service_->GetParkLocation()->GetID()) {
    //   map_service_->GetParkLocation()->ReleasePoint(order->GetDriveOrders()
    //                                                     .back()
    //                                                     .GetRoute()
    //                                                     ->GetSteps()
    //                                                     .back()
    //                                                     .destination->GetID());
    // }
  } else {
    BOOST_LOG_TRIVIAL(info)
        << "Assigning next drive order to vehicle " << vehicle_id;
    auto& drive_order = order->GetDriveOrders().at(order->GetProgressIndex());

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

void Phase2AssignNextDriveOrder::SetOrderFinished(Vehicle* vehicle,
                                                  TransportOrder* order) {
  auto vehicle_id = vehicle->GetID();
  auto order_id = order->GetID();

  transport_order_service_->UpdateOrderState(order_id,
                                             TransportOrderState::kFinished);
  vehicle_service_->UpdateVehicleProcessState(vehicle_id, ProcessState::kIdle);
  vehicle_service_->UpdateVehicleTransportOrder(vehicle_id, std::nullopt);
  router_->SelectRoute(vehicle, {});

  // Recheck TransportOrders dispathcable or not
  auto related_orders =
      transport_order_service_->FilterBy([&order_id](TransportOrder* order) {
        return order->GetDependencies().find(order_id) ==
               order->GetDependencies().end();
      });

  for (auto& related_order : related_orders) {
    if (!transport_order_service_->HasUnfinishedDependencies(
            related_order->GetID()))
      transport_order_service_->UpdateOrderState(
          related_order->GetID(), TransportOrderState::kDispatchable);
  }
}

}  // namespace tcs