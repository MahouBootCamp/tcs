#include "tcs/router/default_router.h"

namespace tcs {

DefaultRouter::DefaultRouter(Map *map) : map_{map}, algo_{map} {}

bool DefaultRouter::ChechRoutability(TransportOrder *order) {
  auto future_orders = order->GetFutureDriveOrders();
  if (future_orders.size() <= 1) return true;
  return CheckRoutability(future_orders);
}

std::optional<std::vector<DriveOrder>> DefaultRouter::GetRoute(
    Point *start_point, TransportOrder *order) {
  auto drive_orders = order->GetFutureDriveOrders();
  RouteResult result{drive_orders, drive_orders};
  ComputeRoute(drive_orders, result, 0, start_point);
  if (result.routable)
    return std::move(result.best_route);
  else
    return std::nullopt;
}

std::optional<Route> DefaultRouter::GetRoute(Point *start_point,
                                             Point *destination_point) {
  return algo_.ComputeRoute(start_point, destination_point);
}

double DefaultRouter::GetCost(Point *start_point, Point *destination_point) {
  return algo_.ComputeRoute(start_point, destination_point)->get_cost();
}

void DefaultRouter::SelectRoute(Vehicle *vehicle,
                                std::vector<DriveOrder> drive_orders) {
  std::scoped_lock<std::mutex> lk{mut_};
  selected_routes_.insert({vehicle, drive_orders});
}

std::unordered_map<Vehicle *, std::vector<DriveOrder>>
DefaultRouter::GetSelectedRoutes() {
  std::scoped_lock<std::mutex> lk{mut_};
  return selected_routes_;
}

std::unordered_set<Point *> DefaultRouter::GetTargetedPoints() {
  std::scoped_lock<std::mutex> lk{mut_};
  std::unordered_set<Point *> res;
  for (auto &pair : selected_routes_) {
    res.insert(
        pair.second.back().get_route().value().get_steps().back().destination);
  }
  return res;
}

std::unordered_set<Point *> DefaultRouter::ExpandDestination(
    DriveOrder &drive_order) {
  auto dst_id = drive_order.get_destination().site;
  auto location = map_->get_location(dst_id);
  if (location) {
    std::unordered_set<Point *> res;
    for (auto &point_id : location->get_linked_points())
      res.insert(map_->get_point(point_id));
    return res;
  } else
    return {map_->get_point(dst_id)};
}

bool DefaultRouter::CheckRoutability(std::vector<DriveOrder> &drive_orders,
                                     std::size_t index, Point *point) {
  if (index == drive_orders.size()) return true;
  auto points = ExpandDestination(drive_orders[index]);
  if (index == 0) {
    for (auto &p : points) {
      if (CheckRoutability(drive_orders, index + 1, p)) return true;
    }
    return false;
  } else {
    for (auto &p : points) {
      if (algo_.Routable(point, p) &&
          CheckRoutability(drive_orders, index + 1, p))
        return true;
    }
    return false;
  }
}

void DefaultRouter::ComputeRoute(std::vector<DriveOrder> &drive_orders,
                                 RouteResult &result, std::size_t index,
                                 Point *point) {
  if (index == drive_orders.size()) {
    if (result.current_cost < result.best_cost) {
      result.routable = true;
      result.best_route = result.current_route;
      result.best_cost = result.current_cost;
    }
  } else {
    bool routable = false;
    double cost = result.current_cost;
    auto points = ExpandDestination(drive_orders[index]);
    if (points.find(point) != points.end()) {
      // No need to move so replace destinations with current one.
      points = {point};
    }
    for (auto dst : points) {
      auto route = GetRoute(point, dst);
      if (!route.has_value()) continue;
      result.current_cost = cost + route->get_cost();
      result.current_route[index].set_rotue(std::move(route));
      ComputeRoute(drive_orders, result, index + 1, dst);
      routable = true;
    }
    if (!routable)
      result.current_cost = std::numeric_limits<double>::infinity();
  }
}

}  // namespace tcs