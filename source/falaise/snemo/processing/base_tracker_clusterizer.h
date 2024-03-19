/// \file falaise/snemo/processing/base_tracker_clusterizer.h
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
 *   Base class for tracker clusterizing algorithms
 *
 */
#ifndef FALAISE_SNEMO_PROCESSING_BASE_TRACKER_CLUSTERIZER_H
#define FALAISE_SNEMO_PROCESSING_BASE_TRACKER_CLUSTERIZER_H 1

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
  }

  namespace processing {

    /// \brief The base abstract class for all tracker clustering algorithms
    class base_tracker_clusterizer
    {
    public:
      // Typedefs
      using hit_type = snemo::datamodel::calibrated_tracker_hit;
      using hit_handle_type = snemo::datamodel::TrackerHitHdl;
      using hit_collection_type = snemo::datamodel::TrackerHitHdlCollection;
      using calo_hit_collection_type = snemo::datamodel::CalorimeterHitHdlCollection;

      /// Default constructor
      base_tracker_clusterizer(const std::string & name = "anonymous");

      /// Destructor
      virtual ~base_tracker_clusterizer();

      /// Set logging priority level
      void set_logging_priority(datatools::logger::priority logging_priority_);

      /// Get logging priority
      datatools::logger::priority get_logging_priority() const;

      /// Return the clusterizer ID
      const std::string & get_id() const;

      /// Return the tracker locator
      const snemo::geometry::gg_locator & get_gg_locator() const;

      /// Check the geometry manager
      bool has_geometry_manager() const;

      /// Address the geometry manager
      void set_geometry_manager(const geomtools::manager & gmgr_);

      /// Return a non-mutable reference to the geometry manager
      const geomtools::manager & get_geometry_manager() const;

      /// Check if theclusterizer is initialized
      bool is_initialized() const;

      /// Main clustering process
      int process(const base_tracker_clusterizer::hit_collection_type & gg_hits_,
                  const base_tracker_clusterizer::calo_hit_collection_type & calo_hits_,
                  snemo::datamodel::tracker_clustering_data & clustering_);

      // Smart print
      void tree_dump(std::ostream & out_ = std::clog,
										 const std::string & title_ = "",
                     const std::string & indent_ = "",
										 bool inherit_ = false) const;

      /// Initialize the clusterizer through configuration properties
      virtual void initialize(const datatools::properties & setup_) = 0;

      /// Reset the clusterizer
      virtual void reset() = 0;

      /// OCD support
      static void ocd_support(datatools::object_configuration_description &,
                              const std::string & prefix_ = "");

      // Manage the timestamp associated to processed events
  
      bool has_event_timestamp() const;
  
      const snemo::time::time_point & get_event_timestamp() const;

      void set_event_timestamp(const snemo::time::time_point &);

      void reset_event_timestamp();

    protected:
      
      /// Initialize the clusterizer through configuration properties
      void _initialize(const datatools::properties & setup_);

      /// Reset the clusterizer
      void _reset();

      /// Set default attribute values
      void _set_defaults();

      /// Clear working arrays
      void _clear_working_arrays();

      /// Set the initialization flag
      void _set_initialized(bool);
  
      /// Prepare cluster for processing
      virtual int _prepare_process(const base_tracker_clusterizer::hit_collection_type & gg_hits_,
                                   const base_tracker_clusterizer::calo_hit_collection_type & calo_hits_,
                                   snemo::datamodel::tracker_clustering_data & clustering_);

      /// Specific clustering algorithm
      virtual int _process_algo(const base_tracker_clusterizer::hit_collection_type & gg_hits_,
                                const base_tracker_clusterizer::calo_hit_collection_type & calo_hits_,
                                snemo::datamodel::tracker_clustering_data & clustering_) = 0;

      /// Post processing
      virtual int _post_process(const base_tracker_clusterizer::hit_collection_type & gg_hits_,
                                const base_tracker_clusterizer::calo_hit_collection_type & calo_hits_,
                                snemo::datamodel::tracker_clustering_data & clustering_);

      /// Post processing to collect unclustered hits
      void _post_process_collect_unclustered_hits(const base_tracker_clusterizer::hit_collection_type & gg_hits_,
                                                  snemo::datamodel::tracker_clustering_data & clustering_);

    protected:
      
      datatools::logger::priority _logging_priority = datatools::logger::priority::PRIO_FATAL; //!< Logging priority

    private:

      bool isInitialized_ = false; //!< Initialization status
      std::string id_; //!< Identifier of the clusterizer algorithm
      const geomtools::manager * geoManager_ = nullptr; //!< The SuperNEMO geometry manager
      const snemo::geometry::gg_locator * geigerLocator_ = nullptr; //!< Locator for geiger cells
      geomtools::id_selector cellSelector_; //!< A selector of GIDs
      snreco::detail::GeigerTimePartitioner preClusterizer_; //!< The time-clustering algorithm

      // Internal work space:
      snemo::time::time_point eventTimestamp_ = time::invalid_point() ; //!< Current event timestamp
      hit_collection_type ignoredHits_;  //!< Hits not used as input for any clustering algorithm
      std::vector<hit_collection_type> promptClusters_; //!< Clusters of only prompt hits
      std::vector<hit_collection_type> delayedClusters_; //!< Clusters of only delayed hits
    };

  } // end of namespace processing

} // end of namespace snemo

#endif // FALAISE_SNEMO_PROCESSING_BASE_TRACKER_CLUSTERIZER_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
