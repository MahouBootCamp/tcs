#include "tcs/util/map.h"

namespace tcs {

MapResource* Map::GetResource(MapObjectID id) {
  auto itr = object_pool_.find(id);
  if (itr == object_pool_.end() || !IsResource(itr->second->GetType()))
    return nullptr;
  else
    return static_cast<MapResource*>(itr->second.get());
}

Point* Map::GetPoint(MapObjectID id) {
  return point_ids_.find(id) == point_ids_.end()
             ? nullptr
             : static_cast<Point*>(object_pool_[id].get());
}

Path* Map::GetPath(MapObjectID id) {
  return path_ids_.find(id) == path_ids_.end()
             ? nullptr
             : static_cast<Path*>(object_pool_[id].get());
}

Location* Map::GetLocation(MapObjectID id) {
  return location_ids_.find(id) == location_ids_.end()
             ? nullptr
             : static_cast<Location*>(object_pool_[id].get());
}

Block* Map::GetBlock(MapObjectID id) {
  return block_ids_.find(id) == block_ids_.end()
             ? nullptr
             : static_cast<Block*>(object_pool_[id].get());
}

Vehicle* Map::GetVehicle(MapObjectID id) {
  return vehicle_ids_.find(id) == vehicle_ids_.end()
             ? nullptr
             : static_cast<Vehicle*>(object_pool_[id].get());
}

std::unordered_set<MapResource*> Map::GetResources(
    std::unordered_set<MapObjectID>& id_set) {
  std::unordered_set<MapResource*> resources;
  for (auto& id : id_set) {
    auto resource = GetResource(id);
    if (resource) resources.insert(resource);
  }
  return resources;
}

std::unordered_set<MapResource*> Map::GetAllResources() {
  std::unordered_set<MapResource*> res;
  for (auto& id : point_ids_)
    res.insert(static_cast<MapResource*>(object_pool_.find(id)->second.get()));
  for (auto& id : path_ids_)
    res.insert(static_cast<MapResource*>(object_pool_.find(id)->second.get()));
  for (auto& id : location_ids_)
    res.insert(static_cast<MapResource*>(object_pool_.find(id)->second.get()));
  return res;
}

std::unordered_set<Point*> Map::GetAllPoints() {
  std::unordered_set<Point*> res;
  for (auto& id : point_ids_)
    res.insert(static_cast<Point*>(object_pool_.find(id)->second.get()));
  return res;
}

std::unordered_set<Path*> Map::GetAllPaths() {
  std::unordered_set<Path*> res;
  for (auto& id : path_ids_)
    res.insert(static_cast<Path*>(object_pool_.find(id)->second.get()));
  return res;
}

std::unordered_set<Location*> Map::GetAllLocations() {
  std::unordered_set<Location*> res;
  for (auto& id : location_ids_)
    res.insert(static_cast<Location*>(object_pool_.find(id)->second.get()));
  return res;
}

std::unordered_set<Block*> Map::GetAllBlocks() {
  std::unordered_set<Block*> res;
  for (auto& id : block_ids_)
    res.insert(static_cast<Block*>(object_pool_.find(id)->second.get()));
  return res;
}

std::unordered_set<Vehicle*> Map::GetAllVehicles() {
  std::unordered_set<Vehicle*> res;
  for (auto& id : vehicle_ids_)
    res.insert(static_cast<Vehicle*>(object_pool_.find(id)->second.get()));
  return res;
}

}  // namespace tcs