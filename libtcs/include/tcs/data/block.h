#ifndef BLOCK_H
#define BLOCK_H

#include <unordered_set>

#include "tcs/data/map_object.h"

namespace tcs {

class Block : public MapObject {
 public:
  Block(MapObjectID id) : MapObject(id, MapObjectType::kBlock) {}

 private:
  std::unordered_set<MapObjectID> resources_;
};

}  // namespace tcs

#endif /* BLOCK_H */
