#ifndef FALAISE_LTTC_PLUGIN_TRACK_HH
#define FALAISE_LTTC_PLUGIN_TRACK_HH

// Standard library:
#include <vector>
#include <iostream>

// This project:
#include <lttc/point.hh>

namespace lttc {
  
  struct track
  {
    enum flags_type
      {
       LINE   = 0x1,
       CIRCLE = 0x2,
       KINK   = 0x4
      };
    
    track() = default;
    
    track(int id_, const polyline & pl_, const uint32_t flags_ = 0);

    void draw(std::ostream & out_) const;
    
    int      id = -1;
    uint32_t flags = 0;
    polyline pl;
    point    kink;
   
  };

  typedef std::vector<track> track_collection;
  
} // namespace lttc 

#endif // FALAISE_LTTC_PLUGIN_TRACK_HH
