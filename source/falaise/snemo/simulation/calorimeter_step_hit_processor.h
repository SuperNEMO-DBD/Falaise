/// \file falaise/snemo/simulation/calorimeter_step_hit_processor.h
/* Author(s) :    Steven Calvez <calvez@lal.in2p3.fr>
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

// Third party:
// - Bayeux/mctools :
#include <mctools/calorimeter_step_hit_processor.h>

namespace snemo {

namespace geometry {
class locator_plugin;
}

namespace simulation {

/// \brief A basic processor of simulated step hits in SuperNEMO calorimeter blocks
class calorimeter_step_hit_processor : public mctools::calorimeter_step_hit_processor {
 public:
  /// Find the Gid of the calorimeter block at a given position
  virtual bool locate_calorimeter_block(const geomtools::vector_3d& position_,
                                        geomtools::geom_id& gid_) const;

  /// Main setup routine
  virtual void initialize(const ::datatools::properties& config,
                          ::datatools::service_manager& services);

 private:
  const snemo::geometry::locator_plugin* geoLocator_ = nullptr;  //!< SuperNEMO Locator plugin

  // Registration macro :
  MCTOOLS_STEP_HIT_PROCESSOR_REGISTRATION_INTERFACE(calorimeter_step_hit_processor)
};

}  // end of namespace simulation

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_SIMULATION_CALORIMETER_STEP_HIT_PROCESSOR_H
