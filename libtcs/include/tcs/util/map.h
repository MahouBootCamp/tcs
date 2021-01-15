#ifndef MAP_H
#define MAP_H

#include <memory>
#include <unordered_map>

#include "tcs/data/block.h"
#include "tcs/data/location.h"
#include "tcs/data/map_object.h"
#include "tcs/data/map_resource.h"
#include "tcs/data/path.h"
#include "tcs/data/point.h"
#include "tcs/data/vehicle.h"

namespace tcs {

// A Map manages all MapObjcets
// REVIEW: Wrap it with a service class and add concurrent protection?
// TODO: add trigger events for dispatch.
class Map {
  friend class MapBuilder;

 public:
  MapResource* GetResource(MapObjectID id);
  Point* GetPoint(MapObjectID id);
  Path* GetPath(MapObjectID id);
  Location* GetLocation(MapObjectID id);
  Block* GetBlock(MapObjectID id);
  Vehicle* GetVehicle(MapObjectID id);

  std::unordered_set<const MapResource*> GetResourcesByID(
      const std::unordered_set<MapObjectID>& id_set);

  std::unordered_set<const MapResource*> GetAllResources();
  std::unordered_set<const Point*> GetAllPoints();
  std::unordered_set<const Path*> GetAllPaths();
  std::unordered_set<const Location*> GetAllLocations();
  std::unordered_set<const Block*> GetAllBlocks();
  std::unordered_set<const Vehicle*> GetAllVehicles();

  SpecialLocation* GetChargeLocation() { return charge_location_; }
  SpecialLocation* GetParkLocation() { return park_location_; }

  // REVIEW: Add functions that retrive a set of objects by a set of ids?

 private:
  // Map can only be constructed from its builder
  Map() = default;

  std::unordered_map<MapObjectID, std::unique_ptr<MapObject>> object_pool_;
  std::unordered_set<MapObjectID> point_ids_;
  std::unordered_set<MapObjectID> path_ids_;
  std::unordered_set<MapObjectID> location_ids_;
  std::unordered_set<MapObjectID> block_ids_;
  std::unordered_set<MapObjectID> vehicle_ids_;

  SpecialLocation* charge_location_;
  SpecialLocation* park_location_;
};

}  // namespace tcs

#endif /* MAP_H */
