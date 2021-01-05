#include "tcs/dispatcher/phase1_finish_withdrawal.h"

namespace tcs {

void Phase1FinishWithdrawal::Run() {
  BOOST_LOG_TRIVIAL(info) << "Phase 1: Finish Withdrawal...";
  auto vehicles = vehicle_service_->FilterVehiclesByProcessState(
      ProcessState::kAwaitingOrder);
  for (auto& vehicle : vehicles) {
    auto order = transport_order_service_->GetTransportOrder(
        vehicle->GetTransportOrder().value());
    if (order->GetState() == TransportOrderState::kWithdraw) {
      FinishWithDrawal(vehicle, order);
    }
  }
}

void Phase1FinishWithdrawal::FinishWithDrawal(Vehicle* vehicle,
                                              TransportOrder* order) {
  transport_order_service_->UpdateOrderState(
      vehicle->GetTransportOrder().value(), TransportOrderState::kFailed);
  vehicle_service_->UpdateVehicleProcessState(vehicle->GetID(),
                                              ProcessState::kIdle);
  vehicle_service_->UpdateVehicleTransportOrder(vehicle->GetID(),
                                                std::nullopt);
  router_->SelectRoute(vehicle, {});

  // If charge / park order, cancel reservation of charge point
  // if (order->get_drive_orders().back().GetDestination().site ==
  //     map_service_->GetChargeLocation()->GetID()) {
  //   map_service_->GetChargeLocation()->ReleasePoint(order->get_drive_orders()
  //                                                       .back()
  //                                                       .GetRoute()
  //                                                       ->GetSteps()
  //                                                       .back()
  //                                                       .destination->GetID());
  // }

  // if (order->get_drive_orders().back().GetDestination().site ==
  //     map_service_->GetParkLocation()->GetID()) {
  //   map_service_->GetParkLocation()->ReleasePoint(order->get_drive_orders()
  //                                                     .back()
  //                                                     .GetRoute()
  //                                                     ->GetSteps()
  //                                                     .back()
  //                                                     .destination->GetID());
  // }
}

}  // namespace tcs