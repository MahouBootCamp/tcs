#ifndef ROUTE_H
#define ROUTE_H

#include <vector>

#include "tcs/data/path.h"
#include "tcs/data/point.h"

namespace tcs {

struct Step {
  std::size_t index;
  const Point* source;
  const Point* destination;
  const Path* path;
  // MapObjectID source;
  // MapObjectID destination;
  // MapObjectID path;
};

inline bool operator==(const Step& s1, const Step& s2) {
  return s1.index == s2.index && s1.source == s2.source &&
         s1.destination == s2.destination && s1.path == s2.path;
}

class Route {
 public:
  Route() = default;
  Route(std::vector<Step> steps, double cost) : steps_{steps}, cost_{cost} {}

  const std::vector<Step>& GetSteps() const { return steps_; }
  double GetCost() const { return cost_; }

 private:
  std::vector<Step> steps_;
  double cost_;
};

}  // namespace tcs

#endif /* ROUTE_H */
