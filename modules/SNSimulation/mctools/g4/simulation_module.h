/// \file mctools/g4/simulation_module.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-07-04
 * Last modified : 2013-12-13
 *
 * Description:
 *
 * Copyright (C) 2011-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 */

#ifndef SNSIM_SIMULATION_MODULE_H
#define SNSIM_SIMULATION_MODULE_H 1

// Third party:
// - Bayeux/dpp :
#include <dpp/base_module.h>
#include <mygsl/prng_state_manager.h>
#include <mygsl/seed_manager.h>

// This project:
#include <mctools/g4/manager_parameters.h>

namespace geomtools {
// Forward declaration :
class manager;
}  // namespace geomtools

namespace snsim {
// Forward declaration :
class simulated_data;

// Foward declarations
class manager;
class simulation_ctrl;

/// \brief The Geant4 simulation module based on Bayeux/dpp API
class simulation_module : public dpp::base_module {
 public:
  /// Constructor
  simulation_module(datatools::logger::priority logging_priority = datatools::logger::PRIO_FATAL);

  /// Destructor
  virtual ~simulation_module();

  /// Initialization
  virtual void initialize(const datatools::properties& /* config_ */,
                          datatools::service_manager& /* service_mgr_ */,
                          dpp::module_handle_dict_type& /* modules_map_ */);

  /// Reset
  virtual void reset();

  /// Read/Process/Fill/Delete data
  virtual dpp::base_module::process_status process(datatools::things& /* event_record_ */);

  /// Set the geometry service label (only used if no geometry manager is provided)
  void set_geo_label(const std::string&);

  /// Get the geometry service label (only used if no geometry manager is provided)
  const std::string& get_geo_label() const;

  /// Set the SD bank label
  void set_sd_label(const std::string&);

  /// Get the SD bank label
  const std::string& get_sd_label() const;

  /// Set the reference to an external geometry manager
  void set_geometry_manager(const geomtools::manager& geometry_manager_);

  /// Set the simulation manager parameters
  void set_geant4_parameters(const manager_parameters& params);

  /// Return a non mutable reference to the simulation manager parameters
  const manager_parameters& get_geant4_parameters() const;

  /// Return a reference to the embedded seed manager
  const mygsl::seed_manager& get_seed_manager() const;

  /// Return a non mutable reference to the manager of PRNG's states
  const mygsl::prng_state_manager& get_state_manager() const;

 protected:
  void _initialize_manager(datatools::service_manager& /* smgr_ */);

  void _terminate_manager();

  int _simulate_event(datatools::things& /* event_record_ */);

 private:
  std::string geometryServiceName_;  //!< The label of the Geometry service to be accessed
  std::string simdataBankName_;      //!< The label of the 'simulated_data' bank in the event record

  const geomtools::manager* geometryManagerRef_;  //!< Non-mutable reference to the geometry manager

  manager_parameters
      geant4Parameters_;       //!< The configuration parameters for the GEANT4 simulation manager
  manager* geant4Simulation_;  //!< The embedded GEANT4 simulation manager
  simulation_ctrl*
      geant4SimulationController_;  //!< The embedded control object for thread synchronization

  // Registration of the module :
  DPP_MODULE_REGISTRATION_INTERFACE(simulation_module)
};

}  // namespace snsim

// Object configuration description (OCD) support :
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(snsim::simulation_module)

#endif  // SNSIM_SIMULATION_MODULE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
