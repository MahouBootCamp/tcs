#include "tcs/router/shortest_path_algorithm.h"

namespace tcs {

ShortestPathAlgorithm::ShortestPathAlgorithm(MapService* map_service)
    : paths_(map_service->GetAllPaths()) {
  auto points = map_service->GetAllPoints();

  for (auto& point : points) {
    auto vertex = boost::add_vertex(graph_);
    point_to_vertex_dict_[point] = vertex;
    vertex_to_point_dict_[vertex] = point;
  }

  for (auto& path : paths_) {
    auto src = point_to_vertex_dict_[map_service->GetPoint(path->GetSource())];
    auto dst = point_to_vertex_dict_[map_service->GetPoint(path->GetDestination())];
    auto edge = boost::add_edge(src, dst, path->GetLength(), graph_);
    if (!edge.second)
      throw std::invalid_argument("Constructing dijkstra algorithm failed");
  }
}

bool ShortestPathAlgorithm::Routable(const Point* source, const Point* destination) {
  if (source == destination) return true;
  auto boost_dst = point_to_vertex_dict_[destination];
  std::vector<BoostVertex> parent_vector(num_vertices(graph_));
  boost::dijkstra_shortest_paths(
      graph_, point_to_vertex_dict_[source],
      predecessor_map(boost::make_iterator_property_map(
          parent_vector.begin(), get(boost::vertex_index, graph_))));
  if (parent_vector[boost_dst] == boost_dst)
    return false;
  else
    return true;
}

std::optional<Route> ShortestPathAlgorithm::ComputeRoute(const Point* source,
                                                         const Point* destination) {
  if (source == destination)
    return Route{};  // Return a empty route with 0 cost

  std::vector<BoostVertex> parent_vector(num_vertices(graph_));
  std::vector<double> cost_vector(num_vertices(graph_));

  auto boost_src = point_to_vertex_dict_[source];
  auto boost_dst = point_to_vertex_dict_[destination];

  dijkstra_shortest_paths(
      graph_, boost_src,
      predecessor_map(
          boost::make_iterator_property_map(parent_vector.begin(),
                                            get(boost::vertex_index, graph_)))
          .distance_map(boost::make_iterator_property_map(
              cost_vector.begin(), get(boost::vertex_index, graph_))));

  std::list<BoostVertex> boost_path{boost_dst};
  BoostVertex prev = parent_vector[boost_dst];

  if (prev == boost_dst) return std::nullopt;

  while (prev != boost_src) {
    boost_path.push_front(prev);
    prev = parent_vector[prev];
  }
  boost_path.push_front(prev);

  return Route{PathToSteps(boost_path), cost_vector[boost_dst]};
}

std::vector<Step> ShortestPathAlgorithm::PathToSteps(
    std::list<BoostVertex>& boost_path) {
  std::vector<Step> step_vertor;

  auto sz = boost_path.size();
  if (sz != 0) {
    size_t i = 0;
    auto itr = boost_path.begin();
    while (i != sz - 1) {
      auto src = vertex_to_point_dict_[*itr];
      itr++;
      auto dst = vertex_to_point_dict_[*itr];
      i += 1;
      auto path_itr =
          std::find_if(paths_.begin(), paths_.end(), [src, dst](const Path* p) {
            return p->GetSource() == src->GetID() &&
                   p->GetDestination() == dst->GetID();
          });
      step_vertor.push_back({i, src, dst, *path_itr});
    }
  }

  return step_vertor;
}

}  // namespace tcs