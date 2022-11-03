// \file lttc_utils.cc

// Ourselves:
#include <lttc/lttc_utils.hh>

namespace lttc {

  std::string to_string(missing_cell_reason_type why_)
  {
    switch (why_) {
    case MCR_NO_TRIGGER : return std::string("no-trigger");
    case MCR_DEAD_CELL  : return std::string("dead-cell");
    case MCR_BUSY_CELL  : return std::string("busy-cell");
    case MCR_OTHER_TRACK : return std::string("other-track");
    default:
      break;
    }
    return std::string("undefined");
  }
 
  // friend
  std::ostream & operator<<(std::ostream & out_, const cluster_missing_hit_data & cmh_)
  {
    out_ << "{cell-id=" << cmh_.cid << ",why=" << to_string(cmh_.why) << '}';
    return out_;
  }
 

} // namespace lttc 
