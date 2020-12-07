#ifndef TRANSPORT_ORDER_H
#define TRANSPORT_ORDER_H

#include <optional>

#include "tcs/data/map_object.h"
#include "tcs/order/drive_order.h"
#include "tcs/order/route.h"

namespace tcs {

using TransportOrderId = int;
using TransportOrderRef = std::optional<TransportOrderId>;

class TransportOrder {
 public:
 private:
  TransportOrderId id_;
  MapObjectRef vehicle_;
};

}  // namespace tcs

#endif /* TRANSPORT_ORDER_H */
