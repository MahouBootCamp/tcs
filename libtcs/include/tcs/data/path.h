#ifndef PATH_H
#define PATH_H

#include <unordered_set>

#include "tcs/data/map_resource.h"

namespace tcs {

class Path : public MapResource {
 public:
  Path(MapObjectID id, MapObjectID source, MapObjectID destination,
       double length)
      : MapResource(id, MapObjectType::kPath),
        source_(source),
        destination_(destination),
        length_(length) {}

 private:
  MapObjectID source_;
  MapObjectID destination_;
  double length_;
};

}  // namespace tcs

#endif /* PATH_H */
