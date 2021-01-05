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

enum class PointType {
  kHalt,    // Vehicles report and can halt for operation
  kPark,    // Vehicles report and can park here if idle
  kCharge,  // Vehicles report and can charge here if low power
  kReport   // Vehicles must report upon arriving a Report Point
};

// Points are positions on map
class Point : public MapResource {
 public:
  Point(MapObjectID id, Coordinate coordinate, PointType type)
      : MapResource{id, MapObjectType::kPoint},
        coordinate_{coordinate},
        point_type_{type} {}

  Coordinate GetCoordinate() { return coordinate_; }
  void SetCoordinate(Coordinate coordinate) { coordinate_ = coordinate; }

  PointType GetType() { return point_type_; }

  void AddInPath(MapObjectID path_id) { in_paths_.insert(path_id); }
  void RemoveInPath(MapObjectID path_id) { in_paths_.erase(path_id); }
  std::unordered_set<MapObjectID>& GetInPaths() { return in_paths_; }

  void AddOutPath(MapObjectID path_id) { out_paths_.insert(path_id); }
  void RemoveOutPath(MapObjectID path_id) { out_paths_.erase(path_id); }
  std::unordered_set<MapObjectID>& GetOutPaths() { return out_paths_; }

  MapObjectRef GetLinkedLocation() { return linked_location_; }
  void SetLinkedLocation(MapObjectRef location_ref) {
    linked_location_ = location_ref;
  }

  MapObjectRef GetOccupyingVehicle() { return occupying_vehicle_; }
  void SetOccupyingVehicle(MapObjectRef vehicle_ref) {
    occupying_vehicle_ = vehicle_ref;
  }

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
