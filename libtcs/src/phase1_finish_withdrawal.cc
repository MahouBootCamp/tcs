#include "tcs/dispatcher/phase1_finish_withdrawal.h"

namespace tcs {

void Phase1FinishWithdrawal::Run() {
  BOOST_LOG_TRIVIAL(info) << "Phase 1: Finish Withdrawal...";
  auto vehicles = vehicle_service_->FilterVehiclesByProcessState(
      ProcessState::kAwaitingOrder);
  for (auto& vehicle : vehicles) {
    auto order = transport_order_service_->GetTransportOrder(
        vehicle_service_->ReadVehicleTransportOrder(vehicle->GetID()).value());
    if (transport_order_service_->ReadOrderState(order->GetID()) ==
        TransportOrderState::kWithdraw) {
      FinishWithDrawal(vehicle, order);
    }
  }
}

void Phase1FinishWithdrawal::FinishWithDrawal(const Vehicle* vehicle,
                                              const TransportOrder* order) {
  transport_order_service_->UpdateOrderState(order->GetID(),
                                             TransportOrderState::kFailed);
  vehicle_service_->UpdateVehicleProcessState(vehicle->GetID(),
                                              ProcessState::kIdle);
  vehicle_service_->UpdateVehicleTransportOrder(vehicle->GetID(), std::nullopt);
  router_->SelectRoute(vehicle, {});

  // If charge / park order, cancel reservation of charge point
  // if (order->GetDriveOrders().back().GetDestination().site ==
  //     map_service_->GetChargeLocation()->GetID()) {
  //   map_service_->GetChargeLocation()->ReleasePoint(order->GetDriveOrders()
  //                                                       .back()
  //                                                       .GetRoute()
  //                                                       ->GetSteps()
  //                                                       .back()
  //                                                       .destination->GetID());
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
}

}  // namespace tcs