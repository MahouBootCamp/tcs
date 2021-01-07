#ifndef MAP_SERVICE_H
#define MAP_SERVICE_H

#include "tcs/data/vehicle.h"
#include "tcs/util/map.h"

namespace tcs {

// TODO: Add multiprocess protection
class MapService {
 public:
  MapService(Map* map) : map_{map} {}

  MapResource* GetResource(MapObjectID resource_id) {
    return map_->GetResource(resource_id);
  }

  Point* GetPoint(MapObjectID point_id) { return map_->GetPoint(point_id); }

  Path* GetPath(MapObjectID path_id) { return map_->GetPath(path_id); }

  Location* GetLocation(MapObjectID location_id) {
    return map_->GetLocation(location_id);
  }

  Block* GetBlock(MapObjectID block_id) { return map_->GetBlock(block_id); }

  // Vehicle* GetVehicle(MapObjectID vehicle_id) {
  //   return map_->GetVehicle(vehicle_id);
  // }

  std::unordered_set<MapResource*> GetResourcesByID(
      std::unordered_set<MapObjectID>& id_set) {
    return map_->GetResourcesByID(id_set);
  }

  std::unordered_set<MapResource*> GetAllResources() {
    return map_->GetAllResources();
  }

  std::unordered_set<Point*> GetAllPoints() { return map_->GetAllPoints(); }

  std::unordered_set<Path*> GetAllPaths() { return map_->GetAllPaths(); }

  std::unordered_set<Location*> GetAllLocations() {
    return map_->GetAllLocations();
  }

  std::unordered_set<Block*> GetAllBlocks() { return map_->GetAllBlocks(); }

  // std::unordered_set<Vehicle*> GetAllVehicles() {
  //   return map_->GetAllVehicles();
  // }

  SpecialLocation* GetChargeLocation() { return map_->GetChargeLocation(); }
  SpecialLocation* GetParkLocation() { return map_->GetParkLocation(); }

  void UpdatePointProcessingVehicle(MapObjectID point_id, MapObjectRef vehicle_ref);

 private:
  Map* map_;
};

}  // namespace tcs

#endif /* MAP_SERVICE_H */
