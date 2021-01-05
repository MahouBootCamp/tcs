#include "tcs/dispatcher/universal_dispatch_util.h"

namespace tcs {

bool UniversalDispatchUtil::CanBypassDriveOrder(DriveOrder &drive_order,
                                                Vehicle *vehicle) {
  return drive_order.get_route()->get_steps().back().destination->get_id() ==
             vehicle->get_current_point()
                 .value()  // Destination point is current point
         && drive_order.get_destination().operation ==
                kNoOperation;  // No operation to do
}

void UniversalDispatchUtil::AssignOrder(
    Vehicle *vehicle, TransportOrder *order,
    std::optional<std::vector<DriveOrder> > drive_orders) {
  auto vehicle_id = vehicle->get_id();
  auto order_id = order->get_id();
  if (vehicle->get_transport_order().has_value()) {
    // If has dispensable order, abort it and reserve new order to wait for
    // abortion.
    BOOST_LOG_TRIVIAL(debug)
        << "Reserving order " << order_id << " to vehicle " << vehicle_id;
    auto controller = controller_pool_->GetController(vehicle_id);
    // UNDONE: Abort order
    // controller->AbortDriveOrder(false);

    reserve_order_pool_->AddReservation(order_id, vehicle_id);
  } else {
    // Assign it directly
    BOOST_LOG_TRIVIAL(debug)
        << "Assigning order " << order_id << " to vehicle " << vehicle_id;
    reserve_order_pool_->RemoveReservationByOrder(order_id);
    vehicle_service_->UpdateVehicleProcessState(vehicle_id,
                                                ProcessState::kProcessingOrder);
    transport_order_service_->UpdateOrderState(
        order_id, TransportOrderState::kBeingProcessed);
    vehicle_service_->UpdateVehicleTransportOrder(vehicle_id, order_id);
    transport_order_service_->SetTransportOrderVehicleAndDriveOrder(
        order_id, vehicle_id, std::move(drive_orders));
    router_->SelectRoute(vehicle, order->get_drive_orders());

    // Check first drive order
    auto &first_drive_order = order->get_drive_orders().front();
    if (CanBypassDriveOrder(first_drive_order, vehicle)) {
      vehicle_service_->UpdateVehicleProcessState(vehicle_id,
                                                  ProcessState::kAwaitingOrder);
    } else {
      controller_pool_->GetController(vehicle_id)
          ->SetDriveOrder(first_drive_order);
    }
  }
}

void UniversalDispatchUtil::AbortOrderByVehicle(Vehicle *vehicle) {
  auto order_ref = vehicle->get_transport_order();
  if (order_ref.has_value()) {
    auto order = transport_order_service_->GetTransportOrder(order_ref.value());
    AbortOrder(vehicle, order);
  } else {
    vehicle_service_->UpdateVehicleProcessState(vehicle->get_id(),
                                                ProcessState::kIdle);
  }
}

void UniversalDispatchUtil::AbortOrderByOrder(TransportOrder *order) {
  auto vehicle_ref = order->get_vehicle();
  if (vehicle_ref.has_value()) {
    // Abort order
    auto vehicle = vehicle_service_->GetVehicle(vehicle_ref.value());
    AbortOrder(vehicle, order);
  } else {
    // No vehicle processing it, so just mark it failed.
    if (!IsFinalState(order->get_state())) {
      transport_order_service_->UpdateOrderState(order->get_id(),
                                                 TransportOrderState::kFailed);
      reserve_order_pool_->RemoveReservationByOrder(order->get_id());
    }
  }
}

void UniversalDispatchUtil::AbortOrder(Vehicle *vehicle,
                                       TransportOrder *order) {
  if (!IsFinalState(order->get_state()) &&
      order->get_state() != TransportOrderState::kWithdraw) {
    transport_order_service_->UpdateOrderState(order->get_id(),
                                               TransportOrderState::kWithdraw);
  }

  auto controller = controller_pool_->GetController(vehicle->get_id());
  controller->AbortDriveOrder(false);
}

}  // namespace tcs