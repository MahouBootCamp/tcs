#ifndef SIM_VEHICLE_ADAPTER_H
#define SIM_VEHICLE_ADAPTER_H

#include <boost/log/trivial.hpp>
#include <cmath>
#include <queue>

#include "tcs/vehicle/ivehicle_adapter.h"

namespace tcs {

const std::size_t kMaxCommandQueueSize = 10;
const double kSimSpeed = 100;        // mm/s
const long kSimOperation = 1;        // s
const long kSimChargeOperation = 5;  // s
const std::string kLoadOperation = "Load";
const std::string kUnloadOperation = "Unload";

class SimVehicleAdapter : public IVehicleAdapter {
 public:
  ~SimVehicleAdapter();

  void Enable() override;

  void Disable() override;

  bool IsEnabled() override { return enabled_; }

  bool CanProcess(std::unordered_set<std::string> operations) override;

  bool EnqueueCommand(MovementCommand command) override;

  void ClearCommandQueue() override;

  // void InitPosition(MapObjectID point) override {
  //   UpdatePositionEvent().Fire(std::move(point));
  // }

 private:
  void SimulateTasks();

  mutable std::mutex mut_;
  std::atomic_bool enabled_;
  bool loaded_ = false;
  int cnt_ = 0;
  std::queue<MovementCommand> command_queue_;
  std::thread simulate_thread_;
};  // namespace tcs

}  // namespace tcs

#endif /* SIM_VEHICLE_ADAPTER_H */
