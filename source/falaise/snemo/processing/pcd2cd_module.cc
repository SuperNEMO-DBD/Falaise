// -*- mode: c++ ; -*-
/** \file falaise/snemo/processing/pcd2cd_module.cc
 */

// Ourselves:
#include "pcd2cd_module.h"

// Standard library:
#include <fstream>
#include <sstream>
#include <stdexcept>

// Third party:
// - Bayeux/datatools:
#include <datatools/service_manager.h>
#include <datatools/utils.h>
// - Bayeux/geomtools:
#include <geomtools/geometry_service.h>
#include <geomtools/manager.h>
// - Bayeux/mctools:
#include <mctools/utils.h>

// This project:
#include <falaise/property_set.h>
#include <falaise/quantity.h>
#include <falaise/snemo/datamodels/data_model.h>
#include <falaise/snemo/datamodels/geomid_utils.h>
#include <falaise/snemo/services/services.h>

namespace snemo {

  namespace processing {

    // Registration instantiation macro :
    DPP_MODULE_REGISTRATION_IMPLEMENT(pcd2cd_module,
                                      "snemo::processing::pcd2cd_module")

    void pcd2cd_module::initialize(const datatools::properties& ps,
                                    datatools::service_manager& services,
                                    dpp::module_handle_dict_type& /*unused*/) {

      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Module '" << get_name() << "' is already initialized ! ");

      geoManager = snemo::service_handle<snemo::geometry_svc>{services};

      this->base_module::_common_initialize(ps);
      falaise::property_set fps{ps};

      _pcd_input_tag_  = fps.get<std::string>("pCD_label", snedm::labels::precalibrated_data());
      _cpcd_input_tag_ = fps.get<std::string>("CpCD_label", snedm::labels::clusterized_precalibrated_data());
      _cd_output_tag_  = fps.get<std::string>("CD_label", snedm::labels::calibrated_data());
      _tcd_output_tag_ = fps.get<std::string>("TCD_label", snedm::labels::tracker_clustering_data());
      // _ccd_output_tag_ = fps.get<std::string>("CCD_label", snedm::labels::clusterized_calibrated_data());

      // Configure calorimeter energy calibration method

      std::string calo_energy_method_label = fps.get<std::string>("calo_energy_method", "");

      if (calo_energy_method_label == "200mV/MeV") {
	DT_LOG_NOTICE(get_logging_priority(), "calorimeter energy calibration method = '" << calo_energy_method_label << "'");
	_pcd2cd_calo_energy_method_ = CALO_ENERGY_200MV;

      } else if (calo_energy_method_label == "pol0_table") {
	DT_LOG_NOTICE(get_logging_priority(), "calorimeter energy calibration method = '" << calo_energy_method_label << "'");
	_pcd2cd_calo_energy_method_ = CALO_ENERGY_POL0_TABLE;

	// Initialise calo pol0 energy constants
	_pcd2cd_calo_energy_constants_.reserve(712);
	for (int om=0; om<712; om++)
	  _pcd2cd_calo_energy_constants_.push_back({0});

	// Fill calo pol0 energy constants
	std::string pol0_table_path = fps.get<std::string>("calo_energy_method.database");
	datatools::fetch_path_with_env(pol0_table_path);
	int nb_entries = this->parse_calibration_constants(pol0_table_path, _pcd2cd_calo_energy_constants_);
	DT_LOG_NOTICE(get_logging_priority(), "`- " << nb_entries << " entries parsed in '" << pol0_table_path << "'");

      } else if (calo_energy_method_label == "pol1_table") {
	DT_LOG_NOTICE(get_logging_priority(), "calorimeter energy calibration method = '" << calo_energy_method_label << "'");
	_pcd2cd_calo_energy_method_ = CALO_ENERGY_POL1_TABLE;

	// Initialise calo pol1 energy constants
	_pcd2cd_calo_energy_constants_.reserve(712);
	for (int om=0; om<712; om++)
	  _pcd2cd_calo_energy_constants_.push_back({0,0});

	// Fill calo pol1 energy constants
	std::string pol1_table_path = fps.get<std::string>("calo_energy_method.database");
	datatools::fetch_path_with_env(pol1_table_path);
	int nb_entries = this->parse_calibration_constants(pol1_table_path, _pcd2cd_calo_energy_constants_);
	DT_LOG_NOTICE(get_logging_priority(), "`- " << nb_entries << " entries parsed in '" << pol1_table_path << "'");

      } else if (!calo_energy_method_label.empty()) {
	DT_LOG_ERROR(get_logging_priority(), "wrong calorimeter energy calibration method '" << calo_energy_method_label << "'");
	_pcd2cd_calo_energy_method_ = CALO_ENERGY_NONE;

      } else {
	DT_LOG_WARNING(get_logging_priority(), "no calorimeter energy calibration method provided");
	_pcd2cd_calo_energy_method_ = CALO_ENERGY_NONE;
      }

      // Configure calorimeter energy threshold
      {
	double default_threshold = 0;

	if (fps.has_key("calo_energy_threshold.default"))
	  default_threshold = fps.get<double>("calo_energy_threshold.default") * CLHEP::MeV;

	_pcd2cd_calo_energy_thresholds_.reserve(712);

	for (int om=0; om<712; om++)
	  _pcd2cd_calo_energy_thresholds_.push_back(default_threshold);


	// Fill calo energy thresholds
	if (fps.has_key("calo_energy_threshold.database")) {
	  std::string energy_threshold_table_path = fps.get<std::string>("calo_energy_threshold.database");
	  datatools::fetch_path_with_env(energy_threshold_table_path);
	  int nb_entries = this->parse_calibration_constants(energy_threshold_table_path, _pcd2cd_calo_energy_thresholds_);
	  DT_LOG_NOTICE(get_logging_priority(), "calorimeter energy thresholds");
	  DT_LOG_NOTICE(get_logging_priority(), "`- " << nb_entries << " entries parsed in '" << energy_threshold_table_path << "'");
	}
      }

      // Configure calorimeter time calibration method

      std::string calo_time_method_label = fps.get<std::string>("calo_time_method", "");

      if (calo_time_method_label == "t0_table") {
	DT_LOG_NOTICE(get_logging_priority(), "calorimeter time calibration method = '" << calo_time_method_label << "'");
	_pcd2cd_calo_time_method_ = CALO_TIME_T0_TABLE;

	// Initialise calo t0 constants
	_pcd2cd_calo_t0_constants_.reserve(712);
	for (int om=0; om<712; om++)
	  _pcd2cd_calo_t0_constants_.push_back(0);

	// Fill calo t0 constants
	std::string t0_table_path = fps.get<std::string>("calo_time_method.database");
	datatools::fetch_path_with_env(t0_table_path);
	int nb_entries = this->parse_calibration_constants(t0_table_path, _pcd2cd_calo_t0_constants_);
	DT_LOG_NOTICE(get_logging_priority(), "`- " << nb_entries << " entries parsed in '" << t0_table_path << "'");

      } else if (!calo_time_method_label.empty()) {
	DT_LOG_ERROR(get_logging_priority(), "wrong calorimeter time calibration method '" << calo_time_method_label << "'");
	_pcd2cd_calo_time_method_ = CALO_TIME_NONE;

      } else {
	DT_LOG_ERROR(get_logging_priority(), "no calorimeter time calibration method provided");
	_pcd2cd_calo_time_method_ = CALO_TIME_NONE;
      }

      // Configure tracker time calibration method

      std::string tracker_time_method_label = fps.get<std::string>("tracker_time_method", "");

      if (tracker_time_method_label == "t0_table") {

	DT_LOG_NOTICE(get_logging_priority(), "tracker time calibration method = '" << tracker_time_method_label << "'");
	_pcd2cd_tracker_time_method_ = TRACKER_TIME_T0_TABLE;

	// Initialise tracker t0 constants
	_pcd2cd_tracker_anode_t0_constants_.reserve(2034);
	_pcd2cd_tracker_bottom_cathode_t0_constants_.reserve(2034);
	_pcd2cd_tracker_top_cathode_t0_constants_.reserve(2034);
	for (int gg=0; gg<2034; gg++) {
	  _pcd2cd_tracker_anode_t0_constants_.push_back(0);
	  _pcd2cd_tracker_bottom_cathode_t0_constants_.push_back(0);
	  _pcd2cd_tracker_top_cathode_t0_constants_.push_back(0);
	}

	// Fill tracker anode t0 constants
	if (fps.has_key("tracker_time_method.anode.database")) {
	  std::string t0_table_path = fps.get<std::string>("tracker_time_method.anode.database");
	  datatools::fetch_path_with_env(t0_table_path);
	  int nb_entries = this->parse_calibration_constants(t0_table_path, _pcd2cd_tracker_anode_t0_constants_);
	  DT_LOG_NOTICE(get_logging_priority(), "`- " << nb_entries << " entries parsed in '" << t0_table_path << "'");
	}

	// Fill tracker bottom cathode t0 constants
	if (fps.has_key("tracker_time_method.bottom_cathode.database")) {
	  std::string t0_table_path = fps.get<std::string>("tracker_time_method.bottom_cathode.database");
	  datatools::fetch_path_with_env(t0_table_path);
	  int nb_entries = this->parse_calibration_constants(t0_table_path, _pcd2cd_tracker_bottom_cathode_t0_constants_);
	  DT_LOG_NOTICE(get_logging_priority(), "`- " << nb_entries << " entries parsed in '" << t0_table_path << "'");
	}

	// Fill tracker top cathode t0 constants
	if (fps.has_key("tracker_time_method.top_cathode.database")) {
	  std::string t0_table_path = fps.get<std::string>("tracker_time_method.top_cathode.database");
	  datatools::fetch_path_with_env(t0_table_path);
	  int nb_entries = this->parse_calibration_constants(t0_table_path, _pcd2cd_tracker_bottom_cathode_t0_constants_);
	  DT_LOG_NOTICE(get_logging_priority(), "`- " << nb_entries << " entries parsed in '" << t0_table_path << "'");
	}

      } else if (!tracker_time_method_label.empty()) {
	DT_LOG_ERROR(get_logging_priority(), "wrong tracker time calibration method '" << tracker_time_method_label << "'");
	_pcd2cd_tracker_time_method_ = TRACKER_TIME_NONE;

      } else {
	DT_LOG_NOTICE(get_logging_priority(), "no tracker time calibration method provided");
	_pcd2cd_tracker_time_method_ = TRACKER_TIME_NONE;
      }

      // Configure tracker radius calibration method

      std::string tracker_radius_method_label = fps.get<std::string>("tracker_radius_method", "");

      if (tracker_radius_method_label == "r=1cm") {
	DT_LOG_NOTICE(get_logging_priority(), "tracker radius calibration method = '" << tracker_radius_method_label << "'");
	_pcd2cd_tracker_radius_method_ = TRACKER_RADIUS_FALAISE;

      } else if (tracker_radius_method_label == "manu") {
	DT_LOG_NOTICE(get_logging_priority(), "tracker radius calibration method = '" << tracker_radius_method_label << "'");
	_pcd2cd_tracker_radius_method_ = TRACKER_RADIUS_MANU;

      } else if (!tracker_radius_method_label.empty()) {
	DT_LOG_ERROR(get_logging_priority(), "wrong tracker radius calibration method '" << tracker_radius_method_label << "'");
	_pcd2cd_tracker_radius_method_ = TRACKER_RADIUS_NONE;

      } else {
	DT_LOG_ERROR(get_logging_priority(), "no tracker radius calibration method provided");
	_pcd2cd_tracker_radius_method_ = TRACKER_RADIUS_NONE;
      }

      // Configure tracker height calibration method

      std::string tracker_height_method_label = fps.get<std::string>("tracker_height_method", "");

      if (tracker_height_method_label == "linear_r5r6") {
	DT_LOG_NOTICE(get_logging_priority(), "tracker height calibration method = '" << tracker_height_method_label << "'");
	_pcd2cd_tracker_height_method_ = TRACKER_HEIGHT_LINEAR_R5R6;

      } else if (tracker_height_method_label == "non_linear_r5r6") {
	DT_LOG_NOTICE(get_logging_priority(), "tracker height calibration method = '" << tracker_height_method_label << "'");
	_pcd2cd_tracker_height_method_ = TRACKER_HEIGHT_NON_LINEAR_R5R6;

      } else if (tracker_height_method_label == "linear_r5r6_error") {
	DT_LOG_NOTICE(get_logging_priority(), "tracker height calibration method = '" << tracker_height_method_label << "'");
	_pcd2cd_tracker_height_method_ = TRACKER_HEIGHT_LINEAR_R5R6_ERROR;

      } else if (tracker_height_method_label == "linear_single_r5r6_error" || tracker_height_method_label == "non_linear_single_r5r6_error") {
      DT_LOG_NOTICE(get_logging_priority(), "tracker height calibration method = '" << tracker_height_method_label << "'");
      if (tracker_height_method_label == "linear_single_r5r6_error"){
	_pcd2cd_tracker_height_method_ = TRACKER_HEIGHT_LINEAR_SINGLE_TS_R5R6_ERROR;
      }
      if (tracker_height_method_label == "non_linear_single_r5r6_error"){
	_pcd2cd_tracker_height_method_ = TRACKER_HEIGHT_NON_LINEAR_SINGLE_TS_R5R6_ERROR;
      }

	// Initialise tracker ppt constants to be used in R5||R6 recovery
	_pcd2cd_tracker_ppt_constants_.reserve(2034);
	for (int tr_cell=0; tr_cell<2034; tr_cell++)
	_pcd2cd_tracker_ppt_constants_.push_back({0});

	// Fill calo pol0 energy constants
	std::string ppt_table_path = fps.get<std::string>("tracker_ppt_method.database");
	datatools::fetch_path_with_env(ppt_table_path);
	int nb_entries = this->parse_calibration_constants(ppt_table_path, _pcd2cd_tracker_ppt_constants_);
	DT_LOG_NOTICE(get_logging_priority(), "`- " << nb_entries << " entries parsed in '" << ppt_table_path << "'");

      //load in parameters for single ts error calculation from conf
	_pcd2cd_tracker_height_error_single_ts_top_a_ = fps.get<falaise::length_t>("tracker_height_error_single_ts_top_a", {3.0, "cm"})();
	_pcd2cd_tracker_height_error_single_ts_top_b_ = fps.get<falaise::length_t>("tracker_height_error_single_ts_top_b", {3.0, "cm"})();
	_pcd2cd_tracker_height_error_single_ts_bot_a_ = fps.get<falaise::length_t>("tracker_height_error_single_ts_bot_a", {3.0, "cm"})();
	_pcd2cd_tracker_height_error_single_ts_bot_b_ = fps.get<falaise::length_t>("tracker_height_error_single_ts_bot_b", {3.0, "cm"})();

      } else if (!tracker_height_method_label.empty()) {
	DT_LOG_ERROR(get_logging_priority(), "wrong tracker height calibration method '" << tracker_height_method_label << "'");
	_pcd2cd_tracker_height_method_ = TRACKER_HEIGHT_NONE;

      } else {
	DT_LOG_ERROR(get_logging_priority(), "no tracker height calibration method provided");
	_pcd2cd_tracker_height_method_ = TRACKER_HEIGHT_NONE;
      }

      _pcd2cd_tracker_height_effective_ = fps.get<falaise::length_t>("tracker_height_effective", {1.38, "m"})();
      _pcd2cd_tracker_height_offset_ = fps.get<falaise::length_t>("tracker_height_offset", {0.0, "cm"})(); // -0.01*CLHEP::m);
      _pcd2cd_tracker_height_deceleration_ = fps.get<double>("tracker_height_deceleration", 0);
      _pcd2cd_tracker_height_error_ = fps.get<falaise::length_t>("tracker_height_error", {1.0, "cm"})();

      this->base_module::_set_initialized(true);
    }

    void pcd2cd_module::reset() { this->base_module::_set_initialized(false); }

    int pcd2cd_module::parse_calibration_constants(std::string database_path_, std::vector<double> & constants_) {

      std::ifstream database_file (database_path_.c_str());

      int nb_entries = 0;

      std::string a_line;

      while (std::getline(database_file, a_line)) {

	// Skip comment-like line (starting with '#')
	if (a_line[0] == '#')
	  continue;

	std::stringstream a_stream (a_line);

	int an_om_num;
	a_stream >> an_om_num;
	a_stream >> constants_[an_om_num];

	nb_entries++;
      }

      return nb_entries;
    }

    int pcd2cd_module::parse_calibration_constants(std::string database_path_, std::vector<std::vector<double>> & constants_) {

      std::ifstream database_file (database_path_.c_str());

      int nb_entries = 0;

      std::string a_line;

      while (std::getline(database_file, a_line)) {

	// Skip comment-like line (starting with '#')
	if (a_line[0] == '#')
	  continue;

	std::stringstream a_stream (a_line);

	int an_om_num;
	a_stream >> an_om_num;

	double an_om_constant;
	std::vector<double> all_om_constant;

	while (a_stream >> an_om_constant)
	  all_om_constant.push_back(an_om_constant);

	constants_[an_om_num] = all_om_constant;

	nb_entries++;
      }

      return nb_entries;
    }

    dpp::base_module::process_status pcd2cd_module::process(datatools::things& event) {

      DT_THROW_IF(!is_initialized(), std::logic_error,
                  "Module '" << get_name() << "' is not initialized !");

      // Check if pCD bank exists
      if (!event.has(_pcd_input_tag_)) {
        throw std::logic_error("Missing pCD bank to be processed !");
        return dpp::base_module::PROCESS_ERROR;
      }

      // // Check if CpCD bank exists
      // if (!event.has(_cpcd_input_tag_)) {
      //   throw std::logic_error("Missing CpCD bank to be processed !");
      //   return dpp::base_module::PROCESS_ERROR;
      // }

      // retrieve EH data as mutable (for timestamp update)
      _eh_data_ = &(event.grab<snemo::datamodel::event_header>("EH"));
      _event_id_ = _eh_data_->get_id();
      DT_LOG_DEBUG(get_logging_priority(), "Processing pCD2CD on event #" << _event_id_);

      // retrive pCD data
      _pcd_data_ = &(event.get<snemo::datamodel::precalibrated_data>(_pcd_input_tag_));

      // retrive CpCD data
      _cpcd_data_ = &(event.get<snemo::datamodel::clusterized_precalibrated_data>(_cpcd_input_tag_));

      // prepare general event time from earliest pcd hit
      // (in priority within clusterized hits)
      _event_time_ = 0;

      _cluster_reference_time_.clear();
      _cluster_reference_time_.reserve(_cpcd_data_->clusters().size());

      for (const auto & pcd_cluster : _cpcd_data_->clusters()) {

	for (const auto & pcd_calo_hit : pcd_cluster->calorimeter_hits()) {
	  const double & pcd_calo_time = pcd_calo_hit->get_time();
	  if ((_event_time_ == 0) || (pcd_calo_time < _event_time_))
	    _event_time_ = pcd_calo_time;
	}

	for (const auto & pcd_tracker_hit : pcd_cluster->tracker_hits()) {

	  if (pcd_tracker_hit->has_anodic_time()) {
	    const double & pcd_tracker_time = pcd_tracker_hit->get_anodic_time();
	    if ((_event_time_ == 0) || (pcd_tracker_time < _event_time_))
	      _event_time_ = pcd_tracker_time;
	  }
	}

	const datatools::properties & pcd_cluster_properties = pcd_cluster->get_properties();

	if (pcd_cluster_properties.has_key("reference_pcd_calo_index")) {
	  int reference_pcd_calo_index = pcd_cluster_properties.fetch_integer("reference_pcd_calo_index");
	  const auto & pcd_calo_hit = _pcd_data_->calorimeter_hits().at(reference_pcd_calo_index);
	  _cluster_reference_time_.push_back(pcd_calo_hit->get_time());

	// else if (pcd_cluster_properties.has_key("first_pcd_tracker_index")) {
	//   int first_pcd_tracker_index = pcd_cluster_properties.fetch_integer("first_pcd_tracker_index");
	//   const auto & pcd_tracker_hit = _pcd_data_->tracker_hits().at(first_pcd_tracker_index);
	//   _cluster_reference_time_.push_back(pcd_tracker_hit->get_anodic_time());
	//   DT_LOG_DEBUG(get_logging_priority(), "using pdc tracker hit #" << first_pcd_tracker_index << " as reference time for cluster #" << pcd_cluster->get_cluster_id());
	// }

	} else {
	  // DT_LOG_DEBUG(get_logging_priority(), _event_id_ << " no reference time for cluster #" << pcd_cluster->get_cluster_id());
	  _cluster_reference_time_.push_back(0);
	}

      } // for (pcd_cluster)

      if (_event_time_ == 0) {

	for (const auto & pcd_calo_hit : _cpcd_data_->unclusterized_calorimeter_hits()) {
	  const double & pcd_calo_time = pcd_calo_hit->get_time();
	  if ((_event_time_ == 0) || (pcd_calo_time < _event_time_))
	    _event_time_ = pcd_calo_time;
	}

	if (_event_time_ == 0) {
	  for (const auto & pcd_tracker_hit : _cpcd_data_->unclusterized_tracker_hits()) {
	    if (pcd_tracker_hit->has_anodic_time()) {
	      const double & pcd_tracker_time = pcd_tracker_hit->get_anodic_time();
	      if ((_event_time_ == 0) || (pcd_tracker_time < _event_time_))
		_event_time_ = pcd_tracker_time;
	    }
	  }
	}
      }

      // update event time here according to new reference time [once run_sync_time available in metadata]
      // const double absolute_event_time = _run_sync_time_ + _event_time_;
      // const int64_t absolute_event_time_second = (int64_t) absolute_event_time;
      // const int64_t absolute_event_time_picosecond = (int64_t) ((absolute_event_time - absolute_event_time_second)*1E12);
      // snemo::datamodel::timestamp absolute_event_timestamp (absolute_event_time_second, absolute_event_time_picosecond);
      // _eh_data_->set_timestamp(absolute_event_timestamp);

      // Check if some 'cd_data' are available in the data model:
      auto & cd_data = snedm::getOrAddToEvent<snemo::datamodel::calibrated_data>(_cd_output_tag_, event);
      _cd_data_ = & cd_data;

      // Always rewrite calorimeter hits
      _cd_data_->calorimeter_hits().clear();
      // Always rewrite tracker hits
      _cd_data_->tracker_hits().clear();

      // Check if some 'tcd_data' are available in the data model:
      _tcd_data_ = &(snedm::getOrAddToEvent<snemo::datamodel::tracker_clustering_data>(_tcd_output_tag_, event));

      // Always rewrite clusterize data
      _tcd_data_->clear();

      // // Check if some 'ccd_data' are available in the data model:
      // auto & ccd_data = snedm::getOrAddToEvent<snemo::datamodel::clusterized_calibrated_data>(_ccd_output_tag_, event);

      // // Always rewrite clusterize data
      // ccd_data.clear();

      // Main calorimeter processing method
      process_calo_impl();

      // Main tracker processing method
      process_tracker_impl();

      // // Main tracker cluster method
      // process_tracker_cluster_impl(_pcd_data_, cd_data);

      DT_LOG_DEBUG(get_logging_priority(), "'" << _cd_output_tag_ << "' bank filled with " << cd_data.tracker_hits().size()
		   << " tracker hits and " << cd_data.calorimeter_hits().size() << " calorimeter hits");

      // if (datatools::logger::is_debug(get_logging_priority())) {
      // 	// cd_data.tree_dump();
      // 	boost::property_tree::ptree print_opts;
      // 	print_opts.put("list_hits", true);
      // 	cd_data.print_tree(std::clog, print_opts);
      // }

      return dpp::base_module::PROCESS_SUCCESS;
    }

    bool pcd2cd_module::calibrate_calo_hit(const snemo::datamodel::precalibrated_calorimeter_hit & pcd_calo_hit_,
					   snemo::datamodel::calibrated_calorimeter_hit & cd_calo_hit_) {

      DT_LOG_TRACE(get_logging_priority(), "Calibrating calo hit from " << snemo::datamodel::om_label(pcd_calo_hit_.get_geom_id()));

      cd_calo_hit_.set_geom_id(pcd_calo_hit_.get_geom_id());
      // cd_calo_hit_.grab_geom_id().set_type(cd_calo_hit_.get_geom_id().get_type()+1);

      const int calo_om_num = snemo::datamodel::om_num(pcd_calo_hit_.get_geom_id());

      if (_pcd2cd_calo_energy_method_ == CALO_ENERGY_POL0_TABLE) {
	const double & pcd_calo_constant0 = _pcd2cd_calo_energy_constants_[calo_om_num][0] * CLHEP::MeV/(1E-9*CLHEP::volt*CLHEP::second);
	const double & pcd_calo_charge = pcd_calo_hit_.get_charge();
	cd_calo_hit_.set_energy(-pcd_calo_charge * pcd_calo_constant0);

      } else if (_pcd2cd_calo_energy_method_ == CALO_ENERGY_POL1_TABLE) {
	const double & pcd_calo_constant0 = _pcd2cd_calo_energy_constants_[calo_om_num][0] * CLHEP::MeV/(1E-9*CLHEP::volt*CLHEP::second);
	const double & pcd_calo_constant1 = _pcd2cd_calo_energy_constants_[calo_om_num][1] * CLHEP::MeV;
	const double & pcd_calo_charge = pcd_calo_hit_.get_charge();
	cd_calo_hit_.set_energy(-pcd_calo_charge * pcd_calo_constant0 + pcd_calo_constant1);

      } else if (_pcd2cd_calo_energy_method_ == CALO_ENERGY_200MV) {
	cd_calo_hit_.set_energy(pcd_calo_hit_.get_amplitude()*(CLHEP::MeV/(-200E-3*CLHEP::volt)));
	// cd_calo_hit_.set_sigma_energy(pcd_calo_hit_.get_sigma_amplitude()*(CLHEP::MeV/(-200E-3*CLHEP::volt)));
      }

      if (_pcd2cd_calo_time_method_ == CALO_TIME_T0_TABLE) {
	const double & pcd_calo_time = pcd_calo_hit_.get_time();
	const double & calo_t0 = _pcd2cd_calo_t0_constants_[calo_om_num] * CLHEP::ns;
	cd_calo_hit_.set_time(pcd_calo_time - calo_t0 - _event_time_);
	// cd_calo_hit_.set_sigma_time(0);
      }

      // Apply energy threshold
      if (cd_calo_hit_.get_energy() <= _pcd2cd_calo_energy_thresholds_[calo_om_num])
	return false;

      // Retrieve pCD and CD auxiliaries
      const datatools::properties & pcd_calo_hit_properties = pcd_calo_hit_.get_auxiliaries();
      datatools::properties & cd_calo_hit_properties = cd_calo_hit_.grab_auxiliaries();

      // Propagate UDD's parent hit index
      const std::string UDD_parent_key = "UDD.parent";
      if (pcd_calo_hit_properties.has_key(UDD_parent_key)){
	const int UDD_parent_index = pcd_calo_hit_properties.fetch_integer(UDD_parent_key);
	cd_calo_hit_properties.store_integer(UDD_parent_key, UDD_parent_index);
      }

      // Store pCD's parent hit index
      cd_calo_hit_properties.store("pCD.parent", pcd_calo_hit_.get_hit_id());

      if (cd_calo_hit_.get_geom_id().get_type() == 1302) {
	cd_calo_hit_.grab_geom_id().set_any(4); // for MW!!
	cd_calo_hit_properties.store("category", "calo");
      }
      else if (cd_calo_hit_.get_geom_id().get_type() == 1232)
	cd_calo_hit_properties.store("category", "xcalo");
      else if (cd_calo_hit_.get_geom_id().get_type() == 1252)
	cd_calo_hit_properties.store("category", "gveto");

      if (datatools::logger::is_trace(get_logging_priority()))
	cd_calo_hit_.print_tree(std::clog);

      return true;
    }

    void pcd2cd_module::process_calo_impl() {

      const auto & pcd_calo_hits = _pcd_data_->calorimeter_hits();
      auto & cd_calo_hits = _cd_data_->calorimeter_hits();

      for (const auto & pcd_calo_hit : pcd_calo_hits) {

	// Create a new CD calorimeter hit
	auto cd_calo_hit = datatools::make_handle<snemo::datamodel::calibrated_calorimeter_hit>();
	cd_calo_hit->set_hit_id(cd_calo_hits.size());

	// Calibrate it
	if (calibrate_calo_hit(pcd_calo_hit.get(), cd_calo_hit.grab()))
  
	  // Append it to the collection:
	  _cd_data_->calorimeter_hits().push_back(cd_calo_hit);
      }
    }

    void pcd2cd_module::calibrate_tracker_radius(const snemo::datamodel::precalibrated_tracker_hit & ,
						 snemo::datamodel::calibrated_tracker_hit & cd_tracker_hit_) {

      if (_pcd2cd_tracker_radius_method_ == TRACKER_RADIUS_FALAISE) {

	cd_tracker_hit_.set_r(1.1*CLHEP::cm);
	cd_tracker_hit_.set_sigma_r(1.1*CLHEP::mm);


      } else if (_pcd2cd_tracker_radius_method_ == TRACKER_RADIUS_MANU) {

	const double time_usec = cd_tracker_hit_.get_anode_time() / CLHEP::microsecond;

	double radius = 0;

	const double _tracker_drift_model_manu_params_[5] = {0.263223, -0.030965, -0.571594, 6.01392e-02, 1.13142e+03};

	if (time_usec < 0)
	  radius = 0;

	else {

	  if (time_usec > 30)
	    DT_LOG_WARNING(get_logging_priority(), _event_id_ << " CD hit of " << cd_tracker_hit_.get_geom_id()
			   << " with large anode drift time = " << time_usec << " us");

	  const double r2 = _tracker_drift_model_manu_params_[3] * std::log(1 + time_usec * _tracker_drift_model_manu_params_[4]);

	  if (time_usec > 10)
	    radius = r2;

	  else {
	    const double r1 = _tracker_drift_model_manu_params_[0] * std::exp(_tracker_drift_model_manu_params_[1]*time_usec)/std::pow(time_usec,_tracker_drift_model_manu_params_[2]);
	    radius = std::min(r1, r2);
	  }

	  if (radius > 0.5)
	    cd_tracker_hit_.set_peripheral(true);

	  // if (radius > sqrt(0.5))
	  //   return false;

	  cd_tracker_hit_.set_r(radius*4.4*CLHEP::cm);
	  cd_tracker_hit_.set_sigma_r(1.1*CLHEP::mm);

	}

      }

      // } else if (_pcd2cd_tracker_radius_method_ == TRACKER_RADIUS_XXX) {
      // 	// [...]
      // }

    }

    void pcd2cd_module::calibrate_tracker_height(const snemo::datamodel::precalibrated_tracker_hit & pcd_tracker_hit_,
						 snemo::datamodel::calibrated_tracker_hit & cd_tracker_hit_) {

      const bool has_bottom_cathode = pcd_tracker_hit_.has_bottom_cathode_drift_time();
      const bool has_top_cathode = pcd_tracker_hit_.has_top_cathode_drift_time();
      const bool has_both_cathode = (has_bottom_cathode && has_top_cathode);

      double bottom_cathode_drift_time = pcd_tracker_hit_.get_bottom_cathode_drift_time();
      double top_cathode_drift_time = pcd_tracker_hit_.get_top_cathode_drift_time();

      if (has_bottom_cathode) {
	// apply time calibration
	if (_pcd2cd_tracker_time_method_ == TRACKER_TIME_T0_TABLE) {
	  const int tracker_gg_num = snemo::datamodel::gg_num(pcd_tracker_hit_.get_geom_id());
	  bottom_cathode_drift_time -= _pcd2cd_tracker_bottom_cathode_t0_constants_[tracker_gg_num];
	  bottom_cathode_drift_time += _pcd2cd_tracker_anode_t0_constants_[tracker_gg_num];
	}
	// store calibrated time
	cd_tracker_hit_.grab_auxiliaries().update("bottom_drift_time", bottom_cathode_drift_time);
      } else {
	cd_tracker_hit_.set_bottom_cathode_missing(true);
      }

      if (has_top_cathode) {
	// apply time calibration
	if (_pcd2cd_tracker_time_method_ == TRACKER_TIME_T0_TABLE) {
	  const int tracker_gg_num = snemo::datamodel::gg_num(pcd_tracker_hit_.get_geom_id());
	  top_cathode_drift_time -= _pcd2cd_tracker_top_cathode_t0_constants_[tracker_gg_num];
	  top_cathode_drift_time += _pcd2cd_tracker_anode_t0_constants_[tracker_gg_num];
	}
	// store calibrated time
	cd_tracker_hit_.grab_auxiliaries().update("top_drift_time", top_cathode_drift_time);
      } else {
	cd_tracker_hit_.set_top_cathode_missing(true);
      }

      //Tracker height calibration - Use only events with both R5,R6 cathode TS - Linear Plasma Model
      if (_pcd2cd_tracker_height_method_ == TRACKER_HEIGHT_LINEAR_R5R6) {

	const double H = _pcd2cd_tracker_height_effective_;
	const double H0 = _pcd2cd_tracker_height_offset_;

	if (has_both_cathode) {
	  const double plasma_propagation_time = bottom_cathode_drift_time + top_cathode_drift_time;
	  const double z_norm = (bottom_cathode_drift_time-top_cathode_drift_time)/plasma_propagation_time;
	  const double z_abs = z_norm * H  + H0;
	  cd_tracker_hit_.set_z(z_abs);
	  cd_tracker_hit_.set_sigma_z(_pcd2cd_tracker_height_error_);
	}

      //Tracker height calibration - Use only events with both R5,R6 cathode TS - Decelerating Plasma Model
      } else if (_pcd2cd_tracker_height_method_ == TRACKER_HEIGHT_NON_LINEAR_R5R6) {

	const double H = _pcd2cd_tracker_height_effective_;
	const double H0 = _pcd2cd_tracker_height_offset_;
	const double K = _pcd2cd_tracker_height_deceleration_;

	if (has_both_cathode) {
	  const double plasma_propagation_time = bottom_cathode_drift_time + top_cathode_drift_time;
	  const double z_norm = (bottom_cathode_drift_time-top_cathode_drift_time)/plasma_propagation_time;
	  const double z_norm_non_linear = z_norm - (K *0.5 * z_norm * (1 - std::abs(z_norm)));
	  const double z_abs = z_norm_non_linear * H  + H0;
	  double z_error_theory = _pcd2cd_tracker_height_error_*sqrt(1-pow(z_norm,2));
	  double z_error_limit = _pcd2cd_tracker_height_error_*pow(3,-1);
	  const double z_error = std::max(z_error_theory, z_error_limit);
	  cd_tracker_hit_.set_z(z_abs);
	  // todo: error model
	  cd_tracker_hit_.set_sigma_z(z_error);
	}

      }


      //Tracker height calibration - Use events with only R5&&R6 - Z - Dependant Error Distribution Used
	if (_pcd2cd_tracker_height_method_ == TRACKER_HEIGHT_LINEAR_R5R6_ERROR) {

	const double H = _pcd2cd_tracker_height_effective_;
	const double H0 = _pcd2cd_tracker_height_offset_;

	if (has_both_cathode) {
	  const double plasma_propagation_time = bottom_cathode_drift_time + top_cathode_drift_time;
	  const double z_norm = (bottom_cathode_drift_time-top_cathode_drift_time)/plasma_propagation_time;
	  const double z_abs = z_norm * H  + H0;
        //Calc Error - Prevent error falling below set minimumm at before
	  double z_error_theory = _pcd2cd_tracker_height_error_*sqrt(1-pow(z_norm,2));
	  double z_error_limit = _pcd2cd_tracker_height_error_*pow(3,-1);
	  const double z_error = std::max(z_error_theory, z_error_limit);
	  cd_tracker_hit_.set_z(z_abs);
	  cd_tracker_hit_.set_sigma_z(z_error);
	}
      }

      //Tracker height calibration - Use events with both R5&&R6 and R5||R6 cathode TS - Linear Plasma Model - Z Dependant Error Distribution Used
      else if (_pcd2cd_tracker_height_method_ == TRACKER_HEIGHT_LINEAR_SINGLE_TS_R5R6_ERROR) {

	const double H = _pcd2cd_tracker_height_effective_;
	const double H0 = _pcd2cd_tracker_height_offset_;
      //rename error parameters for single ts z reconstruction
	const double error_single_ts_bot_a = _pcd2cd_tracker_height_error_single_ts_bot_a_;
	const double error_single_ts_bot_b = _pcd2cd_tracker_height_error_single_ts_bot_b_;
	const double error_single_ts_top_a = _pcd2cd_tracker_height_error_single_ts_top_a_;
	const double error_single_ts_top_b = _pcd2cd_tracker_height_error_single_ts_top_b_;

      //R5&&R6 HITS
	if (has_both_cathode) {
	  const double plasma_propagation_time = bottom_cathode_drift_time + top_cathode_drift_time;
	  const double z_norm = (bottom_cathode_drift_time-top_cathode_drift_time)/plasma_propagation_time;
	  const double z_abs = z_norm * H  + H0;
	  double z_error_theory = _pcd2cd_tracker_height_error_*sqrt(1-pow(z_norm,2));
	  double z_error_limit = _pcd2cd_tracker_height_error_*pow(3,-1);
	  const double z_error = std::max(z_error_theory, z_error_limit);
	  cd_tracker_hit_.set_z(z_abs);
	  cd_tracker_hit_.set_sigma_z(z_error);
	}

      //R5||R6 HITS

      else if (has_bottom_cathode) {
        //Get PPT for cell from database
	  const double plasma_propagation_time = _pcd2cd_tracker_ppt_constants_.at(snemo::datamodel::gg_num(pcd_tracker_hit_.get_geom_id()));
        //check TS is valid for reconstruction (ie. <PPT) - modify in future to use TS>PPT hits by artifically placing them at end of cell
	  if (bottom_cathode_drift_time<plasma_propagation_time){
	  const double z_norm = ((2*bottom_cathode_drift_time)-plasma_propagation_time)/plasma_propagation_time;
	  const double z_abs = z_norm * H  + H0; 
        //Calc Error (ensure error is never lower than 1/3rd centre error)
	  double z_error_theory = sqrt((pow(error_single_ts_bot_a,2)*pow(1+z_norm,2))+(pow(error_single_ts_bot_b,2)*(1+z_norm)));
	  double z_error_limit = sqrt((pow(error_single_ts_bot_a,2)*pow(1,2))+(pow(error_single_ts_bot_b,2)*(1)))*pow(3,-1);
        //Check Error ensure is not below limit
	  const double z_error = std::max(z_error_theory, z_error_limit);
	  cd_tracker_hit_.set_z(z_abs);
	  cd_tracker_hit_.set_sigma_z(z_error);
	}}

      //Equivilent procedure to bot cathode hit (but Z value inverted)
	else if (has_top_cathode) {
	  const double plasma_propagation_time = _pcd2cd_tracker_ppt_constants_.at(snemo::datamodel::gg_num(pcd_tracker_hit_.get_geom_id()));
	  if (top_cathode_drift_time<plasma_propagation_time){
	  const double z_norm = (plasma_propagation_time-(2*top_cathode_drift_time))/plasma_propagation_time;
	  const double z_abs = z_norm * H  + H0;
	  double z_error_theory = sqrt((pow(error_single_ts_top_a,2)*pow(1-z_norm,2))+(pow(error_single_ts_top_b,2)*(1-z_norm)));
	  double z_error_limit = sqrt((pow(error_single_ts_top_a,2)*pow(1,2))+(pow(error_single_ts_top_b,2)*(1)))*pow(3,-1);
	  const double z_error = std::max(z_error_theory, z_error_limit);
	  cd_tracker_hit_.set_z(z_abs);
	  cd_tracker_hit_.set_sigma_z(z_error);
	}}
      }


      //Tracker height calibration - Use events with both R5&&R6 and R5||R6 cathode TS - Decelerating Plasma Model - Z Dependant Error Distribution Used
      else if (_pcd2cd_tracker_height_method_ == TRACKER_HEIGHT_NON_LINEAR_SINGLE_TS_R5R6_ERROR) {

      const double H = _pcd2cd_tracker_height_effective_;
	const double H0 = _pcd2cd_tracker_height_offset_;
      const double K = _pcd2cd_tracker_height_deceleration_;
      //rename error parameters for single ts z reconstruction
	const double error_single_ts_bot_a = _pcd2cd_tracker_height_error_single_ts_bot_a_;
	const double error_single_ts_bot_b = _pcd2cd_tracker_height_error_single_ts_bot_b_;
	const double error_single_ts_top_a = _pcd2cd_tracker_height_error_single_ts_top_a_;
	const double error_single_ts_top_b = _pcd2cd_tracker_height_error_single_ts_top_b_;

      //R5&&R6 HITS
	if (has_both_cathode) {
	  const double plasma_propagation_time = bottom_cathode_drift_time + top_cathode_drift_time;
	  const double z_norm = (bottom_cathode_drift_time-top_cathode_drift_time)/plasma_propagation_time;
      const double z_norm_non_linear = z_norm - (K *0.5 * z_norm * (1 - std::abs(z_norm)));
	  const double z_abs = z_norm_non_linear * H  + H0;
	  double z_error_theory = _pcd2cd_tracker_height_error_*sqrt(1-pow(z_norm,2));
	  double z_error_limit = _pcd2cd_tracker_height_error_*pow(3,-1);
	  const double z_error = std::max(z_error_theory, z_error_limit);
	  cd_tracker_hit_.set_z(z_abs);
	  cd_tracker_hit_.set_sigma_z(z_error);
	}

      //R5||R6 HITS

      else if (has_bottom_cathode) {
        //Get PPT for cell from database
	  const double plasma_propagation_time = _pcd2cd_tracker_ppt_constants_.at(snemo::datamodel::gg_num(pcd_tracker_hit_.get_geom_id()));
        //check TS is valid for reconstruction (ie. <PPT) - modify in future to use TS>PPT hits by artifically placing them at end of cell
	  if (bottom_cathode_drift_time<plasma_propagation_time){
	  const double z_norm = ((2*bottom_cathode_drift_time)-plasma_propagation_time)/plasma_propagation_time;
	  const double z_abs = z_norm * H  + H0; 
        //Calc Error (ensure error is never lower than 1/3rd centre error)
	  double z_error_theory = sqrt((pow(error_single_ts_bot_a,2)*pow(1+z_norm,2))+(pow(error_single_ts_bot_b,2)*(1+z_norm)));
	  double z_error_limit = sqrt((pow(error_single_ts_bot_a,2)*pow(1,2))+(pow(error_single_ts_bot_b,2)*(1)))*pow(3,-1);
        //Check Error ensure is not below limit
	  const double z_error = std::max(z_error_theory, z_error_limit);
	  cd_tracker_hit_.set_z(z_abs);
	  cd_tracker_hit_.set_sigma_z(z_error);
	}}

      //Equivilent procedure to bot cathode hit (but Z value inverted)
	else if (has_top_cathode) {
	  const double plasma_propagation_time = _pcd2cd_tracker_ppt_constants_.at(snemo::datamodel::gg_num(pcd_tracker_hit_.get_geom_id()));
	  if (top_cathode_drift_time<plasma_propagation_time){
	  const double z_norm = (plasma_propagation_time-(2*top_cathode_drift_time))/plasma_propagation_time;
	  const double z_abs = z_norm * H  + H0;
	  double z_error_theory = sqrt((pow(error_single_ts_top_a,2)*pow(1-z_norm,2))+(pow(error_single_ts_top_b,2)*(1-z_norm)));
	  double z_error_limit = sqrt((pow(error_single_ts_top_a,2)*pow(1,2))+(pow(error_single_ts_top_b,2)*(1)))*pow(3,-1);
	  const double z_error = std::max(z_error_theory, z_error_limit);
	  cd_tracker_hit_.set_z(z_abs);
	  cd_tracker_hit_.set_sigma_z(z_error);
	}}
      }



      // } else if (_pcd2cd_tracker_height_method_ == TRACKER_HEIGHT_XXX) {
      // 	// [...]
      // }

    }

    bool pcd2cd_module::calibrate_tracker_hit(const snemo::datamodel::precalibrated_tracker_hit & pcd_tracker_hit_,
					   snemo::datamodel::calibrated_tracker_hit & cd_tracker_hit_) {

      DT_LOG_TRACE(get_logging_priority(), "Calibrating tracker hit from " << snemo::datamodel::gg_label(pcd_tracker_hit_.get_geom_id()));

      cd_tracker_hit_.set_geom_id(pcd_tracker_hit_.get_geom_id());
      // cd_tracker_hit_.grab_geom_id().set_type(cd_tracker_hit_.get_geom_id().get_type()+1);

      const int tracker_gg_num = snemo::datamodel::gg_num(pcd_tracker_hit_.get_geom_id());

      // Retrieve pCD and CD auxiliaries
      const datatools::properties & pcd_tracker_hit_properties = pcd_tracker_hit_.get_auxiliaries();
      datatools::properties & cd_tracker_hit_properties = cd_tracker_hit_.grab_auxiliaries();

      double reference_time = 0;

      if (pcd_tracker_hit_properties.has_key("pCD.clustering.cluster_id")) {
	int cluster_id = pcd_tracker_hit_properties.fetch_integer("pCD.clustering.cluster_id");
	reference_time = _cluster_reference_time_[cluster_id];
      }

      double anode_time = pcd_tracker_hit_.get_anodic_time();

      // apply time calibration
      if (_pcd2cd_tracker_time_method_ == TRACKER_TIME_T0_TABLE)
	anode_time -= _pcd2cd_tracker_anode_t0_constants_[tracker_gg_num];

      if (reference_time > 0) {
	// store calibrated anode time and calibrate radius
	cd_tracker_hit_.set_anode_time(anode_time - reference_time);
	this->calibrate_tracker_radius(pcd_tracker_hit_, cd_tracker_hit_);
      } else {
	// if no (calorimeter) reference time are found, consider the cell
	// as "delayed", in order to trigger free T0 track fitting ...
	cd_tracker_hit_.set_delayed_time(anode_time - _event_time_);
      }

      // calibrate height
      this->calibrate_tracker_height(pcd_tracker_hit_, cd_tracker_hit_);

      // extract and fill cell xy coordinates
      const geomtools::mapping & mapping = geoManager->get_mapping();
      const geomtools::geom_info & cell_ginfo = mapping.get_geom_info(cd_tracker_hit_.get_geom_id());
      const geomtools::placement & cell_placement = cell_ginfo.get_world_placement();
      const geomtools::vector_3d & cell_pos  = cell_placement.get_translation();
      cd_tracker_hit_.set_xy(cell_pos.getX(), cell_pos.getY());

      // Propagate UDD's parent hit index
      const std::string UDD_parent_key = "UDD.parent";
      if (pcd_tracker_hit_properties.has_key(UDD_parent_key)){
	const int UDD_parent_index = pcd_tracker_hit_properties.fetch_integer(UDD_parent_key);
	cd_tracker_hit_properties.store_integer(UDD_parent_key, UDD_parent_index);
      }

      // Store pCD's parent hit index
      cd_tracker_hit_properties.store("pCD.parent", pcd_tracker_hit_.get_hit_id());

      if (datatools::logger::is_trace(get_logging_priority()))
	cd_tracker_hit_.print_tree(std::clog);

      return true;
    }

    void pcd2cd_module::process_tracker_impl() {

      const auto & pcd_tracker_hits = _pcd_data_->tracker_hits();
      auto & cd_tracker_hits = _cd_data_->tracker_hits();

      // create and append a tracker clustering solution
      auto tcd_solution = datatools::make_handle<snemo::datamodel::tracker_clustering_solution>();
      tcd_solution->set_solution_id(_tcd_data_->size());
      _tcd_data_->append_solution(tcd_solution, true);

      const datatools::properties & pcd_data_properties = _pcd_data_->get_properties();
      const int nb_clusters = pcd_data_properties.fetch_integer("pCD.clustering.nb_clusters");
      const int nb_unclustered_hits = pcd_data_properties.fetch_integer("pCD.clustering.nb_unclustered_tracker_hits");

      // reserve/allocate tracker clustering solution storage
      auto & tcd_clusters = tcd_solution->get_clusters();
      tcd_clusters.reserve(nb_clusters);

      for (int cluster_i=0; cluster_i<nb_clusters; cluster_i++) {
	auto tcd_cluster = datatools::make_handle<snemo::datamodel::tracker_cluster>();
	tcd_cluster->set_cluster_id(cluster_i);
	tcd_clusters.push_back(tcd_cluster);
      }

      auto & tcd_unclustered_hits = tcd_solution->get_unclustered_hits();
      tcd_unclustered_hits.reserve(nb_unclustered_hits);

      // process tracker hit
      for (const auto & pcd_tracker_hit : pcd_tracker_hits) {

	// Create a new CD tracker hit
	auto cd_tracker_hit = datatools::make_handle<snemo::datamodel::calibrated_tracker_hit>();
	cd_tracker_hit->set_hit_id(cd_tracker_hits.size());

	// retrieve pCD cluster ID
	int cluster_id = -1;
	const datatools::properties & pcd_tracker_hit_properties = pcd_tracker_hit->get_auxiliaries();
	if (pcd_tracker_hit_properties.has_key("pCD.clustering.cluster_id"))
	  cluster_id = pcd_tracker_hit_properties.fetch_integer("pCD.clustering.cluster_id");

	// Calibrate it
	calibrate_tracker_hit(pcd_tracker_hit.get(), cd_tracker_hit.grab());

	// add CD hit into TCD solution
	if (cluster_id != -1) {
	  auto & tcd_cluster = tcd_clusters.at(cluster_id);
	  tcd_cluster->hits().push_back(cd_tracker_hit);
	  if (cd_tracker_hit->is_delayed())
	    tcd_cluster->make_delayed();
	} else {
	  tcd_unclustered_hits.push_back(cd_tracker_hit);
	}

	// Append it to the collection:
	_cd_data_->tracker_hits().push_back(cd_tracker_hit);
      }

    }

  }  // end of namespace processing

}  // end of namespace snemo

/********************************
 * OCD support : implementation *
 ********************************/

#include <datatools/object_configuration_description.h>

/** Opening macro for implementation
 *  @arg snemo::processing::pcd2cd_module the full class name
 *  @arg ocd_ is the identifier of the 'datatools::object_configuration_description'
 *            to be initialized (passed by mutable reference).
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::processing::pcd2cd_module, ocd_) {
  ocd_.set_class_name("snemo::processing::pcd2cd_module");
  ocd_.set_class_description(
                             "A module that performs a precalibration of the unified digitized data (UDD) bank of the calorimeter and tracker data writing in the precalibrated data (pCD) bank");
  ocd_.set_class_library("falaise");
  // ocd_.set_class_documentation("");

  dpp::base_module::common_ocd(ocd_);

  {
    // Description of the 'SD_label' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("UDD_label")
      .set_terse_description("The label/name of the 'unified digitized data' bank")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description(
                            "This is the name of the bank to be used   \n"
                            "as the input unified  calorimeter and tracker hits.  \n")
      .set_default_value_string(snedm::labels::unified_digitized_data())
      .add_example(
                   "Use an alternative name for the 'unified digitized data' bank:: \n"
                   "                                \n"
                   "  UDD_label : string = \"UDD2\" \n"
                   "                                \n");
  }

  {
    // Description of the 'CD_label' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("pCD_label")
      .set_terse_description("The label/name of the 'precalibrated data' bank")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description(
                            "This is the name of the bank to be used    \n"
                            "as the output precalibrated calorimeter and tracker hits. \n")
      .set_default_value_string(snedm::labels::precalibrated_data())
      .add_example(
                   "Use an alternative name for the 'precalibrated data' bank:: \n"
                   "                                  \n"
                   "  pCD_label : string = \"pCD2\"   \n"
                   "                                  \n");
  }

  // Additionnal configuration hints :
  ocd_.set_configuration_hints(
                               "Here is a full configuration example in the \n"
                               "``datatools::properties`` ASCII format::    \n"
                               "                                            \n"
                               "  UDD_label    : string = \"UDD\"           \n"
                               "  pCD_label    : string = \"pCD\"           \n"
                               "                                            \n");

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()  // Closing macro for implementation

// Registration macro for class 'snemo::processing::pcd2cd_module' :
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::processing::pcd2cd_module,
				 "snemo::processing::pcd2cd_module")

// end of falaise/snemo/processing/pcd2cd_module.cc
