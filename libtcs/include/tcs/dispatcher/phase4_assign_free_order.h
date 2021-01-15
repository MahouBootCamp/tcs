#ifndef PHASE4_ASSIGN_FREE_ORDER_H
#define PHASE4_ASSIGN_FREE_ORDER_H

#include "tcs/dispatcher/assignment_candidate.h"
#include "tcs/dispatcher/phase.h"
#include "tcs/dispatcher/universal_dispatch_util.h"

namespace tcs {

class Phase4AssignFreeOrder : public Phase {
 public:
  Phase4AssignFreeOrder(MapService* map_service,
                        VehicleService* vehicle_service,
                        TransportOrderService* transport_order_service,
                        IRouter* router, ControllerPool* controller_pool,
                        ReserveOrderPool* reserve_order_pool,
                        UniversalDispatchUtil* universal_dispatch_util)
      : map_service_{map_service},
        vehicle_service_{vehicle_service},
        transport_order_service_{transport_order_service},
        router_{router},
        controller_pool_{controller_pool},
        reserve_order_pool_{reserve_order_pool},
        universal_dispatch_util_{universal_dispatch_util} {}

  void Run() override;

 private:
  std::unordered_set<const Vehicle*> FilterAvailableVehicles();
  std::unordered_set<const TransportOrder*> FilterDispatchableTransportOrders();
  bool CanBypassDriveOrder(const DriveOrder& drive_order,
                           const Vehicle* vehicle) {
    return drive_order.GetRoute()->GetSteps().back().destination->GetID() ==
               vehicle_service_->ReadVehicleCurrentPosition(vehicle->GetID()) &&
           drive_order.GetDestination().operation == kNoOperation;
  }
  // void AssignOrder(AssignmentCandidate& candidate);
  void TryAssignByVehicle(
      const Vehicle* vehicle,
      const std::unordered_set<const TransportOrder*>& orders,
      std::unordered_set<const TransportOrder*>& assigned_orders);
  void TryAssignByOrder(const TransportOrder* order,
                        const std::unordered_set<const Vehicle*>& vehicles,
                        std::unordered_set<const Vehicle*>& assigned_vehicles);

  MapService* map_service_;
  VehicleService* vehicle_service_;
  TransportOrderService* transport_order_service_;
  IRouter* router_;
  ControllerPool* controller_pool_;
  ReserveOrderPool* reserve_order_pool_;
  UniversalDispatchUtil* universal_dispatch_util_;
};

}  // namespace tcs

#endif /* PHASE4_ASSIGN_FREE_ORDER_H */
