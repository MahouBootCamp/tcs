#ifndef MAP_SERVICE_H
#define MAP_SERVICE_H

#include "tcs/data/vehicle.h"
#include "tcs/util/map.h"

namespace tcs {

// TODO: Add multiprocess protection
class MapService {
 public:
  MapService(Map* map) : map_{map} {}

  const Map* GetMap() const { return map_; }

  const MapResource* GetResource(MapObjectID resource_id) const {
    return map_->GetResource(resource_id);
  }

  const Point* GetPoint(MapObjectID point_id) const {
    return map_->GetPoint(point_id);
  }

  const Path* GetPath(MapObjectID path_id) const {
    return map_->GetPath(path_id);
  }

  const Location* GetLocation(MapObjectID location_id) const {
    return map_->GetLocation(location_id);
  }

  const Block* GetBlock(MapObjectID block_id) const {
    return map_->GetBlock(block_id);
  }

  // Vehicle* GetVehicle(MapObjectID vehicle_id) {
  //   return map_->GetVehicle(vehicle_id);
  // }

  std::unordered_set<const MapResource*> GetResourcesByID(
      const std::unordered_set<MapObjectID>& id_set) const {
    return map_->GetResourcesByID(id_set);
  }

  std::unordered_set<const MapResource*> GetAllResources() const {
    return map_->GetAllResources();
  }

  std::unordered_set<const Point*> GetAllPoints() const {
    return map_->GetAllPoints();
  }

  std::unordered_set<const Path*> GetAllPaths() const {
    return map_->GetAllPaths();
  }

  std::unordered_set<const Location*> GetAllLocations() const {
    return map_->GetAllLocations();
  }

  std::unordered_set<const Block*> GetAllBlocks() const {
    return map_->GetAllBlocks();
  }

  // std::unordered_set<Vehicle*> GetAllVehicles() {
  //   return map_->GetAllVehicles();
  // }

  SpecialLocation* GetChargeLocation() const {
    return map_->GetChargeLocation();
  }
  SpecialLocation* GetParkLocation() const { return map_->GetParkLocation(); }

  // void UpdatePointOccupyingVehicle(MapObjectID point_id,
  //                                   MapObjectRef vehicle_ref);

 private:
  Map* map_;
};

}  // namespace tcs

#endif /* MAP_SERVICE_H */
