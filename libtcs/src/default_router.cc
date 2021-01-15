#include "tcs/router/default_router.h"

namespace tcs {

bool DefaultRouter::ChechRoutability(const TransportOrder *order) {
  auto future_orders =
      transport_order_service_->ReadOrderFutureDriveOrders(order->GetID());
  if (future_orders.size() <= 1) return true;
  return CheckRoutability(future_orders);
}

std::optional<std::vector<DriveOrder>> DefaultRouter::GetRoute(
    const Point *start_point, const TransportOrder *order) {
  auto drive_orders =
      transport_order_service_->ReadOrderFutureDriveOrders(order->GetID());
  RouteResult result{drive_orders, drive_orders};
  ComputeRoute(drive_orders, result, 0, start_point);
  if (result.routable)
    return std::move(result.best_route);
  else
    return std::nullopt;
}

std::optional<Route> DefaultRouter::GetRoute(const Point *start_point,
                                             const Point *destination_point) {
  return algo_.ComputeRoute(start_point, destination_point);
}

double DefaultRouter::GetCost(const Point *start_point,
                              const Point *destination_point) {
  return algo_.ComputeRoute(start_point, destination_point)->GetCost();
}

void DefaultRouter::SelectRoute(const Vehicle *vehicle,
                                std::vector<DriveOrder> drive_orders) {
  std::scoped_lock<std::mutex> lk{mut_};
  selected_routes_.insert({vehicle, drive_orders});
}

std::unordered_map<const Vehicle *, std::vector<DriveOrder>>
DefaultRouter::GetSelectedRoutes() {
  std::scoped_lock<std::mutex> lk{mut_};
  return selected_routes_;
}

std::unordered_set<const Point *> DefaultRouter::GetTargetedPoints() {
  std::scoped_lock<std::mutex> lk{mut_};
  std::unordered_set<const Point *> res;
  for (auto &pair : selected_routes_) {
    res.insert(
        pair.second.back().GetRoute().value().GetSteps().back().destination);
  }
  return res;
}

std::unordered_set<const Point *> DefaultRouter::ExpandDestination(
    DriveOrder &drive_order) {
  auto dst_id = drive_order.GetDestination().site;
  auto location = map_service_->GetLocation(dst_id);
  if (location) {
    auto resources =
        map_service_->GetResourcesByID(location->GetLinkedPoints());
    std::unordered_set<const Point *> result;
    for (auto &resource : resources)
      result.insert(static_cast<const Point *>(resource));
    return result;
  } else
    return {map_service_->GetPoint(dst_id)};
}

bool DefaultRouter::CheckRoutability(std::vector<DriveOrder> &drive_orders,
                                     std::size_t index, const Point *point) {
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
                                 const Point *point) {
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
      result.current_cost = cost + route->GetCost();
      result.current_route[index].SetRoute(std::move(route));
      ComputeRoute(drive_orders, result, index + 1, dst);
      routable = true;
    }
    if (!routable)
      result.current_cost = std::numeric_limits<double>::infinity();
  }
}

}  // namespace tcs