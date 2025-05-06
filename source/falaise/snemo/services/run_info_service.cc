// -*- mode: c++ ; -*-
/** \file falaise/snemo/services/run_info_service.cc */

// Ourselves:
#include <falaise/snemo/services/run_info_service.h>

// Standard library:
#include <cstdlib>
#include <string>
#include <algorithm> // copy
#include <iterator>  // ostream_operator

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
    this->datatools::base_service::common_initialize(config_);

    if (config_.has_key("mode")) {
      std::string modeLabel = config_.fetch_string("mode");
      if (modeLabel == "files") {
        _mode_ = MODE_FILES;
      } else if (modeLabel == "csv_files") {
        _mode_ = MODE_CSV_FILES;
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
      DT_LOG_DEBUG(get_logging_priority(), "default DB label = " << std::quoted(service_info::dbServiceName()));
      DT_LOG_DEBUG(get_logging_priority(), "DB label = " << std::quoted(_db_label_));
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

    if (_mode_ == MODE_CSV_FILES) {
      datatools::properties csvFilesConfig;
      config_.export_and_rename_starting_with(csvFilesConfig, "csv_files.", "");
      _init_mode_csv_files_(csvFilesConfig);
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
    } else if (_mode_ == MODE_CSV_FILES) {
      _terminate_mode_csv_files_();
    }
   
    _mode_ = MODE_DEFAULT;
    return 0;
  }

  run_info_service::mode_type run_info_service::get_mode() const
  {
    return _mode_;
  }

  void run_info_service::_init_mode_db_(const datatools::properties & db_config_)
  {
    datatools::logger::priority logging = get_logging_priority();
    logging = datatools::logger::PRIO_DEBUG;
    DT_LOG_DEBUG(logging, "Number of runs : " << _runs_.size());
    std::uint32_t runMin = 0u;
    std::uint32_t runMax = 0xFFFFFFFF;
    if (db_config_.has_key("run_min")) {
      runMin = db_config_.fetch_positive_integer("run_min");
    }
    if (db_config_.has_key("run_max")) {
      runMax = db_config_.fetch_positive_integer("run_max");
    }

    DT_LOG_DEBUG(logging, "DB mode: run min = " << runMin);
    DT_LOG_DEBUG(logging, "DB mode: run max = " << runMax);
    snemo::db::table_selection_type tableSel;
    if (_db_service_ == nullptr) {
      DT_THROW(std::logic_error, "DB service nullptr !!!");
    }
    _db_service_->process_select_all_statement("_test_run_infos", tableSel);
    DT_LOG_DEBUG(logging, "DB mode: Table selection size = " << tableSel.size());
    DT_LOG_DEBUG(logging, "DB mode: Parsing table selection...");
    namespace snrc = snemo::rc;
    namespace snt  = snemo::time;
    for (auto iRow = 0u; iRow < tableSel.size(); iRow++) {
      DT_LOG_DEBUG(logging, "DB mode: Fetching run infos from row #" << iRow << " : ");
      const snemo::db::record_type row = tableSel[iRow];
      snrc::run_id_type runNumber = 0u;
      std::uint32_t runNumberOfEvents = 0u;
      snt::time_duration runDeadtime = snt::from_quantity(0u * CLHEP::second);
      runNumber = std::get<int>(row[0]);
      DT_LOG_DEBUG(logging, "  runNumber = " << runNumber);
      std::uint32_t runCatValue = std::get<int>(row[1]);
      DT_LOG_DEBUG(logging, "  runCatValue = " << runCatValue);
      snrc::run_category runCat = snrc::run_category_from_uint(runCatValue);
      DT_LOG_DEBUG(logging, "  runCat = " << runCat);
      std::string runStartTimeStr = std::get<std::string>(row[2]);
      DT_LOG_DEBUG(logging, "  runStartTimeStr = " << runStartTimeStr);
      snt::time_point runStartTime = snt::time_point_from_string(runStartTimeStr);
      DT_LOG_DEBUG(logging, "  runStartTime = " << snt::to_string(runStartTime));
      std::uint32_t runDurationSec = std::get<int>(row[3]);
      DT_LOG_DEBUG(logging, "  runDurationSec = " << runDurationSec);
      snt::time_duration runDuration = snt::from_quantity(runDurationSec * CLHEP::second);
      DT_LOG_DEBUG(logging, "  runDuration = " << snt::to_string(runDuration));
      snt::time_period runPeriod(runStartTime, runDuration);
      DT_LOG_DEBUG(logging, "  runPeriod = " << snt::to_string(runPeriod));
      DT_LOG_DEBUG(logging, "  runDeadtime = " << snt::to_string(runDeadtime));
      snrc::run_description runDesc =
	snrc::run_description::make_unique_slice(runNumber,
						 runCat,
						 runPeriod,
						 runNumberOfEvents,
						 runDeadtime);
      runDesc.print_tree(std::clog);
      if ((std::uint32_t) runNumber < runMin or (std::uint32_t) runNumber > runMax) {
	continue;
      }
      DT_LOG_DEBUG(logging, " Adding description of run #" << runNumber);
      _runs_.add_run(runDesc);
    }
    
       
    if (datatools::logger::is_debug(logging)) {
      for (const auto & r : _runs_.runs()) {
        DT_LOG_DEBUG(logging, "RunID=" << r.first);
      }
    }
    return;
  }

  void run_info_service::_terminate_mode_db_()
  {
    // DT_THROW(std::logic_error, "Mode 'db' is not implemented yet!");
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
  
  void run_info_service::_init_mode_csv_files_(const datatools::properties & config_)
  {
    if (config_.has_key("run_lists")) {
      std::vector<std::string> runLists;
      config_.fetch("run_lists", runLists);
      for (const auto & runList:runLists) {
        load_run_csv_list(runList);
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

  void run_info_service::_terminate_mode_csv_files_()
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
 
  void run_info_service::load_run_csv_list(const std::string & infile_)
  {
    std::string filename = infile_;
    datatools::fetch_path_with_env(filename);
    std::ifstream fin(filename);
    DT_THROW_IF(!fin, std::runtime_error, "Cannot open run list file '" << filename << "'!");
    datatools::multi_properties runListConfig("run", "type");
    // static boost::escaped_list_separator<char> _separators("", " ,;|", "");
    static boost::char_separator<char> _separators(" \t", "", boost::drop_empty_tokens);
    // typedef boost::tokenizer< boost::escaped_list_separator<char> > Tokenizer;
    typedef boost::tokenizer< boost::char_separator<char> > Tokenizer;
    std::vector<std::string> vec;
    std::string line;
    auto nLine = 0u;
    while (std::getline(fin,line)) {
      nLine++;
      if (line.empty() or line[0] == '#') {
	continue;
      }
      Tokenizer tok(line, _separators);
      vec.assign(tok.begin(),tok.end());
      std::cerr << "vec = ";
      std::copy(vec.begin(), vec.end(), std::ostream_iterator<std::string>(std::cerr, "|"));
      std::cerr << std::endl;

      DT_THROW_IF(vec.size() < 4, std::runtime_error, "Invalid format for CSV run list file '" << filename << "' (line #" << nLine << ")!");
      
      snemo::rc::run_id_type  run_id  = std::stoi(vec[0]);
      snemo::rc::run_category run_cat = snemo::rc::run_category_from_string(vec[1]);
      double qRunStartSecEpoch        = std::stod(vec[2]) * CLHEP::second;
      double qRunDurationRepr         = std::stod(vec[3]) * CLHEP::second;
      namespace snt = snemo::time;
      snt::time_point runStart = snt::time_point_from_epoch_sec(qRunStartSecEpoch);
      snt::time_duration runDuration = snt::time_duration_from_sec(qRunDurationRepr);
      std::uint32_t numberOfEvents = 0;
      snt::time_period runPeriod(runStart, runDuration);
      snt::time_duration runDeadtime(0,0,0,0);
      // Add a record for this run
      datatools::properties & run_section = runListConfig.add_section(std::to_string(run_id), "snemo::rc::run_description");
      run_section.store_string("category", snemo::rc::to_string(run_cat));
      run_section.store_string("period", snt::to_string(runPeriod));
      run_section.store_integer("number_of_events", numberOfEvents);
      run_section.store_string("deadtime", snt::to_string(runDeadtime));
    }
      
    
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
