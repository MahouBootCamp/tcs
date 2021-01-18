#include "tcs/dispatcher/phase4_assign_free_order.h"

namespace tcs {

void Phase4AssignFreeOrder::Run() {
  BOOST_LOG_TRIVIAL(info) << "Phase 4: Assign Free Order...";
  auto vehicles = FilterAvailableVehicles();
  if (vehicles.empty()) {
    BOOST_LOG_TRIVIAL(info) << "No vehicles available for dispatching, "
                               "skipping assign free order phase";
    return;
  }

  auto orders = FilterDispatchableTransportOrders();
  if (orders.empty()) {
    BOOST_LOG_TRIVIAL(info) << "No new orders need dispatching, "
                               "skipping assign free order phase";
    return;
  }

  if (vehicles.size() > orders.size()) {
    // Dispatch based on order
    std::unordered_set<const TransportOrder*> assigned_orders;
    for (auto& vehicle : vehicles) {
      TryAssignByVehicle(vehicle, orders, assigned_orders);
    }
  } else {
    // Dispatch based on vehicle
    std::unordered_set<const Vehicle*> assigned_vehicles;
    for (auto& order : orders) {
      TryAssignByOrder(order, vehicles, assigned_vehicles);
    }
  }
}

std::unordered_set<const Vehicle*>
Phase4AssignFreeOrder::FilterAvailableVehicles() {
  auto all_vehicles = vehicle_service_->GetAllVehicles();
  std::unordered_set<const Vehicle*> result;
  for (auto& vehicle : all_vehicles) {
    auto vehicle_id = vehicle->GetID();
    bool process_no_order = vehicle_service_->ReadVehicleProcessState(
                                vehicle_id) == ProcessState::kIdle &&
                            (vehicle_service_->ReadVehicleState(vehicle_id) ==
                                 VehicleState::kIdle ||
                             vehicle_service_->ReadVehicleState(vehicle_id) ==
                                 VehicleState::kCharging);

    bool process_dispensable_order =
        vehicle_service_->ReadVehicleProcessState(vehicle_id) ==
            ProcessState::kProcessingOrder &&
        transport_order_service_
            ->GetTransportOrder(
                vehicle_service_->ReadVehicleTransportOrder(vehicle_id).value())
            ->GetDispensable();

    if (vehicle_service_->ReadVehicleIntegrationLevel(vehicle_id) ==
            IntegrationLevel::kUtilized &&  // Utilized
        vehicle_service_->ReadVehicleCurrentPosition(vehicle_id)
            .has_value() &&  // Know position
        !vehicle_service_->ReadVehicleNeedCharge(
            vehicle_id) &&  // Do not need charge
        (process_no_order || process_dispensable_order) &&
        reserve_order_pool_->GetReservationsByVehicle(vehicle->GetID())
            .empty())  // Not reserved for order
      result.insert(vehicle);
  }
  return result;
}

std::unordered_set<const TransportOrder*>
Phase4AssignFreeOrder::FilterDispatchableTransportOrders() {
  std::unordered_set<const TransportOrder*> result;
  auto dispatchable_orders = transport_order_service_->FilterOrdersByState(
      TransportOrderState::kDispatchable);
  for (auto& order : dispatchable_orders) {
    if (!reserve_order_pool_->IsReserved(order->GetID())) result.insert(order);
  }
  return result;
}

// void Phase4AssignFreeOrder::AssignOrder(AssignmentCandidate& candidate) {
//   auto vehicle_id = candidate.vehicle->GetID();
//   auto order_id = candidate.transport_order->GetID();
//   if (candidate.vehicle->GetTransportOrder().has_value()) {
//     // If has dispensable order, abort it and reserve new order to wait for
//     // abortion.
//     BOOST_LOG_TRIVIAL(debug)
//         << "Reserving order " << order_id << " to vehicle " << vehicle_id;
//     auto controller = controller_pool_->GetController(vehicle_id);
//
//     // controller->AbortDriveOrder(false);
//     reserve_order_pool_->AddReservation(order_id, vehicle_id);
//   } else {
//     // Assign it directly
//     BOOST_LOG_TRIVIAL(debug)
//         << "Assigning order " << order_id << " to vehicle " << vehicle_id;
//     reserve_order_pool_->RemoveReservationByOrder(order_id);
//     vehicle_service_->UpdateVehicleProcessState(vehicle_id,
//                                                 ProcessState::kProcessingOrder);
//     transport_order_service_->UpdateOrderState(
//         order_id, TransportOrderState::kBeingProcessed);
//     vehicle_service_->UpdateVehicleTransportOrder(vehicle_id, order_id);
//     transport_order_service_->UpdateOrderVehicleAndDriveOrder(
//         order_id, vehicle_id, std::move(candidate.drive_orders));
//     router_->SelectRoute(candidate.vehicle,
//                          candidate.transport_order->GetDriveOrders());

//     // Check first drive order
//     auto& first_drive_order =
//         candidate.transport_order->GetDriveOrders().front();
//     if (CanBypassDriveOrder(first_drive_order, candidate.vehicle)) {
//       vehicle_service_->UpdateVehicleProcessState(vehicle_id,
//                                                   ProcessState::kAwaitingOrder);
//     } else {
//       controller_pool_->GetController(vehicle_id)
//           ->SetDriveOrder(first_drive_order);
//     }
//   }
// }

void Phase4AssignFreeOrder::TryAssignByVehicle(
    const Vehicle* vehicle,
    const std::unordered_set<const TransportOrder*>& orders,
    std::unordered_set<const TransportOrder*>& assigned_orders) {
  BOOST_LOG_TRIVIAL(debug) << "Trying to find an order for vehicle "
                           << vehicle->GetID();
  std::vector<AssignmentCandidate> candidates;
  auto current_point = map_service_->GetPoint(
      vehicle_service_->ReadVehicleCurrentPosition(vehicle->GetID()).value());
  for (auto& order : orders) {
    if (assigned_orders.find(order) == assigned_orders.end()) {
      auto drive_orders = router_->GetRoute(current_point, order);
      if (drive_orders.has_value()) {
        candidates.push_back(
            AssignmentCandidate{vehicle, order, std::move(drive_orders)});
      } else
        continue;
    }
  }

  if (candidates.size() == 0) {
    BOOST_LOG_TRIVIAL(debug)
        << "No order routable for vehicle " << vehicle->GetID();
    return;
  } else {
    std::sort(candidates.begin(), candidates.end(),
              [](AssignmentCandidate& a1, AssignmentCandidate& a2) {
                return a1.total_cost < a2.total_cost;
              });
    auto& selected_candidate = candidates.front();
    universal_dispatch_util_->AssignOrder(
        selected_candidate.vehicle, selected_candidate.transport_order,
        std::move(selected_candidate.drive_orders));
    assigned_orders.insert(selected_candidate.transport_order);
  }
}

void Phase4AssignFreeOrder::TryAssignByOrder(
    const TransportOrder* order,
    const std::unordered_set<const Vehicle*>& vehicles,
    std::unordered_set<const Vehicle*>& assigned_vehicles) {
  BOOST_LOG_TRIVIAL(debug) << "Trying to find a vehicle for transport order "
                           << order->GetID();
  std::vector<AssignmentCandidate> candidates;
  for (auto& vehicle : vehicles) {
    auto current_point = map_service_->GetPoint(
        vehicle_service_->ReadVehicleCurrentPosition(vehicle->GetID()).value());
    if (assigned_vehicles.find(vehicle) == assigned_vehicles.end()) {
      auto drive_orders = router_->GetRoute(current_point, order);
      if (drive_orders.has_value()) {
        candidates.push_back(
            AssignmentCandidate{vehicle, order, std::move(drive_orders)});
      } else
        continue;
    }
  }

  if (candidates.size() == 0) {
    BOOST_LOG_TRIVIAL(debug)
        << "No vehicle routable for transport order " << order->GetID();
    return;
  } else {
    std::sort(candidates.begin(), candidates.end(),
              [](AssignmentCandidate& a1, AssignmentCandidate& a2) {
                return a1.total_cost < a2.total_cost;
              });
    auto& selected_candidate = candidates.front();
    universal_dispatch_util_->AssignOrder(
        selected_candidate.vehicle, selected_candidate.transport_order,
        std::move(selected_candidate.drive_orders));
    assigned_vehicles.insert(selected_candidate.vehicle);
  }
}

}  // namespace tcs