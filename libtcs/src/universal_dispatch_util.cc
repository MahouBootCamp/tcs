#include "tcs/dispatcher/universal_dispatch_util.h"

namespace tcs {

bool UniversalDispatchUtil::CanBypassDriveOrder(DriveOrder &drive_order,
                                                const Vehicle *vehicle) {
  return drive_order.GetRoute()->GetSteps().back().destination->GetID() ==
             vehicle_service_->ReadVehicleCurrentPosition(vehicle->GetID())
                 .value()  // Destination point is current point
         && drive_order.GetDestination().operation ==
                kNoOperation;  // No operation to do
}

void UniversalDispatchUtil::AssignOrder(
    const Vehicle *vehicle, const TransportOrder *order,
    std::optional<std::vector<DriveOrder> > drive_orders) {
  auto vehicle_id = vehicle->GetID();
  auto order_id = order->GetID();
  auto cuurent_order = vehicle_service_->ReadVehicleTransportOrder(vehicle_id);
  if (vehicle_service_->ReadVehicleTransportOrder(vehicle_id).has_value()) {
    // If has dispensable order, abort it and reserve new order to wait for
    // abortion.
    if (transport_order_service_->GetTransportOrder(cuurent_order.value())
            ->GetDispensable()) {
      BOOST_LOG_TRIVIAL(debug)
          << "Reserving order " << order_id << " to vehicle " << vehicle_id;
      auto controller = controller_pool_->GetController(vehicle_id);
      controller->AbortDriveOrder(false);
      reserve_order_pool_->AddReservation(order_id, vehicle_id);
    } else {
      throw std::runtime_error("Assigning order to busy vehicle!");
    }
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
    transport_order_service_->UpdateOrderVehicleAndDriveOrder(
        order_id, vehicle_id, std::move(drive_orders));
    router_->SelectRoute(
        vehicle, transport_order_service_->ReadOrderDriveOrders(order_id));

    // Check first drive order
    auto first_drive_order =
        transport_order_service_->ReadOrderDriveOrders(order_id).front();
    if (CanBypassDriveOrder(first_drive_order, vehicle)) {
      vehicle_service_->UpdateVehicleProcessState(vehicle_id,
                                                  ProcessState::kAwaitingOrder);
    } else {
      controller_pool_->GetController(vehicle_id)
          ->SetDriveOrder(first_drive_order);
    }
  }
}

void UniversalDispatchUtil::AbortOrderByVehicle(const Vehicle *vehicle) {
  auto order_ref =
      vehicle_service_->ReadVehicleTransportOrder(vehicle->GetID());
  if (order_ref.has_value()) {
    auto order = transport_order_service_->GetTransportOrder(order_ref.value());
    AbortOrder(vehicle, order);
  } else {
    vehicle_service_->UpdateVehicleProcessState(vehicle->GetID(),
                                                ProcessState::kIdle);
  }
}

void UniversalDispatchUtil::AbortOrderByOrder(const TransportOrder *order) {
  auto vehicle_ref = transport_order_service_->ReadOrderVehicle(order->GetID());
  if (vehicle_ref.has_value()) {
    // Abort order
    auto vehicle = vehicle_service_->GetVehicle(vehicle_ref.value());
    AbortOrder(vehicle, order);
  } else {
    // No vehicle processing it, so just mark it failed.
    auto order_id = order->GetID();
    if (!IsFinalState(transport_order_service_->ReadOrderState(order_id))) {
      transport_order_service_->UpdateOrderState(order_id,
                                                 TransportOrderState::kFailed);
      reserve_order_pool_->RemoveReservationByOrder(order_id);
    }
  }
}

void UniversalDispatchUtil::AbortOrder(const Vehicle *vehicle,
                                       const TransportOrder *order) {
  auto current_order_state =
      transport_order_service_->ReadOrderState(order->GetID());
  if (!IsFinalState(current_order_state) &&
      current_order_state != TransportOrderState::kWithdraw) {
    transport_order_service_->UpdateOrderState(order->GetID(),
                                               TransportOrderState::kWithdraw);
  }

  auto controller = controller_pool_->GetController(vehicle->GetID());
  controller->AbortDriveOrder(false);
}

}  // namespace tcs