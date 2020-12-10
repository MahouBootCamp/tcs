#include "tcs/router/default_router.h"

namespace tcs {

DefaultRouter::DefaultRouter(Map *map) {}

bool DefaultRouter::ChechRoutability(TransportOrder *order) { return false; }

std::vector<DriveOrder> DefaultRouter::GetRoute(Point *start_point,
                                                TransportOrder *order) {
  return {};
}

std::optional<Route> DefaultRouter::GetRoute(Point *start_point,
                                             Point *destination_point) {
  return std::nullopt;
}

double DefaultRouter::GetCost(Point *start_point, Point *destination_point) {
  return 0;
}

void DefaultRouter::SelectRoute(Vehicle *vehicle,
                                std::list<DriveOrder> &drive_orders) {}

std::unordered_map<Vehicle *, std::vector<DriveOrder>>
    &DefaultRouter::GetSelectedRoutes() {
  return selected_routes_;
}

std::unordered_set<Point *> DefaultRouter::GetTargetedPoints() {
  std::unordered_set<Point *> res;
  for (auto &pair : selected_routes_) {
    res.insert(
        pair.second.back().get_route().value().get_steps().back().destination);
  }
  return res;
}

}  // namespace tcs