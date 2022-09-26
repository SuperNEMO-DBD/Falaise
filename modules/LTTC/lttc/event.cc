#include <lttcc/event.hpp>

namespace lttcc {

  void event::clear()
  {
    tracks.clear();
    hits.clear();
    return;
  }
  
} // namespace lttcc 
