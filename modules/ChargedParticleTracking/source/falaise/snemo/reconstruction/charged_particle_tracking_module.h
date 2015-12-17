/// \file falaise/snemo/reconstruction/charged_particle_tracking_module.h
/* Author(s)     : Mathieu Bongrand <bongrand@lal.in2p3.fr>
 *                 Xavier Garrido <garrido@lal.in2p3.fr>
 * Creation date : 2012-04-20
 * Last modified : 2014-06-05
 *
 * Copyright (C) 2012-2014 Xavier Garrido <garrido@lal.in2p3.fr>
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
 * A module which goal is to process 'tracker_trajectory' data in order to
 * extrapolate vertices position, to determine track curvature and finally to
 * associate particle track with calorimeter hits.
 *
 * History:
 *
 */

#ifndef FALAISE_CHARGEDPARTICLETRACKING_PLUGIN_RECONSTRUCTION_CHARGED_PARTICLE_TRACKING_MODULE_H
#define FALAISE_CHARGEDPARTICLETRACKING_PLUGIN_RECONSTRUCTION_CHARGED_PARTICLE_TRACKING_MODULE_H 1

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>
// - Bayeux/dpp:
#include <dpp/base_module.h>

namespace geomtools {
  class manager;
}

namespace snemo {

  namespace datamodel {
    class calibrated_data;
    class tracker_trajectory_data;
    class particle_track_data;
  }

  namespace reconstruction {

    class vertex_extrapolation_driver;
    class charge_computation_driver;
    class calorimeter_association_driver;
    class alpha_finder_driver;

    /// \brief Charged particle tracking module
    class charged_particle_tracking_module : public dpp::base_module
    {
    public:

      /// Setting Geometry manager
      void set_geometry_manager(const geomtools::manager & gmgr_);

      /// Getting Geometry manager
      const geomtools::manager & get_geometry_manager() const;

      /// Constructor
      charged_particle_tracking_module(datatools::logger::priority = datatools::logger::PRIO_FATAL);

      /// Destructor
      virtual ~charged_particle_tracking_module();

      /// Initialization
      virtual void initialize(const datatools::properties  & setup_,
                              datatools::service_manager   & service_manager_,
                              dpp::module_handle_dict_type & module_dict_);

      /// Reset
      virtual void reset();

      /// Data record processing
      virtual process_status process(datatools::things & data_);

    protected:

      /// Measure particle track physical parameters such as charge, vertices
      void _process(const snemo::datamodel::calibrated_data         & calibrated_data_,
                    const snemo::datamodel::tracker_trajectory_data & tracker_trajectory_data_,
                    snemo::datamodel::particle_track_data           & particle_track_data_);

      /// Post-processing of particle track data
      void _post_process(const snemo::datamodel::calibrated_data & calibrated_data_,
                         snemo::datamodel::particle_track_data   & particle_track_data_);

      /// Give default values to specific class members.
      void _set_defaults();

    private:

      const geomtools::manager * _geometry_manager_; //!< The geometry manager

      std::string _CD_label_;  //!< The label of the calibrated data bank
      std::string _TTD_label_; //!< The label of the tracker trajectory data bank
      std::string _PTD_label_; //!< The label of the particle track data bank

      /// Vertex Extrapolation Driver :
      boost::scoped_ptr<snemo::reconstruction::vertex_extrapolation_driver> _VED_;

      /// Charge Computation Driver :
      boost::scoped_ptr<snemo::reconstruction::charge_computation_driver> _CCD_;

      /// Calorimeter Association Driver :
      boost::scoped_ptr<snemo::reconstruction::calorimeter_association_driver> _CAD_;

      /// Alpha Finder Driver :
      boost::scoped_ptr<snemo::reconstruction::alpha_finder_driver> _AFD_;

      // Macro to automate the registration of the module :
      DPP_MODULE_REGISTRATION_INTERFACE(charged_particle_tracking_module);

    };

  } // namespace reconstruction

} // namespace snemo

#include <datatools/ocd_macros.h>

// Declare the OCD interface of the module
DOCD_CLASS_DECLARATION(snemo::reconstruction::charged_particle_tracking_module)

#endif // FALAISE_CHARGEDPARTICLETRACKING_PLUGIN_RECONSTRUCTION_CHARGED_PARTICLE_TRACKING_MODULE_H

// end of snemo/reconstruction/charged_particle_tracking_module.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
