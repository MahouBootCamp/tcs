#ifndef MAP_SERVICE_H
#define MAP_SERVICE_H

#include <mutex>

#include "tcs/data/vehicle.h"
#include "tcs/util/map.h"

namespace tcs {

class MapService {
 public:
  MapService(Map* map, std::recursive_mutex& global_mutex)
      : map_{map}, global_mutex_{global_mutex} {}

  const Map* GetMap() const;

  const MapResource* GetResource(MapObjectID resource_id) const;

  const Point* GetPoint(MapObjectID point_id) const;

  const Path* GetPath(MapObjectID path_id) const;

  const Location* GetLocation(MapObjectID location_id) const;

  const Block* GetBlock(MapObjectID block_id) const;

  std::unordered_set<const MapResource*> GetResourcesByID(
      const std::unordered_set<MapObjectID>& id_set) const;

  std::unordered_set<const MapResource*> GetAllResources() const;

  std::unordered_set<const Point*> GetAllPoints() const;

  std::unordered_set<const Path*> GetAllPaths() const;

  std::unordered_set<const Location*> GetAllLocations() const;

  std::unordered_set<const Block*> GetAllBlocks() const;

  SpecialLocation* GetChargeLocation() const {
    return map_->GetChargeLocation();
  }
  SpecialLocation* GetParkLocation() const { return map_->GetParkLocation(); }

 private:
  Map* map_;
  std::recursive_mutex& global_mutex_;
};

}  // namespace tcs

#endif /* MAP_SERVICE_H */
