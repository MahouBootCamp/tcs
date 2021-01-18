#ifndef IKERNAL_H
#define IKERNAL_H

#include "tcs/order/transport_order.h"
#include "tcs/vehicle/ivehicle_adapter.h"

namespace tcs {

enum class KernalState { kIdle, kOperating, kExit };

class IKernal {
 public:
  virtual KernalState GetState() = 0;
  virtual void Start() = 0;
  virtual void Exit() = 0;
  virtual void EnableVehicle(MapObjectID vehicle, MapObjectID initial_position,
                             IVehicleAdapter* adapter = nullptr) = 0;
  virtual TransportOrderID AddTransportOrder(
      std::vector<Destination> destinations,
      std::unordered_set<TransportOrderID> dependencies = {}) = 0;
  virtual void WithdrawTransportOrder(TransportOrderID id) = 0;
  virtual ~IKernal() {}
};

}  // namespace tcs

#endif /* IKERNAL_H */
