#ifndef LOCATION_H
#define LOCATION_H

#include <unordered_set>

#include "tcs/data/map_resource.h"

namespace tcs {

const std::string kParkOperation = "Park";
const std::string kChargeOperation = "Charge";

class Location : public MapResource {
 public:
  Location(MapObjectID id, std::unordered_set<std::string> operations,
           std::unordered_set<MapObjectID> linked_points)
      : MapResource(id, MapObjectType::kLocation),
        operations_(operations),
        linked_points_(linked_points) {}

  void AddOperation(std::string operation) { operations_.insert(operation); }
  void RemoveOperation(std::string operation) { operations_.erase(operation); }
  const std::unordered_set<std::string>& get_operations() {
    return operations_;
  }

  void AddLinkedPoint(MapObjectID point_id) { linked_points_.insert(point_id); }
  void RemoveLinkedPoint(MapObjectID point_id) {
    linked_points_.erase(point_id);
  }
  const std::unordered_set<MapObjectID>& get_linked_points() {
    return linked_points_;
  }

 private:
  std::unordered_set<std::string> operations_;
  std::unordered_set<MapObjectID> linked_points_;
};

}  // namespace tcs

#endif /* LOCATION_H */
