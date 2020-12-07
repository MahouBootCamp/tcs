#ifndef IVEHICLE_ADAPTER_H
#define IVEHICLE_ADAPTER_H

namespace tcs {

// Vehicle adapter translates tcs commands into robot commands.
// NOTE: Users need to implement this interface.
class IVehicleAdapter {
  virtual ~IVehicleAdapter() {}
};

}  // namespace tcs

#endif /* IVEHICLE_ADAPTER_H */
