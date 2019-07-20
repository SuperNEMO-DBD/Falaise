/* \file falaise/snemo/visualization/visu_toy_module.h
 * Author(s)     : François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2014-04-24
 * Last modified : 2014-04-24
 *
 * Copyright (C) 2014 François Mauger <mauger@lpccaen.in2p3.fr>
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
 * A very simple visualization module for SuperNEMO event records.
 *
 * History:
 *
 */

#ifndef FALAISE_VISUTOY_PLUGIN_SNEMO_VISUALIZATION_VISU_TOY_MODULE_H
#define FALAISE_VISUTOY_PLUGIN_SNEMO_VISUALIZATION_VISU_TOY_MODULE_H 1

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

namespace visualization {

struct toy_display_driver;

/// \brief Visualization module for SuperNEMO event record
class visu_toy_module : public dpp::base_module {
 public:
  /// Setting geometry manager
  void set_geometry_manager(const geomtools::manager& gmgr_);

  /// Getting geometry manager
  const geomtools::manager& get_geometry_manager() const;

  /// Constructor
  visu_toy_module(datatools::logger::priority = datatools::logger::PRIO_FATAL);

  /// Destructor
  virtual ~visu_toy_module();

  /// Initialization
  virtual void initialize(const datatools::properties& setup_,
                          datatools::service_manager& service_manager_,
                          dpp::module_handle_dict_type& module_dict_);

  /// Reset
  virtual void reset();

  /// Data record processing
  virtual process_status process(datatools::things& data_);

 protected:
  /// Main process function
  process_status _process(datatools::things& data_);

  /// Give default values to specific class members.
  void _set_defaults();

 private:
  const geomtools::manager* _geometry_manager_;  //!< The geometry manager
  int _module_number_;                           /// Module number
  bool _interactive_;                            /// Interactive flag
  boost::scoped_ptr< ::snemo::visualization::toy_display_driver>
      _driver_;           //!< Handle to the embedded visualization driver
  int _event_counter_;    /// Event record counter
  int _min_event_count_;  /// Minimum event record count
  int _max_event_count_;  /// Maximum event record count
  int _usleep_safe_;      /// Time delay between the display of two events (usec)

  // Macro to automate the registration of the module :
  DPP_MODULE_REGISTRATION_INTERFACE(visu_toy_module)
};

}  // namespace visualization

}  // namespace snemo

#include <datatools/ocd_macros.h>

// Declare the OCD interface of the module
DOCD_CLASS_DECLARATION(snemo::visualization::visu_toy_module)

#endif  // FALAISE_VISUTOY_PLUGIN_SNEMO_VISUALIZATION_VISU_TOY_MODULE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
