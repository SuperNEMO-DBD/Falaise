/** \file snemo/reconstruction/cat_tracker_clustering_module.h
 * Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-04-09
 * Last modified: 2014-02-22
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
 *   Module for Geiger hits clustering
 *
 * History:
 *
 */

#ifndef FALAISE_CAT_PLUGIN_SNEMO_RECONSTRUCTION_CAT_TRACKER_CLUSTERING_MODULE_H
#define FALAISE_CAT_PLUGIN_SNEMO_RECONSTRUCTION_CAT_TRACKER_CLUSTERING_MODULE_H 1

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>
// - Bayeux/dpp:
#include <dpp/base_module.h>

// This project:
#include <snemo/datamodels/calibrated_data.h>

namespace geomtools {
  class manager;
}

namespace snemo {

  namespace datamodel {
    class tracker_clustering_data;
  }

  namespace processing {
    class base_tracker_clusterizer;
  }

  namespace reconstruction {

    /// \brief Tracker clustering module using the CAT algorithm
    class cat_tracker_clustering_module : public dpp::base_module
    {

    public:

      /// Set the 'calibrated data' bank label
      void set_cd_label(const std::string &);

      /// Return the 'calibrated data' bank label
      const std::string & get_cd_label() const;

      /// Set the 'tracker clustering data' bank label
      void set_tcd_label(const std::string &);

      /// Return the 'tracker clustering data' bank label
      const std::string & get_tcd_label() const;

      /// Setting geometry manager
      void set_geometry_manager(const geomtools::manager & gmgr_);

      /// Getting geometry manager
      const geomtools::manager & get_geometry_manager() const;

      /// Constructor
      cat_tracker_clustering_module(datatools::logger::priority = datatools::logger::PRIO_FATAL);

      /// Destructor
      virtual ~cat_tracker_clustering_module();

      /// Initialization
      virtual void initialize(const datatools::properties  & setup_,
                              datatools::service_manager   & service_manager_,
                              dpp::module_handle_dict_type & module_dict_);

      /// Reset
      virtual void reset();

      /// Data record processing
      virtual process_status process(datatools::things & data_);

    protected:

      /// Main process function
      void _process(const snemo::datamodel::calibrated_data & calib_data_,
                    // const snemo::datamodel::calibrated_data::tracker_hit_collection_type & calibrated_tracker_hits_,
                    snemo::datamodel::tracker_clustering_data & clustering_data_);

      /// Give default values to specific class members.
      void _set_defaults();

    private:

      const geomtools::manager * _geometry_manager_; //!< The geometry manager
      std::string _CD_label_;                        //!< The label of the input calibrated data bank
      std::string _TCD_label_;                       //!< The label of the output tracker clustering data bank
      boost::scoped_ptr< ::snemo::processing::base_tracker_clusterizer> _driver_; //!< Handle to the embedded clustering algorithm with dynamic memory auto-deletion

      // Macro to automate the registration of the module :
      DPP_MODULE_REGISTRATION_INTERFACE(cat_tracker_clustering_module);

    };

  } // end of namespace reconstruction

} // end of namespace snemo

#include <datatools/ocd_macros.h>

// Declare the OCD interface of the module
DOCD_CLASS_DECLARATION(snemo::reconstruction::cat_tracker_clustering_module)

#endif // FALAISE_CAT_PLUGIN_SNEMO_RECONSTRUCTION_CAT_TRACKER_CLUSTERING_MODULE_H
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
