/* event_server.cc
 *
 * Copyright (C) 2011 Xavier Garrido <garrido@lal.in2p3.fr>
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

// This project:
#include <EventBrowser/io/boost_access.h>
#include <EventBrowser/io/brio_access.h>
#include <EventBrowser/io/event_server.h>

#include <EventBrowser/view/options_manager.h>

// Bayeux/dpp
#include <dpp/input_module.h>
#include <dpp/output_module.h>
// Bayeux/datatools
#include <datatools/multi_properties.h>

namespace snemo {

namespace visualization {

namespace io {

bool event_server::is_initialized() const { return (_status_ & INITIALIZED) != 0u; }

void event_server::set_initialized(const bool initialized_) {
  initialized_ ? _status_ |= INITIALIZED : _status_ &= INITIALIZED;
}

bool event_server::has_sequential_data() const { return (_status_ & SEQUENTIAL) != 0u; }

void event_server::set_sequential(const bool sequential_) {
  sequential_ ? _status_ |= SEQUENTIAL : _status_ &= SEQUENTIAL;
}

bool event_server::has_external_data() const { return (_status_ & EXTERNAL) != 0u; }

void event_server::set_external(const bool external_) {
  external_ ? _status_ |= EXTERNAL : _status_ &= EXTERNAL;
}

bool event_server::has_random_data() const {
  return !has_sequential_data() && !has_external_data();
}

void event_server::set_file_type(const file_type file_type_) { _file_type_ = file_type_; }

event_server::file_type event_server::get_file_type() const { return _file_type_; }

void event_server::set_current_event_number(const int current_event_) {
  _current_event_number_ = current_event_;
}

int event_server::get_current_event_number() const { return _current_event_number_; }

void event_server::set_external_event(event_record& external_event_) { _event_ = &external_event_; }

const event_record& event_server::get_event() const { return *_event_; }

event_record& event_server::grab_event() { return *_event_; }

// ctor:
event_server::event_server() {
  _status_ = UNDEFINED;
  _file_type_ = NONE;
  _data_access_ = nullptr;
  _current_event_number_ = -1;
  _event_ = nullptr;
}

// dtor:
event_server::~event_server() { reset(); }

bool event_server::initialize(const std::vector<std::string>& filenames_) {
  DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized !");

  if (!_at_open_(filenames_)) {
    return false;
  }

  this->fill_selection();

  _event_ = new event_record;

  set_initialized(true);
  return true;
}

bool event_server::reset() {
  DT_THROW_IF(!is_initialized(), std::logic_error, "Not initialized !");

  if (_event_ != nullptr) {
    delete _event_;
    _event_ = nullptr;
  }

  if (_data_access_ != nullptr) {
    delete _data_access_;
    _data_access_ = nullptr;
  }

  _status_ = UNDEFINED;
  _file_type_ = NONE;
  _current_event_number_ = -1;
  _event_selection_.clear();

  set_initialized(false);
  return true;
}

bool event_server::open(const std::vector<std::string>& filenames_) {
  return initialize(filenames_);
}

bool event_server::next_event() {
  if (!read_event(++_current_event_number_)) {
    _current_event_number_--;
    return false;
  }
  return true;
}

bool event_server::read_event(const unsigned int event_number_) {
  DT_THROW_IF(!is_initialized(), std::logic_error, "Not initialized !");
  if (_data_access_->retrieve_event(grab_event(), event_number_)) {
    _current_event_number_ = event_number_;
    return true;
  }
  return false;
}

bool event_server::store_event(const std::string& filename_) const {
  // 2016-08-24 XG : Use (in|out)put module to make sure the metadata
  // store is correctly passed and saved
  dpp::input_module im;
  im.set_single_input_file(_data_access_->get_current_filename());
  im.initialize_simple();

  // Get path to storage location
  std::string sink_label = filename_;
  if (sink_label.empty()) {
    std::string directory = "$PWD";
    datatools::fetch_path_with_env(directory);
    std::ostringstream oss;
    oss << directory << "/"
        << "event_" << get_current_event_number() << ".xml";
    sink_label = oss.str();
  }

  dpp::output_module om;
  om.set_single_output_file(sink_label);
  // Copy metadata store from input to ouput
  datatools::multi_properties& omdata = om.grab_metadata_store();
  omdata = im.get_metadata_store();
  om.initialize_simple();

  auto& a_mutable_event = const_cast<event_record&>(get_event());
  const dpp::base_module::process_status a_status = om.process(a_mutable_event);
  if (a_status != dpp::base_module::PROCESS_OK) {
    DT_LOG_ERROR(view::options_manager::get_instance().get_logging_priority(),
                 "Cannot store the event record ! An error occured during serialization !");
    return false;
  }
  DT_LOG_NOTICE(view::options_manager::get_instance().get_logging_priority(),
                "Event #" << get_current_event_number() << " "
                          << "saved in " << sink_label << " file");
  return true;
}

void event_server::dump_event(std::ostream& out_, const std::string& title_,
                              const std::string& indent_) const {
  const snemo::datamodel::event_header* eh_ptr = nullptr;
  const mctools::simulated_data* sd_ptr = nullptr;
  const snemo::datamodel::unified_digitized_data* udd_ptr = nullptr;
  const snemo::datamodel::calibrated_data* cd_ptr = nullptr;
  const snemo::datamodel::tracker_clustering_data* tc_ptr = nullptr;
  const snemo::datamodel::tracker_trajectory_data* tt_ptr = nullptr;
  const snemo::datamodel::particle_track_data* ptd_ptr = nullptr;

  if (_event_->has(EH_LABEL)) {
    eh_ptr = &(_event_->get<snemo::datamodel::event_header>(EH_LABEL));
  }
  if (_event_->has(SD_LABEL)) {
    sd_ptr = &(_event_->get<mctools::simulated_data>(SD_LABEL));
  }
  if (_event_->has(UDD_LABEL)) {
    udd_ptr = &(_event_->get<snemo::datamodel::unified_digitized_data>(UDD_LABEL));
  }
  if (_event_->has(CD_LABEL)) {
    cd_ptr = &(_event_->get<snemo::datamodel::calibrated_data>(CD_LABEL));
  }
  if (_event_->has(TCD_LABEL)) {
    tc_ptr = &(_event_->get<snemo::datamodel::tracker_clustering_data>(TCD_LABEL));
  }
  if (_event_->has(TTD_LABEL)) {
    tt_ptr = &(_event_->get<snemo::datamodel::tracker_trajectory_data>(TTD_LABEL));
  }
  if (_event_->has(PTD_LABEL)) {
    ptd_ptr = &(_event_->get<snemo::datamodel::particle_track_data>(PTD_LABEL));
  }

  out_ << indent_ << title_ << "Event data #" << _current_event_number_ << std::endl;

  if (eh_ptr != nullptr) {
    eh_ptr->tree_dump(out_, "Event header", indent_);
  } else {
    out_ << indent_ << "No event header." << std::endl;
  }

  if (sd_ptr != nullptr) {
    sd_ptr->tree_dump(out_, "Simulated data", indent_);
  } else {
    out_ << indent_ << "No simulated data." << std::endl;
  }

  if (udd_ptr != nullptr) {
    udd_ptr->tree_dump(out_, "Unified digitized data", indent_);
  } else {
    out_ << indent_ << "No unified digitized data." << std::endl;
  }

  if (cd_ptr != nullptr) {
    cd_ptr->tree_dump(out_, "Calibrated data", indent_);
  } else {
    out_ << indent_ << "No calibrated data." << std::endl;
  }

  if (tc_ptr != nullptr) {
    tc_ptr->tree_dump(out_, "Tracker clustering data", indent_);
  } else {
    out_ << indent_ << "No tracker clustering data." << std::endl;
  }

  if (tt_ptr != nullptr) {
    tt_ptr->tree_dump(out_, "Tracker trajectory data", indent_);
  } else {
    out_ << indent_ << "No tracker trajectory data." << std::endl;
  }

  if (ptd_ptr != nullptr) {
    ptd_ptr->tree_dump(out_, "Particle track data", indent_);
  } else {
    out_ << indent_ << "No particle track data." << std::endl;
  }
}

bool event_server::is_opened() const {
  return (_data_access_ != nullptr ? _data_access_->is_opened() : false);
}

std::string event_server::get_file_type_as_string() const {
  return (_data_access_ != nullptr ? _data_access_->get_file_type_as_string() : "");
}

bool event_server::rewind() {
  _current_event_number_ = -1;
  return (_data_access_ != nullptr ? _data_access_->rewind() : false);
}

bool event_server::close() { return (_data_access_ != nullptr ? _data_access_->close() : false); }

size_t event_server::get_number_of_events() const {
  return (_data_access_ != nullptr ? _data_access_->get_number_of_entries() : 0);
}

std::string event_server::get_current_filename() const {
  return (_data_access_ != nullptr ? _data_access_->get_current_filename() : "");
}

void event_server::tree_dump(std::ostream& out_, const std::string& title_,
                             const std::string& indent_, bool inherit_) const {
  std::string indent;
  if (!indent_.empty()) {
    indent = indent_;
  }
  if (!title_.empty()) {
    out_ << indent << title_ << std::endl;
  }

  out_ << indent << datatools::i_tree_dumpable::tag
       << "Sequential    : " << (has_sequential_data() ? "Yes" : "No") << std::endl;

  out_ << indent << datatools::i_tree_dumpable::tag
       << "External data : " << (has_external_data() ? "Yes" : "No") << std::endl;

  out_ << indent << datatools::i_tree_dumpable::tag
       << "Random access data : " << (has_random_data() ? "Yes" : "No") << std::endl;

  out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
       << "Data access  : " << std::endl;
  if (_data_access_ != nullptr) {
    _data_access_->tree_dump(out_, "", indent_, true);
  }
}

void event_server::dump() const {
  this->tree_dump(std::clog, "snemo::visualization::io::event_server");
}

event_server::event_selection_list_type& event_server::get_event_selection() {
  return _event_selection_;
}

const event_server::event_selection_list_type& event_server::get_event_selection() const {
  return _event_selection_;
}

bool event_server::has_selected_event() const { return !_event_selection_.empty(); }

void event_server::clear_selection() { _event_selection_.clear(); }

void event_server::select_event(const int event_number_) {
  _event_selection_.insert(event_number_);
}

void event_server::remove_event(const int event_number_) { _event_selection_.erase(event_number_); }

void event_server::fill_selection() {
  for (size_t i = 0; i < this->get_number_of_events(); ++i) {
    _event_selection_.insert(i);
  }
}

int event_server::get_first_selected_event() const {
  if (!has_selected_event()) {
    return 0;
  }
  auto it(_event_selection_.begin());
  // std::advance(it, 0);
  return *it;
}

int event_server::get_last_selected_event() const {
  if (!has_selected_event()) {
    if (has_sequential_data()) {
      return this->get_current_event_number() + 1;
    }
    return this->get_number_of_events() - 1;
  }
  auto it(_event_selection_.end());
  std::advance(it, -1);
  return *it;
}

bool event_server::_at_open_(const std::vector<std::string>& filenames_) {
  // first try : BRIO
  {
    auto* try_access = new brio_access;
    if (try_access->is_valid(filenames_)) {
      set_file_type(BRIO);
      set_sequential(false);
      _data_access_ = try_access;
      return try_access->open(filenames_);
    }
    // cleanup
    delete try_access;
  }

  // second try : Boost/Serialization
  {
    auto* try_access = new boost_access;
    if (try_access->is_valid(filenames_)) {
      set_file_type(BOOST_SERIAL);
      set_sequential(!view::options_manager::get_instance().is_preload_required());
      try_access->set_sequential(has_sequential_data());
      _data_access_ = try_access;
      return try_access->open(filenames_);
    }
    // cleanup
    delete try_access;
  }

  DT_LOG_ERROR(view::options_manager::get_instance().get_logging_priority(),
               "Data format of input file not supported !!");
  return false;
}

}  // end of namespace io

}  // end of namespace visualization

}  // end of namespace snemo

// end of event_server.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
