/// \file falaise/snemo/processing/base_tracking_driver.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2012-03-29
 * Last modified : 2024-03-14
 *
 * Copyright (C) 2012-2024 François Mauger <mauger@lpccaen.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Description:
 *   Base class for tracking algorithms
 *
 */
#ifndef FALAISE_SNEMO_PROCESSING_BASE_TRACKING_DRIVER_H
#define FALAISE_SNEMO_PROCESSING_BASE_TRACKING_DRIVER_H 1

// Standard library:
#include <map>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux/datatools:
#include <bayeux/datatools/logger.h>
#include <bayeux/datatools/object_configuration_description.h>
// - Bayeux/geomtools:
#include <bayeux/geomtools/id_selector.h>

// Falaise:
#include "falaise/snemo/datamodels/calibrated_data.h"
#include "falaise/snemo/time/time_utils.h"

// Falaise module:
#include "falaise/snemo/processing/detail/GeigerTimePartitioner.h"
#include "falaise/snemo/processing/tracking_utils.h"
#include "falaise/snemo/processing/detector_description.h"

// Forward declaration :
namespace datatools {
  class properties;
}

namespace geomtools {
  class manager;
}

namespace snemo {

  namespace geometry {
    class gg_locator;
  }

  namespace datamodel {
    class tracker_clustering_data;
    class tracker_trajectory_data;
  }

  namespace processing {

    /// \brief The base abstract class for all tracking algorithms
    class base_tracking_driver
    {
    public:
      // Typedefs
      using hit_type = snemo::datamodel::calibrated_tracker_hit;
      using hit_handle_type = snemo::datamodel::TrackerHitHdl;
      using hit_collection_type = snemo::datamodel::TrackerHitHdlCollection;
      using calo_hit_collection_type = snemo::datamodel::CalorimeterHitHdlCollection;

      /// Default constructor
      base_tracking_driver(const std::string & name = "anonymous");

      /// Destructor
      virtual ~base_tracking_driver();

      /// Set logging priority level
      void set_verbosity(datatools::logger::priority verbosity_);

      /// Get logging priority
      datatools::logger::priority get_verbosity() const;

      /// Return the clusterizer ID
      const std::string & get_id() const;
  
			/// Return the detector description
			const snemo::processing::detector_description &
			get_detector_description() const;

			/// Set the detector description
			void set_detector_description(const snemo::processing::detector_description & det_desc_);
	
      /// Return a non-mutable reference to the geometry manager
      const geomtools::manager & get_geometry_manager() const;
		
      /// Return the tracker locator
      const snemo::geometry::gg_locator & get_gg_locator() const;

      /// Check if theclusterizer is initialized
      bool is_initialized() const;

      /// Main clustering process
      int process(const snemo::datamodel::calibrated_data & cd_,
                  snemo::datamodel::tracker_clustering_data & clustering_,
									snemo::datamodel::tracker_trajectory_data & track_fitting_);

      // Smart print
      void tree_dump(std::ostream & out_ = std::clog,
										 const std::string & title_ = "",
                     const std::string & indent_ = "",
										 bool inherit_ = false) const;

      /// Initialize the clusterizer through configuration properties
			///
			/// Example of supported configuration properties:
			///
			/// # Verbosity:
			/// verbosity : string = "debug"
			///
			/// # Service requirements:
			/// services.geometry : boolean = true
			/// services.calo_locator : boolean = false
			/// services.xcalo_locator : boolean = false
			/// services.gg_locator : boolean = false
			/// services.cell_status : boolean = true
			/// services.om_status : boolean = false
			///
			/// # Masking rules for Geiger cells:
			/// cell_id_mask_rules : string = "..."
			///
			/// # Tracker pre-clusterizer preprocessing:
			/// TPC.delayed_hit_cluster_time : real as time = 10.0 us
			/// TPC.processing_prompt_hits : boolean = true
			/// TPC.processing_delayed_hits : boolean = true
			/// TPC.split_chamber : boolean = true		
			///
			void initialize(const datatools::properties & config_,
											datatools::service_manager & services_);

      /// Reset the clusterizer
      void reset();

      /// OCD support
      static void ocd_support(datatools::object_configuration_description &,
                              const std::string & prefix_ = "");

      // Manage the timestamp associated to processed events:
  
      bool has_event_timestamp() const;
  
      const snemo::time::time_point & get_event_timestamp() const;

      void set_event_timestamp(const snemo::time::time_point &);

      void reset_event_timestamp();

    protected:

      /// Initialize the tracking driver
      virtual void _at_initialize(const datatools::properties & config_,
																	datatools::service_manager & services_) = 0;

      /// Reset the tracking driver
      virtual void _at_reset() = 0;

    private:
      
      /// Initialize the clusterizer through configuration properties
      void _initialize_(const datatools::properties & config_,
												datatools::service_manager & services_);

      /// Reset the clusterizer
      void _reset_();

    protected:

      /// Clear working arrays
      void _clear_working_arrays();
  
      /// Prepare cluster for processing
      virtual int _prepare_process(const base_tracking_driver::hit_collection_type & gg_hits_,
                                   const base_tracking_driver::calo_hit_collection_type & calo_hits_,
                                   snemo::datamodel::tracker_clustering_data & clustering_,
                                   snemo::datamodel::tracker_trajectory_data & track_fitting_);

		public:
			
      /// Tracking algorithm
      virtual int process_tracking(const std::vector<tracking_precluster> & preclusters_,
																	 const base_tracking_driver::calo_hit_collection_type & calo_hits_,
																	 snemo::datamodel::tracker_clustering_data & clustering_,
																	 snemo::datamodel::tracker_trajectory_data & track_fitting_) = 0;

		protected:
			
      /// Post processing
      virtual int _post_process(const base_tracking_driver::hit_collection_type & gg_hits_,
                                const base_tracking_driver::calo_hit_collection_type & calo_hits_,
                                snemo::datamodel::tracker_clustering_data & clustering_,
																snemo::datamodel::tracker_trajectory_data & track_fitting_);

      /// Post processing to collect unclustered hits
      void _post_process_collect_unclustered_hits(const base_tracking_driver::hit_collection_type & gg_hits_,
                                                  snemo::datamodel::tracker_clustering_data & clustering_);

    protected:
      
      datatools::logger::priority _verbosity = datatools::logger::priority::PRIO_FATAL; //!< Logging priority

    private:

      bool _initialized_ = false; //!< Initialization status
      std::string _id_; //!< Identifier of the clusterizer algorithm
			snemo::processing::detector_description _detector_desc_;

      // Internal work space:
      geomtools::id_selector _cellSelector_; //!< A selector of GIDs
      snreco::detail::GeigerTimePartitioner _preClusterizer_; //!< The time-clustering algorithm
      snemo::time::time_point _eventTimestamp_ = time::invalid_point() ; //!< Current event timestamp

		private:
			
      hit_collection_type _ignoredHits_;  //!< Hits not used as input for any clustering algorithm
      std::vector<tracking_precluster> _preclusters_; //!< Preclusters of input hits
			
    };

  } // end of namespace processing

} // end of namespace snemo

#endif // FALAISE_SNEMO_PROCESSING_BASE_TRACKING_DRIVER_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
