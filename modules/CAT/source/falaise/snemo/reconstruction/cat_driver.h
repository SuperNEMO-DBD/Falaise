/** \file falaise/snemo/processing/cat_driver.h
 * Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2012-03-29
 * Last modified : 2014-02-21
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
 *   A driver class that wraps the Channel/CellularAutomatonTracker tracker clustering algorithm.
 *
 * History:
 *
 */

#ifndef FALAISE_CAT_PLUGIN_SNEMO_RECONSTRUCTION_CAT_DRIVER_H
#define FALAISE_CAT_PLUGIN_SNEMO_RECONSTRUCTION_CAT_DRIVER_H 1

// Standard library:
#include <string>

// This project
#include <falaise/snemo/processing/base_tracker_clusterizer.h>
#include <CATAlgorithm/CAT_interface.h>

// Forward declaration :
namespace datatools {
  class properties;
}

namespace snemo {

  namespace reconstruction {

    /// Driver for the Channel/CellularAutomatonTracker (CAT) clustering algorithms
    class cat_driver : public ::snemo::processing::base_tracker_clusterizer
    {
    public:

      static const std::string CAT_ID; /// The CAT string identifier

      /// Set the mag field
      void set_magfield(double);

      /// Return the mag field
      double get_magfield() const;

      /// Default constructor
      cat_driver();

      /// Destructor
      virtual ~cat_driver();

      /// Initialize the clusterizer through configuration properties
      virtual void initialize(const datatools::properties & setup_);

      /// Reset the clusterizer
      virtual void reset();

      /// Main clustering method
      virtual int _process_algo(const snemo::datamodel::calibrated_data::tracker_hit_collection_type & hits_,
                                snemo::datamodel::tracker_clustering_data & clustering_ );

      /// Post-processing
      //virtual int _post_process(const snemo::datamodel::calibrated_data::tracker_hit_collection_type & hits_,
      //                           snemo::datamodel::tracker_clustering_data & clustering_);

    private:

      CAT::setup_data   _CAT_setup_;        /// Configuration data
      CAT::input_data   _CAT_input_;        /// Input data
      CAT::output_data  _CAT_output_;       /// Output data
      CAT::clusterizer  _CAT_clusterizer_;  /// CAT clusterizer
      CAT::sequentiator _CAT_sequentiator_; /// CAT sequentiator
      double            _sigma_z_factor_;   /// Factor for longitudinal error
      double            _magfield_;         /// Enforced magnetic field

    };

  }  // end of namespace reconstruction

}  // end of namespace snemo

#endif // FALAISE_CAT_PLUGIN_SNEMO_RECONSTRUCTION_CAT_DRIVER_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
