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
      FinishWithDrawal(vehicle);
    }
  }
}

void Phase1FinishWithdrawal::FinishWithDrawal(Vehicle* vehicle) {
  transport_order_service_->UpdateOrderState(
      vehicle->get_transport_order().value(), TransportOrderState::kFailed);
  vehicle_service_->UpdateVehicleProcessState(vehicle->get_id(),
                                              ProcessState::kIdle);
  vehicle_service_->UpdateVehicleTransportOrder(vehicle->get_id(),
                                                std::nullopt);
  router_->SelectRoute(vehicle, {});
}

}  // namespace tcs