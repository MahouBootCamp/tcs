#include "tcs/service/vehicle_service.h"

namespace tcs {

IntegrationLevel VehicleService::ReadVehicleIntegrationLevel(
    MapObjectID vehicle_id) const {
  std::scoped_lock<std::recursive_mutex> lock{global_mutex_};
  return map_->GetVehicle(vehicle_id)->GetIntegrationLevel();
}

void VehicleService::UpdateVehicleIntegrationLevel(MapObjectID vehicle_id,
                                                   IntegrationLevel level) {
  std::scoped_lock<std::recursive_mutex> lock{global_mutex_};
  auto vehicle = map_->GetVehicle(vehicle_id);
  vehicle->SetIntegrationLevel(level);
}

VehicleState VehicleService::ReadVehicleState(MapObjectID vehicle_id) const {
  std::scoped_lock<std::recursive_mutex> lock{global_mutex_};
  return map_->GetVehicle(vehicle_id)->GetVehicleState();
}

void VehicleService::UpdateVehicleState(MapObjectID vehicle_id,
                                        VehicleState state) {
  std::scoped_lock<std::recursive_mutex> lock{global_mutex_};
  auto vehicle = map_->GetVehicle(vehicle_id);
  vehicle->SetVehicleState(state);
}

ProcessState VehicleService::ReadVehicleProcessState(
    MapObjectID vehicle_id) const {
  std::scoped_lock<std::recursive_mutex> lock{global_mutex_};
  return map_->GetVehicle(vehicle_id)->GetProcessState();
}

void VehicleService::UpdateVehicleProcessState(MapObjectID vehicle_id,
                                               ProcessState state) {
  std::scoped_lock<std::recursive_mutex> lock{global_mutex_};
  auto vehicle = map_->GetVehicle(vehicle_id);
  auto old_state = vehicle->GetProcessState();
  vehicle->SetProcessState(state);
  process_state_change_event_.Fire(vehicle, old_state, state);
}

TransportOrderRef VehicleService::ReadVehicleTransportOrder(
    MapObjectID vehicle_id) const {
  std::scoped_lock<std::recursive_mutex> lock{global_mutex_};
  return map_->GetVehicle(vehicle_id)->GetTransportOrder();
}

void VehicleService::UpdateVehicleTransportOrder(MapObjectID vehicle_id,
                                                 TransportOrderRef order_ref) {
  std::scoped_lock<std::recursive_mutex> lock{global_mutex_};
  auto vehicle = map_->GetVehicle(vehicle_id);
  vehicle->SetTransportOrder(order_ref);
}

MapObjectRef VehicleService::ReadVehicleCurrentPosition(
    MapObjectID vehicle_id) const {
  std::scoped_lock<std::recursive_mutex> lock{global_mutex_};
  return map_->GetVehicle(vehicle_id)->GetCurrentPoint();
}

void VehicleService::UpdateVehicleCurrentPosition(MapObjectID vehicle_id,
                                                  MapObjectRef point_ref) {
  std::scoped_lock<std::recursive_mutex> lock{global_mutex_};
  auto vehicle = map_->GetVehicle(vehicle_id);
  vehicle->SetCurrentPoint(point_ref);
}

std::size_t VehicleService::ReadVehicleRouteProgressIndex(
    MapObjectID vehicle_id) const {
  std::scoped_lock<std::recursive_mutex> lock{global_mutex_};
  return map_->GetVehicle(vehicle_id)->GetRouteProgressIndex();
}

void VehicleService::UpdateVehicleRouteProgressIndex(MapObjectID vehicle_id,
                                                     std::size_t index) {
  std::scoped_lock<std::recursive_mutex> lock{global_mutex_};
  auto vehicle = map_->GetVehicle(vehicle_id);
  vehicle->SetRouteProgressIndex(index);
}

bool VehicleService::ReadVehicleNeedCharge(MapObjectID vehicle_id) const {
  std::scoped_lock<std::recursive_mutex> lock{global_mutex_};
  return map_->GetVehicle(vehicle_id)->GetNeedCharge();
}

void VehicleService::UpdateVehicleNeedCharge(MapObjectID vehicle_id,
                                             bool need_charge) {
  std::scoped_lock<std::recursive_mutex> lock{global_mutex_};
  auto vehicle = map_->GetVehicle(vehicle_id);
  vehicle->SetNeedCharge(need_charge);
  if (need_charge) VehicleNeedChangeEvent().Fire(vehicle);
}

bool VehicleService::ReadVehicleFinishCharge(MapObjectID vehicle_id) const {
  std::scoped_lock<std::recursive_mutex> lock{global_mutex_};
  return map_->GetVehicle(vehicle_id)->GetFinishCharge();
}

void VehicleService::UpdateVehicleFinishCharge(MapObjectID vehicle_id,
                                               bool finish_charge) {
  std::scoped_lock<std::recursive_mutex> lock{global_mutex_};
  auto vehicle = map_->GetVehicle(vehicle_id);
  vehicle->SetFinishCharge(finish_charge);
}

}  // namespace tcs