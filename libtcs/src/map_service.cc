#include "tcs/service/map_service.h"

namespace tcs {

void MapService::UpdatePointProcessingVehicle(MapObjectID point_id,
                                              MapObjectRef vehicle_ref) {
  auto point = GetPoint(point_id);
  point->SetOccupyingVehicle(vehicle_ref);
}

}  // namespace tcs