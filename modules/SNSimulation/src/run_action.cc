// run_action.cc

// Ourselves:
#include <snsim/run_action.h>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>

// Third party:
// - Boost:
#include <boost/filesystem.hpp>
#include <boost/thread/mutex.hpp>
// - Bayeux/datatools:
#include <datatools/io_factory.h>
#include <datatools/multi_properties.h>
#include <datatools/properties.h>
#include <datatools/things.h>
#include <datatools/utils.h>
// - Bayeux/brio :
#include <brio/writer.h>
// - Bayeux/mygsl:
#include <mygsl/seed_manager.h>
// - Bayeux/geomtools:
#include <geomtools/geomtools_config.h>
// - Bayeux/dpp:
#include <dpp/dpp_config.h>
#include <dpp/output_module.h>
// - Bayeux/genbb_help:
#include <genbb_help/genbb_help_config.h>
// - Bayeux/genvtx:
#include <genvtx/genvtx_config.h>
// - Geant4:
#include <G4Run.hh>
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

#include <G4UImanager.hh>
#include <G4VVisManager.hh>
#include <G4ios.hh>

// This project:
#include <snsim/event_action.h>
#include <snsim/manager.h>
#include <snsim/simulation_ctrl.h>
#include <mctools/mctools_config.h>
#include <mctools/simulated_data.h>
#include <mctools/utils.h>

namespace snsim {

//! \brief PIMPL-ized I/O working resources:
struct run_action::io_work_type {
  //! Default constructor
  io_work_type();
  //! Destructor
  ~io_work_type();
  // I/O engines:
  boost::scoped_ptr<datatools::data_writer> writer;  //!< Plain data writer
  boost::scoped_ptr<brio::writer> brio_writer;       //!< Plain data writer (brio)
  boost::scoped_ptr<dpp::output_module> out_module;  //!< Bank writer
  boost::scoped_ptr<datatools::things> data_record;  //!< Data record for bank
};

run_action::io_work_type::io_work_type() = default;

run_action::io_work_type::~io_work_type() {
  if (writer.get() != nullptr) {
    writer.reset();
  }
  if (brio_writer.get() != nullptr) {
    brio_writer.reset();
  }
  if (data_record.get() != nullptr) {
    data_record.reset();
  }
  if (out_module.get() != nullptr) {
    out_module.reset();
  }
}

// static
const int run_action::NUMBER_OF_EVENTS_MODULO_NONE;
const int run_action::NUMBER_OF_EVENTS_MODULO_DEFAULT;

auto run_action::is_initialized() const -> bool { return _initialized_; }

auto run_action::using_run_header_footer() const -> bool { return _use_run_header_footer_; }

void run_action::set_use_run_header_footer(bool a_new_value) {
  DT_THROW_IF(is_initialized(), std::logic_error, "Object is locked !");
  _use_run_header_footer_ = a_new_value;
}

auto run_action::grab_manager() -> manager& { return *_manager_; }

auto run_action::get_manager() const -> const manager& { return *_manager_; }

auto run_action::save_data() const -> bool { return _save_data_; }

auto run_action::has_number_of_events_modulo() const -> bool {
  return _number_of_events_modulo_ > NUMBER_OF_EVENTS_MODULO_NONE;
}

void run_action::set_number_of_events_modulo(int modulo_) {
  if (modulo_ <= NUMBER_OF_EVENTS_MODULO_NONE) {
    _number_of_events_modulo_ = NUMBER_OF_EVENTS_MODULO_NONE;
  } else {
    _number_of_events_modulo_ = modulo_;
  }
}

auto run_action::get_number_of_events_modulo() const -> int { return _number_of_events_modulo_; }

auto run_action::get_number_of_saved_events() const -> int32_t { return _number_of_saved_events_; }

auto run_action::get_number_of_processed_events() const -> int32_t {
  return _number_of_processed_events_;
}

void run_action::increment_number_of_processed_events() { _number_of_processed_events_++; }

void run_action::increment_number_of_saved_events() { _number_of_saved_events_++; }

void run_action::reset_number_of_saved_events() { _number_of_saved_events_ = 0; }

void run_action::reset_number_of_processed_events() { _number_of_processed_events_ = 0; }

void run_action::set_output_data_format(mctools::io_utils::data_format_type odf_) {
  _output_data_format_ = odf_;
  _save_data_ = true;
}

void run_action::set_output_data_bank_label(const std::string& bl_) {
  if (_output_data_format_ != mctools::io_utils::DATA_FORMAT_INVALID) {
    set_output_data_format(mctools::io_utils::DATA_FORMAT_BANK);
  }
  DT_THROW_IF(_output_data_format_ != mctools::io_utils::DATA_FORMAT_BANK, std::logic_error,
              "Cannot set output data bank label with the plain output data format!");
  _output_data_bank_label_ = bl_;
}

void run_action::set_output_file(const std::string& a_filename) {
  DT_THROW_IF(is_initialized(), std::logic_error,
              "Cannot change output  file name ! Object is locked !");
  _output_file_ = a_filename;
  _save_data_ = true;
}

auto run_action::has_out_module() const -> bool {
  return _io_work_.get() != nullptr && _io_work_->out_module.get() != nullptr;
}

auto run_action::get_out_module() const -> const dpp::output_module& {
  DT_THROW_IF(!has_out_module(), std::logic_error, "Run action has no embedded output module!");
  return *(_io_work_->out_module.get());
}

auto run_action::grab_out_module() -> dpp::output_module& {
  DT_THROW_IF(!has_out_module(), std::logic_error, "Run action has no embedded output module!");
  return *(_io_work_->out_module.get());
}

auto run_action::has_brio_writer() const -> bool {
  return _io_work_.get() != nullptr && _io_work_->brio_writer.get() != nullptr;
}

auto run_action::get_brio_writer() const -> const brio::writer& {
  DT_THROW_IF(!has_brio_writer(), std::logic_error, "Run action has no embedded brio writer!");
  return *(_io_work_->brio_writer.get());
}

auto run_action::grab_brio_writer() -> brio::writer& {
  DT_THROW_IF(!has_brio_writer(), std::logic_error, "Run action has no embedded brio writer!");
  return *(_io_work_->brio_writer.get());
}

auto run_action::has_writer() const -> bool {
  return _io_work_.get() != nullptr && _io_work_->writer.get() != nullptr;
}

auto run_action::grab_writer() -> datatools::data_writer& {
  DT_THROW_IF(!has_writer(), std::logic_error, "Run action has no embedded writer!");
  return *(_io_work_->writer.get());
}

auto run_action::get_writer() const -> const datatools::data_writer& {
  DT_THROW_IF(!has_writer(), std::logic_error, "Run action has no embedded writer!");
  return *(_io_work_->writer.get());
}

void run_action::_set_default() {
  _use_run_header_footer_ = false;
  _number_of_processed_events_ = 0;
  _number_of_events_modulo_ = NUMBER_OF_EVENTS_MODULO_NONE;
  _save_data_ = false;
  _output_data_format_ = mctools::io_utils::DATA_FORMAT_INVALID;
  _output_file_preserve_ = true;
  _output_file_dir_ = ".";
  _output_file_prefix_ = "mctools_g4_";
  _output_file_format_ = "ascii";
  _output_file_compression_ = "gzip";
  _output_file_ = "";
  _manager_ = nullptr;
  _event_action_ = nullptr;
  _brio_general_info_store_label_ = mctools::io_utils::GENERAL_INFO_STORE;
  _brio_plain_simulated_data_store_label_ = mctools::io_utils::PLAIN_SIMULATED_DATA_STORE;
}

run_action::run_action(manager& a_mgr) {
  _initialized_ = false;
  _set_default();
  _manager_ = &a_mgr;
}

run_action::~run_action() {
  if (_initialized_) {
    reset();
  }
}

void run_action::register_event_action(event_action& event_action_) {
  _event_action_ = &event_action_;
}

void run_action::reset() {
  _event_action_ = nullptr;
  if (_io_work_.get() != nullptr) {
    _io_work_.reset();
  }
}

void run_action::initialize(const datatools::properties& a_config) {
  // Fetch parameters:
  loggable_support::_initialize_logging_support(a_config);

  if (a_config.has_flag("file.using_run_header_footer")) {
    set_use_run_header_footer(true);
  }

  if (!has_number_of_events_modulo()) {
    if (a_config.has_key("number_of_events_modulo")) {
      const int number_of_events_modulo = a_config.fetch_integer("number_of_events_modulo");
      set_number_of_events_modulo(number_of_events_modulo);
    }
  }

  if (a_config.has_key("file.save")) {
    _save_data_ = a_config.fetch_boolean("file.save");
  }

  if (a_config.has_flag("file.no_preserve")) {
    _output_file_preserve_ = false;
  }

  if (_save_data_) {
    if (_output_data_format_ == mctools::io_utils::DATA_FORMAT_INVALID) {
      if (a_config.has_key("file.data_format")) {
        const std::string& data_format_label = a_config.fetch_string("file.data_format");
        mctools::io_utils::data_format_type data_format =
            mctools::io_utils::label_to_data_format(data_format_label);
        DT_THROW_IF(data_format == mctools::io_utils::DATA_FORMAT_INVALID, std::logic_error,
                    "Invalid data format '" << data_format_label << "'!");
        set_output_data_format(data_format);
      }
    }

    if (_output_file_.empty()) {
      if (a_config.has_key("file.name")) {
        const std::string output_file = a_config.fetch_string("file.name");
        set_output_file(output_file);
      }
    }
  }

  // Inhibit the data file saving in pipeine mode :
  if (grab_manager().has_simulation_ctrl()) {
    DT_LOG_NOTICE(_logprio(), "Inhibit plain data saving in thread-driven mode !");
    _save_data_ = false;
  }

  bool search_file_directives = true;
  if (!_save_data_) {
    search_file_directives = false;
  }
  if (!_output_file_.empty()) {
    search_file_directives = false;
  }

  if (search_file_directives) {
    if (a_config.has_key("file.directory")) {
      _output_file_dir_ = a_config.fetch_string("file.directory");
    }

    bool using_brio = false;
    if (a_config.has_key("file.format")) {
      std::string format = a_config.fetch_string("file.format");
      bool ok = false;
      if (format == "brio" || format == "trio") {
        using_brio = true;
        ok = true;
      } else if (format == "xml") {
        ok = true;
      } else if (format == "binary") {
        ok = true;
      } else if (format == "ascii") {
        ok = true;
      }
      if (ok) {
        _output_file_format_ = format;
      } else {
        DT_THROW_IF(true, std::logic_error, "Invalid format '" << format << "' !");
      }
    }

    if (!using_brio && a_config.has_key("file.compression")) {
      std::string compression = a_config.fetch_string("file.compression");
      bool ok = false;
      if (compression == "none") {
        ok = true;
      } else if (compression == "gzip") {
        ok = true;
      } else if (compression == "bzip2") {
        ok = true;
      }
      if (ok) {
        _output_file_compression_ = compression;
      } else {
        DT_THROW_IF(true, std::logic_error, "Invalid compression mode '" << compression << "' !");
      }
    }

    if (a_config.has_key("file.prefix")) {
      _output_file_prefix_ = a_config.fetch_string("file.prefix");
    }
  }  // (search_file_directives)

  // Allocate the PIMPL I/O structure:
  _io_work_.reset(new io_work_type);

  _initialized_ = true;
}

void run_action::dump(std::ostream& a_out) const {
  a_out << "run_action::dump:" << std::endl;
  a_out << "|-- Save data           : " << _save_data_ << std::endl;
  a_out << "|-- Output data format  : '" << _output_data_format_ << "'" << std::endl;
  a_out << "|-- Output data bank label : '" << _output_data_bank_label_ << "'" << std::endl;
  a_out << "|-- Output file dir     : '" << _output_file_dir_ << "'" << std::endl;
  a_out << "|-- Output file prefix  : '" << _output_file_prefix_ << "'" << std::endl;
  a_out << "|-- Output file format  : '" << _output_file_format_ << "'" << std::endl;
  a_out << "|-- Output compression  : '" << _output_file_compression_ << "'" << std::endl;
  a_out << "`-- Output file         : '" << _output_file_ << "'" << std::endl;
}

void run_action::_build_run_header() {
  _run_header_.set_description("snsim run header");

  const mygsl::seed_manager& the_seed_manager = _manager_->get_seed_manager();
  std::vector<std::string> seed_labels;
  the_seed_manager.get_labels(seed_labels);
  for (const auto& seed_label : seed_labels) {
    int32_t seed = the_seed_manager.get_seed(seed_label);
    std::ostringstream seed_key;
    seed_key << "seed." << seed_label;
    _run_header_.store(seed_key.str(), seed);
  }
}

void run_action::_build_run_footer() {
  _run_footer_.set_description("snsim run footer");

  _run_footer_.store("number_of_processed_events", _number_of_processed_events_);
  _run_footer_.store("number_of_saved_events", _number_of_saved_events_);

  DT_LOG_NOTICE(_logprio(), "Number of processed events = " << _number_of_processed_events_);
  if (_save_data_) {
    DT_LOG_NOTICE(_logprio(), "Number of saved events = " << _number_of_saved_events_);
  }
}

void run_action::BeginOfRunAction(const G4Run* a_run) {
  std::cout << std::flush;
  std::cerr << std::flush;
  std::clog << std::flush;
  DT_LOG_NOTICE(_logprio(), "# Run " << a_run->GetRunID() << " is starting...");
  _number_of_processed_events_ = 0;
  _number_of_saved_events_ = 0;

  /*************************************************************/
  // External threaded run control :
  if (get_manager().has_simulation_ctrl()) {
    DT_LOG_TRACE(_logprio(), "Check point.");
    simulation_ctrl& SimCtrl = grab_manager().grab_simulation_ctrl();
    {
      DT_LOG_TRACE(_logprio(), "Acquire the lock...");
      boost::mutex::scoped_lock lock(*SimCtrl.event_mutex);
      DT_LOG_TRACE(_logprio(), "Wait for the event control to be available again "
                                   << "for G4 run start and G4 event loop start ...");
      while (SimCtrl.event_availability_status == simulation_ctrl::NOT_AVAILABLE_FOR_G4) {
        DT_LOG_TRACE(_logprio(), "Not yet...");
        SimCtrl.event_available_condition->wait(*SimCtrl.event_mutex);
      }
      if (SimCtrl.event_availability_status == simulation_ctrl::ABORT) {
        DT_LOG_WARNING(_logprio(), "Detect a 'Abort' request from the external process !");
        G4RunManager::GetRunManager()->AbortRun();
      }
      DT_LOG_TRACE(_logprio(), "Starting the G4 run...");
    }
  }

  /*************************************************************/

  // Open the data writer:
  if (_save_data_) {
    bool output_file_directives = false;
    bool using_brio = false;
    if (_output_file_.empty()) {
      DT_LOG_DEBUG(_logprio(), "Trying to build the filename of the output data file...");
      std::ostringstream output_file_oss;
      if (!_output_file_dir_.empty()) {
        output_file_oss << _output_file_dir_;
        if (_output_file_dir_[_output_file_dir_.size() - 1] != '/') {
          output_file_oss << '/';
        }
      }

      // Add the file prefix:
      output_file_oss << _output_file_prefix_;

      // Run number suffix:
      output_file_oss << a_run->GetRunID();

      // Boost serialization format extension:
      if (_output_file_format_ == "ascii") {
        output_file_oss << '.' << datatools::io_factory::format::text_extension();
      } else if (_output_file_format_ == "binary") {
        output_file_oss << '.' << datatools::io_factory::format::binary_extension();
      } else if (_output_file_format_ == "xml") {
        output_file_oss << '.' << datatools::io_factory::format::xml_extension();
      } else if (_output_file_format_ == "brio") {
        using_brio = true;
        output_file_oss << brio::store_info::constants::brio_file_extension();
      } else if (_output_file_format_ == "trio") {
        using_brio = true;
        output_file_oss << brio::store_info::constants::trio_file_extension();
      }

      // Compression extension:
      if (!using_brio) {
        if (_output_file_compression_ == "gzip") {
          output_file_oss << '.' << datatools::io_factory::format::gzip_extension();
        } else if (_output_file_compression_ == "bzip2") {
          output_file_oss << '.' << datatools::io_factory::format::bzip2_extension();
        }
      }
      _output_file_ = output_file_oss.str();
      DT_LOG_DEBUG(_logprio(),
                   "The filename of the output data is built: '" << _output_file_ << "'");
      output_file_directives = true;
    }
    if (using_run_header_footer()) {
      // Rebuild the run header:
      _run_header_.clear();
      _build_run_header();
    }

    std::string output_file_name = _output_file_;
    if (_output_data_format_ == mctools::io_utils::DATA_FORMAT_BANK) {
      DT_LOG_DEBUG(_logprio(), "Initializing the 'bank' data format...");
      // Bank data format:
      // Instantiate an output module:
      using_brio = false;  // Let's the output module manage the file extension
      _io_work_->out_module.reset(new dpp::output_module);
      _io_work_->data_record.reset(new datatools::things);
      datatools::things& event_data = *(_io_work_->data_record.get());
      auto& SD = event_data.add<mctools::simulated_data>(_output_data_bank_label_);
      _event_action_->set_external_event_data(SD);
      _io_work_->out_module->set_single_output_file(output_file_name);
      _io_work_->out_module->set_preserve_existing_output(_output_file_preserve_);
      // Metadata XXX
      //_io_work_->metadata = &_io_work_->out_module->grab_metadata_store();
      _io_work_->out_module->initialize_simple();
    } else {
      DT_LOG_DEBUG(_logprio(), "Initializing the 'plain' data format...");
      // Plain data format:
      datatools::fetch_path_with_env(output_file_name);
      if (!output_file_directives) {
        // Analyse the output file extension and check if a Brio writer should
        // be used to save events:
        using_brio = false;
        const std::string extension = boost::filesystem::extension(output_file_name);
        if (extension == brio::store_info::constants::brio_file_extension()) {
          using_brio = true;
        } else if (extension == brio::store_info::constants::trio_file_extension()) {
          using_brio = true;
        }
      }
      if (using_brio) {
        DT_LOG_DEBUG(_logprio(), "'Plain' data format will use Brio based archives.");
      }
      if (using_run_header_footer()) {
        // Rebuild the run header:
        _run_header_.clear();
        _build_run_header();
      }

      if (using_brio) {
        // Instantiate a Brio writer:
        _io_work_->brio_writer.reset(new brio::writer);
        DT_LOG_NOTICE(_logprio(), "Opening brio serialization output data file '"
                                      << output_file_name << "'...");
        DT_LOG_DEBUG(_logprio(),
                     "Brio writer is opened: '" << _io_work_->brio_writer->is_opened() << "'...");
        if (_io_work_->brio_writer->is_locked()) {
          _io_work_->brio_writer->unlock();
        }
        _io_work_->brio_writer->set_existing_file_protected(_output_file_preserve_);
        _io_work_->brio_writer->open(output_file_name);
        DT_LOG_NOTICE(_logprio(), "Output data file '" << output_file_name << "' is open.");
        if (using_run_header_footer()) {
          DT_LOG_DEBUG(_logprio(),
                       "Brio writer has store '"
                           << _brio_general_info_store_label_ << "' : "
                           << _io_work_->brio_writer->has_store(_brio_general_info_store_label_));

          _io_work_->brio_writer->add_store(_brio_general_info_store_label_,
                                            datatools::properties::SERIAL_TAG);
          _io_work_->brio_writer->select_store(_brio_general_info_store_label_);
          _io_work_->brio_writer->store(_run_header_);
        }
        const std::string store = _brio_plain_simulated_data_store_label_;
        DT_LOG_DEBUG(_logprio(), "Brio writer has store '"
                                     << store
                                     << "' : " << _io_work_->brio_writer->has_store(store));
        _io_work_->brio_writer->add_store(store, mctools::simulated_data::SERIAL_TAG);
        _io_work_->brio_writer->lock();
        _io_work_->brio_writer->select_store(store);
      } else {
        // Instantiate a basic writer:
        DT_LOG_NOTICE(_logprio(), "Opening datatools serialization output data file '"
                                      << output_file_name << "'...");
        _io_work_->writer.reset(new datatools::data_writer);
        _io_work_->writer->init(output_file_name, datatools::using_multiple_archives);
        if (using_run_header_footer()) {
          _io_work_->writer->store(_run_header_);
        }
      }
    }
  }  // _save_data_

  if (G4VVisManager::GetConcreteInstance() != nullptr) {
    G4UImanager* UI = G4UImanager::GetUIpointer();
    UI->ApplyCommand("/vis/scene/notifyHandlers");
  }

  if (get_manager().using_time_stat()) {
    grab_manager().grab_CT_map()["RA"].start();
  }
  DT_LOG_NOTICE(_logprio(), "Run #" << a_run->GetRunID() << " is started.");
}

void run_action::EndOfRunAction(const G4Run* a_run) {
  DT_LOG_NOTICE(_logprio(), "Run #" << a_run->GetRunID() << " is stopping...");

  if (get_manager().using_time_stat()) {
    grab_manager().grab_CT_map()["RA"].stop();
  }

  if (using_run_header_footer()) {
    _run_footer_.clear();
    _build_run_footer();
  }

  // Close the data writer:
  if (_save_data_) {
    DT_LOG_DEBUG(_logprio(), "Closing the data writer for run #" << a_run->GetRunID() << "...");
    if (_output_data_format_ == mctools::io_utils::DATA_FORMAT_BANK) {
      // Bank data format:
      if (has_out_module()) {
        if (_io_work_->out_module->is_initialized()) {
          DT_LOG_NOTICE(_logprio(), "Closing output module ('bank' data format)...");
          _io_work_->out_module->reset();
        }
        DT_LOG_NOTICE(_logprio(), "Destroying output module ('bank' data format)...");
        _io_work_->out_module.reset();
        //_io_work_->metadata = 0;
      }
    } else {
      // Plain data format:
      if (has_brio_writer()) {
        if (_io_work_->brio_writer->is_opened()) {
          if (_use_run_header_footer_) {
            _io_work_->brio_writer->select_store(_brio_general_info_store_label_);
            _io_work_->brio_writer->store(_run_footer_);
          }
          DT_LOG_NOTICE(_logprio(), "Closing brio writer...");
          _io_work_->brio_writer->close();
          _io_work_->brio_writer->unlock();
        }
        DT_LOG_NOTICE(_logprio(), "Destroying brio writer...");
        _io_work_->brio_writer.reset();
      } else if (has_writer()) {
        if (_io_work_->writer->is_initialized()) {
          DT_LOG_NOTICE(_logprio(), "Closing datatools writer...");
          if (_use_run_header_footer_) {
            _io_work_->writer->store(_run_footer_);
          }
          _io_work_->writer->reset();
        }
        DT_LOG_NOTICE(_logprio(), "Destroying datatools writer...");
        _io_work_->writer.reset();
      }
    }
  }

  if (G4VVisManager::GetConcreteInstance() != nullptr) {
    G4UImanager::GetUIpointer()->ApplyCommand("/vis/viewer/update");
  }

  DT_LOG_NOTICE(_logprio(), "Run #" << a_run->GetRunID() << " is stopped.");
}

void run_action::store_data(const mctools::simulated_data& esd_) { _store_data(esd_); }

void run_action::_store_data(const mctools::simulated_data& esd_) {
  if (get_manager().using_time_stat()) {
    grab_manager().grab_CT_map()["IO"].start();
  }

  // First check 'brio::writer' :
  if (this->has_brio_writer()) {
    if (_io_work_->brio_writer->is_opened()) {
      _io_work_->brio_writer->store(esd_);
    }
  }

  // Then check 'datatools::data_writer' :
  if (this->has_writer()) {
    if (_io_work_->writer->is_initialized()) {
      _io_work_->writer->store(esd_);
    }
  }

  // Then check 'dpp::output_module' :
  if (this->has_out_module()) {
    if (_io_work_->out_module->is_initialized()) {
      if (_io_work_->data_record.get() != nullptr) {
        dpp::base_module::process_status ps =
            _io_work_->out_module->process(*(_io_work_->data_record.get()));
        if (ps != dpp::base_module::PROCESS_OK) {
          DT_THROW(std::runtime_error, "Unable to process the output module!");
        }
      }
    }
  }

  if (get_manager().using_time_stat()) {
    grab_manager().grab_CT_map()["IO"].stop();
  }

  increment_number_of_saved_events();
}

}  // namespace snsim

/** Opening macro for implementation
 *  This macro must be used outside of any namespace.
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snsim::run_action, ocd_) {
  // The class name :
  ocd_.set_class_name("snsim::run_action");

  // The class terse description :
  ocd_.set_class_description("The Geant4 simulation mandatory run action");

  // The library the class belongs to :
  ocd_.set_class_library("mctools_g4");

  // The class detailed documentation :
  ocd_.set_class_documentation("This is Geant4 simulation engine embedded run action. \n");

  {
    // Description of the 'logging.priority' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("logging.priority")
        .set_terse_description("Logging priority threshold")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_long_description(
            "Allowed values are:                                    \n"
            "                                                       \n"
            " * ``\"fatal\"``       : print fatal error messages    \n"
            " * ``\"critical\"``    : print critical error messages \n"
            " * ``\"error\"``       : print error messages          \n"
            " * ``\"warning\"``     : print warnings                \n"
            " * ``\"notice\"``      : print notice messages         \n"
            " * ``\"information\"`` : print informational messages  \n"
            " * ``\"debug\"``       : print debug messages          \n"
            " * ``\"trace\"``       : print trace messages          \n"
            "                                                       \n"
            "Default value: ``\"warning\"``                         \n"
            "                                                       \n"
            "Example::                                              \n"
            "                                                       \n"
            "  logging.priority : string = \"warning\"              \n"
            "                                                       \n");
  }

  {
    // Description of the 'number_of_events_modulo' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("number_of_events_modulo")
        .set_terse_description("The period to print the number of processed event")
        .set_traits(datatools::TYPE_INTEGER)
        .set_mandatory(false)
        .set_long_description(
            "Default value: ``0`` (no print)                         \n"
            "                                                        \n"
            "Example::                                               \n"
            "                                                        \n"
            "  number_of_events_modulo : integer = 100               \n"
            "                                                        \n");
  }

  {
    // Description of the 'file.save' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("file.save")
        .set_terse_description("Flag to inhibit the saving of simulated data in some output file")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_mandatory(false)
        .set_long_description(
            "Default value: ``0`` (don't save data)                        \n"
            "                                                              \n"
            "Example::                                                     \n"
            "                                                              \n"
            "  file.save : boolean = 1                                     \n"
            "                                                              \n"
            "This flag may be activated when the Geant4 simulation         \n"
            "engine is used from the ``g4_production`` executable.         \n"
            "This flag must be deactivated when the Geant4 simulation      \n"
            "engine is used from the ``snsim::simulation_module``    \n"
            "class through a data processing pipeline because in this case \n"
            "the pipeline generally implements its own I/O mechanisms.     \n");
  }

  {
    // Description of the 'file.no_preserve' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("file.no_preserve")
        .set_terse_description("Flag to allow the overwriting of some already existing output file")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_mandatory(false)
        .set_triggered_by_flag("file.save")
        .set_long_description(
            "Default value: ``0`` (preserve existing data file)            \n"
            "                                                              \n"
            "Example::                                                     \n"
            "                                                              \n"
            "  file.no_preserve : boolean = 0                              \n"
            "                                                              \n"
            "When activated, this flag may implies the deletion of some    \n"
            "important data file. use it at your own risk !                \n");
  }

  {
    // Description of the 'file.name' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("file.name")
        .set_terse_description("The name of the file where to save the simulated data")
        .set_traits(datatools::TYPE_STRING)
        .set_path(true)
        .set_complex_triggering_conditions(true)
        //.set_triggered_by("file.save")
        .set_long_description(
            "This property is not taken into account if the *output file*  \n"
            "attribute has already been set by a previous  call to the     \n"
            "``snsim::run_action::set_output_file(..;)`` method.     \n"
            "                                                              \n"
            "Example::                                                     \n"
            "                                                              \n"
            "  file.name : string as path = \"g4_production.out.xml\"      \n"
            "                                                              \n"
            "One must use a special extension corresponding to a           \n"
            "data format supported by Bayeux through the ``datatools`` and \n"
            "``brio`` libraries : \n"
            "                                                              \n"
            "  * ``.xml`` or ``.xml.gz`` or ``.xml.bz2`` : Boost XML archive\n"
            "  * ``.txt`` or ``.txt.gz`` or ``.txt.bz2`` : Boost ASCII archive\n"
            "  * ``.data`` or ``.data.gz`` or ``.data.bz2``: Boost binary archive\n"
            "  * ``.brio`` or ``.trio`` : resp. Brio binary and Brio ASCII \n"
            "                                                              \n");
  }

  {
    // Description of the 'file.directory' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("file.directory")
        .set_terse_description("The directory where the output data file is stored")
        .set_traits(datatools::TYPE_STRING)
        .set_path(true)
        .set_complex_triggering_conditions(true)
        .set_long_description(
            "This property is not taken into account if the *output file*  \n"
            "attribute has already been set by a previous  call to the     \n"
            "``snsim::run_action::set_output_file(..;)`` method or   \n"
            "by the ``file.directory`` property.                           \n"
            "                                                              \n"
            "Example::                                                     \n"
            "                                                              \n"
            "  file.directory : string as path = \"/tmp/data\"             \n"
            "                                                              \n");
  }

  {
    // Description of the 'file.format' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("file.prefix")
        .set_terse_description("The prefix of the output data file name")
        .set_traits(datatools::TYPE_STRING)
        .set_complex_triggering_conditions(true)
        .set_long_description(
            "This property is not taken into account if the *output file*  \n"
            "attribute has already been set by a previous  call to the     \n"
            "``snsim::run_action::set_output_file(..;)`` method or   \n"
            "by the ``file.directory`` property.                           \n"
            "                                                              \n"
            "Example::                                                     \n"
            "                                                              \n"
            "  file.prefix : string = \"g4_production_\"                   \n"
            "                                                              \n"
            "Note that the Geant4 current run number will be appended to   \n"
            "the name of the output data file.                             \n");
  }

  {
    // Description of the 'file.format' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("file.format")
        .set_terse_description("The format of the output data file")
        .set_traits(datatools::TYPE_STRING)
        .set_complex_triggering_conditions(true)
        .set_long_description(
            "This property is not taken into account if the *output file*  \n"
            "attribute has already been set by a previous  call to the     \n"
            "``snsim::run_action::set_output_file(..;)`` method or   \n"
            "by the ``file.directory`` property.                           \n"
            "                                                              \n"
            "Allowed values:                                               \n"
            "                                                              \n"
            " ``\"xml\" : Use the Boost portable XML archive format        \n"
            " ``\"ascii\" : Use the Boost portable text archive format     \n"
            " ``\"binary\" : Use the Boost portable binary archive format  \n"
            " ``\"brio\" : Use the portable Brio format (binary)           \n"
            " ``\"trio\" : Use the portable Brio format (text)             \n"
            "                                                              \n"
            "Example::                                                     \n"
            "                                                              \n"
            "  file.format : string = \"xml\"                              \n"
            "                                                              \n");
  }

  {
    // Description of the 'file.format' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("file.compression")
        .set_terse_description("The compression of the output data file")
        .set_traits(datatools::TYPE_STRING)
        .set_complex_triggering_conditions(true)
        .set_long_description(
            "This property is not taken into account if the *output file*  \n"
            "attribute has already been set by a previous  call to the     \n"
            "``snsim::run_action::set_output_file(..;)`` method or   \n"
            "by the ``file.directory`` property.                           \n"
            "                                                              \n"
            "This property is not taken into account for Brio format which \n"
            "is already natively compressed.                               \n"
            "                                                              \n"
            "Allowed values:                                               \n"
            "                                                              \n"
            " ``\"none\" : No compression of the output data file          \n"
            " ``\"gzip\" : Use GZIP compression                            \n"
            " ``\"bzip2\" : Use BZIP2 compression                          \n"
            "                                                              \n"
            "Example::                                                     \n"
            "                                                              \n"
            "  file.compression : string = \"gzip\"                        \n"
            "                                                              \n");
  }

  {
    // Description of the 'file.using_run_header_footer' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("file.using_run_header_footer")
        .set_terse_description(
            "Flag to activate the saving of special informations in run header and footer data "
            "structure")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_mandatory(false)
        .set_triggered_by_flag("file.save")
        .set_long_description(
            "Default value: ``0``                                    \n"
            "                                                        \n"
            "Example::                                               \n"
            "                                                        \n"
            "  file.using_run_header_footer : boolean = 0            \n"
            "                                                        \n");
  }

  // Additionnal configuration hints :
  ocd_.set_configuration_hints(
      "Typical configuration is::                                             \n"
      "                                                                       \n"
      " #@description Run action logging priority                             \n"
      " logging.priority : string = \"warning\"                               \n"
      "                                                                       \n"
      " #@description Print period for event number                           \n"
      " number_of_events_modulo : integer = 100                               \n"
      "                                                                       \n"
      " #@description Inhibit output data file                                \n"
      " file.save : boolean = 1                                               \n"
      "                                                                       \n"
      " #@description Print period for event number                           \n"
      " file.no_preserve : boolean = 0                                        \n"
      "                                                                       \n"
      " #@description The name of the output data file                        \n"
      " #file.name : string as path = \"/tmp/data/g4_sim_data.xml\"           \n"
      "                                                                       \n"
      " #@description Output file directory                                   \n"
      " file.directory : string as path = \"/tmp/data\"                       \n"
      "                                                                       \n"
      " #@description Output file prefix                                      \n"
      " file.prefix : string = \"g4_sim_data_\"                               \n"
      "                                                                       \n"
      " #@description Output file format                                      \n"
      " file.format : string = \"binary\"                                     \n"
      "                                                                       \n"
      " #@description Output file compression                                 \n"
      " file.compression : string = \"gzip\"                                  \n"
      "                                                                       \n"
      " #@description Save run header/footer information                      \n"
      " file.using_run_header_footer : boolean = 0                            \n"
      "                                                                       \n");

  ocd_.set_validation_support(true);

  // Lock the description:
  ocd_.lock();

  // ... and we are done.
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()  // Closing macro for implementation

// Registration macro for class 'snsim::manager' :
DOCD_CLASS_SYSTEM_REGISTRATION(snsim::run_action, "snsim::run_action")
