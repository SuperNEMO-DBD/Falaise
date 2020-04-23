// simulation_ctrl.cc

// http://drdobbs.com/cpp/184401518

// Ourselves:
#include <mctools/g4/simulation_ctrl.h>

// Standard library:
#include <cstdlib>

// Third party:
// - Boost:
#include <boost/bind.hpp>
// - Bayeux/datatools:
#include <datatools/exception.h>

// This project:
#include <mctools/g4/manager.h>

namespace mctools {
namespace g4 {
simulation_ctrl::simulation_ctrl(manager& a_simulation_manager,
                                 uint32_t a_max_counts) {
  simulation_manager = 0;
  simulation_thread = 0;
  event_mutex = 0;
  event_available_condition = 0;
  event_availability_status = simulation_ctrl::NOT_AVAILABLE_FOR_G4;
  stop_requested = false;
  counts = 0;
  max_counts = a_max_counts;
  set_simulation_manager(a_simulation_manager);
  event_mutex = new boost::mutex;
  event_available_condition = new boost::condition;
}

simulation_ctrl::~simulation_ctrl() {
  stop_requested = true;
  if (event_mutex != 0) {
    boost::mutex::scoped_lock lock(*event_mutex);
    stop_requested = true;
    event_availability_status = simulation_ctrl::ABORT;
    event_available_condition->notify_one();
  }
  if(simulation_thread != 0) {
    simulation_thread->join();
  }
  if (event_available_condition != 0) {
    delete event_available_condition;
    event_available_condition = 0;
  }
  if (event_mutex != 0) {
    delete event_mutex;
    event_mutex = 0;
  }
  if (simulation_thread) {
    delete simulation_thread;
    simulation_thread = 0;
  }
  simulation_manager = 0;
}

void simulation_ctrl::set_simulation_manager(manager & a_simulation_manager) {
  simulation_manager = &a_simulation_manager;
}

void simulation_ctrl::start() {
  simulation_thread = new boost::thread(boost::bind (&simulation_ctrl::start_simulation_run, this));
}

void simulation_ctrl::start_simulation_run() {
  {
    boost::mutex::scoped_lock lock (*event_mutex);
    while (event_availability_status == simulation_ctrl::NOT_AVAILABLE_FOR_G4) {
      event_available_condition->wait(*event_mutex);
    }
  }
  simulation_manager->run_simulation ();
}

void simulation_ctrl::set_stop_requested() {
  stop_requested = true;
}

bool simulation_ctrl::is_stop_requested() const {
  return stop_requested;
}

}  // end of namespace g4
}  // end of namespace mctools

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
