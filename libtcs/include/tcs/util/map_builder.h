#ifndef MAP_BUILDER_H
#define MAP_BUILDER_H

#include <stdexcept>
#include <vector>

#include "tcs/util/map.h"

namespace tcs {

// Map's builder
class MapBuilder {
 public:
  static MapBuilder& GetInstance() {
    static MapBuilder instance;
    return instance;
  }

  // Call after a build if you want to create a new map
  void Init();

  // Build a map
  Map* Build();

  Map* BuildFromFile(std::string file_path) {
    // UNDONE
    return nullptr;
  }

  void SaveToFile(std::string file_path, Map* map) {
    // UNDONE
    return;
  }

  MapObjectID AddPoint(Coordinate coordinate,
                       PointType type = PointType::kHalt);
  MapObjectID AddPath(MapObjectID source, MapObjectID destination,
                      double length);
  MapObjectID AddLocation(std::unordered_set<std::string> operations,
                          std::unordered_set<MapObjectID> linked_points = {});
  MapObjectID AddBlock(std::unordered_set<MapObjectID> resources = {});
  MapObjectID AddVehicle();

  void Link(MapObjectID point, MapObjectID location);
  void Unlink(MapObjectID point, MapObjectID location);
  void AddToBlock(MapObjectID resource, MapObjectID block);
  void RemoveFromBlock(MapObjectID resource, MapObjectID block);

  void RemovePoint(MapObjectID id);
  void RemovePath(MapObjectID id);
  void RemoveLocation(MapObjectID id);
  void RemoveBlock(MapObjectID id);
  void RemoveVehicle(MapObjectID id);

 private:
  MapBuilder() { Init(); }
  void AddPoint(MapObjectID id, Coordinate coordinate, PointType type);
  void AddPath(MapObjectID id, MapObjectID source, MapObjectID destination,
               double length);
  void AddLocation(MapObjectID id, std::unordered_set<std::string> operations,
                   std::unordered_set<MapObjectID> linked_points);
  void AddBlock(MapObjectID id, std::unordered_set<MapObjectID> resources);
  void AddVehicle(MapObjectID id);

  Map* map_ = nullptr;
  MapObjectID next_id_ = 0;
};

}  // namespace tcs

#endif /* MAP_BUILDER_H */
