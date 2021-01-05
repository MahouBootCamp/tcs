#include "tcs/util/controller_pool.h"

namespace tcs {

void ControllerPool::AttachVehicleController(MapObjectID vehicle,
                                             IVehicleAdapter* adapter) {
  auto vehicle_ptr = map_->GetVehicle(vehicle);
  if (!vehicle_ptr) throw std::invalid_argument("Vehicle not found");
  if (!adapter) adapter = new SimVehicleAdapter{};
  auto ptr =
      std::make_unique<DefaultController>(vehicle_ptr, adapter, scheduler_);
  controller_pool_.insert(std::make_pair(vehicle, std::move(ptr)));
}

void ControllerPool::DetachVehicleController(MapObjectID vehicle) { return; }

IVehicleController* ControllerPool::GetController(MapObjectID vehicle) {
  auto itr = controller_pool_.find(vehicle);
  if (itr == controller_pool_.end()) return nullptr;
  return itr->second.get();
}

}  // namespace tcs