/** \file falaise/snemo/processing/base_tracker_clusterizer.h
 * Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2012-03-29
 * Last modified : 2014-02-07
 *
 * Copyright (C) 2012-2014 François Mauger <mauger@lpccaen.in2p3.fr>
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
 *
 * History:
 */
#ifndef FALAISE_SNEMO_PROCESSING_BASE_TRACKER_CLUSTERIZER_H
#define FALAISE_SNEMO_PROCESSING_BASE_TRACKER_CLUSTERIZER_H 1

// Standard library:
#include <map>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux/datatools:
#include <datatools/logger.h>
#include <datatools/object_configuration_description.h>

// Falaise:
#include <snemo/datamodels/calibrated_data.h>

// Falaise module:
#include <TrackerPreClustering/pre_clusterizer.h>

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
      typedef snemo::datamodel::calibrated_tracker_hit                           hit_type;
      typedef snemo::datamodel::calibrated_tracker_hit::handle_type              hit_handle_type;
      typedef snemo::datamodel::calibrated_data::tracker_hit_collection_type     hit_collection_type;
      typedef snemo::datamodel::calibrated_data::calorimeter_hit_collection_type calo_hit_collection_type;

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

      /// Default constructor
      base_tracker_clusterizer(const std::string & id_ = "anonymous");

      /// Destructor
      virtual ~base_tracker_clusterizer();

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

    protected :

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

      // /// Post processing of the ignored hits
      // void _post_process_ignored_hits(snemo::datamodel::tracker_clustering_data & clustering_);

      /// Post processing to collect unclustered hits
      void _post_process_collect_unclustered_hits(const base_tracker_clusterizer::hit_collection_type & gg_hits_,
                                                  snemo::datamodel::tracker_clustering_data & clustering_);

    protected:

      datatools::logger::priority _logging_priority; /// Logging priority

    private:

      bool                                  _initialized_;           /// Initialization status
      std::string                           _id_;                    /// Identifier of the clusterizer algorithm
      const geomtools::manager *            _geometry_manager_;      /// The SuperNEMO geometry manager
      const snemo::geometry::gg_locator *   _gg_locator_;            /// Locator dedicated to the SuperNEMO tracking chamber
      TrackerPreClustering::setup_data      _tpc_setup_data_;        /// The configuration data for the time-clustering algorithm
      TrackerPreClustering::pre_clusterizer _pc_;                    /// The time-clustering algorithm

      // Internal work space:
      hit_collection_type                   _ignored_hits_;          /// Hits that are not used as input for any clustering algorithm
      std::vector<hit_collection_type>      _prompt_time_clusters_;  /// Collection of pre-clusters of only delayed hits
      std::vector<hit_collection_type>      _delayed_time_clusters_; /// Collection of pre-clusters of only delayed hits

    };

  }  // end of namespace processing

}  // end of namespace snemo

#endif // FALAISE_SNEMO_PROCESSING_BASE_TRACKER_CLUSTERIZER_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
