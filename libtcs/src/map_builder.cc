#include "tcs/util/map_builder.h"

namespace tcs {

void MapBuilder::Init() {
  if (map_) delete map_;
  map_ = new Map();
  next_id_ = 0;
}

Map* MapBuilder::Build() {
  // Set charge & park locations
  auto points = map_->GetAllPoints();
  std::unordered_set<MapObjectID> charge_points;
  std::unordered_set<MapObjectID> park_points;
  for (auto& p : points) {
    auto type = p->GetType();
    if (type == PointType::kCharge)
      charge_points.insert(p->GetID());
    else if (type == PointType::kPark)
      park_points.insert(p->GetID());
  }

  auto charge_location = std::make_unique<SpecialLocation>(
      next_id_, kChargeOperation, charge_points);
  map_->charge_location_ = charge_location.get();
  map_->location_ids_.insert(next_id_);
  map_->object_pool_.insert({next_id_, std::move(charge_location)});
  for (auto& p : charge_points) {
    map_->GetPoint(p)->SetLinkedLocation(next_id_);
  }
  next_id_ += 1;

  auto park_location =
      std::make_unique<SpecialLocation>(next_id_, kParkOperation, park_points);
  map_->park_location_ = park_location.get();
  map_->location_ids_.insert(next_id_);
  map_->object_pool_.insert({next_id_, std::move(park_location)});
  for (auto& p : charge_points) {
    map_->GetPoint(p)->SetLinkedLocation(next_id_);
  }
  next_id_ += 1;

  Map* tmp = map_;
  map_ = nullptr;
  return tmp;
}

MapObjectID MapBuilder::AddPoint(Coordinate coordinate, PointType type) {
  AddPoint(next_id_, coordinate, type);
  auto id = next_id_;
  next_id_ += 1;
  return id;
}

void MapBuilder::AddPoint(MapObjectID id, Coordinate coordinate,
                          PointType type) {
  auto ptr = std::make_unique<Point>(id, coordinate, type);
  map_->point_ids_.insert(id);
  map_->object_pool_.insert({id, std::move(ptr)});
}

MapObjectID MapBuilder::AddPath(MapObjectID source, MapObjectID destination,
                                double length) {
  AddPath(next_id_, source, destination, length);
  auto id = next_id_;
  next_id_ += 1;
  return id;
}

void MapBuilder::AddPath(MapObjectID id, MapObjectID source,
                         MapObjectID destination, double length) {
  auto src = map_->GetPoint(source);
  auto dst = map_->GetPoint(destination);

  if (src == nullptr || dst == nullptr)
    throw std::invalid_argument("Point id not exist");
  if (length <= 0) throw std::invalid_argument("Length not positive");

  auto ptr = std::make_unique<Path>(id, source, destination, length);
  map_->path_ids_.insert(id);
  map_->object_pool_.insert({id, std::move(ptr)});
  src->AddOutPath(id);
  dst->AddInPath(id);
}

MapObjectID MapBuilder::AddLocation(
    std::unordered_set<std::string> operations,
    std::unordered_set<MapObjectID> linked_points) {
  AddLocation(next_id_, operations, linked_points);
  auto id = next_id_;
  next_id_ += 1;
  return id;
}

void MapBuilder::AddLocation(MapObjectID id,
                             std::unordered_set<std::string> operations,
                             std::unordered_set<MapObjectID> linked_points) {
  if (operations.size() == 0)
    throw std::invalid_argument("At least 1 operation should be defined");

  std::vector<Point*> point_vec;
  for (auto& point_id : linked_points) {
    auto point_ptr = map_->GetPoint(point_id);
    if (point_ptr)
      point_vec.push_back(point_ptr);
    else
      throw std::invalid_argument("Point id not exist");
  }

  auto ptr = std::make_unique<Location>(id, operations, linked_points);
  map_->location_ids_.insert(id);
  map_->object_pool_.insert({id, std::move(ptr)});
  for (auto& point_ptr : point_vec) {
    point_ptr->SetLinkedLocation(id);
  }
}

MapObjectID MapBuilder::AddBlock(std::unordered_set<MapObjectID> resources) {
  AddBlock(next_id_, resources);
  auto id = next_id_;
  next_id_ += 1;
  return id;
}

void MapBuilder::AddBlock(MapObjectID id,
                          std::unordered_set<MapObjectID> resources) {
  std::vector<MapResource*> resource_vec;
  for (auto& res_id : resources) {
    auto res_ptr = map_->GetResource(res_id);
    if (res_ptr)
      resource_vec.push_back(res_ptr);
    else
      throw std::invalid_argument("Resource id not exist");
  }

  auto ptr = std::make_unique<Block>(id, resources);
  map_->block_ids_.insert(id);
  map_->object_pool_.insert({id, std::move(ptr)});
  for (auto& res_ptr : resource_vec) {
    res_ptr->AddBlock(id);
  }
}

MapObjectID MapBuilder::AddVehicle() {
  AddVehicle(next_id_);
  auto id = next_id_;
  next_id_ += 1;
  return id;
}

void MapBuilder::AddVehicle(MapObjectID id) {
  auto ptr = std::make_unique<Vehicle>(id);
  map_->vehicle_ids_.insert(id);
  map_->object_pool_.insert({id, std::move(ptr)});
}

void MapBuilder::Link(MapObjectID point, MapObjectID location) {
  auto point_ptr = map_->GetPoint(point);
  auto location_ptr = map_->GetLocation(location);
  if (!point_ptr) throw std::invalid_argument("Point id not exist");
  if (!location_ptr) throw std::invalid_argument("Location id not exist");

  auto location_ref = point_ptr->GetLinkedLocation();
  if (location_ref.has_value()) {
    if (location_ref.value() == location)
      return;
    else
      Unlink(point, location_ref.value());
  }

  location_ptr->AddLinkedPoint(point);
  point_ptr->SetLinkedLocation(location);
}

void MapBuilder::Unlink(MapObjectID point, MapObjectID location) {
  auto point_ptr = map_->GetPoint(point);
  auto location_ptr = map_->GetLocation(location);
  if (!point_ptr) throw std::invalid_argument("Point id not exist");
  if (!location_ptr) throw std::invalid_argument("Location id not exist");

  if (point_ptr->GetLinkedLocation().has_value() &&
      point_ptr->GetLinkedLocation().value() == location) {
    point_ptr->SetLinkedLocation(std::nullopt);
    location_ptr->RemoveLinkedPoint(point);
  } else
    throw std::invalid_argument("Point not linked to location");
}

void MapBuilder::AddToBlock(MapObjectID resource, MapObjectID block) {
  auto resouce_ptr = map_->GetResource(resource);
  auto block_ptr = map_->GetBlock(block);

  if (!resouce_ptr) throw std::invalid_argument("Resource id not exist");
  if (!block_ptr) throw std::invalid_argument("Block id not exist");

  block_ptr->AddResource(resource);
  resouce_ptr->AddBlock(block);
}

void MapBuilder::RemoveFromBlock(MapObjectID resource, MapObjectID block) {
  auto resouce_ptr = map_->GetResource(resource);
  auto block_ptr = map_->GetBlock(block);

  if (!resouce_ptr) throw std::invalid_argument("Resource id not exist");
  if (!block_ptr) throw std::invalid_argument("Block id not exist");

  if (block_ptr->GetResources().find(resource) ==
      block_ptr->GetResources().end()) {
    throw std::invalid_argument("Resource not in block");
  } else {
    block_ptr->RemoveResource(resource);
    resouce_ptr->RemoveBlock(block);
  }
}

void MapBuilder::RemovePoint(MapObjectID id) {
  auto point_ptr = map_->GetPoint(id);
  if (!point_ptr) throw std::invalid_argument("Point id not exist");

  if (point_ptr->GetLinkedLocation().has_value()) {
    Unlink(id, point_ptr->GetLinkedLocation().value());
  }

  for (auto& block : point_ptr->GetBlocks()) {
    map_->GetBlock(block)->RemoveResource(id);
  }

  std::unordered_set<MapObjectID> in_paths{point_ptr->GetInPaths()};
  std::unordered_set<MapObjectID> out_paths{point_ptr->GetOutPaths()};
  for (auto& path : in_paths) {
    RemovePath(path);
  }
  for (auto& path : out_paths) {
    RemovePath(path);
  }

  map_->point_ids_.erase(id);
  map_->object_pool_.erase(id);
}

void MapBuilder::RemovePath(MapObjectID id) {
  auto path_ptr = map_->GetPath(id);
  if (!path_ptr) throw std::invalid_argument("Path id not exist");

  for (auto& block : path_ptr->GetBlocks()) {
    map_->GetBlock(block)->RemoveResource(id);
  }

  map_->GetPoint(path_ptr->GetSource())->RemoveOutPath(id);
  map_->GetPoint(path_ptr->GetDestination())->RemoveInPath(id);
  map_->path_ids_.erase(id);
  map_->object_pool_.erase(id);
}

void MapBuilder::RemoveLocation(MapObjectID id) {
  auto location_ptr = map_->GetLocation(id);
  if (!location_ptr) throw std::invalid_argument("Location id not exist");

  for (auto& block : location_ptr->GetBlocks()) {
    map_->GetBlock(block)->RemoveResource(id);
  }

  for (auto& point_id : location_ptr->GetLinkedPoints()) {
    map_->GetPoint(point_id)->SetLinkedLocation(std::nullopt);
  }

  map_->location_ids_.erase(id);
  map_->object_pool_.erase(id);
}

void MapBuilder::RemoveBlock(MapObjectID id) {
  auto block_ptr = map_->GetBlock(id);
  if (!block_ptr) throw std::invalid_argument("Block id not exist");

  for (auto& resource_id : block_ptr->GetResources()) {
    map_->GetResource(resource_id)->RemoveBlock(id);
  }

  map_->block_ids_.erase(id);
  map_->object_pool_.erase(id);
}

void MapBuilder::RemoveVehicle(MapObjectID id) {
  auto vehicle_ptr = map_->GetVehicle(id);
  if (!vehicle_ptr) throw std::invalid_argument("Vehicle id not exist");

  map_->vehicle_ids_.erase(id);
  map_->object_pool_.erase(id);
}

}  // namespace tcs