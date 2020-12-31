#ifndef MAP_SERVICE_H
#define MAP_SERVICE_H

#include "tcs/data/vehicle.h"
#include "tcs/util/map.h"

namespace tcs {

class MapService {
 public:
  MapService(Map* map) : map_{map} {}

  std::unordered_set<Vehicle*> GetAllVehicles() {
    return map_->GetAllVehicles();
  }

  Point* GetPoint(MapObjectID point_id) { return map_->get_point(point_id); }
  std::unordered_set<Point*> GetAllPoints() { return map_->GetAllPoints(); }

 private:
  Map* map_;
};

}  // namespace tcs

#endif /* MAP_SERVICE_H */
