// event_action.cc

// Ourselves:
#include <snsim/event_action.h>

// Standard library:
#include <cstdlib>

// Third party:
// - Boost:
#include <boost/algorithm/string/predicate.hpp>
// - Geant4:
#include <G4ParticleTable.hh>
#include <globals.hh>

// In C++11, no register keyword, remove once updated to G4 10.2
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-register"
#endif
#include <G4RunManager.hh>
#ifdef __clang__
#pragma clang diagnostic pop
#endif

#include <G4Event.hh>
#include <G4EventManager.hh>
#include <G4Trajectory.hh>
#include <G4TrajectoryContainer.hh>
#include <G4VVisManager.hh>
#include <G4ios.hh>
// - Bayeux/datatools:
#include <datatools/ioutils.h>
#include <datatools/properties.ipp>

// This project:
#include <mctools/biasing/primary_event_bias.h>
#include <snsim/detector_construction.h>
#include <snsim/manager.h>
#include <snsim/run_action.h>
#include <snsim/sensitive_detector.h>
#include <snsim/simulation_ctrl.h>
#include <mctools/simulated_data.ipp>

namespace snsim {

auto event_action::get_run_action() const -> const run_action& { return *_run_action_; }

auto event_action::grab_run_action() -> run_action& { return *_run_action_; }

auto event_action::get_event_data() const -> const mctools::simulated_data& {
  if (_external_event_data_ != nullptr) {
    return *_external_event_data_;
  }
  return _event_data_;
}

auto event_action::grab_event_data() -> mctools::simulated_data& {
  if (_external_event_data_ != nullptr) {
    return *_external_event_data_;
  }
  return _event_data_;
}

void event_action::set_external_event_data(mctools::simulated_data& a_external_event_data) {
  _external_event_data_ = &a_external_event_data;
}

auto event_action::is_initialized() const -> bool { return _initialized_; }

event_action::event_action(run_action& run_action_, const detector_construction& dctor_) {
  _initialized_ = false;

  _save_only_tracked_events_ = true;

  _run_action_ = &run_action_;
  _run_action_->register_event_action(*this);
  _detector_construction_ = &dctor_;
  _aborted_event_ = false;
  _killed_event_ = false;
  _external_event_data_ = nullptr;
}

event_action::~event_action() {
  if (is_initialized()) {
    reset();
  }
  DT_LOG_TRACE(_logprio(), "Done.");
}

void event_action::reset() {
  DT_THROW_IF(!is_initialized(), std::logic_error, "Not initialized !");
  _initialized_ = false;
  _at_reset_();
  _save_only_tracked_events_ = true;
}

void event_action::_at_init_() {}

void event_action::_at_reset_() {}

void event_action::initialize(const datatools::properties& config_) {
  DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized !");

  loggable_support::_initialize_logging_support(config_);

  if (config_.has_key("event_model.hit_collection_type")) {
    const std::string event_model_collection_type =
        config_.fetch_string("event_model.hit_collection_type");
    mctools::simulated_data& event_data = this->grab_event_data();
    event_data.reset_collection_type();
    if (event_model_collection_type == "plain") {
      event_data.set_collection_type(mctools::simulated_data::PLAIN_HIT_COLLECTION_TYPE);
    } else if (event_model_collection_type == "handle") {
      event_data.set_collection_type(mctools::simulated_data::HANDLE_HIT_COLLECTION_TYPE);
    } else {
      DT_THROW_IF(true, std::logic_error,
                  "Invalid hit collection type '" << event_model_collection_type << "' !");
    }
  }

  if (config_.has_key("save_only_tracked_events")) {
    _save_only_tracked_events_ = config_.fetch_boolean("save_only_tracked_events");
  }

  // end of fetching.

  _at_init_();

  _initialized_ = true;
}

auto event_action::is_save_only_tracked_events() const -> bool {
  return _save_only_tracked_events_;
}

auto event_action::is_aborted_event() const -> bool { return _aborted_event_; }

void event_action::set_aborted_event(bool a_) { _aborted_event_ = a_; }

auto event_action::is_killed_event() const -> bool { return _killed_event_; }

void event_action::set_killed_event(bool k_) { _killed_event_ = k_; }

void event_action::BeginOfEventAction(const G4Event* event_) {
  DT_LOG_TRACE_ENTERING(_logprio());

  G4int event_id = event_->GetEventID();
  DT_LOG_DEBUG(_logprio(), "Event #" << event_id << " starts.");
  if (_run_action_->has_number_of_events_modulo()) {
    bool print_it = false;
    std::string message_head;
    if (!print_it && _run_action_->get_manager().get_number_of_events() != manager::NO_LIMIT &&
        event_id == (int)_run_action_->get_manager().get_number_of_events() - 1) {
      print_it = true;
      message_head = "Last event #";
    }
    if (!print_it && (event_id % _run_action_->get_number_of_events_modulo()) == 0) {
      print_it = true;
      message_head = "Event #";
    }
    if (print_it) {
      // Trace explicitely the event number :
      DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE, message_head << event_id);
    }
  }
  if (is_aborted_event()) {
    G4RunManager::GetRunManager()->AbortEvent();
    DT_LOG_WARNING(_logprio(), "Event #" << event_id << " is aborted. Exiting.");
    DT_LOG_TRACE_EXITING(_logprio());
    return;
  }

  /*
    if (G4RunManager::GetRunManager ()->runAborted)
    {
    G4RunManager::GetRunManager ()->AbortEvent ();
    }
  */

  bool record_prng_states = true;
  if (record_prng_states) {
    const manager& const_mgr = _run_action_->get_manager();
    if (const_mgr.has_prng_state_save_modulo()) {
      DT_LOG_DEBUG(_logprio(), "Record PRNG states for event #" << event_id);
      if ((event_id % const_mgr.get_prng_state_save_modulo()) == 0) {
        auto* mgr = const_cast<manager*>(&const_mgr);
        mgr->record_current_prng_states();
      }
    }
  }

  // BUGFIX: FM+AC 2014-09-05:
  // In case the manager record the track history of some previous event,
  // we clean it:
  if (_run_action_->get_manager().has_track_history()) {
    _run_action_->grab_manager().grab_track_history().reset();
  }

  if (_run_action_->get_manager().using_time_stat()) {
    _run_action_->grab_manager().grab_CT_map()["EA"].start();
  }

  DT_LOG_TRACE_EXITING(_logprio());
}

void event_action::EndOfEventAction(const G4Event* event_) {
  DT_LOG_TRACE_ENTERING(_logprio());

  if (_run_action_->get_manager().using_time_stat()) {
    _run_action_->grab_manager().grab_CT_map()["EA"].stop();
  }

  G4int event_id = event_->GetEventID();

  if (is_aborted_event()) {
    DT_LOG_ERROR(_logprio(), "Event #" << event_id << " is aborted.");
    return;
  }

  if (is_killed_event()) {
    DT_LOG_DEBUG(_logprio(), "Event #" << event_id << " is killed and thus not tracked!");
    // grab_event_data().grab_properties().store_flag(mctools::biasing::primary_event_bias::biased_event_status_key(),
    //                                                mctools::biasing::primary_event_bias::killed_event_label());
  }

  if (!is_killed_event()) {
    bool draw_trajectory = _run_action_->get_manager().is_interactive();
    if (draw_trajectory) {
      // get number of stored trajectories:
      G4TrajectoryContainer* trajectory_col = event_->GetTrajectoryContainer();
      int n_trajectories = 0;
      if (trajectory_col != nullptr) {
        n_trajectories = trajectory_col->entries();
      }

      // extract the trajectories and draw them:
      if (G4VVisManager::GetConcreteInstance() != nullptr) {
        for (int i = 0; i < n_trajectories; i++) {
          auto* trj = (G4Trajectory*)((*(event_->GetTrajectoryContainer()))[i]);
          trj->DrawTrajectory();
        }
      }
    }
  }

  // Flag to discard events without sensitive hits:
  bool save_this_event = !_run_action_->get_manager().dont_save_no_sensitive_hit_events();

  if (!is_killed_event()) {
    _process_sensitive_hits_(event_, save_this_event);
  } else {
    if (is_save_only_tracked_events()) {
      // We don't save killed events:
      save_this_event = false;
    }
  }

  DT_LOG_DEBUG(_logprio(), "Event data #" << event_id);
  DT_LOG_DEBUG(_logprio(), "Save this event = " << std::boolalpha << save_this_event);
  if (is_debug() && save_this_event) {
    get_event_data().tree_dump(std::clog, "Saved event: ", "[debug]: ");
  }

  _run_action_->increment_number_of_processed_events();

  // Save output simulated data:
  if (_run_action_->save_data() && save_this_event) {
    _save_data_();
    DT_LOG_DEBUG(_logprio(), "Event saved.");
  }

  // Clear the working sensitive hit collections:
  _clear_hits_collections_(event_);

  // Multi-thread control:
  _mt_control_();

  DT_LOG_TRACE_EXITING(_logprio());
}  // EndOfEventAction

void event_action::_mt_control_() {
  bool must_abort_run = false;

  // External threaded run control :
  if (_run_action_->get_manager().has_simulation_ctrl()) {
    DT_LOG_TRACE(_logprio(), "Using external run control...");
    simulation_ctrl& SimCtrl = _run_action_->grab_manager().grab_simulation_ctrl();
    {
      DT_LOG_TRACE(_logprio(), "Acquire the event control lock...");
      boost::mutex::scoped_lock lock(*SimCtrl.event_mutex);
      DT_LOG_TRACE(_logprio(), "Wait for event control to be available again...");
      while (SimCtrl.event_availability_status != simulation_ctrl::AVAILABLE_FOR_G4) {
        DT_LOG_TRACE(_logprio(), "Not yet...");
        SimCtrl.event_available_condition->wait(*SimCtrl.event_mutex);
      }
      DT_LOG_TRACE(_logprio(), "Ok let's go on...");
      if (SimCtrl.event_availability_status == simulation_ctrl::ABORT) {
        must_abort_run = true;
      }
      SimCtrl.event_availability_status = simulation_ctrl::NOT_AVAILABLE_FOR_G4;
      SimCtrl.counts++;
      DT_LOG_TRACE(_logprio(), "Notify the external simulation run manager...");
      SimCtrl.event_available_condition->notify_one();
    }

    // Wait for the release of the event control by the external process :
    {
      DT_LOG_TRACE(_logprio(),
                   "Wait for the release of the event control by the external process...");
      boost::mutex::scoped_lock lock(*SimCtrl.event_mutex);
      while (SimCtrl.event_availability_status == simulation_ctrl::NOT_AVAILABLE_FOR_G4) {
        DT_LOG_TRACE(_logprio(), "Not yet...");
        SimCtrl.event_available_condition->wait(*SimCtrl.event_mutex);
      }
      DT_LOG_TRACE(_logprio(), "Ok ! The event control is released by the external process...");
      if (SimCtrl.event_availability_status == simulation_ctrl::ABORT) {
        DT_LOG_WARNING(_logprio(), "Detected an 'Abort' request from the external process...");
        must_abort_run = true;
      }
    }

    if (SimCtrl.is_stop_requested()) {
      DT_LOG_TRACE(_logprio(), "is_stop_requested...");
      must_abort_run = true;
    }

    if (SimCtrl.max_counts > 0 && (SimCtrl.counts > SimCtrl.max_counts)) {
      DT_LOG_TRACE(_logprio(), "Max counts was reached.");
      must_abort_run = true;
    }
  }

  // Abort run condition :
  if (must_abort_run) {
    G4RunManager::GetRunManager()->AbortRun();
  }
}

void event_action::_save_data_() {
  // Store the event if run action has this ability:
  _run_action_->store_data(get_event_data());
}

void event_action::_clear_hits_collections_(const G4Event* event_) {
  // Detach the hits collections from this event :
  G4HCofThisEvent* HCE = event_->GetHCofThisEvent();
  // if (HCE == nullptr) {
  // clog << datatools::io::devel
  //           << "event_action::EndOfEventAction: Detach '"
  //           << hcol->GetName () << "' hits collection"
  //           << endl;
  //}
  for (int i = 0; i < (int)HCE->GetCapacity(); i++) {
    G4VHitsCollection* hcol = HCE->GetHC(i);
    if (hcol != nullptr) {
      // clog << datatools::io::devel
      //           << "event_action::EndOfEventAction: Detach '"
      //           << hcol->GetName () << "' hits collection"
      //           << endl;
      HCE->AddHitsCollection(i, nullptr);
    }
  }
}

void event_action::_process_sensitive_hits_(const G4Event* event_, bool& save_this_event_) {
  // Process the list of sensitive hits:
  if (_run_action_->get_manager().using_time_stat()) {
    _run_action_->grab_manager().grab_CT_map()["HP"].start();
  }

  G4HCofThisEvent* HCE = event_->GetHCofThisEvent();

  DT_LOG_DEBUG(_logprio(), "List of sensitive hit collections : ");
  for (int i = 0; i < (int)HCE->GetCapacity(); i++) {
    G4VHitsCollection* hc = HCE->GetHC(i);
    if (hc != nullptr) {
      DT_LOG_DEBUG(_logprio(), "Hit collection '" << hc->GetName() << "' for sensitive detector '"
                                                  << hc->GetSDname() << "' @ " << hc);
    }
  }

  // Process the list of sensitive hits:
  if (_run_action_->get_manager().using_time_stat()) {
    _run_action_->grab_manager().grab_CT_map()["HP"].start();
  }

  DT_LOG_DEBUG(_logprio(), "List of sensitive hit collections : ");
  for (int i = 0; i < (int)HCE->GetCapacity(); i++) {
    G4VHitsCollection* hc = HCE->GetHC(i);
    if (hc != nullptr) {
      DT_LOG_DEBUG(_logprio(), "Hit collection '" << hc->GetName() << "' for sensitive detector '"
                                                  << hc->GetSDname() << "' @ " << hc);
    }
  }

  // Loop on the dictionnary of sensitive detectors:
  int public_sensitive_category_counter = 0;
  for (const auto& iSD : _detector_construction_->get_sensitive_detectors()) {
    const std::string& sensitive_category = iSD.first;
    sensitive_detector& the_detector = *iSD.second;

    DT_LOG_DEBUG(_logprio(),
                 "Processing hits from sensitive detector '" << sensitive_category << "'...");
    // sensitive_detector::make_hit_collection_name (sensitive_category)
    G4VHitsCollection* the_hits_collection = HCE->GetHC(the_detector.get_HCID());
    if (the_hits_collection == nullptr) {
      DT_LOG_DEBUG(_logprio(),
                   "No hit to process from sensitive detector '" << sensitive_category << "'...");
      continue;
    }

    /** Note:
     * MC hit categories with a name starting with "__" (double underscore)
     * are conventionally considered as 'private' hit categories, added by the user
     * for special purpose (debug, visualization...).
     * 'official' MC hit categories should never starts with "__" and should
     * be associated to some sensitive detector.
     */
    const bool is_private_category = boost::starts_with(sensitive_category, "__");
    if (!is_private_category) {
      public_sensitive_category_counter++;
    }

    auto* SHC = dynamic_cast<sensitive_hit_collection*>(the_hits_collection);
    DT_LOG_DEBUG(_logprio(), "Collection has " << SHC->GetSize() << " hits.");

    /** Search for some step hit processors attached to the
     * sensitive detector: one can have several step hit
     * processors associated to a single sensitive detector.
     */
    //_phits_.clear ();
    if (!the_detector.get_hit_processors().empty()) {
      // Collect a 'phits' vector of pointers on 'base step hits':
      ::mctools::base_step_hit_processor::step_hit_ptr_collection_type phits;
      // 2011-04-05 FM : this should accelerate the process a little bit :
      phits.reserve(SHC->get_hits().size());
      for (size_t ihit = 0; ihit < SHC->get_hits().size(); ihit++) {
        phits.push_back(&(SHC->grab_hits().at(ihit)->grab_hit_data()));
      }

      // 'phits' is used as the input of the 'process' method
      // from each processor attached to the sensitive
      // category/detector:
      for (auto& iproc : the_detector.grab_hit_processors()) {
        /*
          const string & hit_proc_name = iproc->first;
          cerr << "DEVEL: " << "snsim::event_action::EndOfEventAction: "
          << "hit_proc_name = `" << hit_proc_name << "'" << endl;
        */
        ::mctools::base_step_hit_processor* hit_proc = iproc.second;
        hit_proc->process(phits, grab_event_data());
      }
    } else {
      // No hit processor for this sensitive
      // category/detector: step hits are dropped.
    }
  }

  // One tags events with 'physical' hits (not private ones)
  if (public_sensitive_category_counter > 0) {
    DT_LOG_DEBUG(_logprio(),
                 "Tagging current event with 'physical' sensitive hits categories to be saved...");
    save_this_event_ = true;
  }

  if (_run_action_->get_manager().using_time_stat()) {
    _run_action_->grab_manager().grab_CT_map()["HP"].stop();
  }
}

}  // namespace snsim

/** Opening macro for implementation
 *  This macro must be used outside of any namespace.
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snsim::event_action, ocd_) {
  // The class name :
  ocd_.set_class_name("snsim::event_action");

  // The class terse description :
  ocd_.set_class_description("The Geant4 simulation mandatory event action");

  // The library the class belongs to :
  ocd_.set_class_library("mctools_g4");

  // The class detailed documentation :
  ocd_.set_class_documentation("This is Geant4 simulation engine embedded event action.      \n");

  logger::declare_ocd_logging_configuration(ocd_, "warning");

  // Additionnal configuration hints :
  ocd_.set_configuration_hints(
      "Typical configuration is::                                             \n"
      "                                                                       \n"
      " #@description Event action logging priority                           \n"
      " logging.priority : string = \"warning\"                               \n"
      "                                                                       \n");

  ocd_.set_validation_support(true);

  // Lock the description:
  ocd_.lock();

  // ... and we are done.
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()  // Closing macro for implementation

// Registration macro for class 'snsim::event_action' :
DOCD_CLASS_SYSTEM_REGISTRATION(snsim::event_action, "snsim::event_action")
