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
class Map {
  friend class MapBuilder;

 public:
  MapResource* get_resource(MapObjectID id);
  Point* get_point(MapObjectID id);
  Path* get_path(MapObjectID id);
  Location* get_location(MapObjectID id);
  Block* get_block(MapObjectID id);
  Vehicle* get_vehicle(MapObjectID id);

  std::unordered_set<MapResource*> GetResources(
      std::unordered_set<MapObjectID>& id_set);

  std::unordered_set<MapResource*> GetAllResources();
  std::unordered_set<Point*> GetAllPoints();
  std::unordered_set<Path*> GetAllPaths();
  std::unordered_set<Location*> GetAllLocations();
  std::unordered_set<Block*> GetAllBlocks();
  std::unordered_set<Vehicle*> GetAllVehicles();

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
};

}  // namespace tcs

#endif /* MAP_H */
