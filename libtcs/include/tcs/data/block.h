#ifndef BLOCK_H
#define BLOCK_H

#include <unordered_set>

#include "tcs/data/map_object.h"

namespace tcs {

class Block : public MapObject {
 public:
  Block(MapObjectID id, std::unordered_set<MapObjectID> resources)
      : MapObject(id, MapObjectType::kBlock), resources_(resources) {}

  void AddResource(MapObjectID resource_id) { resources_.insert(resource_id); }
  void RemoveResource(MapObjectID resource_id) {
    resources_.erase(resource_id);
  }
  const std::unordered_set<MapObjectID>& get_resources() { return resources_; }

 private:
  std::unordered_set<MapObjectID> resources_;
};

}  // namespace tcs

#endif /* BLOCK_H */
