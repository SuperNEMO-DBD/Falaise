/// \file mctools/simulation_ctrl.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-07-04
 * Last modified : 2013-06-03
 *
 * Description:
 *
 * Copyright (C) 2011-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 */

#ifndef SNSIM_SIMULATION_CTRL_H
#define SNSIM_SIMULATION_CTRL_H 1

// Third party:
// - Boost :
#include <boost/cstdint.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>

// This project:
#include <snsim/loggable_support.h>

namespace snsim {

// Forward declarations
class manager;
class simulation_run;

/** Object that hosts control parameters and data structures (thread/mutex/condition)
 * for a multi-threaded running of the G4 simulation manager (simulation module)
 */
class simulation_ctrl : public loggable_support {
 public:
  enum event_availability_status_type { NOT_AVAILABLE_FOR_G4 = 0, AVAILABLE_FOR_G4 = 1, ABORT = 2 };

 public:
  /// Constructor
  simulation_ctrl(manager& a_simulation_manager, uint32_t a_max_counts = 0);

  /// Destructor
  ~simulation_ctrl();

  /// Set the Geant4 simulation manager
  void set_simulation_manager(manager& a_simulation_manager);

  /// Start
  void start();

  /// Start the simulation run
  /// As it's run inthe thread, shouldn't it be private?
  void start_simulation_run();

  /// Set the requested stop flag
  void set_stop_requested();

  /// Check the requested stop flag
  bool is_stop_requested() const;

 public:
  // Why are these public?
  manager* simulation_manager;
  boost::thread* simulation_thread;
  boost::mutex* event_mutex;
  boost::condition* event_available_condition;
  int event_availability_status;
  bool stop_requested;
  uint32_t counts;
  uint32_t max_counts;
};

}  // namespace snsim

#endif  // SNSIM_SIMULATION_CTRL_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
