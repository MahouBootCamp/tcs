#ifndef DEFAULT_SCHEDULER_H
#define DEFAULT_SCHEDULER_H

#include <boost/log/trivial.hpp>
#include <unordered_map>

#include "tcs/scheduler/ischeduler.h"
#include "tcs/util/executor.h"

namespace tcs {

// TODO: Add multithread protection
class DefaultScheduler : public IScheduler {
 public:
  DefaultScheduler(Executor *executor) : executor_{executor} {}

  void Claim(IVehicleController *vehicle,
             std::vector<std::unordered_set<MapResource *>> resource_sequence)
      override;

  void Unclaim(IVehicleController *vehicle) override;

  void UpdateProgressIndex(IVehicleController *vehicle,
                           std::size_t index) override {
    return;
  }

  void Allocate(IVehicleController *vehicle,
                std::unordered_set<MapResource *> resources) override;

  void AllocateNow(IVehicleController *vehicle,
                   std::unordered_set<MapResource *> resources) override;

  void Free(IVehicleController *vehicle,
            std::unordered_set<MapResource *> resources) override;

  void FreeAll(IVehicleController *vehicle) override;

 private:
  std::unordered_map<IVehicleController *,
                     std::vector<std::unordered_set<MapResource *>>>
      claims_by_vehicle_;
  Executor *executor_;
};

}  // namespace tcs

#endif /* DEFAULT_SCHEDULER_H */
