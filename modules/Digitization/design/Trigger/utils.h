#ifndef SNEMO_TRIGGER_UTILS_H
#define SNEMO_TRIGGER_UTILS_H

// This project:
#include "memory.h"

namespace snemo {
  namespace trigger {

    typedef ::snemo::digitization::memory<9,2> mem1_type;
    typedef ::snemo::digitization::memory<8,2> mem2_type;
    typedef ::snemo::digitization::memory<8,2> mem3_type;
    typedef ::snemo::digitization::memory<8,3> mem4_type;
    typedef ::snemo::digitization::memory<8,3> mem5_type;
    typedef ::snemo::digitization::memory<6,3> mem6_type;

  }
}

#endif // SNEMO_TRIGGER_UTILS_H
