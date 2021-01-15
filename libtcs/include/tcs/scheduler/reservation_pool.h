#ifndef RESERVATION_POOL_H
#define RESERVATION_POOL_H

#include <boost/log/trivial.hpp>
#include <unordered_map>
#include <unordered_set>

#include "tcs/data/map_resource.h"
#include "tcs/vehicle/ivehicle_controller.h"

namespace tcs {

struct ReservationEntry {
  const MapResource* resource;
  IVehicleController* vehicle;
  int count;  // Can only be 0 or 1

  ReservationEntry(const MapResource* resource)
      : resource{resource}, vehicle{nullptr}, count{0} {}
};

// Manages information of allocations
class ReservationPool {
 public:
  ReservationEntry& GetReservationEntry(const MapResource* resource);
  std::unordered_set<const MapResource*> GetAllocatedResources(
      IVehicleController* vehicle);
  bool ResourcesAvailable(IVehicleController* vehicle,
                          std::unordered_set<const MapResource*>& resources);
  void Free(IVehicleController* vehicle,
            std::unordered_set<const MapResource*>& resources);
  void FreeAll(IVehicleController* vehicle);
  std::unordered_set<const MapResource*> FilterCompletelyFreeResources(
      const std::unordered_set<const MapResource*>& resources);

 private:
  std::unordered_set<const MapResource*> GetFreeableResources(
      IVehicleController* vehicle,
      const std::unordered_set<const MapResource*>& resources);

  std::unordered_map<const MapResource*, ReservationEntry> reservation_pool_;
};

}  // namespace tcs

#endif /* RESERVATION_POOL_H */
