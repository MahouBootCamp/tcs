#ifndef PHASE2_ASSIGN_NEXT_DRIVE_ORDER_H
#define PHASE2_ASSIGN_NEXT_DRIVE_ORDER_H

#include "tcs/dispatcher/phase.h"

namespace tcs {

class Phase2AssignNextDriveOrder : public Phase {
 public:
  // Phase2AssignNextDriveOrder(TransportOrderService* transport_order_service,
  //                            VehicleService* vehicle_service, IRouter*
  //                            router, ControllerPool* controller_pool)
  //     : transport_order_service_{transport_order_service},
  //       vehicle_service_{vehicle_service},
  //       router_{router},
  //       controller_pool_{controller_pool} {}
  void Run() override;

 private:
  void CheckForNextOrder(Vehicle* vehicle);

  bool CanBypassDriveOrder(DriveOrder& drive_order, Vehicle* vehicle) {
    return drive_order.GetRoute()->GetSteps().back().destination->GetID() ==
               vehicle->GetCurrentPoint().value() &&
           drive_order.GetDestination().operation == kNoOperation;
  }

  void SetOrderFinished(Vehicle* vehicle, TransportOrder* order);

  TransportOrderService* transport_order_service_;
  VehicleService* vehicle_service_;
  IRouter* router_;
  ControllerPool* controller_pool_;
  MapService* map_service_;
};

}  // namespace tcs

#endif /* PHASE2_ASSIGN_NEXT_DRIVE_ORDER_H */
