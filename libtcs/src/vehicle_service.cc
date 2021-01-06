#include "tcs/service/vehicle_service.h"

namespace tcs {

void VehicleService::UpdateVehicleState(MapObjectID vehicle_id,
                                        VehicleState state) {
  auto vehicle = GetVehicle(vehicle_id);

  auto old_state = vehicle->GetVehicleState();
  vehicle->SetVehicleState(state);
  vehicle_state_change_event_.Fire(std::move(old_state), std::move(state));
}

void VehicleService::UpdateVehicleProcessState(MapObjectID vehicle_id,
                                               ProcessState state) {
  auto vehicle = GetVehicle(vehicle_id);

  auto old_state = vehicle->GetProcessState();
  vehicle->SetProcessState(state);
  process_state_change_event_.Fire(std::move(old_state), std::move(state));
}

void VehicleService::UpdateVehicleTransportOrder(MapObjectID vehicle_id,
                                                 TransportOrderRef order_ref) {
  auto vehicle = GetVehicle(vehicle_id);
  vehicle->SetTransportOrder(order_ref);
}

void VehicleService::UpdateVehicleCurrentPosition(MapObjectID vehicle_id,
                                                  MapObjectRef point_ref) {
  auto vehicle = GetVehicle(vehicle_id);
  vehicle->SetCurrentPoint(point_ref);
}

void VehicleService::UpdateVehicleRouteProgressIndex(MapObjectID vehicle_id,
                                                     std::size_t index) {
  auto vehicle = GetVehicle(vehicle_id);
  vehicle->SetRouteProgressIndex(index);
}

void VehicleService::UpdateVehicleNeedCharge(MapObjectID vehicle_id,
                                             bool need_charge) {
  auto vehicle = GetVehicle(vehicle_id);
  vehicle->SetNeedCharge(need_charge);
}

void VehicleService::UpdateVehicleFinishCharge(MapObjectID vehicle_id,
                                               bool finish_charge) {
  auto vehicle = GetVehicle(vehicle_id);
  vehicle->SetFinishCharge(finish_charge);
}

}  // namespace tcs