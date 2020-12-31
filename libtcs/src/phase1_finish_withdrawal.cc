#include "tcs/dispatcher/phase1_finish_withdrawal.h"

namespace tcs {

void Phase1FinishWithdrawal::Run() {
  BOOST_LOG_TRIVIAL(info) << "Phase 1: Finish Withdrawal...";
  auto vehicles = vehicle_service_->FilterVehiclesByProcessState(
      ProcessState::kAwaitingOrder);
  for (auto& vehicle : vehicles) {
    auto order = transport_order_service_->GetTransportOrder(
        vehicle->get_transport_order().value());
    if (order->get_state() == TransportOrderState::kWithdraw) {
      FinishWithDrawal(vehicle, order);
    }
  }
}

void Phase1FinishWithdrawal::FinishWithDrawal(Vehicle* vehicle,
                                              TransportOrder* order) {
  transport_order_service_->UpdateOrderState(
      vehicle->get_transport_order().value(), TransportOrderState::kFailed);
  vehicle_service_->UpdateVehicleProcessState(vehicle->get_id(),
                                              ProcessState::kIdle);
  vehicle_service_->UpdateVehicleTransportOrder(vehicle->get_id(),
                                                std::nullopt);
  router_->SelectRoute(vehicle, {});

  // If charge / park order, cancel reservation of charge point
  if (order->get_drive_orders().back().get_destination().site ==
      map_service_->GetChargeLocation()->get_id()) {
    map_service_->GetChargeLocation()->ReleasePoint(order->get_drive_orders()
                                                        .back()
                                                        .get_route()
                                                        ->get_steps()
                                                        .back()
                                                        .destination->get_id());
  }

  if (order->get_drive_orders().back().get_destination().site ==
      map_service_->GetParkLocation()->get_id()) {
    map_service_->GetParkLocation()->ReleasePoint(order->get_drive_orders()
                                                      .back()
                                                      .get_route()
                                                      ->get_steps()
                                                      .back()
                                                      .destination->get_id());
  }
}

}  // namespace tcs