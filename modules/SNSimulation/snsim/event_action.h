/// \file snsim/event_action.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-04-10
 * Last modified: 2015-09-14
 *
 * License:
 *
 * Description:
 *
 *   G4 user event action class
 *
 * History:
 *
 */

#ifndef SNSIM_EVENT_ACTION_H
#define SNSIM_EVENT_ACTION_H 1

// Third party:
// - Geant4:
#include <G4UserEventAction.hh>

// This project:
#include <bayeux/mctools/base_step_hit_processor.h>
#include <bayeux/mctools/simulated_data.h>
#include <snsim/loggable_support.h>

namespace datatools {
// Forward declaration:
class properties;
}  // namespace datatools

namespace snsim {

// Forward declarations:
class run_action;
class detector_construction;

/// \brief Geant4 event action interface class
class event_action : public G4UserEventAction, public loggable_support {
 public:
  // typedef ::mctools::simulated_data sim_data_type;

  /// Check initialization flag
  bool is_initialized() const;

  /// Check aborted event flag
  bool is_aborted_event() const;

  /// Set aborted event flag
  void set_aborted_event(bool = true);

  /// Check akilled event flag
  bool is_killed_event() const;

  /// Set killed event flag
  void set_killed_event(bool = true);

  /// Set external event data
  void set_external_event_data(mctools::simulated_data &a_external_event_data);

  /// Set the flag to save only tracked/unkilled events
  bool is_save_only_tracked_events() const;

  /// Return non mutable event data
  const mctools::simulated_data &get_event_data() const;

  /// Return mutable event data
  mctools::simulated_data &grab_event_data();

  /// Return non mutable run action
  const run_action &get_run_action() const;

  /// Return mutable run action
  run_action &grab_run_action();

  /// Constructor
  event_action(run_action &run_action_, const detector_construction &dctor_);

  /// Destructor
  virtual ~event_action();

  /// Initialization from a set of configuration parameters
  void initialize(const ::datatools::properties &config_);

  /// Reset
  void reset();

  // Geant4 interface :

  void BeginOfEventAction(const G4Event *);

  void EndOfEventAction(const G4Event *);

 private:
  /// Action performed at initialization
  static void _at_init_();

  /// Action performed at reset
  static void _at_reset_();

  /// Action performed at end of event
  void _process_sensitive_hits_(const G4Event *event_, bool &save_this_event_);

  /// Action performed at end of event
  void _save_data_();

  /// Action performed at end of event
  static void _clear_hits_collections_(const G4Event *);

  /// Action performed at end of event for multithreaded run
  void _mt_control_();

 private:
  // Management:
  bool _initialized_;  //!< Initialization flag

  // Configuration:
  bool _save_only_tracked_events_;  //!< Flag to save only tracked/unkilled event

  // Working data:
  const detector_construction *_detector_construction_;  //!< Handle to the G4 detector construction
  run_action *_run_action_;                              //!< Handle to the G4 run action
  mctools::simulated_data _event_data_;                  //!< Embedded simulated event model
  mctools::simulated_data *_external_event_data_;  //!< Handle to an external simulated event model
  bool _aborted_event_;                            //!< Flag to abort the current event
  bool _killed_event_;                             //!< Flag to kill the current event
  mctools::base_step_hit_processor::step_hit_ptr_collection_type
      _phits_;  //!< Collection of step hit processors
};

}  // namespace snsim

/// OCD support : interface
#include <bayeux/datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(snsim::event_action)

#endif  // SNSIM_EVENT_ACTION_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
