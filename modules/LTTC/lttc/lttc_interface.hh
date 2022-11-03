/* -*- mode: c++ -*- */
// lttc_interface.hh

#ifndef LTTC_INTERFACE_HH
#define LTTC_INTERFACE_HH 1

// Standard library:
#include <vector>
#include <iostream>
#include <string>

// Third party:
// - Bayeux
#include <bayeux/datatools/logger.h>

// This project:
#include <lttc/lttc_config.hh>
#include <lttc/tracker_hit.hh>

namespace lttc {

  /// \brief Configuration data of the Lttc algorithm(s)
  ///
  /// This class contains the minimal set of parameters needed to run the Lttc algorithm(s)
  struct setup_data
  {
  public:
    setup_data() = default;
    bool check() const;
    void reset();

  public:

    /// Verbosity level
    datatools::logger::priority logging = datatools::logger::PRIO_FATAL;

  };

  /// \brief Input data model
  struct input_data
  {
  public:
    
    input_data() = default;
    bool check() const;
    tracker_hit & add_tracker_hit();

  public:
    
    std::vector<tracker_hit> tracker_hits;
    
  };

  /// \brief Output data model
  struct output_data
  {
  public:
    output_data() = default;

  public:
    //  cluster_data clusters;
  };

}  // namespace lttc

#endif  // LTTC_INTERFACE_HH

// end of lttc_interface.hh
