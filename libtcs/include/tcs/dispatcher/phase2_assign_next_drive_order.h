#ifndef PHASE2_ASSIGN_NEXT_DRIVE_ORDER_H
#define PHASE2_ASSIGN_NEXT_DRIVE_ORDER_H

#include "tcs/dispatcher/phase.h"
#include "tcs/router/irouter.h"
#include "tcs/service/map_service.h"
#include "tcs/service/transport_order_service.h"
#include "tcs/service/vehicle_service.h"
#include "tcs/util/controller_pool.h"

namespace tcs {

class Phase2AssignNextDriveOrder : public Phase {
 public:
  Phase2AssignNextDriveOrder(MapService* map_service,
                             VehicleService* vehicle_service,
                             TransportOrderService* transport_order_service,
                             IRouter* router, ControllerPool* controller_pool)
      : map_service_{map_service},
        vehicle_service_{vehicle_service},
        transport_order_service_{transport_order_service},
        router_{router},
        controller_pool_{controller_pool} {}
  void Run() override;

 private:
  void CheckForNextOrder(const Vehicle* vehicle);

  bool CanBypassDriveOrder(const DriveOrder& drive_order,
                           const Vehicle* vehicle) {
    return drive_order.GetRoute()->GetSteps().back().destination->GetID() ==
               vehicle_service_->ReadVehicleCurrentPosition(vehicle->GetID())
                   .value() &&
           drive_order.GetDestination().operation == kNoOperation;
  }

  void SetOrderFinished(const Vehicle* vehicle, const TransportOrder* order);

  MapService* map_service_;
  VehicleService* vehicle_service_;
  TransportOrderService* transport_order_service_;
  IRouter* router_;
  ControllerPool* controller_pool_;
};

}  // namespace tcs

#endif /* PHASE2_ASSIGN_NEXT_DRIVE_ORDER_H */
