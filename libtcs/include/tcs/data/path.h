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

  MapObjectID GetSource() { return source_; }
  MapObjectID GetDestination() { return destination_; }
  double GetLength() { return length_; }
  void SetLength(double length) { length_ = length; }

 private:
  MapObjectID source_;
  MapObjectID destination_;
  // Length of path. MUST BE POSITIVE.
  double length_;
};

}  // namespace tcs

#endif /* PATH_H */
