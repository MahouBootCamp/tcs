#include "tcs/scheduler/default_scheduler.h"

namespace tcs {

void DefaultScheduler::Claim(
    IVehicleController *vehicle,
    std::vector<std::unordered_set<MapResource *> > resource_sequence) {
  claims_by_vehicle_.insert(std::make_pair(vehicle, resource_sequence));
}

void DefaultScheduler::Unclaim(IVehicleController *vehicle) {
  claims_by_vehicle_.erase(vehicle);
}

void DefaultScheduler::Allocate(IVehicleController *vehicle,
                                std::unordered_set<MapResource *> resources) {
  
}

void DefaultScheduler::AllocateNow(
    IVehicleController *vehicle, std::unordered_set<MapResource *> resources) {}

void DefaultScheduler::Free(IVehicleController *vehicle,
                            std::unordered_set<MapResource *> resources) {}

void DefaultScheduler::FreeAll(IVehicleController *vehicle) {}

}  // namespace tcs