#ifndef ASSIGNMENT_CANDIDATE_H
#define ASSIGNMENT_CANDIDATE_H

#include "tcs/data/vehicle.h"

namespace tcs {

struct AssignmentCandidate {
  const Vehicle* vehicle;
  const TransportOrder* transport_order;
  std::optional<std::vector<DriveOrder>> drive_orders;
  double total_cost;

  AssignmentCandidate(const Vehicle* vehicle, const TransportOrder* transport_order,
                      std::optional<std::vector<DriveOrder>> drive_orders)
      : vehicle{vehicle},
        transport_order{transport_order},
        drive_orders{drive_orders} {
    total_cost = 0;
    for (auto& drive_order : drive_orders.value())
      total_cost += drive_order.GetRoute()->GetCost();
  }
};

}  // namespace tcs

#endif /* ASSIGNMENT_CANDIDATE_H */
