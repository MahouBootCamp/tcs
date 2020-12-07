#ifndef MAP_RESOURCE_H
#define MAP_RESOURCE_H

#include "tcs/data/map_object.h"

namespace tcs {

class MapResource : public MapObject {
 public:
  MapResource(MapObjectID id, MapObjectType type) : MapObject(id, type) {}
  MapObjectRef get_block() const { return block_ref_; }
  void set_block(MapObjectRef block) { block_ref_ = block; }

 private:
  MapObjectRef block_ref_;
};

inline bool IsResource(MapObjectType type) {
  return type == MapObjectType::kPoint || type == MapObjectType::kPath ||
         type == MapObjectType::kLocation;
}

}  // namespace tcs

#endif /* MAP_RESOURCE_H */
