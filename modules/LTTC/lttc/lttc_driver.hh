/** \file lttc/lttc_driver.hh
 * Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2021-11-16
 * Last modified : 2021-11-16
 *
 * Copyright (C) 2021 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *   A driver class that wraps the Lttc tracker clustering algorithm.
 *
 */

#ifndef FALAISE_LTTC_PLUGIN_LTTC_DRIVER_HH
#define FALAISE_LTTC_PLUGIN_LTTC_DRIVER_HH 1

// Standard library:
#include <string>

// This project
#include <falaise/snemo/processing/base_tracker_clusterizer.h>
#include <lttc/lttc_interface.hh>

// Forward declaration :
namespace datatools {
  class properties;
}

namespace snemo {

  namespace geometry {
    class calo_locator;
    class xcalo_locator;
    class gveto_locator;
  }  // namespace geometry

  namespace reconstruction {

    /// Driver for the Lttc clustering algorithms
    class lttc_driver
      : public ::snemo::processing::base_tracker_clusterizer
    {
    public:
      static const std::string LTTC_ID;  /// The LTTC string identifier

      /// Default constructor
      lttc_driver();

      /// Destructor
      virtual ~lttc_driver();

      /// Initialize the clusterizer through configuration properties
      virtual void initialize(const datatools::properties& setup_);

      /// Reset the clusterizer
      virtual void reset();

      /// OCD support:
      static void init_ocd(datatools::object_configuration_description& ocd_);

    protected:
      /// Set default attributes
      void _set_defaults();

      /// Main clustering method
      virtual int _process_algo(const base_tracker_clusterizer::hit_collection_type & gg_hits_,
                                const base_tracker_clusterizer::calo_hit_collection_type & calo_hits_,
                                snemo::datamodel::tracker_clustering_data & clustering_);

    private:
      // lttc::setup_data _lttc_setup_;         ///< Configuration data
      // lttc::input_data _lttc_input_;         ///< Input data
      // lttc::output_data _lttc_output_;       ///< Output data
      // lttc::clusterizer _CAT_clusterizer_;   //< Lttc clusterizer
 
      // Geometry locators
      const snemo::geometry::calo_locator*  _calo_locator_ = nullptr; ///< main wall calorimeter locator
      const snemo::geometry::xcalo_locator* _xcalo_locator_ = nullptr; ///< X-wall calorimeter locator
      const snemo::geometry::gveto_locator* _gveto_locator_ = nullptr;
      
    };

  }  // end of namespace reconstruction

}  // end of namespace snemo

#include <datatools/ocd_macros.h>

// Declare the OCD interface of the module
DOCD_CLASS_DECLARATION(snemo::reconstruction::lttc_driver)

#endif // FALAISE_LTTC_PLUGIN_LTTC_DRIVER_HH

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
