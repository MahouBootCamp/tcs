#ifndef PHASE_H
#define PHASE_H

#include <boost/log/trivial.hpp>

#include "tcs/dispatcher/idispatcher.h"
#include "tcs/dispatcher/reserve_order_pool.h"
#include "tcs/router/irouter.h"
#include "tcs/service/map_service.h"
#include "tcs/service/transport_order_service.h"
#include "tcs/service/vehicle_service.h"
#include "tcs/util/controller_pool.h"

namespace tcs {

class Phase {
 public:
  virtual void Run() = 0;
};

}  // namespace tcs

#endif /* PHASE_H */
