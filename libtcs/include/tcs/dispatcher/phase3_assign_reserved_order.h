#ifndef PHASE3_ASSIGN_RESERVED_ORDER_H
#define PHASE3_ASSIGN_RESERVED_ORDER_H

#include "tcs/dispatcher/phase.h"

namespace tcs {

class Phase3AssignReservedOrder : public Phase {
 public:
  void Run() override;
};

}  // namespace tcs

#endif /* PHASE3_ASSIGN_RESERVED_ORDER_H */
