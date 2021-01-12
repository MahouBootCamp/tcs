#ifndef DEFAULT_ROUTER_H
#define DEFAULT_ROUTER_H

#include <limits>
#include <mutex>

#include "tcs/router/irouter.h"
#include "tcs/router/shortest_path_algorithm.h"
#include "tcs/service/map_service.h"
#include "tcs/util/map.h"

namespace tcs {

class DefaultRouter : public IRouter {
 public:
  // REVIEW: Use map service instead of map pointer?
  DefaultRouter(MapService *map_service)
      : map_service_{map_service}, algo_{map_service_->GetMap()} {}

  bool ChechRoutability(TransportOrder *order) override;
  std::optional<std::vector<DriveOrder>> GetRoute(
      Point *start_point, TransportOrder *order) override;
  std::optional<Route> GetRoute(Point *start_point,
                                Point *destination_point) override;
  double GetCost(Point *start_point, Point *destination_point) override;
  void SelectRoute(Vehicle *vehicle,
                   std::vector<DriveOrder> drive_orders) override;

  std::unordered_map<Vehicle *, std::vector<DriveOrder>> GetSelectedRoutes()
      override;

  std::unordered_set<Point *> GetTargetedPoints() override;

 private:
  // REVIEW: Move this function into map service?
  std::unordered_set<Point *> ExpandDestination(DriveOrder &drive_order);

  bool CheckRoutability(std::vector<DriveOrder> &drive_orders,
                        std::size_t index = 0, Point *point = nullptr);

  struct RouteResult {
    std::vector<DriveOrder> current_route;
    std::vector<DriveOrder> best_route;
    bool routable = false;
    double current_cost = 0;
    double best_cost = std::numeric_limits<double>::infinity();
  };

  void ComputeRoute(std::vector<DriveOrder> &drive_orders, RouteResult &result,
                    std::size_t index = 0, Point *point = nullptr);

  // Protect cached routes
  mutable std::mutex mut_;

  MapService *map_service_;
  std::unordered_map<Vehicle *, std::vector<DriveOrder>> selected_routes_;
  ShortestPathAlgorithm algo_;
};

}  // namespace tcs

#endif /* DEFAULT_ROUTER_H */
