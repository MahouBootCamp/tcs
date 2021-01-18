#include "tcs/service/map_service.h"

namespace tcs {

const Map* MapService::GetMap() const {
  std::scoped_lock<std::recursive_mutex> lock{global_mutex_};
  return map_;
}

const MapResource* MapService::GetResource(MapObjectID resource_id) const {
  std::scoped_lock<std::recursive_mutex> lock{global_mutex_};
  return map_->GetResource(resource_id);
}

const Point* MapService::GetPoint(MapObjectID point_id) const {
  std::scoped_lock<std::recursive_mutex> lock{global_mutex_};
  return map_->GetPoint(point_id);
}

const Path* MapService::GetPath(MapObjectID path_id) const {
  std::scoped_lock<std::recursive_mutex> lock{global_mutex_};
  return map_->GetPath(path_id);
}

const Location* MapService::GetLocation(MapObjectID location_id) const {
  std::scoped_lock<std::recursive_mutex> lock{global_mutex_};
  return map_->GetLocation(location_id);
}

const Block* MapService::GetBlock(MapObjectID block_id) const {
  std::scoped_lock<std::recursive_mutex> lock{global_mutex_};
  return map_->GetBlock(block_id);
}

std::unordered_set<const MapResource*> MapService::GetResourcesByID(
    const std::unordered_set<MapObjectID>& id_set) const {
  std::scoped_lock<std::recursive_mutex> lock{global_mutex_};
  return map_->GetResourcesByID(id_set);
}

std::unordered_set<const MapResource*> MapService::GetAllResources() const {
  std::scoped_lock<std::recursive_mutex> lock{global_mutex_};
  return map_->GetAllResources();
}

std::unordered_set<const Point*> MapService::GetAllPoints() const {
  std::scoped_lock<std::recursive_mutex> lock{global_mutex_};
  return map_->GetAllPoints();
}

std::unordered_set<const Path*> MapService::GetAllPaths() const {
  std::scoped_lock<std::recursive_mutex> lock{global_mutex_};
  return map_->GetAllPaths();
}

std::unordered_set<const Location*> MapService::GetAllLocations() const {
  std::scoped_lock<std::recursive_mutex> lock{global_mutex_};
  return map_->GetAllLocations();
}

std::unordered_set<const Block*> MapService::GetAllBlocks() const {
  std::scoped_lock<std::recursive_mutex> lock{global_mutex_};
  return map_->GetAllBlocks();
}

}  // namespace tcs