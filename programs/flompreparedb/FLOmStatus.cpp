#include <falaise/tags.h>

#include <fstream>
#include <sstream>
#include <bitset>
#include <filesystem>
#include <cstdlib>

#include <gsl/gsl_cdf.h>

#include <bayeux/datatools/kernel.h>
#include <bayeux/datatools/urn.h>
#include <bayeux/datatools/urn_query_service.h>
#include <bayeux/datatools/logger.h>
#include <bayeux/datatools/exception.h>
#include <bayeux/datatools/multi_properties.h>
#include <bayeux/geomtools/manager.h>
#include <bayeux/geomtools/geometry_service.h>
#include <bayeux/geomtools/mapping.h>
#include <bayeux/datatools/clhep_units.h>

#include <falaise/snemo/services/services.h>
#include <falaise/snemo/datamodels/geomid_utils.h>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string.hpp>

#include "FLOmStatus.hpp"

namespace FLOmStatus {

  void Config::print(std::ostream & out_) const
  {
    out_ << "geomConfigUrn      = " << geomConfigUrn << '\n';
    out_ << "variantConfigUrn   = " << variantConfigUrn << '\n';
    out_ << "variantConfigPath  = " << variantConfigPath << '\n';
    out_ << "variantProfileUrn  = " << variantProfileUrn << '\n';
    out_ << "variantProfilePath = " << variantProfilePath << '\n';
    out_ << "servicesConfigUrn  = " << servicesConfigUrn << '\n';
    out_ << "servicesConfigPath = " << servicesConfigPath << '\n';
    out_ << "omStatusPath  = " << omStatusPath << '\n';
    out_ << "calibDataPath = " << calibDataPath << '\n';
    out_ << "calibVersion  = " << calibVersion << '\n';
    out_ << "target        = " << target << '\n';
    out_ << "calibMethod   = " << calibMethod << '\n';
    out_ << "inputFileList = " << inputFileList << '\n';
    out_ << "#inputFiles   = " << inputFiles.size() << '\n';
    for (auto i = 0u; i < inputFiles.size(); i++) {
      out_ << "Input file #" << i << " = " << inputFiles[i] << '\n';
    }
    out_ << "outputFormat = " << outputFormat << '\n';
    out_ << "outputFile   = " << outputFile << '\n';
    return;
  }

  void Application::load_list(const std::string & list_path_)
  {
    std::string listPath = list_path_;
    datatools::fetch_path_with_env(listPath);
    std::ifstream fin(listPath);
    auto lineCount = 0u;
    while (fin and not fin.eof()) {
      std::string rawLine;
      std::getline(fin, rawLine);
      boost::algorithm::trim(rawLine);
      lineCount++;
      if (rawLine.empty()) continue;
      if (rawLine[0] == '#') continue;
      std::string inputFile = rawLine;
      if (inputFile[0] != '/') {
	inputFile = this->config.calibDataPath + "/" + inputFile;
      }
      this->config.inputFiles.push_back(inputFile);
    }
    return;
  }
 
  void Application::init(const Config & config_)
  {
    
    this->config = config_;

    if (this->config.calibDataPath.empty()) {
      if (const char * envPath = std::getenv("SNOMCALIBRATIONDATA_DIR")) {    
	this->config.calibDataPath =  std::string(envPath);
 	DT_LOG_DEBUG(config.verbosity,
		     "OM calibration data path = " << std::quoted(this->config.calibDataPath));
      } else {
	DT_THROW(std::logic_error, "No SNOMCALIBRATIONDATA_DIR environment variable");
      }
    }

    if (this->config.omStatusPath.empty()) {
      std::string knownOmStatusPath = this->config.calibDataPath
	+ "/data/" + this->config.calibVersion  + "/known_om_status_map.csv";
      datatools::fetch_path_with_env(knownOmStatusPath);
      if (std::filesystem::exists(knownOmStatusPath)) {
	this->config.omStatusPath = knownOmStatusPath;
      }
    }
    
    if (not config.omStatusPath.empty()) {
      DT_LOG_DEBUG(config.verbosity,
		   "Using OM status path = " << std::quoted(this->config.omStatusPath));
    }
 
    if (this->config.inputFileList.empty()) {
      this->config.inputFileList = this->config.calibDataPath
	+ "/data/" + this->config.calibVersion  + "/OM_status_calib.lis";
    }
   
    if (not this->config.inputFileList.empty()) {
      DT_LOG_DEBUG(config.verbosity,
		   "Loading list of input files = " << std::quoted(this->config.inputFileList));
      datatools::fetch_path_with_env(this->config.inputFileList);
      DT_THROW_IF(not std::filesystem::exists(this->config.inputFileList), std::logic_error,
		  "file " << std::quoted(config.inputFileList) << " des not exist");
      load_list(config.inputFileList);
    }

    if (datatools::logger::is_debug(config.verbosity)) {
      for (const auto & inputFile : config.inputFiles) {
	std::cerr << "[debug] Input file = " << std::quoted(inputFile) << '\n';
      } 
    }
    
    if (this->config.calibMethod == "pol1" or this->config.calibMethod == "pol2") {
      DT_LOG_DEBUG(config.verbosity, "config.calibMethod = " << config.calibMethod);
    } else {
      // DT_THROW(std::logic_error, "Unspecified requested calibration method " << std::quoted(this->config.calibMethod));
    }

    datatools::kernel & bxKernel = datatools::kernel::instance();
    const datatools::urn_query_service & bxUrnQuery = bxKernel.get_urn_query();
    {
      const std::string & geomConfigCategory = falaise::tags::geometry_setup_category();
      DT_THROW_IF(!bxUrnQuery.check_urn_info(config.geomConfigUrn, geomConfigCategory),
		  std::logic_error,
		  "Cannot query URN='" << config.geomConfigUrn << "'!");
    }
    datatools::urn_info geomUrnInfo =
      bxUrnQuery.get_urn_info(config.geomConfigUrn);
    if (datatools::logger::is_debug(config.verbosity)) {
      geomUrnInfo.tree_dump(std::cerr, "geomUrnInfo", "[debug] ");
    }

    if (geomUrnInfo.has_topic("alias_of")) {
      std::string aliasOf = geomUrnInfo.get_component("alias_of");
      DT_LOG_DEBUG(config.verbosity, "found aliasOf = " << aliasOf);
      config.geomConfigUrn = aliasOf;
      geomUrnInfo = bxUrnQuery.get_urn_info(config.geomConfigUrn);
      if (datatools::logger::is_debug(config.verbosity)) {
	geomUrnInfo.tree_dump(std::cerr, "geomUrnInfo", "[debug] ");
      }
    }

    if (geomUrnInfo.has_topic("variants")) {
      DT_LOG_DEBUG(config.verbosity, "has variants");
      const std::vector<std::string> & vv = geomUrnInfo.get_components_by_topic("variants");
      std::string variantConfigUrn;
      if (vv.size() == 1) {
	variantConfigUrn = geomUrnInfo.get_component("variants");
	DT_LOG_DEBUG(config.verbosity, "variantConfigUrn = " << variantConfigUrn);
      }
      config.variantConfigUrn = variantConfigUrn;
      if (not config.variantConfigUrn.empty()) {
        // Resolve variants file:
        std::string conf_variants_category = "configuration";
        std::string conf_variants_mime;
        std::string conf_variants_path;
        DT_THROW_IF(! bxUrnQuery.resolve_urn_to_path(config.variantConfigUrn,
						     conf_variants_category,
						     conf_variants_mime,
						     conf_variants_path),
                    std::logic_error,
                    "Cannot resolve URN='" << config.variantConfigUrn << "'!");
        config.variantConfigPath = conf_variants_path;
        DT_LOG_DEBUG(config.verbosity,
		     "Resolved variant service's configuration file is '" << conf_variants_path << "'");
      }
    }

    // Variants profile:
    if (not config.variantProfilePath.empty()) {
      // Force the variant profile path:
      DT_THROW_IF(not config.variantProfileUrn.empty(),
                  std::logic_error,
                  "Required variants profile URN='"
                  << config.variantProfileUrn << "' "
                  << "conflicts with required variants profile path='"
                  << config.variantProfilePath << "'!");
    } else if (not config.variantProfileUrn.empty()) {
      // Determine the variant profile path from a blessed variant profile URN:
      std::string conf_variantsProfile_category = "configuration";
      std::string conf_variantsProfile_mime;
      std::string conf_variantsProfile_path;
      DT_THROW_IF(! bxUrnQuery.resolve_urn_to_path(config.variantProfileUrn,
						   conf_variantsProfile_category,
						   conf_variantsProfile_mime,
						   conf_variantsProfile_path),
                  std::logic_error,
                  "Cannot resolve variants profile URN='" << config.variantProfileUrn << "'!");
      config.variantProfilePath = conf_variantsProfile_path;
    }

    DT_LOG_DEBUG(config.verbosity, "variantProfilePath = " << config.variantProfilePath);

    // Start the variant service:
    datatools::configuration::variant_service & theVariantService = this->variantService;
    datatools::configuration::variant_service::config variantParams;
    variantParams.config_filename = config.variantConfigPath;
    variantParams.profile_load = config.variantProfilePath;
    theVariantService.configure(variantParams);
    theVariantService.start();
    {
      const std::string & servicesConfigCategory = falaise::tags::services_category();
      DT_THROW_IF(!bxUrnQuery.check_urn_info(config.servicesConfigUrn, servicesConfigCategory),
		  std::logic_error,
		  "Cannot query URN='" << config.servicesConfigUrn << "'!");
    }
    datatools::urn_info servicesUrnInfo =
      bxUrnQuery.get_urn_info(config.servicesConfigUrn);
    if (datatools::logger::is_debug(config.verbosity)) {
      servicesUrnInfo.tree_dump(std::cerr, "servicesUrnInfo", "[debug] ");
    }
    // Determine the variant profile path from a blessed variant profile URN:
    std::string conf_services_category = "configuration";
    std::string conf_services_mime;
    std::string conf_services_path;
    DT_THROW_IF(! bxUrnQuery.resolve_urn_to_path(config.servicesConfigUrn,
						 conf_services_category,
						 conf_services_mime,
						 conf_services_path),
		std::logic_error,
		"Cannot resolve profile URN='" << config.servicesConfigUrn << "'!");
    config.servicesConfigPath = conf_services_path;

    
    services.set_logging_priority(config.verbosity);
    datatools::multi_properties servicesConfig;
    datatools::fetch_path_with_env(config.servicesConfigPath);
    servicesConfig.read(config.servicesConfigPath);
    services.load(servicesConfig);
    services.initialize();
    if (datatools::logger::is_debug(config.verbosity)) {
      services.tree_dump(std::cerr, "Services", "[debug] ");
    }

    if (not config.omStatusPath.empty()) {
      _init_om_status_service_(config.omStatusPath);
    }

    if (not config.target.empty()) {
      if (config.target != "status" and config.target != "ecalib") {
	DT_THROW(std::logic_error, "Invalid target " << std::quoted(config.target));
      }
    } else {
      config.target = "status";
    }
     
    if (not config.outputFormat.empty()) {
      if (config.outputFormat != "sql" and config.outputFormat != "csv") {
	DT_THROW(std::logic_error, "Invalid output format " << std::quoted(config.outputFormat));
      }
    } else {
      config.outputFormat = "csv";
    }
				  
    if (config.outputFormat != "sql" and config.outputFormat != "csv") {
      DT_THROW(std::logic_error, "Invalid output format " << std::quoted(config.outputFormat));
    }

    if (config.outputFile.empty()) {
      if (config.target == "status") {
	if (config.outputFormat == "sql") {
	  config.outputFile = "om_status_change_events_cmd.sql";
	} else if (config.outputFormat == "csv") {
	  config.outputFile = "om_status_change_events.csv";
	}
      } else if (config.target == "ecalib") {
	if (config.outputFormat == "sql") {
	  config.outputFile = "om_ecalib_records.sql";
	} else if (config.outputFormat == "csv") {
	  config.outputFile = "om_ecalib_records.csv";
	}
      }
    }
    return;
  }

  void Application::_init_om_status_service_(const std::string & path_)
  {
    auto verbosity = datatools::logger::PRIO_FATAL;
    std::string omStatusPath = path_;
    datatools::fetch_path_with_env(omStatusPath);
    datatools::properties omServiceConfig;
    omServiceConfig.store("mode", "files");
    std::vector<std::string> omStatusMapFiles;
    omStatusMapFiles.push_back(omStatusPath); 
    omServiceConfig.store("files.om_maps", omStatusMapFiles);
    const geomtools::geometry_service & geometryService
      = services.get<geomtools::geometry_service>(snemo::service_info::geometryServiceName());
    const geomtools::manager & geometryManager = geometryService.get_geom_manager();
    _om_status_service_.set_geometry_manager(geometryManager);
    _om_status_service_.set_logging_priority(verbosity);
    _om_status_service_.initialize_standalone(omServiceConfig);
    if (datatools::logger::is_debug(verbosity)) {
      // _om_status_service_.print_tree(std::cerr);
      _om_status_service_.tree_dump(std::cerr, "OM status services", "[debug] ");
    }
    return;
  }
  
  void Application::reset()
  {
    if (_om_status_service_.is_initialized()) {
      // Terminate the service:
      _om_status_service_.reset();
    }
    
    if (services.is_initialized()) {
      // Terminate the service manager:
      services.reset();
    }

    if (variantService.is_started()) {
      // Terminate the variant service:
      variantService.stop();
    }
    
    return;
  }

  int Application::run()
  {
    int exitCode = 0;
    DT_LOG_DEBUG(config.verbosity, "Running...");
    // const geomtools::geometry_service & geometryService
    //   = services.get<geomtools::geometry_service>(snemo::service_info::geometryServiceName());
    // const geomtools::manager & geometryManager = geometryService.get_geom_manager();
    // const geomtools::mapping & geometryMapping = geometryManager.get_mapping();
    // const geomtools::id_mgr & geomIdManager = geometryMapping.get_id_manager();
    // const auto & categories = geomIdManager.categories_by_name();
    // const geomtools::model_factory & geometryModelFactory = geometryManager.get_factory();
    // const geomtools::logical_volume::dict_type & logicalDist = geometryModelFactory.get_logicals();
    // const auto & geomInfos = geometryMapping.get_geom_infos();
    // if (datatools::logger::is_debug(config.verbosity)) {
    //   DT_LOG_DEBUG(config.verbosity, "Geometry manager");
    //   geometryManager.tree_dump(std::cerr);
    //   DT_LOG_DEBUG(config.verbosity, "categories.size = " << categories.size());
    //   DT_LOG_DEBUG(config.verbosity, "logicalDist.size = " << logicalDist.size());
    //   DT_LOG_DEBUG(config.verbosity, "geomInfos.size = " << geomInfos.size());
    //   DT_LOG_DEBUG(config.verbosity, "actionName = " << config.actionName);
    // }

    // Load CSV source files with status and energy calibration informations:
    std::map<int, snemo::rc::calorimeter_om_status_history> omStatusHistoryDict;
    auto successCount = 0u;
    for (const auto & filePath : config.inputFiles) {
      if (datatools::logger::is_debug(config.verbosity)) {
	std::cerr << "\n\n"
		  << "###########################################\n";
      }
      DT_LOG_DEBUG(config.verbosity, "Parsing input file " << std::quoted(filePath));
      om_record_dict omRecordsDict;
      int parseCode = load_file(filePath, omRecordsDict);
      if (parseCode != EXIT_SUCCESS) {
	DT_LOG_FATAL(config.verbosity, "Failed to parse input file " << std::quoted(filePath));
	continue;
      }
      successCount++;
      DT_LOG_DEBUG(config.verbosity, "calib run ID=" << _current_calib_run_id_ << " : omRecordDict.size=" << omRecordsDict.size());
      for (const auto & omRecEntry : omRecordsDict) {
	auto omNum = omRecEntry.first;
	auto omGid = snemo::datamodel::om_gid(omNum);
	const auto & omRecords = omRecEntry.second;
	DT_LOG_DEBUG(config.verbosity, "omNum=" << omNum << " omGid=" << omGid << " -> omRecords.size=" << omRecords.size());
	if (omStatusHistoryDict.count(omNum) == 0) {
	  snemo::rc::calorimeter_om_status_history emptyHistory;
	  omStatusHistoryDict.emplace(omNum, emptyHistory);
	}
	auto & omStatusHistory = omStatusHistoryDict.find(omNum)->second;
	// DT_LOG_DEBUG(config.verbosity, "omRecord.start_period=" << omRecord.start_period);
	// DT_LOG_DEBUG(config.verbosity, "omRecord.stop_period=" << omRecord.stop_period);
	for (const auto & omRecord : omRecords) {
	  auto status = omRecord.status;
	  if (_om_status_service_.is_initialized()) {
	    std::uint32_t knownStatus = _om_status_service_.get_om_status(omGid, omRecord.start_period);
	    if (snemo::rc::calorimeter_om_status::is_dead(knownStatus)) {
	      DT_LOG_DEBUG(config.verbosity, "Forcing OM=" << omGid << " (#" << omNum << ") status to DEAD @" << omRecord.start_period << " [run=" << _current_calib_run_id_ << "]");
	      status = snemo::rc::calorimeter_om_status::OM_DEAD;
	    } else if (snemo::rc::calorimeter_om_status::is_off(knownStatus)) {
	      DT_LOG_DEBUG(config.verbosity, "Forcing OM=" << omGid << " (#" << omNum << ") status to OFF @" << omRecord.start_period << " [run=" << _current_calib_run_id_ << "]");
	      status = snemo::rc::calorimeter_om_status::OM_OFF;
	    }
	  }
	  omStatusHistory.add(snemo::time::time_period(omRecord.start_period, omRecord.stop_period), status);
	}
      }
    }
    DT_LOG_DEBUG(config.verbosity, "omStatusHistoryDict.size = " << omStatusHistoryDict.size());
    DT_LOG_DEBUG(config.verbosity, "Total number of input files = " << config.inputFiles.size());
    DT_LOG_DEBUG(config.verbosity, "Number of successfully parsed input files = " << successCount);   

    if (datatools::logger::is_debug(config.verbosity)) {
      std::cerr << "[debug] OM Status histories:\n";
      for (const auto & [key,value] : omStatusHistoryDict) {
	value.print(std::cerr, "[debug] OM #" + std::to_string(key) + " : ");	
      }
    }

    if (config.target == "status") {   
      DT_LOG_DEBUG(config.verbosity, "target = " << std::quoted(config.target));

      const auto nbOms =  snemo::datamodel::number_of_oms_demo();
      auto totalEventCount = 0u;
      std::vector<snemo::rc::calorimeter_om_status_change_event_list> changeEventListCol(nbOms);
      for (const auto & [omNum,omStatusHistory] : omStatusHistoryDict) {
	auto & changeEventList = changeEventListCol[omNum];
	snemo::rc::build_calorimeter_om_status_change_events_from_history(omStatusHistory,changeEventList);
	totalEventCount += changeEventList.size();
      }
      DT_LOG_DEBUG(config.verbosity, "Number of OM status change events = " << totalEventCount);   
      if (config.outputFormat == "csv") {
	std::ofstream fout(config.outputFile);
	fout << " om_number;            timestamp;   event_type;                        bit;\n";
	for (auto omNum = 0u; omNum < nbOms; omNum++) {
	  const auto & changeEventList = changeEventListCol[omNum];
	  if (changeEventList.empty()) continue;
	  for (auto iEv = 0u; iEv < changeEventList.size(); iEv++) {
	    std::string bitRepr = "";
	    const auto & changeEvent = changeEventList.event((int)iEv);
	    fout << std::left << std::setw(9) << omNum
		 << " ; " << std::setw(20) << snemo::time::to_string(changeEvent.timestamp());
	    if (changeEvent.is_set_bit_event()) {
	      fout  << "; " << std::setw(11) << "set_bit";
	      snemo::rc::calorimeter_om_status::status_bit targetBit = changeEvent.bit();
	      bitRepr = snemo::rc::calorimeter_om_status::status_to_string(targetBit);
	    } else if (changeEvent.is_unset_bit_event()) {
	      fout  << "; " << std::setw(11) << "unset_bit";
	      snemo::rc::calorimeter_om_status::status_bit targetBit = changeEvent.bit();
	      bitRepr = snemo::rc::calorimeter_om_status::status_to_string(targetBit);
	    } else if (changeEvent.is_reset_bits_event()) {
	      fout  << "; " << std::setw(11) << "reset_bits";
	    }
	    fout << " ; " << std::setw(25) << bitRepr;
	    fout << " ;\n";
	  }
	}
	fout.close();
      } else if (config.outputFormat == "sql") {
	std::ostringstream sqlCommand;
	sqlCommand << "INSERT INTO OM_Status_Change_Events VALUES \\\n";
	auto eventCount = 0u;
	for (auto omNum = 0u; omNum < nbOms; omNum++) {
	  const auto & changeEventList = changeEventListCol[omNum];
	  if (changeEventList.empty()) continue;
	  for (auto iEv = 0u; iEv < changeEventList.size(); iEv++) {
	    std::string bitRepr;
	    const auto & changeEvent = changeEventList.event((int)iEv);
	    eventCount++;
	    sqlCommand << "(NULL"
		       << ", " << omNum
		       << ", " << std::quoted(snemo::time::to_string(changeEvent.timestamp()));
	    if (changeEvent.is_set_bit_event()) {
	      sqlCommand  << ", " << std::quoted("set_bit");
	      snemo::rc::calorimeter_om_status::status_bit targetBit = changeEvent.bit();
	      bitRepr = snemo::rc::calorimeter_om_status::status_to_string(targetBit);
	    } else if (changeEvent.is_unset_bit_event()) {
	      sqlCommand  << ", " << std::quoted("unset_bit");
	      snemo::rc::calorimeter_om_status::status_bit targetBit = changeEvent.bit();
	      bitRepr = snemo::rc::calorimeter_om_status::status_to_string(targetBit);
	    }
	    if (bitRepr.empty()) {
	      sqlCommand  << ", " << "NULL";
	    } else {
	      sqlCommand  << ", " << std::quoted(bitRepr);
	    }
	    sqlCommand << ")";
	    if (eventCount < totalEventCount) {
	      sqlCommand << ',';
	    }
	    sqlCommand << " \\\n"; 
	  }
	}
	sqlCommand << ";\n";

	std::ofstream fout(config.outputFile);
	fout << sqlCommand.str();
	fout.close();
      }
    }
    
    if (config.target == "ecalib") {   
      // XXX
    }
    
    return exitCode;
  }

  int om_record::parse(const std::vector<std::string> & tokens_,
		       const std::string & calib_method_,
		       const int format_id_)
  {
    auto verbosity = datatools::logger::PRIO_ERROR;
    // DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE, "#tokens = " << tokens_.size());
    if (tokens_.size() < 4) {
      DT_LOG_ERROR(verbosity, "not enough tokens");
      return EXIT_FAILURE;
    }
    int colId = 0;
    try {
      this->om_num = std::stoi(boost::algorithm::trim_copy(tokens_[colId++]));
    } catch (std::exception & error) {
      DT_LOG_ERROR(verbosity, "invalid format for OM number");
      return EXIT_FAILURE;
    }
    std::string startPeriodRepr = boost::algorithm::trim_copy(tokens_[colId++]);
    if (startPeriodRepr == "-1") {
      DT_LOG_ERROR(verbosity, "invalid format for start period");
      return EXIT_FAILURE; 
    }
    if (startPeriodRepr.find('-') != startPeriodRepr.npos) {
      this->start_period = snemo::time::time_point_from_string(startPeriodRepr);
    } else {
      DT_LOG_DEBUG(verbosity, "read start period as a double from "
		   << std::quoted(startPeriodRepr));
      // TMPFIX
      // int startPeriodElapsed = std::stoi(startPeriodRepr);
      // this->start_period = snemo::time::time_point_from_epoch_sec(startPeriodElapsed * 1.0);
      double startPeriodElapsed = std::stod(startPeriodRepr);
      DT_LOG_DEBUG(verbosity,
		   "startPeriodElapsed=" << startPeriodElapsed);
      // TMPFIX
      // if (startPeriodElapsed < 1.e9) startPeriodElapsed *= 2;
      this->start_period = snemo::time::time_point_from_epoch_sec(startPeriodElapsed * CLHEP::second);
      DT_LOG_DEBUG(verbosity,
		   "start_period=" << snemo::time::to_string(this->start_period));
    }
 
    std::string stopPeriodRepr = boost::algorithm::trim_copy(tokens_[colId++]);
    if (stopPeriodRepr == "-1") {
      DT_LOG_ERROR(verbosity, "invalid format for stop period");
      return EXIT_FAILURE; 
    }
    if (stopPeriodRepr.find('-') != stopPeriodRepr.npos) {
      this->stop_period = snemo::time::time_point_from_string(stopPeriodRepr);
    } else {
      double stopPeriodElapsed = std::stod(stopPeriodRepr);
      DT_LOG_DEBUG(verbosity,
		   "stopPeriodElapsed=" << stopPeriodElapsed);
      this->stop_period = snemo::time::time_point_from_epoch_sec(stopPeriodElapsed * CLHEP::second);
      DT_LOG_DEBUG(verbosity,
		   "stop_period=" << snemo::time::to_string(this->stop_period));
    }
    if ( this->stop_period < this->start_period ) {
      // TMPFIX
      // DT_LOG_ERROR(datatools::logger::PRIO_DEBUG, "fix period ends from "
      // 		   << snemo::time::to_string(this->start_period));
      // this->start_period = this->stop_period - snemo::time::time_duration_from_string("00:30:00.000000");
      DT_THROW(std::runtime_error, "invalid period");
      // DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "invalid period");
      // return EXIT_FAILURE;
    }
    
    std::istringstream omBitStream(boost::algorithm::trim_copy(tokens_[colId++]));
    std::bitset<16> omBitset;
    omBitStream >> omBitset;
    if (! omBitStream) {
      DT_LOG_ERROR(verbosity, "cannot parse status bitset");
      return EXIT_FAILURE;
    }
    auto omBits = omBitset.to_ulong();
    this->status = omBits;
    // skip calibration data
    bool parseCalibInfos = true;
    if (snemo::rc::calorimeter_om_status::is_off(omBits)) {
      omBits = snemo::rc::calorimeter_om_status::OM_OFF;
      parseCalibInfos = false;
    }
    if (snemo::rc::calorimeter_om_status::is_dead(omBits)) {
      omBits = snemo::rc::calorimeter_om_status::OM_DEAD;
      parseCalibInfos = false;
    }
    if (snemo::rc::calorimeter_om_status::is_no_bi_monitoring(omBits)) {
      parseCalibInfos = false;
    }
    if (snemo::rc::calorimeter_om_status::is_bad_ecalib_fit(omBits)) {
      parseCalibInfos = false;
    }
    
    std::string calibType = calib_method_;
    if (format_id_ == 0) {
      // nothing special...
      // std::string dummyId = boost::algorithm::trim_copy(tokens_[colId++]);
    } else if (format_id_ == 1) {
      // nothing special...
    } else {
      DT_THROW(std::runtime_error, "unsupported format ID");    
    }

    if (parseCalibInfos) {
      this->calib_method = calibType;
    }
    
    if (parseCalibInfos) {
      DT_LOG_DEBUG(verbosity,
		   "calib_method=" << std::quoted(this->calib_method) << " with " << tokens_.size() << " tokens");
      if (this->calib_method.empty()) {
	DT_LOG_DEBUG(verbosity, "no calibration method is available");
      } else if (this->calib_method == "pol1") {
	DT_LOG_DEBUG(verbosity, "detected 'pol1' calibration method");
	if (tokens_.size() < 10) {
	  DT_LOG_ERROR(verbosity, "not enough tokens for 'pol1' calibration method");
	  return EXIT_FAILURE;
	}
	try {
	  this->pol1_params.a = std::stod(boost::algorithm::trim_copy(tokens_[colId++]));
	  this->pol1_params.var_a = std::stod(boost::algorithm::trim_copy(tokens_[colId++]));
	  this->pol1_params.fit_chi2 = std::stod(boost::algorithm::trim_copy(tokens_[colId++]));
	  this->pol1_params.fit_ndof = std::stoi(boost::algorithm::trim_copy(tokens_[colId++]));
	  this->pol1_params.bad_fit = false;
	} catch (std::exception & error) {
	  DT_LOG_ERROR(verbosity, error.what());
	  return EXIT_FAILURE;
	}
      } else if(this->calib_method == "pol2") {
	DT_LOG_DEBUG(verbosity, "detected 'pol2' calibration method");
	if (tokens_.size() < 15) {
	  DT_LOG_ERROR(verbosity, "not enough tokens for 'pol2' calibration method");
	  return EXIT_FAILURE;
	}
	try {
	  this->pol2_params.a = std::stod(boost::algorithm::trim_copy(tokens_[colId++]));
	  this->pol2_params.b = std::stod(boost::algorithm::trim_copy(tokens_[colId++]));
	  this->pol2_params.var_a = std::stod(boost::algorithm::trim_copy(tokens_[colId++]));
	  this->pol2_params.var_b = std::stod(boost::algorithm::trim_copy(tokens_[colId++]));
	  this->pol2_params.cov_ab = std::stod(boost::algorithm::trim_copy(tokens_[colId++]));
	  this->pol2_params.fit1_chi2 = std::stod(boost::algorithm::trim_copy(tokens_[colId++]));
	  this->pol2_params.fit1_ndof = std::stoi(boost::algorithm::trim_copy(tokens_[colId++]));
	  this->pol2_params.fit2_chi2 = std::stod(boost::algorithm::trim_copy(tokens_[colId++]));
	  this->pol2_params.fit2_ndof = std::stoi(boost::algorithm::trim_copy(tokens_[colId++]));
	  this->pol2_params.bad_fit = false;
	} catch (std::exception & error) {
	  DT_LOG_ERROR(verbosity, error.what());
	  return EXIT_FAILURE;
	}
      } else {
	DT_THROW(std::logic_error, "unsupported OM calibration method " << std::quoted(calib_method_));
      }
    }
    return EXIT_SUCCESS;
  }

  // friend
  std::ostream & operator<<(std::ostream & out_, const om_energy_calib_charge_pol1_params & params_)
  {
    out_
      << "a=" << params_.a
      << "+/-" << std::sqrt(params_.var_a)
      << ';' << "fit_chi2=" << params_.fit_chi2
      << ';' << "fit_ndof=" << params_.fit_ndof
      << ';' << "bad_fit=" << params_.bad_fit
      ;
    return out_;
  }

  // friend
  std::ostream & operator<<(std::ostream & out_, const om_energy_calib_charge_pol2_params & params_)
  {
    out_
      << "a=" << params_.a
      << "+/-" << std::sqrt(params_.var_a)
      << ';' << "b=" << params_.b
      << "+/-" << std::sqrt(params_.var_b)
      << ';' << "cov_ab=" << params_.cov_ab
      << ';' << "fit1_chi2=" << params_.fit1_chi2
      << ';' << "fit1_ndof=" << params_.fit1_ndof
      << ';' << "fit2_chi2=" << params_.fit2_chi2
      << ';' << "fit2_ndof=" << params_.fit2_ndof
      << ';' << "bad_fit=" << params_.bad_fit
      ;
    return out_;
  }

  // friend
  std::ostream & operator<<(std::ostream & out_, const om_record & om_rec_)
  {
    out_ << '{'
	 << "om_num=" << om_rec_.om_num
	 << ';' << "start_period=" << om_rec_.start_period
	 << ';' << "stop_period=" << om_rec_.stop_period
	 << ';' << "status=" << om_rec_.status
	 << ';' << "calib.method=" << om_rec_.calib_method;
    if (not om_rec_.calib_method.empty()) {
      out_ << ';' << "calib.version=" << om_rec_.calib_version;
      if (om_rec_.calib_method == "pol1") {
	out_ << ';' << "pol1.parameters=[" << om_rec_.pol1_params << ']';
      }
      if (om_rec_.calib_method == "pol2") {
	out_ << ';' << "pol2.parameters=[" << om_rec_.pol2_params << ']';
      }
    }
    out_ << '}';
    return out_;
  }

  int Application::load_file(const std::string & filepath_, om_record_dict & dict_)
  {
    auto verbosity = datatools::logger::PRIO_ERROR;
    // verbosity = config.verbosity;
    int errorCode = EXIT_SUCCESS;
    dict_.clear();
    std::string filePath = filepath_;
    datatools::fetch_path_with_env(filePath);
    DT_LOG_DEBUG(verbosity, "filepath = " << std::quoted(filepath_));
    std::filesystem::path fPath(filePath);
    int formatId = 0;
    // if (fPath.filename().string() > "OM_calibration_run_1414.txt") {
    //   formatId = 1;
    // }
    std::ifstream fin(filePath);
    if (!fin) {
      DT_LOG_FATAL(verbosity, "cannot open file " << std::quoted(filePath));
      return EXIT_FAILURE;
    }
    auto lineCount = 0u;
    std::int32_t calibRunId = -1;
    while (fin and not fin.eof()) {
      std::string rawLine;
      std::getline(fin, rawLine);
      boost::algorithm::trim(rawLine);
      lineCount++;
      DT_LOG_DEBUG(verbosity, "=====> rawLine #" << lineCount << " = " << std::quoted(rawLine));
      if (rawLine.empty()) continue;
      if (rawLine[0] == '#') {
	if (rawLine.size() > 15) {
	  //  0123456789012345678
	  //  #@calib_run_id=4173
	  std::string metaKey = rawLine.substr(0,15); 
	  DT_LOG_DEBUG(verbosity, "metaKey = " << metaKey);
	  if (metaKey == "#@calib_run_id=") {
	    DT_THROW_IF(calibRunId > 0, std::logic_error, "calibration run ID is already set (line #" << lineCount << ')');
	    std::string metaValue = rawLine.substr(15); 
	    DT_LOG_DEBUG(verbosity, "metaValue = " << metaValue);
	    calibRunId = std::stoi(metaValue);
	    DT_LOG_DEBUG(verbosity, "header parsed : calibRunId = " << calibRunId);
	    _current_calib_run_id_ = calibRunId;
	  }
	}
	continue;
      }
      std::vector<std::string> tokens;
      boost::split(tokens, rawLine, boost::algorithm::is_any_of(";"));
      DT_LOG_DEBUG(verbosity, "OM record from tokens.size= " << tokens.size());
      om_record omRecord;
      if (omRecord.parse(tokens, config.calibMethod, formatId) != EXIT_SUCCESS) {
	// DT_LOG_FATAL(verbosity, "invalid format at line #" << lineCount);
	return EXIT_FAILURE;
      }
      DT_LOG_DEBUG(verbosity, "OM record = " << omRecord);
      bool badFit = false;
      if (omRecord.calib_method == "pol1") {
	double pValueFit = gsl_cdf_chisq_Q(omRecord.pol1_params.fit_chi2,
					   omRecord.pol1_params.fit_ndof);
	omRecord.pol1_params.bad_fit = pValueFit < config.badFitThreshold;
	badFit = omRecord.pol1_params.bad_fit;
      } else if (omRecord.calib_method == "pol2") {
	double pValueFit1 = gsl_cdf_chisq_Q(omRecord.pol2_params.fit1_chi2, omRecord.pol2_params.fit1_ndof);
	double pValueFit2 = gsl_cdf_chisq_Q(omRecord.pol2_params.fit2_chi2, omRecord.pol2_params.fit2_ndof);
	omRecord.pol2_params.bad_fit = (pValueFit1 < config.badFitThreshold) or (pValueFit2 < config.badFitThreshold);
	badFit = omRecord.pol2_params.bad_fit;
      }
      if (badFit) {
	// Set the bad energy calibration fit status bit:
	omRecord.status |= snemo::rc::calorimeter_om_status::OM_BAD_ECALIB_FIT;
      }
      if (not dict_.count(omRecord.om_num)) {
	om_record_col empty;
	dict_.emplace(omRecord.om_num, empty);	
      }
      dict_[omRecord.om_num].push_back(omRecord);
      fin >> std::ws;
    }
    
    return errorCode;
  }

}

