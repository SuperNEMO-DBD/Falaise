// -*- mode: c++ ; -*-
/** \file falaise/snemo/services/run_info_service.cc */

// Ourselves:
#include <falaise/snemo/services/run_info_service.h>

// Boost;
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

// This project:
#include <falaise/snemo/services/services.h>

DATATOOLS_SERVICE_REGISTRATION_IMPLEMENT(snemo::run_info_service,
                                         "snemo::run_info_service")

namespace snemo {

  run_info_service::run_info_service()
    : datatools::base_service("RunInfoService",
                              "Run info service",
                              "Service publishing run info")
  {
    return;
  }

  run_info_service::~run_info_service()
  {
    if (is_initialized()) {
      reset();
    }
    return;
  }
  
  bool run_info_service::is_initialized() const
  {
    return _initialized_;
  }

  int run_info_service::initialize(const datatools::properties & config_,
                                   datatools::service_dict_type & services_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Service is already initialized!");
 
    if (config_.has_key("mode")) {
      std::string modeLabel = config_.fetch_string("mode");
      if (modeLabel == "files") {
        _mode_ = MODE_FILES;
      } else if (modeLabel == "db") {
        _mode_ = MODE_DB;
      } else {
        DT_THROW(std::logic_error, "Invalid mode label '" << modeLabel << "'!");
      }
    }

    if (_mode_ == MODE_DB) {
      if (config_.has_key("db_label")) {
        _db_label_ = config_.fetch_string("db_label");     
      }
      if (_db_label_.empty()) {
        _db_label_ = service_info::dbServiceName();
      }
      if (datatools::has<snemo::db_service>(services_, _db_label_)) {
        _db_service_ = &datatools::get<snemo::db_service>(services_, _db_label_);
      }
      datatools::properties dbConfig;
      config_.export_and_rename_starting_with(dbConfig, "db.", "");
      _init_mode_db_(dbConfig);
    }

    if (_mode_ == MODE_FILES) {
      datatools::properties filesConfig;
      config_.export_and_rename_starting_with(filesConfig, "files.", "");
      _init_mode_files_(filesConfig);
    }
      
    _initialized_ = true;
    return 0;
  }

  int run_info_service::reset()
  {
    DT_THROW_IF(not is_initialized(), std::logic_error, "Service is not initialized!");
    
    _initialized_ = false;
    _runs_.reset();
    if (_mode_ == MODE_DB) {
      _terminate_mode_db_();
    } else if (_mode_ == MODE_FILES) {
      _terminate_mode_files_();
    }
   
    _mode_ = MODE_DEFAULT;
    return 0;
  }

  run_info_service::mode_type run_info_service::get_mode() const
  {
    return _mode_;
  }

  void run_info_service::_init_mode_db_(const datatools::properties &)
  {
    DT_THROW(std::logic_error, "Mode 'db' is not implemented yet!");
    return;
  }

  void run_info_service::_terminate_mode_db_()
  {
    DT_THROW(std::logic_error, "Mode 'db' is not implemented yet!");
    return;
  }
  
  void run_info_service::_init_mode_files_(const datatools::properties & config_)
  {
    if (config_.has_key("run_lists")) {
      std::vector<std::string> runLists;
      config_.fetch("run_lists", runLists);
      for (const auto & runList:runLists) {
        load_run_list(runList);
      }
    }
    DT_LOG_DEBUG(get_logging_priority(), "Number of runs : " << _runs_.size());
    if (datatools::logger::is_debug(get_logging_priority())) {
      for (const auto & r : _runs_.runs()) {
        std::cerr << "[debug] " << "RunID=" << r.first << std::endl;
      }
    }
    return;
  }

  void run_info_service::_terminate_mode_files_()
  {
    return;
  }

  const rc::run_list & run_info_service::get_run_list() const
  {
    return _runs_;
  }

  rc::run_list & run_info_service::grab_run_list()
  {
    return _runs_;
  }
 
  bool run_info_service::has_run_info(const rc::run_id_type run_id_) const
  {
    return _runs_.has_run(run_id_) != 0;
  }
   
  const rc::run_description &
  run_info_service::get_run_info(const rc::run_id_type run_id_) const
  {
    if (_mode_ == MODE_DB) {
      DT_THROW(std::logic_error, "Mode 'db' is not implemented yet!");
    } 
    auto found = _runs_.runs().find(run_id_);
    DT_THROW_IF(found == _runs_.runs().end(), std::logic_error,
                "Cannot find run description for run ID=" << run_id_ << '!');
    return found->second;
  }

  void run_info_service::load_run_list(const std::string & infile_)
  {
    std::string filename = infile_;
    datatools::fetch_path_with_env(filename);
    std::ifstream fin(filename);
    DT_THROW_IF(!fin, std::runtime_error, "Cannot open run list file '" << filename << "'!");
    datatools::multi_properties runListConfig("run", "type");
    std::string runListConfigFilename = filename;
    runListConfig.read(runListConfigFilename);
    if (datatools::logger::is_debug(get_logging_priority())) {
      boost::property_tree::ptree options;
      options.put("indent", "[debug] ");
      options.put("title", "Run list config from '" + infile_ + "'");
      runListConfig.print_tree(std::cerr, options);
    }
    _runs_.load(runListConfig);
    return;
  }
 
  void run_info_service::print_tree(std::ostream & out_,
                                               const boost::property_tree::ptree & options_) const
  {
    i_tree_dumpable::base_print_options popts;
    popts.configure_from(options_);
    this->base_service::print_tree(out_, base_print_options::force_inheritance(options_));
  
    out_ << popts.indent << i_tree_dumpable::tag
         << "Mode : ";
    if (_mode_ == MODE_FILES) out_ << "'file'";
    else out_ << "'db'";
    out_ << std::endl;
 
    out_ << popts.indent << i_tree_dumpable::tag
         << "DB label : '" << _db_label_ << "'" << std::endl;

    out_ << popts.indent << i_tree_dumpable::tag
         << "DB service : " << std::boolalpha << (_db_service_ != nullptr) << std::dec << std::endl;
    
    out_ << popts.indent << i_tree_dumpable::inherit_tag(popts.inherit)
         << "Runs: " << _runs_.size() << std::endl;

    {
      boost::property_tree::ptree popts2;
      std::string inheritLastItemTag = (popts.inherit ? "|   " : "    ");
      popts2.put("indent", popts.indent + inheritLastItemTag);
      _runs_.print_tree(out_, popts2);
    }
    
    // {
    //   uint32_t rCount = 0;
    //   for (const auto & r : _runs_.runs()) {
    //     out_ << popts.indent << i_tree_dumpable::inherit_skip_tag(popts.inherit);
    //     bool lastItem = false;
    //     std::string inheritLastItemTag = (popts.inherit ? "|   " : "    ");
    //     if (rCount + 1 == _runs_.size()) {
    //       lastItem = true;
    //     }
    //     if (lastItem) {
    //       out_ << i_tree_dumpable::last_tag;
    //     } else {
    //       out_ << i_tree_dumpable::tag;
    //     } 
    //     out_ << "Description for run " << r.first << " : " << std::endl;
    //     {
    //       boost::property_tree::ptree popts2;
    //       popts2.put("indent", popts.indent + inheritLastItemTag + (lastItem ? "    ": "|   "));
    //       r.second.print_tree(out_, popts2);
    //     }
    //     rCount++;
    //   }
    // }

    return;
  }
  
}  // namespace snemo
