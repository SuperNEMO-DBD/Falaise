// -*- mode: c++ ; -*-
/** \file falaise/snemo/simulation/calorimeter_step_hit_processor.h
 * Author(s) :    Steven Calvez <calvez@lal.in2p3.fr>
 * Creation date: 2014-04-15
 * Last modified: 2014-04-15
 *
 * License:
 *
 * Description:
 *
 *  A basic processor of simulated step hits in SuperNEMO calorimeter blocks.
 *
 *  This algorithm clusterizes Geant4 step hits belonging to calorimeter
 *  blocks. This object inherits most of the functionalities of the generic
 *  'calorimeter_step_hit_processor' from Bayeux/mctools library : it just
 *  overloads the search of block's geometry ID method by using geometry
 *  locators especially built for SuperNEMO.
 *
 *  CAUTION: this processor can manage only one geometry mapping category
 *  so care should be taken to attach only one geometry model (with the proper
 *  mapping category) to this processor.
 *
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_SIMULATION_CALORIMETER_STEP_HIT_PROCESSOR_H
#define FALAISE_SNEMO_SIMULATION_CALORIMETER_STEP_HIT_PROCESSOR_H 1

// - Bayeux/mctools :
#include <mctools/calorimeter_step_hit_processor.h>

// This project:
#include <falaise/snemo/geometry/calo_locator.h>
#include <falaise/snemo/geometry/xcalo_locator.h>
#include <falaise/snemo/geometry/gveto_locator.h>

namespace snemo {

  namespace simulation {

    class calorimeter_step_hit_processor : public mctools::calorimeter_step_hit_processor
    {
    public:

      /// Find the Gid of the calorimeter block at a given position
      virtual bool locate_calorimeter_block(const geomtools::vector_3d & position_,
                                            geomtools::geom_id & gid_) const;

      /// Main setup routine
      virtual void initialize (const ::datatools::properties & config_,
                               ::datatools::service_manager & service_mgr_);

    private:

      geometry::calo_locator  _calo_locator_;  //!< A fast locator for main wall calorimeters
      geometry::xcalo_locator _xcalo_locator_; //!< A fast locator for X-wall calorimeters
      geometry::gveto_locator _gveto_locator_; //!< A fast locator for gamma veto calorimeters

      // Registration macro :
      MCTOOLS_STEP_HIT_PROCESSOR_REGISTRATION_INTERFACE (calorimeter_step_hit_processor);
    };

  } // end of namespace simulation

} // end of namespace snemo

#endif // FALAISE_SNEMO_SIMULATION_CALORIMETER_STEP_HIT_PROCESSOR_H
