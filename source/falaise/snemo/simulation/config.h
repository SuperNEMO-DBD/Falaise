// falaise/snemo/simulation/config.h
/* Author(s) :    F. Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2022-11-29
 * Last modified: 2022-12-05
 *
 * License: GPL3
 *
 * Description:
 *
 *   Simulation configuration utilities for SuperNEMO.
 *
 */

#ifndef FALAISE_SNEMO_SIMULATION_CONFIG_H
#define FALAISE_SNEMO_SIMULATION_CONFIG_H 1

// Standard library:
#include <string>

namespace snemo {

  namespace simulation {

    const std::string & default_simulation_tag();
    
  } // end of namespace simulation

} // end of namespace snemo

#endif // FALAISE_SNEMO_SIMULATION_CONFIG_H
