#ifndef SHORTEST_PATH_ALGORITHM_H
#define SHORTEST_PATH_ALGORITHM_H

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <optional>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "tcs/order/route.h"
#include "tcs/util/map.h"

namespace tcs {

class ShortestPathAlgorithm {
  using BoostGraph =
      boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS,
                            boost::no_property,
                            boost::property<boost::edge_weight_t, double>>;
  using BoostVertex = boost::graph_traits<BoostGraph>::vertex_descriptor;
  using BoostPath = boost::graph_traits<BoostGraph>::edge_descriptor;

 public:
  ShortestPathAlgorithm(Map* map);
  bool Routable(Point* source, Point* destination);
  std::optional<Route> ComputeRoute(Point* source, Point* destination);

 private:
  std::vector<Step> PathToSteps(std::list<BoostVertex>& boost_path);

  BoostGraph graph_;
  std::unordered_map<Point*, BoostVertex> point_to_vertex_dict_;
  std::unordered_map<BoostVertex, Point*> vertex_to_point_dict_;
  std::unordered_set<Path*> paths_;
};

}  // namespace tcs

#endif /* SHORTEST_PATH_ALGORITHM_H */
