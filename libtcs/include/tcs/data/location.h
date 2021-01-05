#ifndef LOCATION_H
#define LOCATION_H

#include <unordered_set>

#include "tcs/data/map_resource.h"

namespace tcs {

const std::string kNoOperation = "";
const std::string kParkOperation = "Park";
const std::string kChargeOperation = "Charge";

// A Location is a set of points where AGVs can execute a set of operations
class Location : public MapResource {
 public:
  Location(MapObjectID id, std::unordered_set<std::string> operations,
           std::unordered_set<MapObjectID> linked_points)
      : MapResource{id, MapObjectType::kLocation},
        operations_{operations},
        linked_points_{linked_points} {}

  void AddOperation(std::string operation) { operations_.insert(operation); }
  void RemoveOperation(std::string operation) { operations_.erase(operation); }
  std::unordered_set<std::string>& GetOperations() { return operations_; }

  void AddLinkedPoint(MapObjectID point_id) { linked_points_.insert(point_id); }
  void RemoveLinkedPoint(MapObjectID point_id) {
    linked_points_.erase(point_id);
  }
  std::unordered_set<MapObjectID>& GetLinkedPoints() {
    return linked_points_;
  }

 protected:
  std::unordered_set<std::string> operations_;
  std::unordered_set<MapObjectID> linked_points_;
};

class SpecialLocation : public Location {
 public:
  SpecialLocation(MapObjectID id, std::string operation,
                  std::unordered_set<MapObjectID> linked_points)
      : Location{id, {operation}, linked_points} {}

  void ReservePoint(MapObjectID point_id) {
    reserved_points_.insert(point_id);
    linked_points_.erase(point_id);
  }

  void ReleasePoint(MapObjectID point_id) {
    reserved_points_.erase(point_id);
    linked_points_.insert(point_id);
  }

 private:
  std::unordered_set<MapObjectID> reserved_points_;
};

}  // namespace tcs

#endif /* LOCATION_H */
