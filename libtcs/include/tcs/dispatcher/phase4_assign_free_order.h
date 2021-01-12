#ifndef PHASE4_ASSIGN_FREE_ORDER_H
#define PHASE4_ASSIGN_FREE_ORDER_H

#include "tcs/dispatcher/assignment_candidate.h"
#include "tcs/dispatcher/phase.h"

namespace tcs {

class Phase4AssignFreeOrder : public Phase {
 public:
  Phase4AssignFreeOrder(MapService* map_service,
                        VehicleService* vehicle_service,
                        TransportOrderService* transport_order_service,
                        IRouter* router, ControllerPool* controller_pool,
                        ReserveOrderPool* reserve_order_pool)
      : map_service_{map_service},
        vehicle_service_{vehicle_service},
        transport_order_service_{transport_order_service},
        router_{router},
        controller_pool_{controller_pool},
        reserve_order_pool_{reserve_order_pool} {}

  void Run() override;

 private:
  std::unordered_set<Vehicle*> FilterAvailableVehicles();
  std::unordered_set<TransportOrder*> FilterDispatchableTransportOrders();
  bool CanBypassDriveOrder(DriveOrder& drive_order, Vehicle* vehicle) {
    return drive_order.GetRoute()->GetSteps().back().destination->GetID() ==
               vehicle->GetCurrentPoint().value() &&
           drive_order.GetDestination().operation == kNoOperation;
  }
  void AssignOrder(AssignmentCandidate& candidate);
  void TryAssignByVehicle(Vehicle* vehicle,
                          std::unordered_set<TransportOrder*>& orders,
                          std::unordered_set<TransportOrder*>& assigned_orders);
  void TryAssignByOrder(TransportOrder* order,
                        std::unordered_set<Vehicle*>& vehicles,
                        std::unordered_set<Vehicle*>& assigned_vehicles);

  MapService* map_service_;
  VehicleService* vehicle_service_;
  TransportOrderService* transport_order_service_;
  IRouter* router_;
  ControllerPool* controller_pool_;
  ReserveOrderPool* reserve_order_pool_;
};

}  // namespace tcs

#endif /* PHASE4_ASSIGN_FREE_ORDER_H */
