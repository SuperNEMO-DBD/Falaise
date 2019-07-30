// -*- mode: c++ ; -*-
/** \file falaise/snemo/reconstruction/trackfit_tracker_fitting_module.h
 * Author(s) :    Xavier Garrido <garrido@lal.in2p3.fr>
 * Creation date: 2012-10-07
 * Last modified: 2014-02-28
 *
 * Copyright (C) 2011-2014 Xavier Garrido <garrido@lal.in2p3.fr>
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
 *
 * Description:
 *
 *   Module for Geiger hits fitting
 *
 * History:
 *
 */

#ifndef FALAISE_TRACKFIT_PLUGIN_SNEMO_RECONSTRUCTION_TRACKFIT_TRACKER_FITTING_MODULE_H
#define FALAISE_TRACKFIT_PLUGIN_SNEMO_RECONSTRUCTION_TRACKFIT_TRACKER_FITTING_MODULE_H 1

#include <memory>

// Third party:
// - Bayeux/dpp :
#include <dpp/base_module.h>



namespace geomtools {
class manager;
}

namespace snemo {

namespace datamodel {
class tracker_clustering_data;
class tracker_trajectory_data;
}  // namespace datamodel

namespace processing {
class base_tracker_fitter;
}

namespace reconstruction {

/// \brief The data processing module for the fitting of particles trajectories in the tracker
class trackfit_tracker_fitting_module : public dpp::base_module {
 public:
  /// Constructor
  trackfit_tracker_fitting_module(datatools::logger::priority = datatools::logger::PRIO_FATAL);

  /// Destructor
  virtual ~trackfit_tracker_fitting_module();

  /// Setting geometry manager
  void set_geometry_manager(const geomtools::manager& gmgr_);

  /// Getting geometry manager
  const geomtools::manager& get_geometry_manager() const;

  /// Initialization
  virtual void initialize(const datatools::properties& config,
                          datatools::service_manager& services,
                          dpp::module_handle_dict_type& /* unused */);

  /// Reset
  virtual void reset();

  /// Data record processing
  virtual process_status process(datatools::things& event);

 protected:
  /// Special method to process and generate trajectory data
  void _process(const snemo::datamodel::tracker_clustering_data& clusters,
                snemo::datamodel::tracker_trajectory_data& trajectories);

  /// Give default values to specific class members.
  void _set_defaults();

 private:
  const geomtools::manager* geoManager_;  //!< The geometry manager
  std::string TCDTag_;  //!< The label of the input tracker clustering data bank
  std::string TTDTag_;  //!< The label of the output tracker trajectory data bank
  std::unique_ptr<snemo::processing::base_tracker_fitter>
      fitterAlgo_;  //!< Handle to the embedded fitter algorithm with dynamic memory auto-deletion

  // Macro to automate the registration of the module :
  DPP_MODULE_REGISTRATION_INTERFACE(trackfit_tracker_fitting_module)
};

}  // end of namespace reconstruction

}  // end of namespace snemo

#include <datatools/ocd_macros.h>

// Declare the OCD interface of the module
DOCD_CLASS_DECLARATION(snemo::reconstruction::trackfit_tracker_fitting_module)

#endif  // FALAISE_TRACKFIT_PLUGIN_SNEMO_RECONSTRUCTION_TRACKFIT_TRACKER_FITTING_MODULE_H
