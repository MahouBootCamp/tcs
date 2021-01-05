#ifndef MAP_RESOURCE_H
#define MAP_RESOURCE_H

#include <unordered_set>

#include "tcs/data/map_object.h"

namespace tcs {

// MapResource would be assigned to vehicles.
// A MapResource can only be assigned to one vehicle at the same time.
class MapResource : public MapObject {
 public:
  MapResource(MapObjectID id, MapObjectType type) : MapObject{id, type} {}
  std::unordered_set<MapObjectID>& GetBlocks() { return blocks_; }
  void AddBlock(MapObjectID block) { blocks_.insert(block); }
  void RemoveBlock(MapObjectID block) { blocks_.erase(block); }

 private:
  std::unordered_set<MapObjectID> blocks_;
};

inline bool IsResource(MapObjectType type) {
  return type == MapObjectType::kPoint || type == MapObjectType::kPath ||
         type == MapObjectType::kLocation;
}

}  // namespace tcs

#endif /* MAP_RESOURCE_H */
