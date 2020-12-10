#ifndef ROUTE_H
#define ROUTE_H

#include <vector>

#include "tcs/data/path.h"
#include "tcs/data/point.h"

namespace tcs {

struct Step {
  std::size_t index;
  Point* source;
  Point* destination;
  Path* path;
  // MapObjectID source;
  // MapObjectID destination;
  // MapObjectID path;
};

class Route {
 public:
  Route() = default;
  Route(std::vector<Step> steps, double cost) : steps_(steps), cost_(cost) {}

  std::vector<Step>& get_steps() { return steps_; }
  double get_cost() { return cost_; }

 private:
  std::vector<Step> steps_;
  double cost_;
};

}  // namespace tcs

#endif /* ROUTE_H */
