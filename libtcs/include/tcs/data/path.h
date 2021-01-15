#ifndef PATH_H
#define PATH_H

#include <unordered_set>

#include "tcs/data/map_resource.h"

namespace tcs {

// A Path is an abstract route between Points with a direction
class Path : public MapResource {
 public:
  Path(MapObjectID id, MapObjectID source, MapObjectID destination,
       double length)
      : MapResource{id, MapObjectType::kPath},
        source_{source},
        destination_{destination},
        length_{length} {}

  MapObjectID GetSource() const { return source_; }
  MapObjectID GetDestination() const { return destination_; }
  double GetLength() const { return length_; }

 private:
  MapObjectID source_;
  MapObjectID destination_;
  // Length of path. MUST BE POSITIVE.
  double length_;
};

}  // namespace tcs

#endif /* PATH_H */
