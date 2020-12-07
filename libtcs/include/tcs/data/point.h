#ifndef POINT_H
#define POINT_H

#include <unordered_set>

#include "tcs/data/map_resource.h"

namespace tcs {

// Cartesian coordinate. Unit: milimeter.
struct Coordinate {
  double x;
  double y;
  double z;
};

enum class PointType { kHalt, kPark, kReport };

class Point : public MapResource {
 public:
  Point(MapObjectID id, Coordinate coordinate, PointType type)
      : MapResource(id, MapObjectType::kPoint),
        coordinate_(coordinate),
        point_type_(type) {}

 private:
  Coordinate coordinate_;
  PointType point_type_;
  std::unordered_set<MapObjectID> in_paths_;
  std::unordered_set<MapObjectID> out_paths_;
  MapObjectRef linked_location_;
  MapObjectRef occupying_vehicle_;
};

}  // namespace tcs

#endif /* POINT_H */
