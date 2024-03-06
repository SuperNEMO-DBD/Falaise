// This Project
#include "FLpCDToCpCDAlgo.h"

// This project:
#include <bayeux/datatools/exception.h>
#include <falaise/snemo/datamodels/data_model.h>
#include <falaise/snemo/datamodels/event_header.h>
#include <falaise/snemo/datamodels/precalibrated_data.h>

namespace FLpCDToCpCD {

  FLpCDToCpCDAlgorithm::FLpCDToCpCDAlgorithm()
  {
    _eh_tag_ = snedm::labels::event_header();
    _pcd_tag_ = snedm::labels::precalibrated_data();
    _cpcd_tag_ = snedm::labels::clusterized_precalibrated_data();
    _throw_at_missing_input_ = false;
    _skip_cluster_without_calo_hits_ = false;
    return;
  }

  const std::string & FLpCDToCpCDAlgorithm::eh_tag() const
  {
    return _eh_tag_;
  }

  const std::string & FLpCDToCpCDAlgorithm::pcd_tag() const
  {
    return _pcd_tag_;
  }

  const std::string & FLpCDToCpCDAlgorithm::cpcd_tag() const
  {
    return _cpcd_tag_;
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

  void FLpCDToCpCDAlgorithm::process(const datatools::things & input_data_record_,
				     data_records_col & output_data_records_)
  {
    DT_LOG_DEBUG(verbosity(), "Processing input data record...");
    // DT_LOG_DEBUG(verbosity(), "Processing input data record...");
    output_data_records_.clear();
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
    const auto & inputPcdData
      = input_data_record_.get<snemo::datamodel::precalibrated_data>(_pcd_tag_);

    const auto & inputEhData
      = input_data_record_.get<snemo::datamodel::event_header>(_eh_tag_);

    size_t nb_clusters = 0;
    if (inputPcdData.get_properties().has_key("pCD.clustering.nclusters")) {
      nb_clusters = inputPcdData.get_properties().fetch_integer("pCD.clustering.nclusters");
    }
    DT_LOG_DEBUG(verbosity(), "Extracted number of clusters : " << nb_clusters);
    if (nb_clusters == 0) return;
    output_data_records_.clear();
    for (unsigned int i = 0; i < nb_clusters; i++) {
      output_data_records_.push_back(datatools::make_handle<datatools::things>());
      auto & newRecord = output_data_records_.back().grab();
      DT_LOG_DEBUG(verbosity(), "making a new output Event Header for cluster #" << i);
      auto & newEhData = snedm::addToEvent<snemo::datamodel::event_header>(_eh_tag_, newRecord);
      newEhData = inputEhData;
      newEhData.get_properties().store_integer("CpCD.cluster_id", i);
      DT_LOG_DEBUG(verbosity(), "making a new output CpCD for cluster #" << i);
      auto & newPcdData = snedm::addToEvent<snemo::datamodel::precalibrated_data>(_cpcd_tag_, newRecord);
      auto & caloHits = newPcdData.calorimeter_hits();
      auto & trackerHits = newPcdData.tracker_hits();
      DT_LOG_DEBUG(verbosity(), "  # calo hits    = " << caloHits.size());
      DT_LOG_DEBUG(verbosity(), "  # tracker hits = " << trackerHits.size());
    }

    auto & inputCaloHits = inputPcdData.calorimeter_hits();
    std::set<int> unclusterizedCaloHitIndexes;
    DT_LOG_DEBUG(verbosity(), "Processing " << inputCaloHits.size() << " input calo hits...");
    for (auto iCaloHit = 0u; iCaloHit < inputCaloHits.size(); iCaloHit++) {
      auto & inputCaloHit = inputCaloHits[iCaloHit];
      if (not inputCaloHit->get_auxiliaries().has_flag("pCD.clustering.clusterized")) {
	unclusterizedCaloHitIndexes.insert(iCaloHit);
	continue;
      }
      int clusterId = inputCaloHit->get_auxiliaries().fetch_integer("pCD.clustering.cluster_id");
      DT_LOG_DEBUG(verbosity(), "  Calo hit #" << iCaloHit << " is clusterized with cluster ID=" << clusterId);
      snemo::datamodel::precalibrated_data & clusterPcdData
	= output_data_records_[clusterId].grab().grab<snemo::datamodel::precalibrated_data>(_cpcd_tag_);
      clusterPcdData.calorimeter_hits().push_back(inputCaloHit);
      clusterPcdData.calorimeter_hits().back()->grab_auxiliaries().erase_all_starting_with("pCD.clustering.");
    }

    auto & inputTrackerHits = inputPcdData.tracker_hits();
    std::set<int> unclusterizedTrackerHitIndexes;
    DT_LOG_DEBUG(verbosity(), "Processing " << inputTrackerHits.size() << " input tracker hits...");
    for (auto iTrackerHit = 0u; iTrackerHit < inputTrackerHits.size(); iTrackerHit++) {
      auto & inputTrackerHit = inputTrackerHits[iTrackerHit];
      if (not inputTrackerHit->get_auxiliaries().has_flag("pCD.clustering.clusterized")) {
	unclusterizedTrackerHitIndexes.insert(iTrackerHit);
	continue;
      }
      int clusterId = inputTrackerHit->get_auxiliaries().fetch_integer("pCD.clustering.cluster_id");
      DT_LOG_DEBUG(verbosity(), "  Tracker hit #" << iTrackerHit << " is clusterized with cluster ID=" << clusterId);
      snemo::datamodel::precalibrated_data & clusterPcdData
	= output_data_records_[clusterId].grab().grab<snemo::datamodel::precalibrated_data>(_cpcd_tag_);
      clusterPcdData.tracker_hits().push_back(inputTrackerHit);
      clusterPcdData.tracker_hits().back()->grab_auxiliaries().erase_all_starting_with("pCD.clustering.");
    }

    DT_LOG_DEBUG(verbosity(), "Input data record has been processed.");
    return;
  }

} // namespace FLpCDToCpCD
