/// \file mctools/g4/manager.cc
/*
 * Copyright (C) 2011-2016 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 */

// Ourselves:
#include <mctools/g4/manager.h>

// Standard library:
#include <ctime>
#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <map>

// Third party:
// - Boost:
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
// - CLHEP:
#include <CLHEP/Random/Random.h>
// - Bayeux/datatools:
#include <datatools/ioutils.h>
#include <datatools/multi_properties.h>
#include <datatools/service_manager.h>
#include <datatools/exception.h>
// - Bayeux/mygsl:
#include <mygsl/random_utils.h>
// - Bayeux/genvtx:
#include <genvtx/i_vertex_generator.h>
// - Bayeux/genbb_help:
#include <genbb_help/manager.h>
#include <genbb_help/i_genbb.h>

// This project:
#include <mctools/g4/run_action.h>
#include <mctools/g4/event_action.h>
#include <mctools/g4/detector_construction.h>
#include <mctools/g4/primary_generator.h>
#include <mctools/g4/physics_list.h>
#include <mctools/g4/tracking_action.h>
#include <mctools/g4/stepping_action.h>
#include <mctools/g4/stacking_action.h>
#include <mctools/g4/simulation_ctrl.h>
#include <mctools/g4/data_libraries.h>

// G4 stuff:
#include <globals.hh>
#include <G4ParticleDefinition.hh>
#include <G4UIterminal.hh>
#include <G4UImanager.hh>

// In C++11, no register keyword, remove once updated to G4 10.2
#ifdef G4MULTITHREADED
#include <G4MTRunManager.hh>
#else
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-register"
#endif
#include <G4RunManager.hh>
#ifdef __clang__
#pragma clang diagnostic pop
#endif
#endif


#ifdef G4UI_USE_TCSH
#include "G4UItcsh.hh" // For terminal tcsh use
// #pragma message("NOTE: G4UI_USE_TCSH is set")
#endif // G4UI_USE_TCSH

#ifdef G4VIS_USE
// #pragma message("NOTE: G4VIS_USE is set")
#include "G4VisExecutive.hh"
#endif // G4VIS_USE

namespace mctools {
  namespace g4 {

    //----------------------------------------------------------------------
    // Static constanst settings
    //----------------------------------------------------------------------
    const uint32_t manager::NUMBER_OF_EVENTS_UPPER_LIMIT;
    const uint32_t manager::NUMBER_OF_EVENTS_LOWER_LIMIT;
    const uint32_t manager::NUMBER_OF_EVENTS_WARNING_LIMIT;
    const uint32_t manager::NO_LIMIT;
    const uint32_t manager::DEFAULT_PRNG_STATES_SAVE_MODULO;

    auto manager::g4_manager_label() -> const std::string &
    {
      static const std::string _s("MGR");
      return _s;
    }

    auto manager::vertex_generator_label() -> const std::string &
    {
      static const std::string _s("VG");
      return _s;
    }

    auto manager::event_generator_label() -> const std::string &
    {
      static const std::string _s("EG");
      return _s;
    }

    auto manager::shpf_label() -> const std::string &
    {
      static const std::string _s("SHPF");
      return _s;
    }

    auto manager::default_prng_id() -> const std::string &
    {
      static const std::string _s("taus2");
      return _s;
    }

    auto manager::default_prng_states_file() -> const std::string &
    {
      static const std::string _s("mctools_g4_prng_states.data");
      return _s;
    }

    //----------------------------------------------------------------------
    // PUBLIC INTERFACE
    //----------------------------------------------------------------------
    manager::manager() {
      _initialized_           = false;
      _simulation_ctrl_label_ = "";
      _simulation_ctrl_       = nullptr;
      _service_manager_       = nullptr;
      _external_geom_manager_ = nullptr;
      _vertex_generator_      = nullptr;
      _event_generator_       = nullptr;
      _init_defaults();
    }

    manager::~manager() {
      // std::cerr << "DEVEL: mctools::g4::manager::DTOR: Entering..." << std::endl;
      if (_initialized_) {
        // std::cerr << "DEVEL: mctools::g4::manager::DTOR: Reset..." << std::endl;
        reset();
      }
      // std::cerr << "DEVEL: mctools::g4::manager::DTOR: Exiting." << std::endl;
    }

    void manager::initialize(const datatools::multi_properties & multi_config_) {
      DT_THROW_IF(_initialized_, std::logic_error, "Manager is already initialized !");
      _multi_config_ = &multi_config_;
      this->initialize_impl();
      _initialized_ = true;
    }

    void manager::reset() {
      // std::cerr << "DEVEL: mctools::g4::manager::reset: Entering..." << std::endl;
      DT_THROW_IF(!_initialized_, std::logic_error, "Manager is not initialized !");
      _initialized_ = false;
      this->reset_impl();
      _init_defaults();
      // std::cerr << "DEVEL: mctools::g4::manager::reset: Exiting." << std::endl;
    }

    void manager::run_simulation() {
      DT_THROW_IF(! _initialized_, std::logic_error, "Manager is not initialized !");
      DT_LOG_NOTICE(_logprio(),"Starting the simulation...");
      this->run_simulation_impl();
      DT_LOG_NOTICE(_logprio(),"Simulation stops !");
    }

    auto manager::is_initialized() const -> bool {
      return _initialized_;
    }

    auto manager::is_automatic() const -> bool {
      return is_batch() && _g4_macro_.empty();
    }

    //----------------------------------------------------------------------
    // Simulation Initializations (Geometry/Physics, possibly via services)
    //----------------------------------------------------------------------
    auto manager::has_service_manager() const -> bool {
      return _service_manager_ != nullptr;
    }

    void manager::set_service_manager(datatools::service_manager& sm) {
      DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
      _service_manager_ = &sm;
    }

    auto manager::grab_service_manager() -> datatools::service_manager & {
      DT_THROW_IF(_service_manager_ == nullptr, std::logic_error,
                  "No service manager is available !");
      return *_service_manager_;
    }

    auto manager::get_service_manager() const -> const datatools::service_manager & {
      DT_THROW_IF(_service_manager_ == nullptr, std::logic_error,
                  "No service manager is available !");
      return *_service_manager_;
    }

    auto manager::has_external_geom_manager() const -> bool {
      return _external_geom_manager_ != nullptr;
    }

    void manager::set_external_geom_manager(const geomtools::manager& gm) {
      DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
      DT_THROW_IF(_geom_manager_.is_initialized(),
                  std::logic_error,
                  "Embedded geometry manager is already initialized ! "
                  << "Cannot substitute another geometry manager !");
      DT_THROW_IF(!gm.is_initialized(),
                  std::logic_error,
                  "External geometry manager is not initialized !");
      _external_geom_manager_ = &gm;
    }

    auto manager::get_geom_manager() const -> const geomtools::manager & {
      if (has_external_geom_manager()) {
        DT_THROW_IF(! _external_geom_manager_->is_initialized(),
                    std::logic_error,
                    "External geometry manager is not initialized !");
        return *_external_geom_manager_;
      }
      DT_THROW_IF(! _geom_manager_.is_initialized(),
                  std::logic_error, "Geometry manager is not initialized !");
      return _geom_manager_;
    }

    //----------------------------------------------------------------------
    // Simulation User Actions (EvtGen, Run, Event etc)
    //----------------------------------------------------------------------
    void manager::set_event_generator_name(const std::string & eg_name_) {
      DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
      _eg_name_ = eg_name_;
    }

    void manager::set_event_generator_seed(int seed_) {
      if (_eg_prng_.is_initialized()) {
        DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
      }
      _eg_prng_seed_ = seed_;
    }

    auto manager::get_eg_manager() const -> const genbb::manager& {
      return _eg_manager_;
    }

    auto manager::grab_eg_manager() -> genbb::manager& {
      return _eg_manager_;
    }

    auto manager::has_event_generator() const -> bool {
      return _event_generator_ != nullptr;
    }

    auto manager::get_event_generator() const -> const genbb::i_genbb& {
      DT_THROW_IF(!this->has_event_generator(),
                  std::logic_error,
                  "No event generator is available !");
      return *_event_generator_;
    }

    void manager::set_vertex_generator_name(const std::string & vg_name_) {
      DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
      _vg_name_ = vg_name_;
    }

    void manager::set_vertex_generator_seed(int seed_) {
      if (_vg_prng_.is_initialized()) {
        DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
      }
      _vg_prng_seed_ = seed_;
    }

    auto manager::get_vg_manager() const -> const genvtx::manager& {
      return _vg_manager_;
    }

    auto manager::grab_vg_manager() -> genvtx::manager& {
      return _vg_manager_;
    }

    auto manager::has_vertex_generator() const -> bool {
      return _vertex_generator_ != nullptr;
    }

    auto manager::get_vertex_generator() const -> const genvtx::i_vertex_generator& {
      DT_THROW_IF(!this->has_vertex_generator(),
                  std::logic_error,
                  "No vertex generator is available !");
      return *_vertex_generator_;
    }

    auto manager::grab_user_event_action() -> event_action& {
      DT_THROW_IF(! _initialized_, std::logic_error, "Manager is not initialized !");
      return *_user_event_action_;
    }


    auto manager::get_number_of_events() const -> uint32_t {
      return _number_of_events_;
    }

    void manager::set_number_of_events(uint32_t n) {
      DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
      DT_THROW_IF(n < NUMBER_OF_EVENTS_LOWER_LIMIT,
                  std::domain_error,
                  "Invalid 'zero' number of events !");
      DT_THROW_IF(n > NUMBER_OF_EVENTS_UPPER_LIMIT,
                  std::domain_error,
                  "Invalid 'too large' number of events !");
      _number_of_events_ = n;
      if (_number_of_events_ > NUMBER_OF_EVENTS_WARNING_LIMIT && !has_simulation_ctrl()) {
        DT_LOG_WARNING(_logprio(), "Number of events is high = " << _number_of_events_);
      }
      DT_LOG_DEBUG(_logprio(), "Number of events = " << _number_of_events_);
    }


    auto manager::has_number_of_events_modulo() const -> bool {
      return _number_of_events_modulo_ > run_action::NUMBER_OF_EVENTS_MODULO_NONE;
    }

    auto manager::get_number_of_events_modulo() const -> int {
      return _number_of_events_modulo_;
    }

    void manager::set_number_of_events_modulo(int a_m) {
      if (a_m <= run_action::NUMBER_OF_EVENTS_MODULO_NONE) {
        _number_of_events_modulo_ = run_action::NUMBER_OF_EVENTS_MODULO_NONE;
      } else {
        _number_of_events_modulo_ = a_m;
      }
      DT_LOG_DEBUG(_logprio(), "Number of events modulo = " << _number_of_events_modulo_);
    }

    void manager::set_use_run_header_footer(bool a_use_run_header_footer) {
      _use_run_header_footer_ = a_use_run_header_footer;
    }

    auto manager::using_run_header_footer() const -> bool {
      return _use_run_header_footer_;
    }

    auto manager::is_batch() const -> bool {
      return ! is_interactive();
    }

    auto manager::is_interactive() const -> bool {
      return _interactive_;
    }

    void manager::set_interactive(bool new_value_) {
      DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
      _interactive_ = new_value_;
      if (! _interactive_) { _g4_visualization_ = false;
}
    }

    void manager::set_g4_visualization(bool new_value_) {
      DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
#ifdef G4VIS_USE
      _g4_visualization_ = new_value_;
      if (_g4_visualization_) { _interactive_ = true;
}
#else // ! G4VIS_USE
      _g4_visualization_ = false;
      if (new_value_) {
        DT_LOG_WARNING(_logprio(), "G4 visualization is not available !");
      }
#endif // G4VIS_USE
    }

    auto manager::has_g4_visualization() const -> bool {
      return is_interactive() && _g4_visualization_;
    }


    auto manager::has_g4_macro() const -> bool {
      return ! _g4_macro_.empty();
    }

    auto manager::get_g4_macro() const -> const std::string& {
      return _g4_macro_;
    }

    void manager::set_g4_macro(const std::string& g4_macro_) {
      DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
      _g4_macro_ = g4_macro_;
    }

    void manager::set_use_track_history(const bool use) {
      _use_track_history_ = use;
    }

    auto manager::has_track_history() const -> bool {
      return _use_track_history_;
    }

    auto manager::get_track_history() const -> const track_history& {
      return _track_history_;
    }

    auto manager::grab_track_history() -> track_history& {
      return _track_history_;
    }

    auto manager::forbids_private_hits() const -> bool {
      return _forbid_private_hits_;
    }

    void manager::set_forbid_private_hits(bool a_forbid) {
      DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
      _forbid_private_hits_ = a_forbid;
    }

    void manager::set_dont_save_no_sensitive_hit_events(bool a_dont_save) {
      DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
      _dont_save_no_sensitive_hit_events_ = a_dont_save;
    }

    auto manager::dont_save_no_sensitive_hit_events() const -> bool {
      return _dont_save_no_sensitive_hit_events_;
    }


    //----------------------------------------------------------------------
    // Multithreaded Control
    //----------------------------------------------------------------------
    auto manager::has_simulation_ctrl() const -> bool {
      return _simulation_ctrl_ != nullptr;
    }

    void manager::set_simulation_ctrl(simulation_ctrl& a_simulation_ctrl) {
      DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
      DT_THROW_IF(has_simulation_ctrl(),
                  std::logic_error,
                  "Operation prohibited ! Manager already got a 'simulation ctrl' object !");
      _simulation_ctrl_ = &a_simulation_ctrl;
    }

    void manager::reset_simulation_ctrl() {
      DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
      _simulation_ctrl_ = nullptr;
    }

    auto manager::get_simulation_ctrl() const -> const simulation_ctrl& {
      DT_THROW_IF(! has_simulation_ctrl(),
                  std::logic_error,
                  "Manager has no 'simulation ctrl' object !");
      return *_simulation_ctrl_;
    }

    auto manager::grab_simulation_ctrl() -> simulation_ctrl& {
      DT_THROW_IF(! has_simulation_ctrl(),
                  std::logic_error,
                  "Manager has no 'simulation ctrl' object !");
      return *_simulation_ctrl_;
    }

    //----------------------------------------------------------------------
    // Random number (over)control
    //----------------------------------------------------------------------
    auto manager::grab_vg_prng() -> mygsl::rng& {
      return _vg_prng_;
    }

    auto manager::grab_eg_prng() -> mygsl::rng& {
      return _eg_prng_;
    }

    auto manager::grab_shpf_prng() -> mygsl::rng& {
      return _shpf_prng_;
    }

    auto manager::get_mgr_prng() const -> const mygsl::rng& {
      return _mgr_prng_;
    }

    auto manager::grab_mgr_prng() -> mygsl::rng& {
      return _mgr_prng_;
    }

    auto manager::get_g4_prng() const -> const g4_prng& {
      return _g4_prng_;
    }

    auto manager::grab_g4_prng() -> g4_prng& {
      return _g4_prng_;
    }


    auto manager::get_seed_manager() const -> const mygsl::seed_manager& {
      return _seed_manager_;
    }

    auto manager::grab_seed_manager() -> mygsl::seed_manager& {
      return _seed_manager_;
    }

    auto manager::get_state_manager() const -> const mygsl::prng_state_manager& {
      return _prng_state_manager_;
    }

    auto manager::grab_state_manager() -> mygsl::prng_state_manager& {
      DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
      return _prng_state_manager_;
    }

    void manager::record_current_prng_states() {
      // G4 PRNG :
      _mgr_prng_.to_buffer(_prng_state_manager_.get_state(g4_manager_label()).state_buffer);

      // Vertex generator PRNG :
      _vg_prng_.to_buffer(_prng_state_manager_.get_state(vertex_generator_label()).state_buffer);

      // Event generator PRNG :
      _eg_prng_.to_buffer(_prng_state_manager_.get_state(event_generator_label()).state_buffer);

      // PRNG of the Step Hit Processor Factory :
      _shpf_prng_.to_buffer(_prng_state_manager_.get_state(shpf_label()).state_buffer);

      _prng_state_manager_.increment_counter(get_prng_state_save_modulo());
      if (is_debug()) { _prng_state_manager_.dump(std::clog);
}
      _prng_state_manager_.store();
    }




    void manager::set_prng_state_save_modulo(int a_modulo) {
      if (a_modulo < 1) {
        _prng_state_save_modulo_ = 0;
        return;
      }
      _prng_state_save_modulo_ = a_modulo;
    }

    auto manager::get_prng_state_save_modulo() const -> int {
      return _prng_state_save_modulo_;
    }

    auto manager::has_prng_state_save_modulo() const -> bool {
      return _prng_state_save_modulo_ > 0;
    }


    void manager::reset_output_prng_seeds_file() {
      _output_prng_seeds_file_ = "";
    }

    void manager::reset_input_prng_seeds_file() {
      _input_prng_seeds_file_ = "";
    }

    auto manager::has_input_prng_seeds_file() const -> bool {
      return ! _input_prng_seeds_file_.empty();
    }

    auto manager::get_input_prng_seeds_file() const -> const std::string& {
      return _input_prng_seeds_file_;
    }

    void manager::set_input_prng_seeds_file(const std::string & fn_) {
      _input_prng_seeds_file_ = fn_;
    }

    auto manager::has_output_prng_seeds_file() const -> bool {
      return ! _output_prng_seeds_file_.empty();
    }

    auto manager::get_output_prng_seeds_file() const -> const std::string& {
      return _output_prng_seeds_file_;
    }

    void manager::set_output_prng_seeds_file(const std::string & fn_) {
      _output_prng_seeds_file_ = fn_;
    }

    void manager::reset_output_prng_states_file() {
      _output_prng_states_file_ = "";
    }

    auto manager::get_output_prng_states_file() const -> const std::string & {
      return _output_prng_states_file_;
    }

    auto manager::has_output_prng_states_file() const -> bool {
      return ! _output_prng_states_file_.empty();
    }

    void manager::set_output_prng_states_file(const std::string & fn_) {
      _output_prng_states_file_ = fn_;
    }

    void manager::reset_input_prng_states_file() {
      _input_prng_states_file_ = "";
    }

    auto manager::get_input_prng_states_file() const -> const std::string & {
      return _input_prng_states_file_;
    }

    auto manager::has_input_prng_states_file() const -> bool {
      return ! _input_prng_states_file_.empty();
    }

    void manager::set_input_prng_states_file(const std::string & fn_) {
      _input_prng_states_file_ = fn_;
    }

    void manager::set_shpf_prng_seed(int seed_) {
      DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
      _shpf_prng_seed_ = seed_;
    }

    void manager::set_mgr_prng_seed(int rseed_) {
      DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
      if (rseed_ < 0) {
        _mgr_prng_seed_ = mygsl::random_utils::SEED_INVALID;
      } else {
        _mgr_prng_seed_ = rseed_;
      }
    }

    auto manager::has_mgr_prng_seed() const -> bool {
      return _mgr_prng_seed_ != mygsl::random_utils::SEED_INVALID;
    }

    auto manager::get_mgr_prng_seed() const -> int {
      return _mgr_prng_seed_;
    }

    auto manager::has_init_seed_method() const -> bool {
      return !_init_seed_method_.empty();
    }

    void manager::set_init_seed_method(const std::string & method_) {
      _init_seed_method_ = method_;
    }

    auto manager::get_init_seed_method() const -> const std::string & {
      return _init_seed_method_;
    }

    //----------------------------------------------------------------------
    // I/O (over)control
    //----------------------------------------------------------------------
    void manager::set_output_data_format_by_label(const std::string & ff_) {
      io_utils::data_format_type df = io_utils::label_to_data_format(ff_);
      DT_THROW_IF(df == io_utils::DATA_FORMAT_INVALID, std::logic_error, "Invalid output data format '" << ff_ << "'!");
      set_output_data_format(df);
    }

    void manager::reset_output_data_format() {
      _output_data_format_ = io_utils::DATA_FORMAT_INVALID;
    }

    void manager::set_output_data_format(io_utils::data_format_type of_) {
      _output_data_format_ = of_;
      if (_output_data_format_ == io_utils::DATA_FORMAT_BANK) {
        if (_output_data_bank_label_.empty()) {
          set_output_data_bank_label(event_utils::event_default_simulated_data_label());
        }
      }
    }

    void manager::set_output_data_bank_label(const std::string & bl_) {
      _output_data_bank_label_ = bl_;
      if (_output_data_format_ != io_utils::DATA_FORMAT_INVALID) {
        set_output_data_format(io_utils::DATA_FORMAT_BANK);
      }
      DT_THROW_IF(_output_data_format_ != io_utils::DATA_FORMAT_BANK, std::logic_error,
                  "Cannot set output data bank label with the plain output data format!");
    }

    auto manager::get_output_data_format() const -> io_utils::data_format_type {
      return _output_data_format_;
    }

    void manager::set_output_data_file(const std::string & fn_) {
      _output_data_file_ = fn_;
    }

    auto manager::using_time_stat() const -> bool {
      return _use_time_stat_;
    }

    void manager::set_using_time_stat(bool new_value_) {
      DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
      _use_time_stat_ = new_value_;
    }

    auto manager::get_CT_map() const -> const manager::CT_map & {
      return _CTs_;
    }

    auto manager::grab_CT_map() -> manager::CT_map& {
      return _CTs_;
    }

    //----------------------------------------------------------------------
    // Output Profiles
    //----------------------------------------------------------------------
    auto manager::has_supported_output_profile(const std::string & profile_id_) const -> bool {
      return _supported_output_profile_ids_.find(profile_id_) != _supported_output_profile_ids_.end();
    }

    void manager::add_supported_output_profile(const std::string& profile_id_,
                                               const std::string& description_) {
      DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
      std::string profile_id = profile_id_;
      boost::trim(profile_id);
      DT_THROW_IF(profile_id.empty(),
                  std::logic_error,
                  "Empty output profile Id !");
      DT_THROW_IF(profile_id.find('+') != profile_id.npos,
                  std::logic_error,
                  "Forbidden '+' character in the output profile Id '" << profile_id << "' !");
      DT_THROW_IF(profile_id.find(' ') != profile_id.npos,
                  std::logic_error,
                  "Forbidden space character in the output profile Id '" << profile_id << "' !");
      DT_THROW_IF(has_supported_output_profile(profile_id), std::logic_error,
                  "Supported profile with Id '" << profile_id << "' already exists !");
      _supported_output_profile_ids_[profile_id] = description_;
      DT_LOG_NOTICE(_logprio(), "Output profile '" << profile_id << "' is supported.");
    }

    auto manager::get_supported_output_profiles() const -> const std::map<std::string, std::string>& {
      return _supported_output_profile_ids_;
    }

    auto manager::has_activated_output_profiles() const -> bool {
      return !_activated_output_profile_ids_.empty();
    }

    auto manager::has_activated_output_profile(const std::string& id) const -> bool {
      return _activated_output_profile_ids_.count(id) == 1;
    }

    void manager::activate_output_profile(const std::string& id) {
      DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
      DT_THROW_IF(! has_supported_output_profile(id),
                  std::logic_error,
                  "Unsupported output profile '" << id << "' !");
      _activated_output_profile_ids_.insert(id);
      DT_LOG_NOTICE(_logprio(), "Activate the output profile '" << id << "'");
    }

    void manager::deactivate_output_profile(const std::string& id) {
      DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
      if (has_activated_output_profile(id)) {
        _activated_output_profile_ids_.erase(id);
      }
    }

    void manager::set_output_profiles_activation_rule(const std::string& rule_) {
      DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
      _output_profiles_activation_rule_ = rule_;
    }

    void manager::apply_output_profiles_activation_rule(const std::string& rule) {
      DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
      if (rule.empty()) { return;
}
      std::vector<std::string> activated_output_profile_ids;
      boost::split(activated_output_profile_ids, rule, boost::is_any_of("+"));
      for (size_t i {0}; i < activated_output_profile_ids.size(); ++i) {
        std::string profile_id = activated_output_profile_ids[i];
        boost::trim(profile_id);
        activate_output_profile(profile_id);
      }
    }

    void manager::fetch_activated_output_profile_ids(std::vector<std::string>& ids) const {
      for (const auto& i : _activated_output_profile_ids_) {
        ids.push_back(i);
      }
    }

    auto manager::get_activated_output_profile_ids() const -> const std::set<std::string>& {
      return _activated_output_profile_ids_;
    }


    //----------------------------------------------------------------------
    // Dump-To-Stream
    //----------------------------------------------------------------------
    void manager::dump(std::ostream& os) const {
      dump_base(os, "mctools::g4::manager: ", "");
    }

    void manager::dump_base(std::ostream& os,
                            const std::string& title,
                            const std::string& iIndent) const {
      std::string indent;
      if (!iIndent.empty()) { indent = iIndent;
}
      if (!title.empty()) {
        os << indent << title << std::endl;
      }
      os << indent << "|-- Debug:        " << is_debug() << std::endl;
      os << indent << "|-- Batch:        " << is_batch() << std::endl;
      os << indent << "|-- Interactive:  " << is_interactive() << std::endl;
      os << indent << "|-- Automatic:    " << is_automatic() << std::endl;
      os << indent << "|-- G4 visu.:     " << has_g4_visualization() << std::endl;
      os << indent << "|-- G4 macro:     '" << get_g4_macro() << "'" << std::endl;
      os << indent << "|-- No events:    " << get_number_of_events() << std::endl;
      os << indent << "|-- G4 PRNG seed: " << _mgr_prng_seed_ << " " << std::endl;
      os << indent << "|-- G4 PRNG name: '"
         << (get_mgr_prng().is_initialized()? get_mgr_prng().name(): "[none]")
         << "' " << std::endl;
      os << indent << "|-- PRNG seeds input file:   '"
         << _input_prng_seeds_file_ << "' " << std::endl;
      os << indent << "|-- PRNG seeds output file:  '"
         << _output_prng_seeds_file_ << "' " << std::endl;
      os << indent << "|-- PRNG states input file:  '"
         << _input_prng_states_file_ << "' " << std::endl;
      os << indent << "|-- PRNG states output file: '"
         << _output_prng_states_file_ << "' " << std::endl;
      os << indent << "|-- Supported output profiles = ";

      if (static_cast<unsigned int>(!_supported_output_profile_ids_.empty()) != 0U) {
        os << _supported_output_profile_ids_.size() << std::endl;
      } else {
        os << "<none>" << std::endl;
      }

      for (auto i = _supported_output_profile_ids_.begin();
           i != _supported_output_profile_ids_.end();
           ++i) {
        auto j = i;
        j++;
        os << "|   ";
        if (j == _supported_output_profile_ids_.end()) {
          os << "`-- ";
        } else {
          os << "|-- ";
        }
        os << "Profile : '" << i->first << "' : " << i->second << std::endl;
      }

      os << indent << "|-- Activated output profiles = ";
      if (static_cast<unsigned int>(!_activated_output_profile_ids_.empty()) != 0U) {
        os << _activated_output_profile_ids_.size() << std::endl;
      } else {
        os << "<none>" << std::endl;
      }

      for (auto i = _activated_output_profile_ids_.begin();
           i != _activated_output_profile_ids_.end();
           ++i) {
        auto j = i;
        j++;
        os << "|   ";
        if (j == _activated_output_profile_ids_.end()) {
          os << "`-- ";
        } else {
          os << "|-- ";
        }
        os << "Profile : '" << *i << std::endl;
      }
      os << indent << "`-- end" << std::endl;
    }



    //----------------------------------------------------------------------
    // Private implementation details
    //----------------------------------------------------------------------
    void manager::initialize_impl() {
      /****************
       * MAIN MANAGER *
       ****************/
      _init_core();

      /********************
       * GEOMETRY MANAGER *
       ********************/
      _init_geometry();

      /********************
       * VERTEX GENERATOR *
       ********************/
      _init_vertex_generator();

      /*******************
       * EVENT GENERATOR *
       *******************/
      _init_event_generator();

      /****************************
       * USER ACTIONS FOR GEANT 4 *
       ****************************/
      // Run manager:
#ifdef G4MULTITHREADED
      _g4_run_manager_ = new G4MTRunManager;
      _g4_run_manager_->SetNumberOfThreads(2);
#else
      _g4_run_manager_ = new G4RunManager;
#endif
      /*** User initializations ***/
      // Detector construction:
      _init_detector_construction ();

      // Physics list:
      _init_physics_list ();

#ifdef G4VIS_USE
      // G4 visualization:
      _g4_vis_manager_ = nullptr;
      if (has_g4_visualization()) {
        std::cerr << "DEVEL: manager::initialize_impl: new G4VisExecutive !!!" << std::endl;
        _g4_vis_manager_ = new G4VisExecutive;
        _g4_vis_manager_->Initialize();
      }
#endif

      // User actions:
      // Run action:
      _init_run_action ();

      // Event action:
      _init_event_action ();

      // Primary generator:
      _init_primary_generator_action ();

      // Tracking action:
      _init_tracking_action ();

      // Stepping action:
      _init_stepping_action ();

      // Stacking action:
      _init_stacking_action ();

      // G4 kernel initialization:
      DT_LOG_NOTICE(_logprio(), "G4 kernel initialization...");
      _g4_run_manager_->Initialize();

      _g4_UI_ = G4UImanager::GetUIpointer();

      /*****************
       * FINAL ACTIONS *
       *****************/
      // init the dictionary of PRNG internal state records :
      _init_prngs_states();
    } // end of manager::_at_init()

    void manager::reset_impl() {
      if (_use_time_stat_) {
        for (auto & _CT : _CTs_) {
          _CT.second.tree_dump(std::clog, _CT.first);
          _CT.second.reset();
        }
      }

#ifdef G4VIS_USE
      if (_g4_vis_manager_ != nullptr) {
        delete _g4_vis_manager_;
        _g4_vis_manager_ = nullptr;
      }
#endif
      if (_g4_run_manager_ != nullptr) {
        delete _g4_run_manager_;
        _g4_run_manager_ = nullptr;
      }

      if (_g4_stepping_verbosity_ != nullptr) {
        delete _g4_stepping_verbosity_;
        _g4_stepping_verbosity_ = nullptr;
      }

      if (has_event_generator()) {
        _event_generator_ = nullptr;
      }

      if (has_vertex_generator()) {
        _vertex_generator_ = nullptr;
      }

      _vg_prng_.reset();
      _eg_prng_.reset();
      _shpf_prng_.reset();
      _mgr_prng_.reset();

      reset_simulation_ctrl();

      _track_history_.reset();
    }

    void manager::run_simulation_impl() {
      std::string g4_macro;
      if (has_g4_macro()) {
        g4_macro = _g4_macro_;
        datatools::fetch_path_with_env(g4_macro);
        if (! boost::filesystem::exists(g4_macro)) {
          std::ostringstream message;
          message << "Macro '" << g4_macro << "' does not exist !";
          G4Exception("mctools::g4::manager::_at_run_simulation",
                      "FileError",
                      RunMustBeAborted,
                      message.str().c_str());
        }
      }

      // Interactive mode:
      if (is_interactive()) {
        DT_LOG_NOTICE(_logprio(), "Entering interactive mode...");
#ifdef G4VIS_USE
        if (has_g4_visualization()) {
          if (_g4_vis_manager_ != nullptr) {
            //_g4_vis_manager_->SetVerboseLevel("quiet");
          }
        }
#endif

        // G4UIterminal is a(dumb) terminal.
        G4UIsession * g4_session = nullptr;

#ifdef G4UI_USE_TCSH
        g4_session = new G4UIterminal(new G4UItcsh);
#else
        g4_session = new G4UIterminal();
#endif
        if (! g4_macro.empty()) {
          std::ostringstream command_oss;
          command_oss << "/control/execute ";
          command_oss << g4_macro;
          _g4_UI_->ApplyCommand(command_oss.str());
        }
        g4_session->SessionStart();
        delete g4_session;
      } else { // Batch mode:
        DT_LOG_NOTICE(_logprio(),"Entering batch mode...");

#ifdef G4VIS_USE
        if (has_g4_visualization()) {
          if (_g4_vis_manager_ != nullptr) {
            //_g4_vis_manager_->SetVerboseLevel("quiet");
          }
        }
#endif

        // pure automatic mode: no macro is executed, only beam on
        if (is_automatic()) {
          DT_LOG_NOTICE(_logprio(), "Entering automatic mode...");

          /*    command = "/random/setSavingFlag true";
                UI->ApplyCommand(command);
                cout << "APPLYING RESET" << std::endl;
                command = "/random/resetEngineFrom currentRun.rndm";
                UI->ApplyCommand(command);
          */

          // set tracking verbosity:
          {
            std::ostringstream command_oss;
            command_oss << "/tracking/verbose ";
            command_oss << _g4_tracking_verbosity_;
            _g4_UI_->ApplyCommand(command_oss.str());
          }

          // shoot events:
          {
            std::ostringstream command_oss;
            command_oss << "/run/beamOn ";
            uint32_t effective_number_of_events = NO_LIMIT;
            /*
              if (_number_of_events == NO_LIMIT)
              {
              //command_oss << _number_of_events;
              }
            */
            if (! has_simulation_ctrl()) {
              effective_number_of_events = _number_of_events_;
            }
            DT_THROW_IF((effective_number_of_events == 0)
                        ||(effective_number_of_events > NO_LIMIT),
                        std::logic_error,
                        "Invalid number of events for Geant4 !");
            command_oss << effective_number_of_events;
            DT_LOG_NOTICE(_logprio(), "G4 command = " << command_oss.str());
            _g4_UI_->ApplyCommand(command_oss.str());
          }
        } else {
          std::ostringstream command_oss;
          command_oss << "/control/execute ";
          command_oss << g4_macro;
          _g4_UI_->ApplyCommand(command_oss.str());
        }
      }

      {
        // 2011-03-03 FM :
        DT_LOG_NOTICE(_logprio(),
                      "Record the final PRNG internal states after the session has stopped normaly.");
        this->record_current_prng_states();
      }
    } // end of manager::_at_run_simulation()


    void manager::_init_defaults() {
      // Pointers:
      _multi_config_            = nullptr;
      _g4_stepping_verbosity_   = nullptr;
      _g4_run_manager_          = nullptr;
      _g4_UI_                   = nullptr;
      _user_physics_list_       = nullptr;
      _user_primary_generator_  = nullptr;
      _user_run_action_         = nullptr;
      _user_event_action_       = nullptr;
      _user_tracking_action_    = nullptr;
      _user_stepping_action_    = nullptr;
      _user_stacking_action_    = nullptr;
#ifdef G4VIS_USE
      _g4_vis_manager_          = nullptr;
#endif // G4VIS_USE

      // Default seeds:
      _vg_prng_seed_             = mygsl::random_utils::SEED_INVALID;
      _eg_prng_seed_             = mygsl::random_utils::SEED_INVALID;
      _mgr_prng_seed_            = mygsl::random_utils::SEED_INVALID;
      _shpf_prng_seed_           = mygsl::random_utils::SEED_INVALID;

      _number_of_events_         = NO_LIMIT;
      _number_of_events_modulo_  = run_action::NUMBER_OF_EVENTS_MODULO_NONE;
      _g4_macro_                 = "";
      _g4_tracking_verbosity_    = 0;
      _input_prng_states_file_   = "";
      _output_prng_states_file_  = "";
      _prng_state_save_modulo_   = DEFAULT_PRNG_STATES_SAVE_MODULO;
      _init_seed_method_         = "";
      _input_prng_seeds_file_    = "";
      _output_prng_seeds_file_   = "";

      _output_data_format_ = io_utils::DATA_FORMAT_INVALID;
      _output_data_bank_label_ = "";

      _interactive_      = false;
      _g4_visualization_ = false;

      // Output profiles support:
      _output_profiles_activation_rule_.clear();
      _activated_output_profile_ids_.clear();
      _supported_output_profile_ids_.clear();

      _forbid_private_hits_               = false;
      _dont_save_no_sensitive_hit_events_ = false;
      _use_run_header_footer_             = false;

      _use_track_history_ = false;

      // - Data Libs:
      geant4_data_library dl;
      dl.configure_data();
    }

    void manager::_init_core() {
      // Main manager:
      const datatools::properties & manager_config
        = _multi_config_->get("manager").get_properties();

      loggable_support::_initialize_logging_support(manager_config);

      DT_LOG_DEBUG(_logprio(), "Manager config :");
      if (is_debug()) {
        manager_config.tree_dump(std::clog);
      }

      DT_LOG_NOTICE(_logprio(), "Main manager settings...");

      // Load a file with the content of the embedded 'seed_manager' for PRNGs seeding:
      if (has_input_prng_seeds_file()) {
        DT_LOG_NOTICE(_logprio(),
                      "PRNG seed dictionary from '" << get_input_prng_seeds_file() << "'...");
        std::string input_prng_seeds_file = get_input_prng_seeds_file();
        datatools::fetch_path_with_env(input_prng_seeds_file);
        if (has_output_prng_seeds_file()) {
          // Ensure the input/output seed files are different :
          std::string test_output_prng_seeds_file = get_output_prng_seeds_file();
          datatools::fetch_path_with_env(test_output_prng_seeds_file);
          DT_THROW_IF(input_prng_seeds_file == test_output_prng_seeds_file,
                      std::logic_error,
                      "Input and output PRNG seed dictionary file have the same path '"
                      << input_prng_seeds_file << "' !");
        }
        if (boost::filesystem::exists(input_prng_seeds_file)) {
          DT_LOG_NOTICE(_logprio(),
                        "Loading seeds dictionary from '" << input_prng_seeds_file << "'...");
          std::ifstream seeds_file(input_prng_seeds_file.c_str());
          DT_THROW_IF(! seeds_file,
                      std::runtime_error,
                      "Cannot open the seeds dictionary file '" << input_prng_seeds_file << "' !");
          seeds_file >> _seed_manager_;
          DT_THROW_IF(! seeds_file,
                      std::runtime_error,
                      "Invalid input or format ! "
                      << "Cannot load the PRNG seed dictionary from file '" << input_prng_seeds_file << "' !");
        } else {
          DT_THROW(std::runtime_error,
                   "No PRNG seed dictionary input file '" << input_prng_seeds_file << "' to be loaded.");
        }
      }

      // Begin of property parsing:

      // 2016-01-27 FM: add config property:
      if (! dont_save_no_sensitive_hit_events()) {
        // If not already set, we try to fetch this parameter:
        if (manager_config.has_key("dont_save_no_sensitive_hit_events")) {
          bool dont_save = manager_config.fetch_boolean("dont_save_no_sensitive_hit_events");
          set_dont_save_no_sensitive_hit_events(dont_save);
        }
      }

      if (! has_init_seed_method()) {
        // 2014-07-04 XG : Get initialization method for seeds. If seeds are equal
        // to 0 then their values may be initialized through 4 differents methods:
        // - 'current_time' i.e. getting seed value from time in seconds,
        // - 'current_pid'  i.e. getting seed value from the program PID,
        // - 'urandom' i.e. getting seed value from /dev/urandom
        // - 'random' i.e. getting seed value from the standard random pseudo device
        if (manager_config.has_key("init_seed_method")) {
          set_init_seed_method(manager_config.fetch_string("init_seed_method"));
        }
      }
      if (! has_init_seed_method()) {
        set_init_seed_method("random");
      }
      uint32_t flag = mygsl::seed_manager::INIT_SEED_FROM_UNDEFINED;
      if (get_init_seed_method() == "current_time") {
        flag = mygsl::seed_manager::INIT_SEED_FROM_CURRENT_TIME;
      } else if (get_init_seed_method() == "current_pid") {
        flag = mygsl::seed_manager::INIT_SEED_FROM_CURRENT_PID;
      } else if (get_init_seed_method() == "urandom") {
        flag = mygsl::seed_manager::INIT_SEED_FROM_URANDOM;
      } else if (get_init_seed_method() == "random") {
        flag = mygsl::seed_manager::INIT_SEED_FROM_RANDOM_DEVICE;
      } else {
        DT_THROW(std::logic_error,
                 "Initialization method '" << get_init_seed_method() << "' is unknown or not supported !");
      }
      DT_LOG_DEBUG(_logprio(),
                   "Initialization of PRNG seeds is based on the '" << get_init_seed_method() << "' method");
      _seed_manager_.set_init_seed_flags(flag);

      // 2011-02-26 FM : only search for the 'number_of_events' property
      // if '_number_of_events_' has not been set yet :
      if (_number_of_events_ == NO_LIMIT) {
        if (manager_config.has_key("number_of_events")) {
          int noevts = manager_config.fetch_integer("number_of_events");
          uint32_t unoevts;
          if (noevts <= 0) {
            unoevts = NO_LIMIT;
          } else {
            unoevts = noevts;
          }
          set_number_of_events(unoevts);
        }
      }

      // 2014-05-01 FM: add support for output profiles:
      // If no supported output profiles have been defined before,
      // search them from setup properties:
      if (_supported_output_profile_ids_.empty()) {
        DT_LOG_TRACE(_logprio(), "No supported output profiles are defined yet...");
        if (manager_config.has_key("output_profiles")) {
          DT_LOG_TRACE(_logprio(), "Fetch supported output profiles yet...");
          std::vector<std::string> sops;
          manager_config.fetch("output_profiles", sops);
          for (const auto & profile_id : sops) {
            std::ostringstream key_oss;
            key_oss << "output_profiles." << profile_id << ".description";
            std::string profile_description;
            if (manager_config.has_key(key_oss.str())) {
              profile_description = manager_config.fetch_string(key_oss.str());
            }
            add_supported_output_profile(profile_id, profile_description);
          }
        }
      }

      // If no output profiles have been activated before,
      // search them from a setup property:
      if (! has_activated_output_profiles()) {
        DT_LOG_TRACE(_logprio(), "No activated output profiles is defined yet...");
        // If no activation rule for output profiles is defined for this session:
        if (_output_profiles_activation_rule_.empty()) {
          DT_LOG_TRACE(_logprio(), "No activation rule for output profiles is defined yet...");
          // Search one from the configuration object:
          if (manager_config.has_key("output_profiles.activation")) {
            DT_LOG_TRACE(_logprio(), "Found an activation rule for output profiles...");
            std::string output_profiles_activation_rule =
              manager_config.fetch_string("output_profiles.activation");
            set_output_profiles_activation_rule(output_profiles_activation_rule);
          }
        }
        apply_output_profiles_activation_rule(_output_profiles_activation_rule_);
      }

      // 2011-02-26 FM : only search for the 'g4_macro' property if '_g4_macro_' is not set yet :
      if (! _g4_macro_.empty()) {
        if (manager_config.has_key("g4_macro")) {
          std::string g4m = manager_config.fetch_string( "g4_macro");
          set_g4_macro(g4m);
        }
      }

      if (_g4_tracking_verbosity_ == 0) {
        if (manager_config.has_key("g4_tracking_verbosity")) {
          int tracking_verbosity = manager_config.fetch_integer("g4_tracking_verbosity");
          if (tracking_verbosity <= 0) {
            tracking_verbosity = 0;
          }
          if (tracking_verbosity >= 10) {
            tracking_verbosity = 10;
          }
          _g4_tracking_verbosity_ = tracking_verbosity;
        }
      }
      // End of property parsing.

      // Begin of special configuration:

      // Init the seed of PRNG embedded in components :
      _init_seeds();

      // Some specific init for the G4 PRNG :
      _init_prngs();

      // Some specific init for the G4 PRNG :
      if (_use_time_stat_) {
        _init_time_stat();
      }
    }

    void manager::_init_geometry() {
      // Geometry manager:
      DT_LOG_NOTICE(_logprio(), "Geometry manager settings...");
      if (has_external_geom_manager()) {
        DT_LOG_NOTICE(_logprio(), "Use external geometry manager...");
        DT_THROW_IF(! _external_geom_manager_->is_initialized(),
                    std::logic_error,
                    "External geometry manager is not initialized !");
      } else {
        DT_LOG_NOTICE(_logprio(), "Use embedded geometry manager...");
        const datatools::properties & geometry_config
          = _multi_config_->get("geometry").get_properties();
        if (is_debug()) {
          DT_LOG_DEBUG(_logprio(), "Geometry configuration : ");
          geometry_config.tree_dump(std::clog);
        }
        DT_THROW_IF(! geometry_config.has_key("manager.config"),
                    std::logic_error, "Missing geometry configuration !");
        std::string geom_mgr_prop_filename = geometry_config.fetch_string("manager.config");
        datatools::fetch_path_with_env(geom_mgr_prop_filename);
        datatools::properties geom_mgr_config;
        datatools::properties::read_config(geom_mgr_prop_filename, geom_mgr_config);
        _geom_manager_.set_mapping_requested(true);
        if (_use_time_stat_) {
          _CTs_["GB"].start();
        }
        _geom_manager_.initialize(geom_mgr_config);
        if (_use_time_stat_) {
          _CTs_["GB"].stop();
        }
      }
    }

    void manager::_init_vertex_generator() {
      // Vertex generator:
      DT_LOG_NOTICE(_logprio(),"Vertex generator settings...");
      if (_multi_config_->has_section("vertex_generator")) {
        const datatools::properties & vertex_generator_config
          = _multi_config_->get("vertex_generator").get_properties();
        _vg_manager_.set_external_random(_vg_prng_);
        if (has_service_manager()) {
          _vg_manager_.set_service_manager(*_service_manager_);
        }
        _vg_manager_.set_geometry_manager(get_geom_manager());
        _vg_manager_.set_generator_name(_vg_name_);
        if (vertex_generator_config.has_key("manager.config")) {
          // Using an external configuration file:
          std::string vtx_gtor_prop_filename
            = vertex_generator_config.fetch_string("manager.config");
          datatools::fetch_path_with_env(vtx_gtor_prop_filename);
          datatools::properties vtx_gtor_config;
          datatools::properties::read_config(vtx_gtor_prop_filename,
                                             vtx_gtor_config);
          _vg_manager_.initialize(vtx_gtor_config);
        } else {
          _vg_manager_.initialize(vertex_generator_config);
        }

        DT_THROW_IF(!_vg_manager_.is_initialized(),
                    std::logic_error,
                    "Vertex generator manager is not initialized !");
        if (is_debug()) {
          DT_LOG_DEBUG(_logprio(),"Vertex generator manager : ");
          _vg_manager_.tree_dump(std::clog);
        }
        std::string vg_name = _vg_name_;
        if (vg_name.empty()) {
          // Search for a default/current generator from the manager:
          if (_vg_manager_.has_generator_name()) {
            vg_name = _vg_manager_.get_generator_name();
          }
        }
        DT_THROW_IF(! _vg_manager_.has_generator(vg_name),
                    std::logic_error,
                    "Cannot find vertex generator named '"
                    + vg_name + "' !");
        _vertex_generator_ = &_vg_manager_.grab(vg_name);
      } else {
        DT_LOG_WARNING(_logprio(), "No vertex generator settings.");
      }
    }

    void manager::_init_event_generator() {
      // Event generator:
      DT_LOG_NOTICE(_logprio(), "Primary event generator settings...");
      datatools::properties primary_generator_config;
      DT_THROW_IF(!_multi_config_->has_section("event_generator"),
                  std::logic_error,
                  "Missing primary event generator configuration !");
      primary_generator_config = _multi_config_->get("event_generator").get_properties();
      _eg_manager_.set_external_prng(_eg_prng_);
      if (has_service_manager()) {
        _eg_manager_.set_service_manager(grab_service_manager());
      }
      if (primary_generator_config.has_key("manager.config")) {
        // Using an external configuration file:
        std::string event_gtor_prop_filename
          = primary_generator_config.fetch_string("manager.config");
        datatools::fetch_path_with_env(event_gtor_prop_filename);
        datatools::properties event_gtor_config;
        datatools::properties::read_config(event_gtor_prop_filename,
                                           event_gtor_config);
        _eg_manager_.initialize(event_gtor_config);
      } else {
        _eg_manager_.initialize(primary_generator_config);
      }
      DT_THROW_IF(!_eg_manager_.is_initialized(),
                  std::logic_error,
                  "Primary event generator manager is not initialized !");
      std::string eg_name = _eg_name_;
      if (eg_name.empty()) {
         // Search for a default/current generator from the manager:
         if (_eg_manager_.has_default_generator()) {
          eg_name = _eg_manager_.get_default_generator();
        }
      }
      DT_THROW_IF(! _eg_manager_.has(eg_name),
                  std::logic_error,
                  "Cannot find primary event generator named '" << eg_name << "' !");
      _event_generator_ = &_eg_manager_.grab(eg_name);
    }

    void manager::_init_detector_construction() {
      // Detector construction:
      DT_LOG_NOTICE(_logprio(), "Detector construction...");
      DT_THROW_IF(! _multi_config_->has_section("detector_construction"),
                  std::logic_error,
                  "Missing detector construction configuration !");
      const datatools::properties & detector_construction_config
        = _multi_config_->get("detector_construction").get_properties();
      _user_detector_construction_ = new detector_construction(*this);
      _user_detector_construction_->grab_step_hit_processor_factory().set_external_prng(_shpf_prng_);
      _user_detector_construction_->set_geometry_manager(get_geom_manager());
      _user_detector_construction_->initialize(detector_construction_config);
      _g4_run_manager_->SetUserInitialization(_user_detector_construction_);
    }

    void manager::_init_physics_list() {
      DT_LOG_NOTICE(_logprio(), "Physics list initialization...");
      DT_THROW_IF(! _multi_config_->has_section("physics_list"),
                  std::logic_error,
                  "Missing physics list configuration !");
      const datatools::properties & physics_list_config
        = _multi_config_->get("physics_list").get_properties();
      _user_physics_list_ = new physics_list;
      _user_physics_list_->initialize(physics_list_config);
      _g4_run_manager_->SetUserInitialization(_user_physics_list_);
      DT_LOG_DEBUG(_logprio(), "Physics list: ");
      if (is_debug()) { _user_physics_list_->tree_dump(std::clog);
}
    }

    void manager::_init_run_action() {
      DT_LOG_NOTICE(_logprio(), "Run action initialization...");
      DT_THROW_IF(! _multi_config_->has_section("run_action"),
                  std::logic_error,
                  "Missing run action configuration !");
      const datatools::properties & run_action_config
        = _multi_config_->get("run_action").get_properties();
      _user_run_action_ = new run_action(*this);
      if (_output_data_format_ == io_utils::DATA_FORMAT_INVALID) {
        _output_data_format_ = io_utils::DATA_FORMAT_PLAIN;
      }
      _user_run_action_->set_output_data_format(_output_data_format_);
      if (_output_data_format_ == io_utils::DATA_FORMAT_BANK) {
        _user_run_action_->set_output_data_bank_label(_output_data_bank_label_);
      }
      if (! _output_data_file_.empty()) {
        _user_run_action_->set_output_file(_output_data_file_);
      }
      if (has_number_of_events_modulo()) {
        _user_run_action_->set_number_of_events_modulo(get_number_of_events_modulo());
      }
      _user_run_action_->set_use_run_header_footer(using_run_header_footer());
      _user_run_action_->initialize(run_action_config);
      _g4_run_manager_->SetUserAction(_user_run_action_);
    }

    void manager::_init_event_action() {
      DT_LOG_NOTICE(_logprio(), "Event action initialization...");
      DT_THROW_IF(! _multi_config_->has_section("event_action"),
                  std::logic_error,
                  "Missing event action configuration !");
      const datatools::properties & event_action_config
        = _multi_config_->get("event_action").get_properties();
      _user_event_action_ = new event_action(*_user_run_action_,
                                             *_user_detector_construction_);
      _user_event_action_->initialize(event_action_config);
      _g4_run_manager_->SetUserAction(_user_event_action_);
    }

    void manager::_init_primary_generator_action() {
      DT_LOG_NOTICE(_logprio(), "Primary generator action initialization...");
      DT_THROW_IF(!_multi_config_->has_section("primary_generator_action"),
                  std::logic_error,
                  "Missing primary event generator action configuration !");
      const datatools::properties & primary_generator_config
        = _multi_config_->get("primary_generator_action").get_properties();

      _user_primary_generator_ = new primary_generator;
      _user_primary_generator_->set_run_action(*_user_run_action_);
      _user_primary_generator_->set_event_action(*_user_event_action_);
      if (has_vertex_generator()) {
        _user_primary_generator_->set_vertex_generator(*_vertex_generator_);
      }
      _user_primary_generator_->set_event_generator(*_event_generator_);
      _user_primary_generator_->initialize(primary_generator_config);
      _g4_run_manager_->SetUserAction(_user_primary_generator_);
    }

    void manager::_init_tracking_action() {
      DT_LOG_NOTICE(_logprio(), "Tracking action initialization...");
      if (_multi_config_->has_section("tracking_action")) {
        const datatools::properties &  tracking_action_config  = _multi_config_->get("tracking_action").get_properties();
        _user_tracking_action_ = new tracking_action;
        _user_tracking_action_->initialize(tracking_action_config);
        _g4_run_manager_->SetUserAction(_user_tracking_action_);
      }
    }

    void manager::_init_stepping_action() {
      DT_LOG_NOTICE(_logprio(), "Stepping action initialization...");
      if (_multi_config_->has_section("stepping_action")) {
        const datatools::properties & stepping_action_config = _multi_config_->get("stepping_action").get_properties();
        _user_stepping_action_ = new stepping_action;
        _user_stepping_action_->initialize(stepping_action_config);
        _g4_run_manager_->SetUserAction(_user_stepping_action_);
      }
    }

    void manager::_init_stacking_action() {
      DT_LOG_NOTICE(_logprio(), "Stacking action initialization...");
      if (_multi_config_->has_section("stacking_action")) {
        const datatools::properties & stacking_action_config = _multi_config_->get("stacking_action").get_properties();
        _user_stacking_action_ = new stacking_action;
        _user_stacking_action_->initialize(stacking_action_config);
        _g4_run_manager_->SetUserAction(_user_stacking_action_);
      }
    }

    void manager::_init_seeds() {
      // All seeds being 'SEED_AUTO' (see mygsl::seed_manager)
      // are initialized with some source of entropy :
      // 2012-06-08 FM : fix to make all seeds different (and
      // automatically randomize 'SEED_AUTO'):
      _seed_manager_.ensure_different_seeds();
      if (is_debug()) {
        _seed_manager_.dump(std::clog);
      }

      // Then pickup the seed associated to each embedded component :

      // the G4 manager itself :
      if (_seed_manager_.has_seed(g4_manager_label())) {
        int seed = _seed_manager_.get_seed(g4_manager_label());
        set_mgr_prng_seed(seed);
        DT_LOG_NOTICE(_logprio(), "Using registered seed for '"
                      << g4_manager_label() << "' : "
                      << seed);
      }

      // the vertex generator :
      if (_seed_manager_.has_seed(vertex_generator_label())) {
        int seed = _seed_manager_.get_seed(vertex_generator_label());
        set_vertex_generator_seed(seed);
        DT_LOG_NOTICE(_logprio(), "Using registered seed for '"
                      << vertex_generator_label() << "' : "
                      << seed);
      }

      // the event generator :
      if (_seed_manager_.has_seed(event_generator_label())) {
        int seed = _seed_manager_.get_seed(event_generator_label());
        set_event_generator_seed(seed);
        DT_LOG_NOTICE(_logprio(), "Using registered seed for '"
                      << event_generator_label() << "' : "
                      << seed);
      }

      // the step hit processor factory :
      if (_seed_manager_.has_seed(shpf_label())) {
        int seed = _seed_manager_.get_seed(shpf_label());
        set_shpf_prng_seed(seed);
        DT_LOG_NOTICE(_logprio(), "Using registered seed for '"
                      << shpf_label() << "' : "
                      << seed);
      }

      // Checks:
      DT_THROW_IF(! mygsl::seed_manager::seed_is_valid(_vg_prng_seed_),
                  std::logic_error,
                  "Invalid vertex generator seed value !");
      DT_THROW_IF(! mygsl::seed_manager::seed_is_valid(_eg_prng_seed_),
                  std::logic_error,
                  "Invalid event generator seed value !");
      DT_THROW_IF(! mygsl::seed_manager::seed_is_valid(_mgr_prng_seed_),
                  std::logic_error,
                  "Invalid G4 generator seed value !");
      DT_THROW_IF(! mygsl::seed_manager::seed_is_valid(_shpf_prng_seed_),
                  std::logic_error,
                  "Invalid SHPF generator seed value !");

      // Save a file with the content of the embedded 'seed_manager' :
      if (has_output_prng_seeds_file()) {
        std::string seeds_filename = get_output_prng_seeds_file();
        datatools::fetch_path_with_env(seeds_filename);
        if (boost::filesystem::exists(seeds_filename)) {
          std::string backup_seeds_filename = seeds_filename + ".~backup~";
          DT_LOG_NOTICE(_logprio(),
                        "Backuping the former Random Number Generators seeds file '"
                        << seeds_filename << "' to '" << backup_seeds_filename << "'...");
          if (boost::filesystem::exists(backup_seeds_filename)) {
            DT_LOG_NOTICE(_logprio(),
                          "Deleting old backup Random Number Generators seeds file '"
                          << backup_seeds_filename << "'...");
            boost::filesystem::remove(backup_seeds_filename);
          }
          boost::filesystem::rename(seeds_filename, backup_seeds_filename);
        }
        DT_LOG_NOTICE(_logprio(),
                      "Saving the Random Number Generator seeds in file '"
                      << seeds_filename << "'...");
        std::ofstream seeds_file(seeds_filename.c_str());
        seeds_file << _seed_manager_ << std::endl;
      }
    }

    void manager::_init_prngs_states() {
      // Load the PRNG internal states buffers :
      if (has_input_prng_states_file()) {
        DT_LOG_NOTICE(_logprio(),  "Loading PRNG internal states from file '"
                      << get_input_prng_states_file() << "'...");
        _prng_state_manager_.load(get_input_prng_states_file());
      }

      // Ensure the creation of all useful buffers :
      // G4 PRNG :
      if (! _prng_state_manager_.has_state(g4_manager_label())) {
        _prng_state_manager_.add_state(g4_manager_label(),
                                       _mgr_prng_.get_internal_state_size());
      }
      // Vertex generator PRNG :
      if (! _prng_state_manager_.has_state(vertex_generator_label())) {
        _prng_state_manager_.add_state(vertex_generator_label(),
                                       _vg_prng_.get_internal_state_size());
      }
      // Event generator PRNG :
      if (! _prng_state_manager_.has_state(event_generator_label())) {
        _prng_state_manager_.add_state(event_generator_label(),
                                       _eg_prng_.get_internal_state_size());
      }
      // PRNG for Step Hit Processors :
      if (! _prng_state_manager_.has_state(shpf_label())) {
        _prng_state_manager_.add_state(shpf_label(),
                                       _shpf_prng_.get_internal_state_size());
      }

      // Setup the output file name for PRNG internal states backup :
      if (has_output_prng_states_file()) {
        DT_LOG_NOTICE(_logprio(), "Setting the file '" << get_output_prng_states_file()
                      << "' for storing the PRNG internal states...");
        _prng_state_manager_.set_filename(get_output_prng_states_file());
      }

      if (has_input_prng_states_file()) {
        DT_LOG_NOTICE(_logprio(), "Initializing the PRNG internal states from the 'state manager'...");

        // G4 PRNG :
        _mgr_prng_.from_buffer(_prng_state_manager_.get_state(g4_manager_label()).state_buffer);

        // Vertex generator PRNG :
        _vg_prng_.from_buffer(_prng_state_manager_.get_state(vertex_generator_label()).state_buffer);

        // Event generator PRNG :
        _eg_prng_.from_buffer(_prng_state_manager_.get_state(event_generator_label()).state_buffer);

        // PRNG for Step Hit Processors :
        _shpf_prng_.from_buffer(_prng_state_manager_.get_state(shpf_label()).state_buffer);
      }
    }

    void manager::_init_prngs() {
      /****************************
       * RANDOM NUMBER GENERATORS *
       ****************************/

      _mgr_prng_.initialize(default_prng_id(),  _mgr_prng_seed_);
      _vg_prng_.initialize(default_prng_id(),   _vg_prng_seed_);
      _eg_prng_.initialize(default_prng_id(),   _eg_prng_seed_);
      _shpf_prng_.initialize(default_prng_id(), _shpf_prng_seed_);

      // >>> 2016-06-27 FM: This part needs a review:
      // bool needs_seed = true;
      // if (has_input_prng_states_file()) {
      //   std::string rsf = get_input_prng_states_file();
      //   datatools::fetch_path_with_env(rsf);
      //   if (boost::filesystem::exists(rsf.c_str())) {
      //     _prng_state_manager_.load();
      //     DT_LOG_NOTICE(_logprio(), "Random Number Generator internal state was restored from file '"
      //                   << rsf << "'...");
      //     needs_seed = false;
      //     // if (_mgr_prng_seed_ != mygsl::random_utils::SEED_INVALID) {
      //     //   DT_LOG_NOTICE(_logprio(),
      //     //                 "Ignoring the seed for the Random Number Generator initialization !");
      //     // }
      //     // _mgr_prng_seed_ = mygsl::random_utils::SEED_INVALID;
      //   } else {
      //     DT_THROW(std::logic_error,
      //              "Random Number Generator initial state file '" << rsf.c_str() << "' does not exist !");
      //   }
      // }

      /*
      // if no status file was loaded, use a seed :
      if (needs_seed) {
      DT_THROW_IF(_mgr_prng_seed_ == mygsl::random_utils::SEED_INVALID,
      std::logic_error,
      "No seed was provided for the Random Number Generator !");
      if (_mgr_prng_seed_ == mygsl::random_utils::SEED_AUTO) {
      int32_t time_seed =(int)(time(0) & 0xFFFFFFFF);
      _mgr_prng_.set_seed(time_seed);
      }
      DT_LOG_NOTICE(_logprio(),
      "Setting the seed for the GEANT4 Random Number Generator : "
      << _mgr_prng_seed_);
      _mgr_prng_.set_seed(_mgr_prng_seed_);
      }
      */
      // <<< 2016-06-27 FM.

      _g4_prng_.set_random(_mgr_prng_);
      CLHEP::HepRandom::setTheEngine(&_g4_prng_);

      DT_LOG_NOTICE(_logprio(),
                    "Setting the HepRandomEngine to '" << _g4_prng_.name() << "'...");
    }

    void manager::_init_time_stat() {
      // Time statistics for "Event Action" :
      _CTs_["EA"] = CT_type {};
      // Time statistics for "Run Action" :
      _CTs_["RA"] = CT_type {};
      // Time statistics for "I/O operations" :
      _CTs_["IO"] = CT_type {};
      // Time statistics for "Sensitive detectors" :
      _CTs_["SD"] =  CT_type {};
      // Time statistics for "Hit processors" :
      _CTs_["HP"] = CT_type {};
      // Time statistics for "Geometry building" :
      _CTs_["GB"] = CT_type {};
      // Time statistics for "Vertex generation" :
      _CTs_["VG"] = CT_type {};
      // Time statistics for "Event generation" :
      _CTs_["EG"] = CT_type {};
    }
  } // end of namespace g4
} // end of namespace mctools

/** Opening macro for implementation
 *  This macro must be used outside of any namespace.
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(mctools::g4::manager,ocd_) {
  // The class name :
  ocd_.set_class_name ("mctools::g4::manager");

  // The class terse description :
  ocd_.set_class_description ("The Geant4 simulation manager class");

  // The library the class belongs to :
  ocd_.set_class_library ("mctools_g4");

  // The class detailed documentation :
  ocd_.set_class_documentation ("The Geant4 simulation manager class embedes \n"
                                "a full Geant4 based simulation engine. It can be\n"
                                "configured through a ``datatools::multi_properties`` object.\n"
                                "Several embedded components are used :                  \n"
                                "                                                        \n"
                                " * a service manager (optional)                         \n"
                                " * a geometry manager                                   \n"
                                " * a vertex generator manager (optional)                \n"
                                " * a primary event generator manager                    \n"
                                " * a PRNG seed manager                                  \n"
                                " * a PRNG internal state manager                        \n"
                                " * a Geant4 run manager with embedded components:       \n"
                                "                                                        \n"
                                "   - detector construction                              \n"
                                "   - physics list                                       \n"
                                "   - run action                                         \n"
                                "   - event action                                       \n"
                                "   - primary generator action                           \n"
                                "   - tracking action (optional)                         \n"
                                "   - stepping action (optional)                         \n"
                                "   - stacking action (optional)                         \n"
                                "                                                        \n"
                                " * a Geant4 user interface (UI) manager                 \n"
                                " * a Geant4 visualization manager (optional)            \n"
                                "                                                                \n"
                                "The ``datatools::multi_properties`` configuration object        \n"
                                "has several sections, each of them with specific setup          \n"
                                "parameters:                                                     \n"
                                "                                                                \n"
                                " * ``[manager]`` : manager's core setup parameters              \n"
                                " * ``[geometry]`` : geometry setup parameters                   \n"
                                " * ``[event_generator]`` : event generator setup parameters     \n"
                                " * ``[vertex_generator]`` : vertex generator setup parameters   \n"
                                " * ``[detector_construction]`` : detector construction setup    \n"
                                "   parameters                                                   \n"
                                " * ``[physics_list]`` : physics list setup parameters           \n"
                                " * ``[run_action]`` : run action setup parameters               \n"
                                " * ``[event_action]`` : event action setup parameters           \n"
                                " * ``[primary_generator_action]`` : primary event generator     \n"
                                "   action setup parameters                                      \n"
                                " * ``[tracking_action]`` : tracking action setup parameters     \n"
                                " * ``[stepping_action]`` : stepping action setup parameters     \n"
                                " * ``[stacking_action]`` : stacking action setup parameters     \n"
                                "                                                                \n"
                                );

  {
    // Description of the 'logging.priority' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("logging.priority")
      .set_section("manager")
      .set_terse_description("Logging priority threshold")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_default_value_string("warning")
      .set_long_description("Allowed values are:                                    \n"
                            "                                                       \n"
                            " * ``\"fatal\"``       : print fatal error messages    \n"
                            " * ``\"critical\"``    : print critical error messages \n"
                            " * ``\"error\"``       : print error messages          \n"
                            " * ``\"warning\"``     : print warnings                \n"
                            " * ``\"notice\"``      : print notice messages         \n"
                            " * ``\"information\"`` : print informational messages  \n"
                            " * ``\"debug\"``       : print debug messages          \n"
                            " * ``\"trace\"``       : print trace messages          \n"
                            )
      .add_example("Explicitly set the logging threshold: ::  \n"
                   "                                          \n"
                   "  [name=\"manager\"]                      \n"
                   "  logging.priority : string = \"notice\"  \n"
                   "                                          \n"
                   )

      ;
  }

  {
    // Description of the 'number_of_events' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("number_of_events")
      .set_section("manager")
      .set_terse_description("Number of events to be simulated")
      .set_traits(datatools::TYPE_INTEGER)
      .set_mandatory(false)
      .set_long_description("Allowed value: from ``1`` to ``100000000``                              \n"
                            "                                                                        \n"
                            "This property is not taken into account if the                          \n"
                            "*number of events* attributes has been set previously through           \n"
                            "the ``mctools::g4::manager::set_number_of_events(...)`` method.         \n"
                            "                                                                        \n"
                            )
      .add_example("Set a specific number of events to simulate: ::   \n"
                   "                                                  \n"
                   "  [name=\"manager\"]                              \n"
                   "  number_of_events : integer = 100000             \n"
                   "                                                  \n"
                   )
      ;
  }

  {
    // Description of the 'output_profiles.supported' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("output_profiles")
      .set_section("manager")
      .set_terse_description("List of supported output profiles")
      .set_traits(datatools::TYPE_STRING,
                  datatools::configuration_property_description::ARRAY
                  )
      .set_mandatory(false)
      .set_long_description("This property is not taken into account if some supported               \n"
                            "output profiles have already been set in the simulation manager,        \n"
                            "through the ``mctools::g4::manager::add_supported_output_profile(...)`` \n"
                            "method.                                                                 \n"
                            )
      .add_example("Define some supported output profiles: ::                       \n"
                   "                                                                \n"
                   "  [name=\"manager\"]                                            \n"
                   "  output_profiles : string[2] =  \\                             \n"
                   "        \"calorimeter_details\"  \\                             \n"
                   "        \"tracker_details\"                                     \n"
                   "                                                                \n"
                   )
      ;
  }

  {
    // Description of the 'output_profiles.supported.${id}.description' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("output_profiles.${output_profiles}.description")
      .set_section("manager")
      .set_terse_description("Description of supported output profiles")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("Each supported output profile can be associated to a description.       \n"
                            "output profiles have already been set in the simulation manager,        \n"
                            "through the ``mctools::g4::manager::add_supported_output_profile(...)`` \n"
                            "method.                                                                 \n"
                            )
      .add_example("Describe the supported output profiles: ::                      \n"
                   "                                                                \n"
                   "  [name=\"manager\"]                                            \n"
                   "  output_profiles : string[2] = \\                              \n"
                   "        \"calorimeter_details\"  \\                             \n"
                   "        \"tracker_details\"                                     \n"
                   "  output_profiles.calorimeter_details.description : string = \\ \n"
                   "        \"Detailed true hits from the calorimeter volume\"      \n"
                   "  output_profiles.tracker_details.description : string = \\     \n"
                   "        \"Detailed true hits from the tracker volume\"          \n"
                   "                                                                \n"
                   )
      ;
  }

  {
    // Description of the 'output_profiles.supported' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("output_profiles.activation")
      .set_section("manager")
      .set_terse_description("The activation rule for output profiles")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("This property is not taken into account if some                               \n"
                            "output profiles have already been activated in the simulation manager,        \n"
                            "through the ``mctools::g4::manager::activate_output_profile(...)`` or the     \n"
                            "``mctools::g4::manager::apply_output_profiles_activation_rule(...)`` methods. \n"
                            )
      .add_example("Activate two output profiles from the list of supported ones: ::\n"
                   "                                                                \n"
                   "  [name=\"manager\"]                                            \n"
                   "  output_profiles : string[4] =  \\                             \n"
                   "        \"ecalo_details\"  \\                                   \n"
                   "        \"hcalo_details\"  \\                                   \n"
                   "        \"muons_details\"  \\                                   \n"
                   "        \"tracker_details\"                                     \n"
                   "  output_profiles.activation: string = \"ecalo_details + muons_details\" \n"
                   )
      ;
  }

  {
    // Description of the 'dont_save_no_sensitive_hit_events' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("dont_save_no_sensitive_hit_events")
      .set_section("manager")
      .set_terse_description("Flag used to record only events with sensitive hits")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      // .set_long_description("This property is not taken into account if the                          \n"
      //                       "*number of events* attributes has been set previously through           \n"
      //                       "the ``mctools::g4::manager::set_number_of_events(...)`` method.         \n"
      //                       "                                                                        \n"
      //                       )
      .add_example("Set a specific number of events to simulate: ::      \n"
                   "                                                     \n"
                   "  [name=\"manager\"]                                 \n"
                   "  dont_save_no_sensitive_hit_events : boolean = true \n"
                   "                                                     \n"
                   )
      ;
  }

  {
    // Description of the 'g4_macro' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("g4_macro")
      .set_section("manager")
      .set_terse_description("Geant4 macro to be executed")
      .set_traits(datatools::TYPE_STRING)
      .set_path(true)
      .set_long_description("This property is not taken into account if a Geant4 *macro*             \n"
                            "has already been set by the simulation manager, through                 \n"
                            "the ``mctools::g4::manager::set_g4_macro(...)`` method.                 \n"
                            "                                                                        \n"
                            )
      .add_example("Set the name of a Geant4 macro: ::                              \n"
                   "                                                                \n"
                   "  [name=\"manager\"]                                            \n"
                   "  g4_macro : string as path = \"g4.mac\"                        \n"
                   "                                                                \n"
                   )
      ;
  }

  {
    // Description of the geometry 'manager.config' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("manager.config")
      .set_section("geometry")
      .set_terse_description("Geometry manager main configuration file")
      .set_traits(datatools::TYPE_STRING)
      .set_path(true)
      .set_mandatory(true)
      .set_long_description("Example::                                                               \n"
                            "                                                                        \n"
                            "  [name=\"geometry\"]                                                   \n"
                            "  manager.config : string as path = \"config/geometry/manager.conf\"   \n"
                            "                                                                        \n"
                            "This property is not taken into account if an external                  \n"
                            "*geometry manager* is used by the simulation manager, through           \n"
                            "the ``mctools::g4::manager::set_external_geom_manager(...)`` method.    \n"
                            "                                                                        \n"
                            )
      ;
  }

  {
    // Description of the vertex generator 'manager.config' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("manager.config")
      .set_section("vertex_generator")
      .set_terse_description("Vertex generator manager main configuration file")
      .set_traits(datatools::TYPE_STRING)
      .set_path(true)
      .set_mandatory(true)
      .set_long_description("Example::                                                                     \n"
                            "                                                                              \n"
                            "  [name=\"vertex_generator\"]                                                 \n"
                            "  manager.config : string as path = \"config/vertex_generator/manager.conf\"  \n"
                            "                                                                              \n"
                            )
      ;
  }

  {
    // Description of the event generator 'manager.config' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("manager.config")
      .set_section("event_generator")
      .set_terse_description("Event generator manager main configuration file")
      .set_traits(datatools::TYPE_STRING)
      .set_path(true)
      .set_mandatory(true)
      .set_long_description("Example::                                                                     \n"
                            "                                                                              \n"
                            "  [name=\"event_generator\"]                                                  \n"
                            "  manager.config : string as path = \"config/event_generator/manager.conf\"   \n"
                            "                                                                              \n"
                            )
      ;
  }

  // Additionnal configuration hints :
  ocd_.set_configuration_hints("The simulation manager can be parameterized and initialized           \n"
                               "through a ``datatools::multi_properties`` object.                     \n"
                               "The configuration contains several sections, each of them is          \n"
                               "related to a specific sub-task. A typical layout is::                 \n"
                               "                                                                      \n"
                               "  #@description Configuration of the mctools Geant4 simulation engine \n"
                               "  #@key_label   \"name\"                                              \n"
                               "  #@meta_label  \"\"                                                  \n"
                               "                                                                      \n"
                               "                                                                      \n"
                               "  [name=\"manager\"]                                                  \n"
                               "  #@config Parameters for the main simulation engine                  \n"
                               "                                                                      \n"
                               "  #@description Manager logging priority                              \n"
                               "  logging.priority : string = \"warning\"                             \n"
                               "                                                                      \n"
                               "  #@description Number of events to be simulated                      \n"
                               "  number_of_events : integer = 10000                                  \n"
                               "                                                                      \n"
                               "                                                                      \n"
                               "  [name=\"geometry\"]                                                 \n"
                               "  #@config Parameters for the geometry modelling                      \n"
                               "                                                                      \n"
                               "  # See OCD support for class ``geomtools::manager``                  \n"
                               "  manager.config : string as path = \"config/geometry/manager.conf\"  \n"
                               "                                                                      \n"
                               "                                                                      \n"
                               "  [name=\"vertex_generator\"]                                         \n"
                               "  #@config Parameters for the vertex generator                        \n"
                               "                                                                      \n"
                               "  # See OCD support for class ``genvtx::manager``                     \n"
                               "  manager.config : string as path = \"config/vertex_generator/manager.conf\"  \n"
                               "                                                                      \n"
                               "                                                                      \n"
                               "  [name=\"event_generator\"]                                          \n"
                               "  #@config Parameters for the GENBB based event generator             \n"
                               "                                                                      \n"
                               "  # See OCD support for class ``genbb::manager``                      \n"
                               "  manager.config : string as path = \"config/event_generator/manager.conf\"  \n"
                               "                                                                      \n"
                               "                                                                      \n"
                               "  [name=\"detector_construction\"]                                    \n"
                               "  #@config Parameters for the detector construction                   \n"
                               "                                                                      \n"
                               "  # See OCD support for class ``mctools::g4::detector_construction``  \n"
                               "                                                                      \n"
                               "                                                                      \n"
                               "  [name=\"physics_list\"]                                             \n"
                               "  #@config Parameters for the physics list                            \n"
                               "                                                                      \n"
                               "  # See OCD support for class ``mctools::g4::physics_list``           \n"
                               "                                                                      \n"
                               "                                                                      \n"
                               "  [name=\"run_action\"]                                               \n"
                               "  #@config Parameters for the run action                              \n"
                               "                                                                      \n"
                               "  # See OCD support for class ``mctools::g4::run_action``             \n"
                               "                                                                      \n"
                               "                                                                      \n"
                               "  [name=\"event_action\"]                                             \n"
                               "  #@config Parameters for the event action                            \n"
                               "                                                                      \n"
                               "  # See OCD support for class ``mctools::g4::event_action``           \n"
                               "                                                                      \n"
                               "                                                                      \n"
                               "  [name=\"primary_generator_action\"]                                 \n"
                               "  #@config Parameters for the primary generator action                \n"
                               "                                                                      \n"
                               "  # See OCD support for class ``mctools::g4::primary_generator``      \n"
                               "                                                                      \n"
                               "                                                                      \n"
                               "  [name=\"tracking_action\"]                                          \n"
                               "  #@config Parameters for the tracking action                         \n"
                               "                                                                      \n"
                               "  # See OCD support for class ``mctools::g4::tracking_action``        \n"
                               "                                                                      \n"
                               "                                                                      \n"
                               "  [name=\"stepping_action\"]                                          \n"
                               "  #@config Parameters for the stepping action                         \n"
                               "                                                                      \n"
                               "  # See OCD support for class ``mctools::g4::stepping_action``        \n"
                               "                                                                      \n"
                               "                                                                      \n"
                               "  [name=\"stacking_action\"]                                          \n"
                               "  #@config Parameters for the stacking action                         \n"
                               "                                                                      \n"
                               "  # See OCD support for class ``mctools::g4::stacking_action``        \n"
                               "                                                                      \n"
                               );

  /** Set the validation support flag :
   *  we activate this if the description of all configuration
   *  properties has been provides (see above). This enables the
   *  OCD tools to check the syntax and validaty of a given
   *  configuration file.
   */
  ocd_.set_validation_support(true);

  // Lock the description:
  ocd_.lock();

  // ... and we are done.
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END() // Closing macro for implementation

// Registration macro for class 'mctools::g4::manager' :
DOCD_CLASS_SYSTEM_REGISTRATION(mctools::g4::manager,"mctools::g4::manager")
