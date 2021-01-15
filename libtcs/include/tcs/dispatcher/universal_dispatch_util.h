#ifndef UNIVERSAL_DISPATCH_UTIL_H
#define UNIVERSAL_DISPATCH_UTIL_H

#include <boost/log/trivial.hpp>

#include "tcs/dispatcher/reserve_order_pool.h"
#include "tcs/router/irouter.h"
#include "tcs/service/map_service.h"
#include "tcs/service/transport_order_service.h"
#include "tcs/service/vehicle_service.h"
#include "tcs/util/controller_pool.h"
#include "tcs/util/executor.h"
#include "tcs/util/map.h"

namespace tcs {

class UniversalDispatchUtil {
 public:
  UniversalDispatchUtil(Executor* executor, MapService* map_service,
                        VehicleService* vehicle_service,
                        TransportOrderService* transport_order_service,
                        IRouter* router, ControllerPool* controller_pool,
                        ReserveOrderPool* reserve_order_pool)
      : executor_{executor},
        map_service_{map_service},
        vehicle_service_{vehicle_service},
        transport_order_service_{transport_order_service},
        router_{router},
        controller_pool_{controller_pool},
        reserve_order_pool_{reserve_order_pool} {}
  bool CanBypassDriveOrder(DriveOrder& drive_order, const Vehicle* vehicle);
  void AssignOrder(const Vehicle* vehicle, const TransportOrder* order,
                   std::optional<std::vector<DriveOrder>> drive_orders);
  void AbortOrderByVehicle(const Vehicle* vehicle);
  void AbortOrderByOrder(const TransportOrder* order);

 private:
  void AbortOrder(const Vehicle* vehicle, const TransportOrder* order);

  Executor* executor_;
  MapService* map_service_;
  VehicleService* vehicle_service_;
  TransportOrderService* transport_order_service_;
  IRouter* router_;
  ControllerPool* controller_pool_;
  ReserveOrderPool* reserve_order_pool_;
};

}  // namespace tcs

#endif /* UNIVERSAL_DISPATCH_UTIL_H */
