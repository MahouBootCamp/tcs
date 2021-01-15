#ifndef PHASE6_PARK_VEHICLE_H
#define PHASE6_PARK_VEHICLE_H

#include "tcs/dispatcher/phase.h"
#include "tcs/dispatcher/universal_dispatch_util.h"
#include "tcs/util/order_pool.h"

namespace tcs {

// UNDONE: We need to review this part
// NOTE: The phase would be skipped
class Phase6ParkVehicle : public Phase {
 public:
  Phase6ParkVehicle(OrderPool* order_pool, MapService* map_service,
                      VehicleService* vehicle_service, IRouter* router,
                      UniversalDispatchUtil* universal_dispatch_util)
      : order_pool_{order_pool},
        map_service_{map_service},
        vehicle_service_{vehicle_service},
        router_{router},
        universal_dispatch_util_{universal_dispatch_util} {}

  void Run() override;

 private:
  void CreateParkOrder(const Vehicle* vehicle);

  OrderPool* order_pool_;
  MapService* map_service_;
  VehicleService* vehicle_service_;
  IRouter* router_;
  UniversalDispatchUtil* universal_dispatch_util_;
};

}  // namespace tcs

#endif /* PHASE6_PARK_VEHICLE_H */
