#ifndef LOCATION_H
#define LOCATION_H

#include <unordered_set>

#include "tcs/data/map_resource.h"

namespace tcs {

const std::string kParkOperation = "Park";
const std::string kChargeOperation = "Charge";

class Location : public MapResource {
 public:
  Location(MapObjectID id) : MapResource(id, MapObjectType::kLocation) {}

 private:
  std::unordered_set<std::string> operations_;
  std::unordered_set<MapObjectID> linked_points_;
};

}  // namespace tcs

#endif /* LOCATION_H */
