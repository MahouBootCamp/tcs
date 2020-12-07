#ifndef IKERNAL_H
#define IKERNAL_H

namespace tcs {

enum class KernalState { kIdle, kOperating, kExit };

class IKernal {
 public:
  virtual KernalState get_state() = 0;
  virtual void Start() = 0;
  virtual void Exit() = 0;
  virtual void SetVehicleController() = 0;
  virtual void AddTransportOrder() = 0;
  virtual ~IKernal() {}
};

}  // namespace tcs

#endif /* IKERNAL_H */
