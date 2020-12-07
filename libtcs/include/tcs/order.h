#ifndef ORDER_H
#define ORDER_H

#include <optional>

#include "tcs/data/map_object.h"

namespace tcs {

using TransportOrderId = int;
using TransportOrderRef = std::optional<TransportOrderId>;

class Route {};

class DriveOrder {};

class TransportOrder {};

}  // namespace tcs

#endif /* ORDER_H */
