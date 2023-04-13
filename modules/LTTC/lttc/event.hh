#ifndef LTTCC_EVENT_HPP
#define LTTCC_EVENT_HPP

// Standard library:
#include <iostream>
#include <vector>

// This project:
#include <lttcc/track.hpp>
#include <lttcc/hit.hpp>

namespace lttcc {
  
  struct event
  {
    event() = default;

    void clear();
    
    track_collection tracks;
    hit_collection hits;
    
  };

} // namespace lttcc 

#endif // LTTCC_EVENT_HPP
