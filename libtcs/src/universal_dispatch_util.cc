#include "tcs/dispatcher/universal_dispatch_util.h"

namespace tcs {

bool UniversalDispatchUtil::CanBypassDriveOrder(DriveOrder &drive_order,
                                                Vehicle *vehicle) {
  return drive_order.GetRoute()->GetSteps().back().destination->GetID() ==
             vehicle->GetCurrentPoint()
                 .value()  // Destination point is current point
         && drive_order.GetDestination().operation ==
                kNoOperation;  // No operation to do
}

void UniversalDispatchUtil::AssignOrder(
    Vehicle *vehicle, TransportOrder *order,
    std::optional<std::vector<DriveOrder> > drive_orders) {
  auto vehicle_id = vehicle->GetID();
  auto order_id = order->GetID();
  if (vehicle->GetTransportOrder().has_value()) {
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
  auto order_ref = vehicle->GetTransportOrder();
  if (order_ref.has_value()) {
    auto order = transport_order_service_->GetTransportOrder(order_ref.value());
    AbortOrder(vehicle, order);
  } else {
    vehicle_service_->UpdateVehicleProcessState(vehicle->GetID(),
                                                ProcessState::kIdle);
  }
}

void UniversalDispatchUtil::AbortOrderByOrder(TransportOrder *order) {
  auto vehicle_ref = order->GetVehicle();
  if (vehicle_ref.has_value()) {
    // Abort order
    auto vehicle = vehicle_service_->GetVehicle(vehicle_ref.value());
    AbortOrder(vehicle, order);
  } else {
    // No vehicle processing it, so just mark it failed.
    if (!IsFinalState(order->GetState())) {
      transport_order_service_->UpdateOrderState(order->GetID(),
                                                 TransportOrderState::kFailed);
      reserve_order_pool_->RemoveReservationByOrder(order->GetID());
    }
  }
}

void UniversalDispatchUtil::AbortOrder(Vehicle *vehicle,
                                       TransportOrder *order) {
  if (!IsFinalState(order->GetState()) &&
      order->GetState() != TransportOrderState::kWithdraw) {
    transport_order_service_->UpdateOrderState(order->GetID(),
                                               TransportOrderState::kWithdraw);
  }

  auto controller = controller_pool_->GetController(vehicle->GetID());
  controller->AbortDriveOrder(false);
}

}  // namespace tcs