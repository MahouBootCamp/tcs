#ifndef DEFAULT_ROUTER_H
#define DEFAULT_ROUTER_H

#include "tcs/router/irouter.h"
#include "tcs/util/map.h"

namespace tcs {

class DefaultRouter : public IRouter {
 public:
  DefaultRouter(Map *map);
  bool ChechRoutability(TransportOrder *order) override;
  std::vector<DriveOrder> GetRoute(Point *start_point,
                                   TransportOrder *order) override;
  std::optional<Route> GetRoute(Point *start_point,
                                Point *destination_point) override;
  double GetCost(Point *start_point, Point *destination_point) override;
  void SelectRoute(Vehicle *vehicle,
                   std::list<DriveOrder> &drive_orders) override;
  std::unordered_map<Vehicle *, std::vector<DriveOrder>> &GetSelectedRoutes()
      override;
  std::unordered_set<Point *> GetTargetedPoints() override;

 private:
  std::unordered_map<Vehicle *, std::vector<DriveOrder>> selected_routes_;
};

}  // namespace tcs

#endif /* DEFAULT_ROUTER_H */
