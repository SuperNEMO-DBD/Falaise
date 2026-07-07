#include "FlUddChannelMonitoring.hh"

#include <memory>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>

// This Project
#include <falaise/snemo/datamodels/calorimeter_digitized_hit.h>
#include <falaise/snemo/datamodels/tracker_digitized_hit.h>
#include <falaise/snemo/datamodels/udd_utils.h>
#include <falaise/snemo/datamodels/unified_digitized_data.h>
// #include <falaise/metadata_utils.h>
#include <falaise/snemo/datamodels/event_header.h>
#include <falaise/snemo/datamodels/geomid_utils.h>
#include <falaise/snemo/processing/calo_signal_baseline.h>

#include <gsl/gsl_statistics_double.h>

namespace FLUddStat {

  void FlUddChannelMonitoring::initialize(const datatools::properties & config_)
  {
    if (config_.has_key("verbosity")) {
      verbosity = (datatools::logger::priority) config_.fetch_integer("verbosity");
    }
    _event_counter_ = 0;
    _om_infos_.reserve(snemo::datamodel::number_of_oms_demo());
    for (auto omNum = 0u; omNum < snemo::datamodel::number_of_oms_demo(); omNum++) {
      om_info omInfo;
      omInfo.om_num = omNum;      
      omInfo.gid = snemo::datamodel::om_gid(omInfo.om_num);
      _om_infos_.push_back(omInfo);
    }
    
    if (config_.has_key("out_dir_path")) {
      _out_dir_path_s_ = config_.fetch_path("out_dir_path");
    }
    
    if (_out_dir_path_s_.empty()) {
      _out_dir_path_s_ = std::filesystem::path("_udd_channel_mon-0000/");
    }

    _out_dir_path_ = (_out_dir_path_s_);
    // if (not _out_dir_path_s_.empty()) {
    //   std::filesystem::remove_all(_out_dir_path_);
    // }
    if (not std::filesystem::create_directories(_out_dir_path_)) {
      DT_THROW(std::logic_error, "Cannot create dir " << _out_dir_path_);
    }
    _out_path_ = _out_dir_path_ / "oms.data";
    _period_duration_ = 300u;
    return;
  }

  void FlUddChannelMonitoring::process(const datatools::things & event_)
  {
    static const uint32_t CLOCK_TICK_NS = 25u;
    const snemo::datamodel::event_header & EH
	= event_.get<snemo::datamodel::event_header>("EH");
    const datatools::event_id & ehID = EH.get_id();
    auto ehRunID = ehID.get_run_number();
    auto ehEventID = ehID.get_event_number();
    const snemo::datamodel::timestamp & ehTs = EH.get_timestamp();
    const snemo::datamodel::unified_digitized_data & UDD
      = event_.get<snemo::datamodel::unified_digitized_data>("UDD");
    std::int32_t uddRunId = UDD.get_run_id();
    std::int32_t uddEventId = UDD.get_event_id();
    std::int64_t uddRefTimestamp = UDD.get_reference_timestamp();
    const std::set<int32_t> & uddTriggerIds = UDD.get_origin_trigger_ids();
    auto nbTriggerIds = uddTriggerIds.size();
    const snemo::datamodel::CalorimeterDigiHitHdlCollection & uddCaloHits = UDD.get_calorimeter_hits();
    const snemo::datamodel::TrackerDigiHitHdlCollection & uddTrackerHits = UDD.get_tracker_hits();
    const datatools::properties & uddAux = UDD.get_auxiliaries();
    // snemo::time::time_duration statPeriodDuration
    //   = snemo::time::from_quantity(_period_duration_ * CLHEP::second);
    if (_event_counter_ == 0) {
      _first_event_timestamp_ = ehTs;
      _run_ref_timestamp_ = uddRefTimestamp;
      _first_event_timepoint_ = ehTs.to_timepoint(); 
      DT_LOG_DEBUG(verbosity, "====== EH first event timestamp    = " << _first_event_timestamp_);
      DT_LOG_DEBUG(verbosity, "====== UDD run reference timestamp = " << _run_ref_timestamp_);
    }
    _event_info_.counts++;
    _event_info_.time_point = ehTs.to_timepoint();
    
    // snemo::time::time_point eventTimepoint = ehTs.to_timepoint(); 
    // if (_stat_period_.contains(eventTimepoint)) {      
    // } else {
    //   auto endPeriod = _stat_period_.end();
    //   _stat_period_ = snemo::time::time_period(endPeriod, statPeriodDuration);
    // }

    int newPeriodIndex = (_event_info_.time_point - _first_event_timepoint_).total_seconds() / _period_duration_;
    bool devel = false;
    if (devel) {
      std::cerr << "[DEVEL] first event timepoint = " << snemo::time::to_string(_first_event_timepoint_) << '\n';
      std::cerr << "[DEVEL] this  event timepoint = " << snemo::time::to_string(_event_info_.time_point) << '\n';
      std::cerr << "[DEVEL] newPeriodIndex = " << newPeriodIndex << '\n';
    }
    if (_event_info_.period_index < 0) {
      if (devel) std::cerr << "[DEVEL] ==> Period index start" << '\n';
      _event_info_.period_index = 0;
      for (auto & omInfo : _om_infos_) {
	omInfo.counts_per_period[0] = 0u;
	baseline_info dummyBaselineInfo;
	omInfo.baseline_per_period[0] = dummyBaselineInfo;
      }
    } else if (newPeriodIndex > _event_info_.period_index) {
      if (devel) std::cerr << "[DEVEL] ==> Period index change" << '\n';
      _event_info_.period_index = newPeriodIndex;
      for (auto & omInfo : _om_infos_) {
	omInfo.counts_per_period[_event_info_.period_index] = 0u;
	baseline_info dummyBaselineInfo;
	omInfo.baseline_per_period[_event_info_.period_index] = dummyBaselineInfo;
      }
    }
    
    std::int64_t uddRelTimestamp = (uddRefTimestamp - _run_ref_timestamp_) * CLOCK_TICK_NS;
    DT_LOG_DEBUG(verbosity, "====== New event ======");
    DT_LOG_DEBUG(verbosity, "EH run ID   = " << ehRunID);
    DT_LOG_DEBUG(verbosity, "EH event ID = " << ehEventID);
    DT_LOG_DEBUG(verbosity, "EH timestamp = " << ehTs);
    DT_LOG_DEBUG(verbosity, "UDD run ID   = " << uddRunId);
    DT_LOG_DEBUG(verbosity, "UDD event ID = " << uddEventId);
    DT_LOG_DEBUG(verbosity, "UDD reference timestamp = " << uddRefTimestamp);
    DT_LOG_DEBUG(verbosity, "UDD relative timestamp = " << uddRelTimestamp);
    DT_LOG_DEBUG(verbosity, "#UDD aux = " << uddAux.size());
    DT_LOG_DEBUG(verbosity, "#UDD trigger IDs = " << nbTriggerIds);
    for (const auto triggerId : uddTriggerIds) {
      DT_LOG_DEBUG(verbosity, " - UDD trigger ID = " << triggerId);
    }
    DT_LOG_DEBUG(verbosity, "#UDD calo hits = " << uddCaloHits.size());
    DT_LOG_DEBUG(verbosity, "#UDD tracker hits = " << uddTrackerHits.size());
    DT_LOG_DEBUG(verbosity, "Period index = " << _event_info_.period_index);

    process_calo_hits(uddCaloHits);
    
    _event_counter_++;
    return;
  }
  
  void FlUddChannelMonitoring::process_calo_hits(const snemo::datamodel::CalorimeterDigiHitHdlCollection & calo_hits_)
  {
    std::unique_ptr<std::ofstream> fCaloHitSignalsPtr;
    if ((_max_nb_signal_save_ > 0) and (_event_counter_ < _max_nb_signal_save_)) {
      std::ostringstream fCaloHitSignalsPath;
      fCaloHitSignalsPath << "_calo_hit_signals-" << _event_counter_ << ".data";
      fCaloHitSignalsPtr = std::make_unique<std::ofstream>(fCaloHitSignalsPath.str());
    }
    for (const auto & caloHitHdl : calo_hits_) {
      const auto & caloHit = *caloHitHdl;
      std::int32_t hitId = caloHit.get_hit_id();
      geomtools::geom_id hitGid = caloHit.get_geom_id();
      std::int64_t hitTimestamp = caloHit.get_timestamp();
      int omNum = snemo::datamodel::om_num(hitGid);
      DT_LOG_DEBUG(verbosity, "=== New Calo hit ===");
      DT_LOG_DEBUG(verbosity, "Calo hit ID  = " << hitId);
      DT_LOG_DEBUG(verbosity, "Calo hit GID = " << hitGid);
      DT_LOG_DEBUG(verbosity, "Calo hit timestamp = " << hitTimestamp);
      DT_LOG_DEBUG(verbosity, "Calo OM num = " << omNum);
      DT_LOG_DEBUG(verbosity, "Has signal waveform = " << std::boolalpha << caloHit.has_waveform());
      if (caloHit.has_waveform()) {
	const std::vector<std::int16_t> & caloHitSignal = caloHit.get_waveform();
	DT_LOG_DEBUG(verbosity, "  #samples in signal waveform = " << caloHitSignal.size());
	if (fCaloHitSignalsPtr) {
	  (*fCaloHitSignalsPtr) << "#@calo_hit.id=" << hitId << '\n';
	  (*fCaloHitSignalsPtr) << "#@calo_hit.gid=" << hitGid << '\n';
	  (*fCaloHitSignalsPtr) << "#@calo_hit.timestamp=" << hitTimestamp << '\n';
	  (*fCaloHitSignalsPtr) << "#@calo_hit.om_num=" << omNum << '\n';
	  for (auto iSample = 0u; iSample < caloHitSignal.size(); iSample++) {
	    auto tdc = iSample;
	    auto adc = caloHitSignal[iSample];
	    (*fCaloHitSignalsPtr) << tdc << ' ' << adc << '\n';
	  }
	  (*fCaloHitSignalsPtr) << "\n\n";
	}
      }

      // Rate survey:
      DT_LOG_DEBUG(verbosity, "LT trigger counter = " << caloHit.get_lt_trigger_counter());
      om_info & omInfo = _om_infos_[omNum];
      bool omTrigger = false;
      bool onlyHtHits = _only_ht_hits_;
      if (caloHit.is_low_threshold_only()) {
	omInfo.lto_counts++;
 	omInfo.lt_counts++;
	if (not onlyHtHits) omTrigger = true;
      }
      if (caloHit.is_high_threshold()) {
	omInfo.lt_counts++;
 	omInfo.ht_counts++;
	omTrigger = true;
      }
      if (caloHit.get_lt_trigger_counter()) {
	omInfo.lt_triggered = true;
      }
      if (omTrigger) {
	// if (omInfo.counts_per_period.count(_event_info_.period_index) == 0) {
	//   omInfo.counts_per_period[_event_info_.period_index] = 0;
	// }
	omInfo.counts_per_period[_event_info_.period_index]++;
      }

      // Baseline survey:
      if (not caloHit.is_low_threshold_only() and not caloHit.is_high_threshold()) {
	snemo::processing::calo_signal_baseline_calculator csbc;
	// csbc.verbosity = datatools::logger::PRIO_DEBUG;
	if (caloHit.has_waveform()) {
	  const std::vector<std::int16_t> & caloHitSignal = caloHit.get_waveform();
	  auto baselineMeas = csbc(caloHitSignal);
	  // std::cerr << "[DEVEL] omNum=" << omNum << " ==> baselineMeas.nb_samples = " << baselineMeas.nb_samples << '\n';
	  if (baselineMeas.nb_samples > _baseline_min_samples_) {
	    double hitBaseline = baselineMeas.value / CLHEP::volt;
	    double hitBaselineErr = baselineMeas.sigma / CLHEP::volt;
	    omInfo.baselines.push_back(hitBaseline);
	    omInfo.baselines_err.push_back(hitBaselineErr);
	    omInfo.baselines_nsamples.push_back(baselineMeas.nb_samples);
	    omInfo.baseline_per_period[_event_info_.period_index].mean.add(hitBaseline);
	    // omInfo.baseline_per_period[_event_info_.period_index].nsamples.push_back(baselineMeas.nb_samples);
	  } else {
	    std::cerr << "\n[baseline issue] OM #" << omNum << '\n';
	    csbc.verbosity = datatools::logger::PRIO_DEBUG;
	    auto baselineMeasIssue = csbc(caloHitSignal);
	    std::cerr << '\t'
		      << " q=" << baselineMeasIssue.quality
		      << " value=" << baselineMeasIssue.value
		      << " #samples=" << baselineMeasIssue.nb_samples
		      << '\n';
	    csbc.verbosity = datatools::logger::PRIO_FATAL;
	  }
	}
	
      } 
    }
    return;  
  }

  void FlUddChannelMonitoring::terminate()
  {
    for (auto & omInfo : _om_infos_) {
      if (not omInfo.lt_triggered) continue;
      
      // Rate survey:
      {
	std::filesystem::path omRateOutPath = _out_dir_path_ / ("om_" + std::to_string(omInfo.om_num) + "-rate.data");
	std::cerr << "[DEVEL] creating file " << omRateOutPath << '\n';
	std::ofstream fOmRateOut(omRateOutPath);
	fOmRateOut << "#@om_num=" << omInfo.om_num << '\n';
	fOmRateOut << "#@period_duration_sec=" << _period_duration_ << '\n';
	if (omInfo.counts_per_period.size() > 1) {
	  mygsl::arithmetic_mean mean;
	  auto periodCount = 1u;
	  for (const auto & [iPeriod,counts] : omInfo.counts_per_period) {
	    mean.add(counts);
	    fOmRateOut << iPeriod << ' ' << (counts+1e-10) << '\n';
	    periodCount++;
	    if (periodCount == omInfo.counts_per_period.size()) break;
	  }
	  omInfo.mean_counts = mean.get_mean();
	  omInfo.sigma_counts = std::sqrt(mean.get_variance(true));
	}
	fOmRateOut.close();
      }
      
      // Baseline survey:
      {
	std::filesystem::path omBaselineOutPath = _out_dir_path_ / ("om_" + std::to_string(omInfo.om_num) + "-baseline.data");

	std::sort(omInfo.baselines.begin(), omInfo.baselines.end());
	double baselineMean = std::numeric_limits<double>::quiet_NaN();
	double baselineSigma = std::numeric_limits<double>::quiet_NaN();
	double baselineRobustMean = std::numeric_limits<double>::quiet_NaN();
	if (omInfo.baselines.size() > 0) {
	  double rejectionRatio = 0.0;
	  if (omInfo.baselines.size()>10) {
	    rejectionRatio = 0.1;
	  }
	  baselineMean = gsl_stats_mean(omInfo.baselines.data(),
					1,
					omInfo.baselines.size());
	  baselineRobustMean = gsl_stats_trmean_from_sorted_data(rejectionRatio,
								 omInfo.baselines.data(),
								 1,
								 omInfo.baselines.size());
	  if (omInfo.baselines.size() > 2) {
	    baselineSigma = gsl_stats_sd_m(omInfo.baselines.data(),
					   1,
					   omInfo.baselines.size(),
					   baselineMean);
	  }
	  std::cerr << "[DEVEL] creating file " << omBaselineOutPath << '\n';
	  std::ofstream fOmBaselineOut(omBaselineOutPath);
	  fOmBaselineOut << "#@om_num=" << omInfo.om_num << '\n';
	  fOmBaselineOut << "#@period_duration_sec=" << _period_duration_ << '\n';
	  fOmBaselineOut << "#@baseline_mean=" << baselineMean << '\n';
	  fOmBaselineOut << "#@baseline_sigma=" << baselineSigma << '\n';
	  fOmBaselineOut << "#@baseline_robust_mean=" << baselineRobustMean << '\n';
	  if (omInfo.counts_per_period.size() > 1) {
	    mygsl::arithmetic_mean mean;
	    auto periodCount = 1u;
	    for (const auto & [iPeriod,baselineInfo] : omInfo.baseline_per_period) {
	      double blMean = std::numeric_limits<double>::quiet_NaN();
	      double blSigma = std::numeric_limits<double>::quiet_NaN();
	      if (baselineInfo.mean.get_n() > 0) {
		blMean = baselineInfo.mean.get_mean();
		mean.add(blMean);
		if (baselineInfo.mean.get_n() > 1) {
		  blSigma = std::sqrt(baselineInfo.mean.get_variance());
		}
	      }
	      fOmBaselineOut << iPeriod << ' ' << blMean << ' ' << blSigma << ' ' << baselineInfo.mean.get_n() << '\n';
	      periodCount++;
	      if (periodCount == omInfo.baseline_per_period.size()) break;
	    }
	    if (mean.get_n() > 0) {
	      omInfo.mean_baseline = mean.get_mean();
	      if (mean.get_n() > 1) {
		omInfo.sigma_baseline = std::sqrt(mean.get_variance(true));
	      }
	    }
	  }
	  fOmBaselineOut.close();
	} else {
	  std::cerr << "[warning] no baseline info for OM #" << omInfo.om_num << '\n';
	}
      }
    }
    static const double minProb = 1e-6;
    for (auto & omInfo : _om_infos_) {
      if (omInfo.counts_per_period.size() > 1) {
	auto periodCount = 1u;
	for (const auto & [iPeriod,counts] : omInfo.counts_per_period) {
	  poisson_pdf pdf(omInfo.mean_counts);
	  double prob = pdf(counts);
	  if (prob < minProb) {
	    omInfo.unstable_rate = true;
	  }
	  periodCount++;
	  if (periodCount == omInfo.counts_per_period.size()) break;
	}
      }
    }
  
    std::ofstream fOmOutPath(_out_path_);
    print(fOmOutPath);
    fOmOutPath.close();
     
    return;
  }

  void FlUddChannelMonitoring::print(std::ostream & out_) const
  {
    for (const auto & omInfo : _om_infos_) {
      out_ << omInfo.om_num
	   << ';' << omInfo.gid
	   << ';' << omInfo.lto_counts
	   << ';' << omInfo.lt_counts
	   << ';' << omInfo.ht_counts
	   << ';' << omInfo.lt_triggered
	   << ';' << std::max<int>(0, omInfo.counts_per_period.size() - 1)
	   << ';' << omInfo.mean_counts
	   << ';' << omInfo.sigma_counts
	   << ';' << omInfo.unstable_rate
	   << ';' << omInfo.mean_baseline
	   << ';' << omInfo.sigma_baseline
	   << ';' << omInfo.unstable_baseline
	   << '\n';
    }
    return;
  }
    
}
