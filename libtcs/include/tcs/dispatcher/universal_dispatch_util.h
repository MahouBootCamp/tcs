#ifndef UNIVERSAL_DISPATCH_UTIL_H
#define UNIVERSAL_DISPATCH_UTIL_H

#include <boost/log/trivial.hpp>

#include "tcs/dispatcher/idispatcher.h"
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
  bool CanBypassDriveOrder(DriveOrder& drive_order, Vehicle* vehicle);
  void AssignOrder(Vehicle* vehicle, TransportOrder* order,
                   std::optional<std::vector<DriveOrder>> drive_orders);
  void AbortOrderByVehicle(Vehicle* vehicle);
  void AbortOrderByOrder(TransportOrder* order);

 private:
};

}  // namespace tcs

#endif /* UNIVERSAL_DISPATCH_UTIL_H */
