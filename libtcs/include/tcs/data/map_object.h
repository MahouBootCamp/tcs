#ifndef MAP_OBJECT_H
#define MAP_OBJECT_H

#include <optional>

namespace tcs {

using MapObjectID = int;
using MapObjectRef = std::optional<int>;
enum class MapObjectType { kPoint, kPath, kLocation, kBlock, kVehicle };

// Base class for all map objects
class MapObject {
 public:
  MapObject(MapObjectID id, MapObjectType type) : id_{id}, type_{type} {}
  MapObjectID GetID() { return id_; }
  MapObjectType GetType() { return type_; }

 private:
  MapObjectID id_;
  MapObjectType type_;
};

}  // namespace tcs

#endif /* MAP_OBJECT_H */
