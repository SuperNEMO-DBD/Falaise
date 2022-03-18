#ifndef FALAISE_LTTC_PLUGIN_LTTC_UTILS_HH
#define FALAISE_LTTC_PLUGIN_LTTC_UTILS_HH

// Standard library:
#include <iostream>
#include <string>
#include <map>

// This project
#include <lttc/tracker_hit.hh>

namespace lttc {

  enum missing_cell_reason_type
    {
     MCR_UNDEFINED   = 0, ///< Undefined reason
     MCR_NO_TRIGGER  = 1, ///< No trigger as expected
     MCR_DEAD_CELL   = 2, ///< Dead cell
     MCR_BUSY_CELL   = 3, ///< Busy cell (dead time)
     MCR_OTHER_TRACK = 4  ///< Cell hit by another track
    };

  std::string to_string(missing_cell_reason_type why_);

  /// \brief Information about a missing hit 
  struct cluster_missing_hit_data
  {
    cell_id cid;
    missing_cell_reason_type why;
    friend std::ostream & operator<<(std::ostream & out_, const cluster_missing_hit_data & cmh_);
  };

  typedef std::map<cell_id, int> hit_cells_map; ///< Map of hit cells
 
} // namespace lttc 

#endif // FALAISE_LTTC_PLUGIN_LTTC_UTILS_HH
  
