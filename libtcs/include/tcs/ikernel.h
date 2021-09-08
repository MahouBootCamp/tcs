#ifndef IKERNEL_H
#define IKERNEL_H

#include "tcs/order/transport_order.h"
#include "tcs/vehicle/ivehicle_adapter.h"

namespace tcs {

enum class KernelState { kIdle, kOperating, kExit };

class IKernel {
 public:
  virtual KernelState GetState() = 0;
  virtual void Start() = 0;
  virtual void Exit() = 0;
  virtual void EnableVehicle(MapObjectID vehicle, MapObjectID initial_position,
                             IVehicleAdapter* adapter = nullptr) = 0;
  virtual TransportOrderID AddTransportOrder(
      std::vector<Destination> destinations,
      std::unordered_set<TransportOrderID> dependencies = {}) = 0;
  virtual void WithdrawTransportOrder(TransportOrderID id) = 0;
  virtual ~IKernel() {}
};

}  // namespace tcs

#endif /* IKERNEL_H */
