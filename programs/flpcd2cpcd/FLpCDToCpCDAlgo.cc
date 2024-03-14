// This Project
#include "FLpCDToCpCDAlgo.h"

// This project:
#include <bayeux/datatools/exception.h>
#include <falaise/snemo/datamodels/data_model.h>
#include <falaise/snemo/datamodels/event_header.h>
#include <falaise/snemo/datamodels/unified_digitized_data.h>
#include <falaise/snemo/datamodels/precalibrated_data.h>

namespace FLpCDToCpCD {

  FLpCDToCpCDAlgorithm::FLpCDToCpCDAlgorithm()
  {
    _eh_tag_ = snedm::labels::event_header();
    _udd_tag_ = snedm::labels::unified_digitized_data();
    _pcd_tag_ = snedm::labels::precalibrated_data();
    _throw_at_missing_input_ = false;
    _skip_cluster_without_calo_hits_ = false;
    return;
  }

  const std::string & FLpCDToCpCDAlgorithm::eh_tag() const
  {
    return _eh_tag_;
  }

  const std::string & FLpCDToCpCDAlgorithm::udd_tag() const
  {
    return _udd_tag_;
  }

  const std::string & FLpCDToCpCDAlgorithm::pcd_tag() const
  {
    return _pcd_tag_;
  }
 
  bool FLpCDToCpCDAlgorithm::skip_cluster_without_calo_hits() const
  {
    return _skip_cluster_without_calo_hits_;
  }
  
  datatools::logger::priority FLpCDToCpCDAlgorithm::verbosity() const
  {
    return _verbosity_;
  }

  void FLpCDToCpCDAlgorithm::set_verbosity(const datatools::logger::priority verbosity_)
  {
    _verbosity_ = verbosity_;
    return;
  }

  void FLpCDToCpCDAlgorithm::process(datatools::things & input_data_record_,
				     data_records_col & output_data_records_)
  {
    DT_LOG_DEBUG(verbosity(), "Processing input data record...");
    boost::property_tree::ptree iPrinterOpts;
    iPrinterOpts.put("indent", "[debug] ");
    iPrinterOpts.put("title", "Input data record");
    iPrinterOpts.put("eh.list_properties", true);
    iPrinterOpts.put("udd.list_hits", true);
    iPrinterOpts.put("udd.hit_details", true);
    iPrinterOpts.put("udd.list_properties", true);
    iPrinterOpts.put("pcd.list_hits", true);
    iPrinterOpts.put("pcd.list_properties", true);
    iPrinterOpts.put("cd.list_hits", true);
    iPrinterOpts.put("cd.list_properties", true);
    iPrinterOpts.put("tcd.list_solutions", true);
    iPrinterOpts.put("tcd.solutions.list_clusters", true);
    iPrinterOpts.put("tcd.solutions.list_unclustered_hits", false);
    iPrinterOpts.put("tcd.solutions.list_properties", true);
    iPrinterOpts.put("tcd.list_properties", true);
    output_data_records_.clear();
    if (datatools::logger::is_debug(verbosity())) {
      snedm::data_record_printer iPrinter(input_data_record_);
      iPrinter.print(std::cerr, iPrinterOpts);
    }
    
    if (not input_data_record_.has(_pcd_tag_)) {
      DT_LOG_DEBUG(verbosity(), "Missing pCD bank");
      if (_throw_at_missing_input_) {
	DT_THROW(std::logic_error, "Missing bank with label '" << _pcd_tag_ << "'");
      }
      return;
    }
    if (not input_data_record_.is_a<snemo::datamodel::precalibrated_data>(_pcd_tag_)) {
      DT_THROW(std::logic_error, "Bank '" << _pcd_tag_ << "' does not contain pCD data");      
    }

    const auto & inputEhData
      = input_data_record_.get<snemo::datamodel::event_header>(_eh_tag_);
    DT_LOG_DEBUG(verbosity(), "Input event ID for EH : " << inputEhData.get_id());
    if (inputEhData.is_event_builder()) {
      std::string evBuilderName = "unknown";
      if (inputEhData.get_properties().has_key(snedm::labels::event_builder_model_key())) {
	evBuilderName = inputEhData.get_properties().fetch_string(snedm::labels::event_builder_model_key());
      }
      DT_THROW(std::logic_error, "Event has already been processed by some event builder algorithm, namely "
	       << std::quoted(evBuilderName));           
    }
    
    auto & inputUddData
      = input_data_record_.grab<snemo::datamodel::unified_digitized_data>(_udd_tag_);
 
    auto & inputPcdData
      = input_data_record_.grab<snemo::datamodel::precalibrated_data>(_pcd_tag_);
     
    // Update run number:
    if (_current_run_number_ != inputEhData.get_id().get_run_number()) {
      _current_run_number_ = inputEhData.get_id().get_run_number();
    }
    // Extract the number of clusters:
    size_t nb_clusters = 0;
    if (inputPcdData.get_properties().has_key("pCD.clustering.nclusters")) {
      nb_clusters = inputPcdData.get_properties().fetch_integer("pCD.clustering.nclusters");
    }
    DT_LOG_DEBUG(verbosity(), "Extracted number of clusters : " << nb_clusters);
    if (nb_clusters == 0) return;
    // Populate output data records:
    output_data_records_.clear();
    for (unsigned int i = 0; i < nb_clusters; i++) {
      if (_current_event_number_ == datatools::event_id::INVALID_EVENT_NUMBER) {
	_current_event_number_ = 0;
      }
      output_data_records_.push_back(datatools::make_handle<datatools::things>());
      
      auto & newRecord = output_data_records_.back().grab();

      DT_LOG_DEBUG(verbosity(), "making a new output event header for cluster #" << i);
      auto & newEhData = snedm::addToEvent<snemo::datamodel::event_header>(_eh_tag_, newRecord);
      newEhData = inputEhData;
      newEhData.get_id().set_run_number(_current_run_number_);
      newEhData.get_id().set_event_number(_current_event_number_);
      newEhData.get_properties().store_flag(snedm::labels::event_builder_key());
      newEhData.get_properties().store_string(snedm::labels::event_builder_model_key(), "pcd2cpcd");
      newEhData.get_properties().store_integer("daq.run_number", inputEhData.get_id().get_run_number());
      newEhData.get_properties().store_integer("daq.event_number", inputEhData.get_id().get_event_number());
      newEhData.get_properties().store_integer("daq.cluster_id", i);

      DT_LOG_DEBUG(verbosity(), "making a new output event UDD for cluster #" << i);
      auto & newUddData = snedm::addToEvent<snemo::datamodel::unified_digitized_data>(_udd_tag_, newRecord);
      newUddData.set_run_id(inputUddData.get_run_id());
      newUddData.set_event_id(inputUddData.get_event_id());
      auto & uddCaloHits    = newUddData.get_calorimeter_hits();
      auto & uddTrackerHits = newUddData.get_tracker_hits();
      DT_LOG_DEBUG(verbosity(), "  # new UDD calo hits    = " << uddCaloHits.size());
      DT_LOG_DEBUG(verbosity(), "  # new UDD tracker hits = " << uddTrackerHits.size());
      // newUddData = inputUddData;

      DT_LOG_DEBUG(verbosity(), "making a new output event pCD for cluster #" << i);
      auto & newPcdData = snedm::addToEvent<snemo::datamodel::precalibrated_data>(_pcd_tag_, newRecord);
      auto & pcdCaloHits = newPcdData.calorimeter_hits();
      auto & pcdTrackerHits = newPcdData.tracker_hits();
      DT_LOG_DEBUG(verbosity(), "  # new pCD calo hits    = " << pcdCaloHits.size());
      DT_LOG_DEBUG(verbosity(), "  # new pCD tracker hits = " << pcdTrackerHits.size());

      // Update event number:
      _current_event_number_++;
    }

    // Split input DAQ-based UDD and pCD hits in several cluster-based events:
    auto & inputPcdCaloHits = inputPcdData.calorimeter_hits();
    std::set<int> unclusterizedPcdCaloHitIndexes;
    DT_LOG_DEBUG(verbosity(), "Processing " << inputPcdCaloHits.size() << " input calo hits...");
    for (auto iPcdCaloHit = 0u; iPcdCaloHit < inputPcdCaloHits.size(); iPcdCaloHit++) {
      // Populate pCD bank with pCD calo hits in the target cluster event:
      auto & inputPcdCaloHit = inputPcdCaloHits[iPcdCaloHit];
      if (not inputPcdCaloHit->get_auxiliaries().has_flag("pCD.clustering.clusterized")) {
	unclusterizedPcdCaloHitIndexes.insert(iPcdCaloHit);
	continue;
      }
      int clusterId = inputPcdCaloHit->get_auxiliaries().fetch_integer("pCD.clustering.cluster_id");
      DT_LOG_DEBUG(verbosity(), "  Calo hit #" << iPcdCaloHit << " is clusterized with cluster ID=" << clusterId);
      snemo::datamodel::precalibrated_data & clusterPcdData
	= output_data_records_[clusterId].grab().grab<snemo::datamodel::precalibrated_data>(_pcd_tag_);
      clusterPcdData.calorimeter_hits().push_back(inputPcdCaloHit);
      clusterPcdData.calorimeter_hits().back()->grab_auxiliaries().erase_all_starting_with("pCD.clustering.");

      // Populate UDD bank with UDD calo hits associated to pCD calo hit:
      auto inputPcdCaloHitGid = inputPcdCaloHit->get_geom_id();
      snemo::datamodel::unified_digitized_data & clusterUddData
	= output_data_records_[clusterId].grab().grab<snemo::datamodel::unified_digitized_data>(_udd_tag_);
      auto & clusterUddCaloHits = clusterUddData.grab_calorimeter_hits();
      for (auto & inputUddCaloHit : inputUddData.grab_calorimeter_hits()) {
	if (inputUddCaloHit->get_geom_id() == inputPcdCaloHitGid) {
	  clusterUddCaloHits.push_back(inputUddCaloHit);
	  clusterUddData.add_origin_trigger_id(inputUddCaloHit->get_origin().get_trigger_id()); 
	}
      }
    }

    auto & inputPcdTrackerHits = inputPcdData.tracker_hits();
    std::set<int> unclusterizedPcdTrackerHitIndexes;
    DT_LOG_DEBUG(verbosity(), "Processing " << inputPcdTrackerHits.size() << " input tracker hits...");
    for (auto iPcdTrackerHit = 0u; iPcdTrackerHit < inputPcdTrackerHits.size(); iPcdTrackerHit++) {
     // Populate pCD bank with pCD tracker hits in the target cluster event:
      auto & inputPcdTrackerHit = inputPcdTrackerHits[iPcdTrackerHit];
      if (not inputPcdTrackerHit->get_auxiliaries().has_flag("pCD.clustering.clusterized")) {
	unclusterizedPcdTrackerHitIndexes.insert(iPcdTrackerHit);
	continue;
      }
      int clusterId = inputPcdTrackerHit->get_auxiliaries().fetch_integer("pCD.clustering.cluster_id");
      DT_LOG_DEBUG(verbosity(), "  Tracker hit #" << iPcdTrackerHit << " is clusterized with cluster ID=" << clusterId);
      snemo::datamodel::precalibrated_data & clusterPcdData
	= output_data_records_[clusterId].grab().grab<snemo::datamodel::precalibrated_data>(_pcd_tag_);
      clusterPcdData.tracker_hits().push_back(inputPcdTrackerHit);
      clusterPcdData.tracker_hits().back()->grab_auxiliaries().erase_all_starting_with("pCD.clustering.");

      // Populate UDD bank with UDD tracker hits associated to pCD tracker hit:
      auto inputPcdTrackerHitGid = inputPcdTrackerHit->get_geom_id();
      snemo::datamodel::unified_digitized_data & clusterUddData
	= output_data_records_[clusterId].grab().grab<snemo::datamodel::unified_digitized_data>(_udd_tag_);
      auto & clusterUddTrackerHits = clusterUddData.grab_tracker_hits();
      for (auto & inputUddTrackerHit : inputUddData.grab_tracker_hits()) {
	if (inputUddTrackerHit->get_geom_id() == inputPcdTrackerHitGid) {
	  clusterUddTrackerHits.push_back(inputUddTrackerHit);
	  for (const snemo::datamodel::tracker_digitized_hit::gg_times & tdht: inputUddTrackerHit->get_times()) {
	    for (auto i = 0u; i < 5; i++) {
	      if (tdht.has_anode_origin(i)) {
		clusterUddData.add_origin_trigger_id(tdht.get_anode_origin(i).get_trigger_id());
	      }
	    }
	    if (tdht.has_bottom_cathode_origin()) {
	      clusterUddData.add_origin_trigger_id(tdht.get_bottom_cathode_origin().get_trigger_id());
	    }
	    if (tdht.has_top_cathode_origin()) {
	      clusterUddData.add_origin_trigger_id(tdht.get_top_cathode_origin().get_trigger_id());
	    }
	  }
	}
      }
    }

    if (datatools::logger::is_debug(verbosity())) {
      DT_LOG_DEBUG(verbosity(), "Output data records : " << output_data_records_.size());
      int odrCount = 0;
      for (const auto & odr : output_data_records_) {
	snedm::data_record_printer oPrinter(*odr);
	// Copy the input printer options:
	boost::property_tree::ptree oPrinterOpts = iPrinterOpts;
	// But update the title:
	oPrinterOpts.put("title", "Ouput data record #" + std::to_string(odrCount));
	oPrinter.print(std::cerr, oPrinterOpts);
	odrCount++;
      }
    }
    DT_LOG_DEBUG(verbosity(), "Input data record has been processed.");
    return;
  }

} // namespace FLpCDToCpCD
