/** \file falaise/snemo/processing/cat_driver.h
 * Author(s)     : Xavier Garrido <garrido@lal.in2p3.fr>
 *                 Mathieu Bongrand <bongrand@lal.in2p3.fr>
 *                 Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2014-03-13
 * Last modified : 2014-03-13
 *
 * Copyright (C) 2012-2014 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *   A driver class that implements the MockTrackerClusterizer tracker clustering algorithm.
 *
 * History:
 *
 */

#ifndef FALAISE_MOCKTRACKERCLUSTERIZER_PLUGIN_SNEMO_RECONSTRUCTION_MOCK_TRACKER_CLUSTERING_DRIVER_H
#define FALAISE_MOCKTRACKERCLUSTERIZER_PLUGIN_SNEMO_RECONSTRUCTION_MOCK_TRACKER_CLUSTERING_DRIVER_H 1

// Standard library:
#include <string>

// This project
#include <falaise/snemo/processing/base_tracker_clusterizer.h>

// Forward declaration :
namespace datatools {
  class properties;
}

namespace snemo {

  namespace reconstruction {

    /// Driver for the MockTrackerClusterizer clustering algorithm
    class mock_tracker_clustering_driver : public ::snemo::processing::base_tracker_clusterizer
    {
    public:

      static const std::string MTC_ID; /// The MockTrackerClusterizer string identifier

      /// Default constructor
      mock_tracker_clustering_driver();

      /// Destructor
      virtual ~mock_tracker_clustering_driver();

      /// Initialize the clusterizer through configuration properties
      virtual void initialize(const datatools::properties & setup_);

      /// Reset the clusterizer
      virtual void reset();

      /// Main clustering method
      virtual int _process_algo(const snemo::datamodel::calibrated_data::tracker_hit_collection_type & hits_,
                                snemo::datamodel::tracker_clustering_data & clustering_ );

      // /// Post-processing
      // virtual int _post_process(const snemo::datamodel::calibrated_data::tracker_hit_collection_type & hits_,
      //                           snemo::datamodel::tracker_clustering_data & clustering_);

      void isolate_tracks(const snemo::datamodel::calibrated_data::tracker_hit_collection_type & hits_,
                          std::vector<snemo::datamodel::calibrated_data::tracker_hit_collection_type> & tracks_) const;

      /// Check if 2 tracker cells are neighbours
      bool are_neighbours(const geomtools::geom_id & tracker_hit_id1_,
                          const geomtools::geom_id & tracker_hit_id2_) const;

    private:

      unsigned int _max_row_distance_;   ///
      unsigned int _max_layer_distance_; ///
      unsigned int _max_sum_distance_;   ///

    };

  }  // end of namespace reconstruction

}  // end of namespace snemo

#include <datatools/ocd_macros.h>

// Declare the OCD interface of the module
DOCD_CLASS_DECLARATION(snemo::reconstruction::mock_tracker_clustering_driver)

#endif // FALAISE_MOCKTRACKERCLUSTERIZER_PLUGIN_SNEMO_RECONSTRUCTION_MOCK_TRACKER_CLUSTERING_DRIVER_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
