#ifndef DEFAULT_SCHEDULER_H
#define DEFAULT_SCHEDULER_H

#include <boost/log/trivial.hpp>
#include <unordered_map>

#include "tcs/scheduler/ischeduler.h"
#include "tcs/scheduler/reservation_pool.h"
#include "tcs/util/executor.h"
#include "tcs/util/map.h"

namespace tcs {

// TODO: Add multithread protection
class DefaultScheduler : public IScheduler {
 public:
  DefaultScheduler(Executor *executor, Map *map)
      : executor_{executor}, map_{map} {}

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

  // Throw if resources unavailable.
  void AllocateNow(IVehicleController *vehicle,
                   std::unordered_set<MapResource *> resources) override;

  void Free(IVehicleController *vehicle,
            std::unordered_set<MapResource *> resources) override;

  void FreeAll(IVehicleController *vehicle) override;

 private:
  void AllocateTask(IVehicleController *vehicle,
                    std::unordered_set<MapResource *> resources);
  // void ReleaseTask(IVehicleController *vehicle,
  //                  std::unordered_set<MapResource *> resources);
  void CheckTask(IVehicleController *vehicle,
                 std::unordered_set<MapResource *> resources);
  void RetryTask();

  // Try allocate resources for vehicle. Return successful or not.
  bool TryAllocate(IVehicleController *vehicle,
                   std::unordered_set<MapResource *> &resources);

  // Expand resources in blocks.
  std::unordered_set<MapResource *> ExpandBlocks(
      std::unordered_set<MapResource *> &resources);

  std::unordered_map<IVehicleController *,
                     std::vector<std::unordered_set<MapResource *>>>
      claims_by_vehicle_;
  Executor *executor_;
  Map *map_;
  ReservationPool reservation_pool_;
  // List of allocations that are currently not available.
  // OPTIMIZE: We can use a priority queue to replace this one.
  std::list<std::pair<IVehicleController *, std::unordered_set<MapResource *>>>
      deferred_allocations_;
};

}  // namespace tcs

#endif /* DEFAULT_SCHEDULER_H */
