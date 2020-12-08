#ifndef DESTINATION_H
#define DESTINATION_H

#include "tcs/data/map_object.h"

namespace tcs {

struct Destination {
  // can be point or location
  MapObjectID site;
  std::string operation;
};

}  // namespace tcs

#endif /* DESTINATION_H */
