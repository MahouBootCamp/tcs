#ifndef ROUTE_H
#define ROUTE_H

#include <vector>

#include "tcs/data/map_object.h"

namespace tcs {

struct Step {
  std::size_t index_;
  MapObjectID source_;
  MapObjectID destination_;
  // MapObjectID path_;
};

class Route {
 public:
  Route() = default;
  Route(std::vector<Step> steps, double cost) : steps_(steps), cost_(cost) {}

 private:
  std::vector<Step> steps_;
  double cost_;
};

}  // namespace tcs

#endif /* ROUTE_H */
