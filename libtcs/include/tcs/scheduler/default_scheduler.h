#ifndef DEFAULT_SCHEDULER_H
#define DEFAULT_SCHEDULER_H

#include "tcs/scheduler/ischeduler.h"

namespace tcs {

class DefaultScheduler : public IScheduler {
 public:
  void Claim(IVehicleController *vehicle,
             std::vector<std::unordered_set<MapResource *> > resource_sequence)
      override;

  void Unclaim(IVehicleController *vehicle) override;

  void UpdateProgressIndex(IVehicleController *vehicle,
                           std::size_t index) override;

  void Allocate(IVehicleController *vehicle,
                std::unordered_set<MapResource *> resources) override;

  void AllocateNow(IVehicleController *vehicle,
                   std::unordered_set<MapResource *> resources) override;

  void Free(IVehicleController *vehicle,
            std::unordered_set<MapResource *> resources) override;

  void FreeAll(IVehicleController *vehicle) override;

 private:
};

}  // namespace tcs

#endif /* DEFAULT_SCHEDULER_H */
