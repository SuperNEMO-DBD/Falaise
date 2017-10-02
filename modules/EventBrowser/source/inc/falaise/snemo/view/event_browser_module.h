/* event_browser_module.h
 * Author(s)     : Xavier Garrido <garrido@lal.in2p3.fr>
 * Creation date : 2011-11-09
 * Last modified : 2011-11-09
 *
 * Copyright (C) 2011 Xavier Garrido <garrido@lal.in2p3.fr>
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
 * Event browser module
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_VISUALIZATION_VIEW_EVENT_BROWSER_MODULE_H
#define FALAISE_SNEMO_VISUALIZATION_VIEW_EVENT_BROWSER_MODULE_H 1

// Load the abstract module interface :
#include <dpp/base_module.h>

namespace datatools {
class things;
}

namespace geomtools {
class manager;
}

namespace snemo {

namespace visualization {

namespace view {

class event_browser;
class event_browser_ctrl;

class event_browser_module : public dpp::base_module {
 public:
  /// Setting geometry manager
  void set_geometry_manager(const geomtools::manager& gmgr_);

  /// Getting geometry manager
  const geomtools::manager& get_geometry_manager() const;

  /// Constructor
  event_browser_module(datatools::logger::priority = datatools::logger::PRIO_FATAL);

  /// Destructor
  virtual ~event_browser_module();

  /// Initialization
  virtual void initialize(const datatools::properties& setup_,
                          datatools::service_manager& service_manager_,
                          dpp::module_handle_dict_type& module_dict_);

  /// Reset
  virtual void reset();

  /// Data record processing
  virtual process_status process(datatools::things& data_);

 protected:
  void _initialize_event_browser();

  void _terminate_event_browser();

  process_status _show_event(datatools::things& event_record_);

 private:
  std::string _Geo_label_;  //!< The label of the Geometry service to be accessed
  const geomtools::manager* _geometry_manager_;  //!< Non-mutable reference to the geometry manager

  event_browser* _event_browser_;  //!< The embedded ROOT event browser
  event_browser_ctrl*
      _event_browser_ctrl_;  //!< The embedded control object for thread synchronization

  // Macro to automate the registration of the module :
  DPP_MODULE_REGISTRATION_INTERFACE(event_browser_module)
};

}  // end of namespace view

}  // end of namespace visualization

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_VISUALIZATION_VIEW_EVENT_BROWSER_MODULE_H

// end of event_browser_module.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
