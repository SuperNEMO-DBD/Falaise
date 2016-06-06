#ifndef SNEMO_TRIGGER_UTILS_H
#define SNEMO_TRIGGER_UTILS_H

// This project:
#include "memory.h"

namespace snemo {
  namespace trigger {

    typedef ::snemo::digitization::memory<9,2> mem1_type; // Sliding Zone Vertical memory
    typedef ::snemo::digitization::memory<8,2> mem2_type; // Sliding Zone Horizontal memory
    typedef ::snemo::digitization::memory<8,2> mem3_type; // Zone Vertical memory
    typedef ::snemo::digitization::memory<6,3> mem4_type; // Zone Horizontal memory
    typedef ::snemo::digitization::memory<8,3> mem5_type; // Zone Vertical for horizontal memory
  }
}

#endif // SNEMO_TRIGGER_UTILS_H
