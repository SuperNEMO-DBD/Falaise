/// \file falaise/snemo/digitization/digitization_module.h
/* Author(s)     : Guillaume Oliviero <goliviero@lpccaen.in2p3.fr>
 *                 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *                 Yves Lemiere <lemiere@lpccaen.in2p3.fr>
 * Creation date : 2016-12-02
 * Last modified : 2016-12-02
 *
 * Copyright (C) 2012-2016 Guillaume Oliviero <goliviero@lpccaen.in2p3.fr>
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
 * A module which goal is to process the SSD bank to the SDD bank (composed
 * by TD (trigger datas) and ROD (readout datas) after a step of digitization.
 *
 * History:
 *
 */

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_DIGITIZATION_MODULE_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_DIGITIZATION_MODULE_H

// Third party :
// - Bayeux/dpp:
#include <dpp/base_module.h>

// - Bayeux/mctools:
#include <mctools/signal/signal_data.h>

// - Bayeux/geomtools:
#include <bayeux/geomtools/manager.h>

namespace snemo {

namespace digitization {

class digitization_driver;
/// \brief Digitization module take simulated signal data bank in input
// and construct a simulated digitized data bank which contain trigger
// data and readout data
class digitization_module : public dpp::base_module {
 public:
  /// Constructor
  digitization_module(
      datatools::logger::priority logging_priority_ = datatools::logger::PRIO_FATAL);

  /// Destructor
  virtual ~digitization_module();

  /// Check the geometry manager
  bool has_geometry_manager() const;

  /// Address the geometry manager
  void set_geometry_manager(const geomtools::manager& gmgr_);

  /// Return a non-mutable reference to the geometry manager
  const geomtools::manager& get_geometry_manager() const;

  /// Initialization
  virtual void initialize(const datatools::properties& setup_,
                          datatools::service_manager& service_manager_,
                          dpp::module_handle_dict_type& module_dict_);
  /// Reset
  virtual void reset();

  /// Data record processing
  virtual process_status process(datatools::things& data_);

 protected:
  /// Measure particle track physical parameters such as charge, vertices
  void _process(const mctools::signal::signal_data& SSD_);

  /// Give default values to specific class members.
  void _set_defaults();

 private:
  std::string _SSD_label_;  //!< The label of the simulated data bank
  std::string _SDD_label_;  //!< The label of the simulated digitized data bank (output)

  const geomtools::manager* _geometry_manager_;  //!< The SuperNEMO geometry manager

  // Digitization driver :
  boost::scoped_ptr<snemo::digitization::digitization_driver> _driver_;

  // Macro to automate the registration of the module :
  DPP_MODULE_REGISTRATION_INTERFACE(digitization_module)
};
}  // end of namespace digitization

}  // end of namespace snemo

#include <datatools/ocd_macros.h>

// Declare the OCD interface of the module
DOCD_CLASS_DECLARATION(snemo::digitization::digitization_module)

#endif  // FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_DIGITIZATION_MODULE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
