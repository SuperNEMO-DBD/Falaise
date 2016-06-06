/* sultan_then_cat_driver.h
 * Author(s)     : Federico Nova <nova@physics.utexas.edu>
 * Creation date : 2014-07-17
 * Last modified : 2015-25-01
 *
 * Copyright (C) 2014 Federico Nova <nova@physics.utexas.edu>
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

#ifndef FALAISE_CAT_PLUGIN_SNEMO_RECONSTRUCTION_SULTAN_THEN_CAT_DRIVER_H
#define FALAISE_CAT_PLUGIN_SNEMO_RECONSTRUCTION_SULTAN_THEN_CAT_DRIVER_H 1

// This project
#include <string>
#include <CATAlgorithm/CAT_interface.h>
#include <sultan/SULTAN_interface.h>
#include <falaise/snemo/processing/base_tracker_clusterizer.h>

// Forward declaration :
namespace datatools {
  class properties;
}

namespace snemo {

  namespace geometry {
    class calo_locator;
    class xcalo_locator;
    class gveto_locator;
  }

  namespace reconstruction {

    /// Driver for the Channel/CellularAutomatonTracker (CAT) clustering algorithms
    class sultan_then_cat_driver : public ::snemo::processing::base_tracker_clusterizer
    {
    public:

      static const std::string SULTAN_THEN_CAT_ID;

      /// Set the mag field
      void set_magfield(double);

      /// Return the mag field
      double get_magfield() const;

      sultan_then_cat_driver ();

      virtual ~sultan_then_cat_driver ();

      /// Initialize the clusterizer through configuration properties
      virtual void initialize (const datatools::properties & setup_);

      /// Reset the clusterizer
      virtual void reset ();

      /// OCD support:
      static void init_ocd(datatools::object_configuration_description & ocd_);

      void convert_sultan_data_to_cat_data();
      void convert_cat_data_to_sultan_data();

      CAT::topology::cell fill_CAT_hit_from_SULTAN_hit(SULTAN::topology::cell sc);
      CAT::topology::calorimeter_hit fill_CAT_calo_hit_from_SULTAN_calo_hit(SULTAN::topology::calorimeter_hit sc);
      CAT::topology::cluster fill_CAT_cluster_from_SULTAN_sequence(SULTAN::topology::sequence ss);
      SULTAN::topology::cell fill_SULTAN_hit_from_CAT_hit(CAT::topology::cell cc);
      SULTAN::topology::calorimeter_hit fill_SULTAN_calo_hit_from_CAT_calo_hit(CAT::topology::calorimeter_hit cc);
      SULTAN::topology::cluster fill_SULTAN_cluster_from_CAT_sequence(CAT::topology::sequence cs);
      SULTAN::topology::scenario fill_SULTAN_scenario_from_CAT_scenario(CAT::topology::scenario cs);
      SULTAN::topology::sequence fill_SULTAN_sequence_from_CAT_sequence(CAT::topology::sequence cs);

    protected:

      /// Set default attributes
      void _set_defaults();

      /// Main clustering method
      virtual int _process_algo(const base_tracker_clusterizer::hit_collection_type & gg_hits_,
                                const base_tracker_clusterizer::calo_hit_collection_type & calo_hits_,
                                snemo::datamodel::tracker_clustering_data & clustering_);

    private:

      CAT::setup_data   _CAT_setup_;
      CAT::input_data   _CAT_input_;
      CAT::output_data  _CAT_output_;
      CAT::clusterizer  _CAT_clusterizer_;
      CAT::sequentiator _CAT_sequentiator_;

      SULTAN::setup_data   _SULTAN_setup_;
      SULTAN::input_data   _SULTAN_input_;
      SULTAN::output_data  _SULTAN_output_;
      SULTAN::clusterizer  _SULTAN_clusterizer_;
      SULTAN::sultan _SULTAN_sultan_;

      double               _sigma_z_factor_;     /// Factor for longitudinal error
      double               _magfield_;           /// Enforced magnetic field
      bool                 _process_calo_hits_;  /// Flag to process associated calorimeter hits

      /// Calorimeter locators
      const snemo::geometry::calo_locator  * _calo_locator_;
      const snemo::geometry::xcalo_locator * _xcalo_locator_;
      const snemo::geometry::gveto_locator * _gveto_locator_;

    };

  }  // end of namespace reconstruction


}  // end of namespace snemo

#include <datatools/ocd_macros.h>

// Declare the OCD interface of the module
DOCD_CLASS_DECLARATION(snemo::reconstruction::sultan_then_cat_driver)

#endif // FALAISE_CAT_PLUGIN_SNEMO_RECONSTRUCTION_SULTAN_THEN_CAT_DRIVER_H

// end of sultan_then_cat_driver.h


/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
