#ifndef FALAISE_LTTC_MISSING_HITS_FINDER_HH
#define FALAISE_LTTC_MISSING_HITS_FINDER_HH

// Standard library:
#include <set>

#include <datatools/logger.h>

// This project
#include <lttc/tracker_hit.hh>
#include <lttc/sngeometry.hh>
#include <lttc/line2.hh>
#include <lttc/lttc_utils.hh>

namespace lttc {

  /// \brief Missing hits finder algorithm
  class missing_hits_finder
  {
  public:

    missing_hits_finder(const tracker & sntracker_,
                        const tracker_hit_collection & hits_,
                        const hit_cells_map & hit_cells_,
                        const std::set<int> & owned_hits_);

    bool find(const point2 & first_,
              const point2 & last_,
              std::vector<cluster_missing_hit_data> & missing_hits_) const;
    
  public:
    
    datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
    
  private:
    
    // Configuration
    const tracker & _sntracker_;   ///< Tracker description
    const tracker_hit_collection & _hits_; ///< Collection of tracker hits
    const hit_cells_map & _hit_cells_; ///< Map of hit cells
    const std::set<int> & _owned_hits_;
    
  };
  
} // namespace lttc 

#endif // FALAISE_LTTC_MISSING_HITS_FINDER_HH
