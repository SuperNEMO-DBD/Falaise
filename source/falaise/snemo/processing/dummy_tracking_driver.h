/// \file falaise/snemo/processing/dummy_tracking_driver.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2024-03-15
 * Last modified : 2024-03-15
 */

#ifndef FALAISE_SNEMO_PROCESSING_DUMMY_TRACKING_DRIVER_H
#define FALAISE_SNEMO_PROCESSING_DUMMY_TRACKING_DRIVER_H 1

// Falaise module:
#include "falaise/snemo/processing/base_tracking_driver.h"

namespace snemo {

  namespace processing {

    /// \brief A dummy tracking driver
    class dummy_tracking_driver
			: public base_tracking_driver
    {
    public:
      
      /// Default constructor
      dummy_tracking_driver();

      /// Destructor
      ~dummy_tracking_driver() override;
 
    protected:

      /// Reset the tracking driver
      void _at_initialize(const datatools::properties & setup_) override;

      /// Reset the tracking driver
      void _at_reset() override;

    public:
         
      /// Tracking algorithm
      int process_tracking(const std::vector<tracking_precluster> & preclusters_,
														const base_tracking_driver::calo_hit_collection_type & calo_hits_,
														snemo::datamodel::tracker_clustering_data & clustering_,
														snemo::datamodel::tracker_trajectory_data & track_fitting_) override;

    };

  } // end of namespace processing

} // end of namespace snemo

#endif // FALAISE_SNEMO_PROCESSING_DUMMY_TRACKING_DRIVER_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
