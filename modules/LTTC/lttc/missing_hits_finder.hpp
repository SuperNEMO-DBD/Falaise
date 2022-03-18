#ifndef FALAISE_LTTC_MISSING_HITS_FINDER_HH
#define FALAISE_LTTC_MISSING_HITS_FINDER_HH

// This project
#include <lttc/tracker_hit.hh>
#include <lttc/sngeometry.hh>

namespace lttc {

  /// \brief Missing hits finder algorithm
  class missing_hits_finder
  {
  public:

    missing_hits_finder(const tracker & sntracker_);
    
  private:
    
    // Configuration
    const tracker * sntracker = nullptr;   ///< Tracker description
  
  };
  
} // namespace lttc 

#endif // FALAISE_LTTC_MISSING_HITS_FINDER_HH
