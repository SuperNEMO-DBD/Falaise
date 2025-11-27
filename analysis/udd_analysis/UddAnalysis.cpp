// - Implementation of UddAnalysis

// Ourselves
#include "UddAnalysis.hpp"

#include <iostream>
#include <fstream>


#include <bayeux/mygsl/mean.h>

// - Falaise
#include "falaise/snemo/datamodels/event_header.h"
#include "falaise/snemo/datamodels/unified_digitized_data.h"
#include "falaise/snemo/datamodels/geomid_utils.h"
#include "falaise/snemo/rc/calorimeter_om_status.h"

DPP_MODULE_REGISTRATION_IMPLEMENT(UddAnalysis, "UddAnalysis")

void UddAnalysis::om_record::print(std::ostream & out_) const
{
  out_ << "OM record:\n";
  out_ << " - OM ID  = " << om_id << '\n';
  out_ << " - OM GID  = " << snemo::datamodel::om_gid(om_id) << '\n';
  out_ << " - status = " << status << '\n';
  out_ << " - ncounts    = " << ncounts << '\n';
  out_ << " - ncountsHT  = " << ncountsHT << '\n';
  out_ << " - ncountsLTO = " << ncountsLTO << '\n';
  out_ << " - ncountsLT  = " << (ncountsHT+ncountsLTO) << '\n';
  out_ << " - ncountsNoT = " << ncountsNoT << '\n';
  return;
}

void UddAnalysis::om_record::print_csv(std::ostream & out_) const
{
  out_ << om_id
       << ' ' << snemo::datamodel::om_gid(om_id)
       << ' ' << status
       << ' ' << ncounts
       << ' ' << ncountsHT
       << ' ' << ncountsLTO
       << ' ' << ncountsNoT
       << '\n';
  return;
}

void UddAnalysis::gg_record::print_csv(std::ostream & out_) const
{
  out_ << gg_id
       << ' ' << snemo::datamodel::gg_gid(gg_id)
       << ' ' << status
       << ' ' << ncountsR0
       << ' ' << ncountsR1
       << ' ' << ncountsR2
       << ' ' << ncountsR3
       << ' ' << ncountsR4
       << ' ' << ncountsR5
       << ' ' << ncountsR6
       << '\n';
  return;
}

UddAnalysis::stats_data::stats_data()
{
  for (const auto& kv : snemo::datamodel::om_map()) {
    om_record omRecord;
    omRecord.om_id = kv.first;
    omRecord.status = 0u;
    om_records[kv.first] = omRecord;
  }
  for (const auto& kv : snemo::datamodel::gg_map()) {
    gg_record ggRecord;
    ggRecord.gg_id = kv.first;
    ggRecord.status = 0u;
    gg_records[kv.first] = ggRecord;
  }
  return;
}

UddAnalysis::stats_data::~stats_data()
{
  // for (const auto& kv : om_records) {
  //   std::clog << "OM: " << kv.first << " : " << kv.second.status << " #" << kv.second.ncounts << '\n';
  // } 
  return;
}

void udd_analysis_init() {  
  std::cerr << "Loading UddAnalysis..." << std::endl;
}

void udd_analysis_fini() {  
  std::cerr << "Unloading UddAnalysis..." << std::endl;
}

// Construct
UddAnalysis::UddAnalysis()
  : dpp::base_module()
{
}

// Destruct
UddAnalysis::~UddAnalysis()
{
  // MUST reset module at destruction
  if (this->is_initialized()) {
    this->UddAnalysis::reset();
  }
  return;
}

// Initialize
void UddAnalysis::initialize(const datatools::properties & config_,
                             datatools::service_manager & /*services_*/,
                             dpp::module_handle_dict_type & /*modules_*/)
{
  // Throw logic exception if we've already initialized this instance
  DT_THROW_IF(this->is_initialized(), std::logic_error, "UddAnalysis already initialized");

  if (config_.has_flag("print_udd_data")) {
    _print_udd_data_ = true;
  }
  
  this->_set_initialized(true);
  return;
}
 
// Reset
void UddAnalysis::reset() {
  // Throw logic exception if we've not initialized this instance
  DT_THROW_IF(!this->is_initialized(), std::logic_error, "UddAnalysis not initialized");
  this->build_report();
  this->print_report();
  this->_set_initialized(false);
  _print_udd_data_ = false;
  return;
}

// Process
dpp::base_module::process_status UddAnalysis::process(datatools::things & event_record_)
{
  datatools::logger::priority verbosity = datatools::logger::PRIO_FATAL;
  // verbosity = datatools::logger::PRIO_DEBUG;

  // Event header bank:
  DT_THROW_IF(not event_record_.has("EH"), std::logic_error, "Missing event header");
  const snemo::datamodel::event_header & EH
    = event_record_.get<snemo::datamodel::event_header>("EH");
  const datatools::event_id & ehID = EH.get_id();
  auto ehRunID = ehID.get_run_number();
  auto ehEventID = ehID.get_event_number();
  DT_LOG_DEBUG(verbosity, "EH run ID   = " << ehRunID);
  DT_LOG_DEBUG(verbosity, "EH event ID = " << ehEventID);

  // UDD bank:
  DT_THROW_IF(not event_record_.has("UDD"), std::logic_error, "Missing UDD bank"); 
  // Access to the "UDD" bank contents as a reference to
  // a non mutable "snemo::datamodel::unified_digitized_data" instance:
  const snemo::datamodel::unified_digitized_data & UDD
    = event_record_.get<snemo::datamodel::unified_digitized_data>("UDD");
  int32_t uddRunId = UDD.get_run_id();
  int32_t uddEventId = UDD.get_event_id();
  DT_LOG_DEBUG(verbosity, "UDD run ID   = " << uddRunId);
  DT_LOG_DEBUG(verbosity, "UDD event ID = " << uddEventId);
  
  DT_THROW_IF(ehRunID != uddRunId, std::logic_error, "EH vs UDD run ID issue");
  DT_THROW_IF(ehEventID != uddEventId, std::logic_error, "EH vs UDD event ID issue");

  // Extract trigger IDs:
  const std::set<int32_t> & uddTriggerIds = UDD.get_origin_trigger_ids();
  auto nbTriggerIds = uddTriggerIds.size();
  DT_LOG_DEBUG(verbosity, "#UDD trigger IDs = " << nbTriggerIds);
  for (const auto triggerId : uddTriggerIds) {
    DT_LOG_DEBUG(verbosity, " - UDD trigger ID = " << triggerId);
  }

  const snemo::datamodel::timestamp & ehTs = EH.get_timestamp();
  DT_LOG_DEBUG(verbosity, "EH timestamp = " << ehTs);
  int64_t uddRefTs = UDD.get_reference_timestamp(); 
  DT_LOG_DEBUG(verbosity, "UDD reference timestamp = " << uddRefTs);
  if (_stats_.eventCounter == 0) {
    _stats_.firstEventTimeStamp = ehTs;
    _stats_.runRefTs = uddRefTs;
    DT_LOG_DEBUG(verbosity, "UDD first event timestamp = " << _stats_.firstEventTimeStamp);
    DT_LOG_DEBUG(verbosity, "UDD run reference timestamp = " << _stats_.runRefTs);
  }
  int64_t uddRelTs = (UDD.get_reference_timestamp() - _stats_.runRefTs) * CLOCK_TICK_NS;
  DT_LOG_DEBUG(verbosity, "UDD relative timestamp = " << uddRelTs);
  int32_t runId = uddRunId;
  int32_t eventId = uddEventId;
  // Extract reference timestamp (in clock tick unit), if any:
  int64_t refTimestamp = -1;
  if (UDD.has_reference_timestamp()) {
    refTimestamp = uddRefTs;
  }
  
  // Extract the number of calorimeter hits:
  std::size_t nbCaloDigiHits = UDD.get_calorimeter_hits().size();
    
  // Extract the number of tracker hits:
  std::size_t nbTrackerDigiHits = UDD.get_tracker_hits().size();

  // Extract the number of trigger ID collected within this digitized event record:
  std::size_t nbOriginTriggerIds = UDD.get_origin_trigger_ids().size();

  if (_print_udd_data_) {
    // General informations:
    std::clog << "This event record's UDD :" << '\n';
    std::clog << "  Run ID = " << runId << '\n';
    std::clog << "  Event ID = " << eventId << '\n';
    std::clog << "  Reference timestamp = " << refTimestamp << " clock ticks" << '\n';
    std::clog << "  Number of calorimeter digi hits = " << nbCaloDigiHits << '\n';
    std::clog << "  Number of tracker digi hits = " << nbTrackerDigiHits << '\n';
    std::clog << "  Number of trigger IDs = " << nbOriginTriggerIds << '\n';

    // Details on calorimeter digitized hits:
    std::clog << "  Calorimeter digitized hits : " << '\n';
  }
    
  std::size_t caloHitCount = 0;
  for (const auto & caloDigiHitPtr : UDD.get_calorimeter_hits()) {
    // Hit ID:
    int32_t hitId = -1;
    if (caloDigiHitPtr->has_hit_id()) {
      hitId = caloDigiHitPtr->get_hit_id();
    }
    // Geometry ID:
    geomtools::geom_id geomId;
    if (caloDigiHitPtr->has_geom_id()) {
      geomId = caloDigiHitPtr->get_geom_id();
    }

    int omId = snemo::datamodel::om_num(geomId);
    int64_t timestamp = caloDigiHitPtr->get_timestamp();
    bool low_threshold_only = caloDigiHitPtr->is_low_threshold_only();
    bool high_threshold = caloDigiHitPtr->is_high_threshold();
    uint16_t lt_trigger_counter = caloDigiHitPtr->get_lt_trigger_counter();
    uint16_t lt_time_counter = caloDigiHitPtr->get_lt_time_counter();
    std::size_t nbOfWaveformSamples = caloDigiHitPtr->get_waveform().size();
    auto & omRec = _stats_.om_records[omId];
    if (high_threshold) {
      omRec.ncountsHT++;
    }
    if (low_threshold_only) {
      omRec.ncountsLTO++;
    }
    if (not high_threshold and not low_threshold_only) {
      omRec.ncountsNoT++;
    }
    omRec.ncounts++;
     
    if (_print_udd_data_) {
      std::clog << "    Hit #" << caloHitCount << '\n';
      std::clog << "      Hit ID = " << hitId << '\n';
      std::clog << "      Geometry ID = " << geomId << " (omID=" << omId << ')' << '\n';
      std::clog << "      Timestamp = " << timestamp << " (@40 MHz)" << '\n';
      std::clog << "      Low threshold only = " << std::boolalpha << low_threshold_only << '\n';
      std::clog << "      High threshold = " << std::boolalpha << high_threshold << '\n';
      std::clog << "      LT trigger counter = " << lt_trigger_counter << '\n';
      std::clog << "      LT time counter = " << lt_time_counter << '\n';
      std::clog << "      Number of waveform samples = " << nbOfWaveformSamples << '\n';
      std::clog << "      OM count = " << omRec.ncounts << '\n';
      std::clog << "      Firmware measurements (optional): " << '\n';
    }
      
    bool hasSomeFirmwareMmeasurements = false;
    if (caloDigiHitPtr->has_fwmeas_baseline()) {
      hasSomeFirmwareMmeasurements = true;	
      if (_print_udd_data_) std::clog << "        Baseline =  " << caloDigiHitPtr->get_fwmeas_baseline() << '\n';
    }
    if (caloDigiHitPtr->has_fwmeas_peak_amplitude()) {
      hasSomeFirmwareMmeasurements = true;	
      if (_print_udd_data_) std::clog << "        Peak amplitude = " << caloDigiHitPtr->get_fwmeas_peak_amplitude() << '\n';	
    }
    if (caloDigiHitPtr->has_fwmeas_peak_cell()) {
      hasSomeFirmwareMmeasurements = true;	
      if (_print_udd_data_) std::clog << "        Peak cell = " << caloDigiHitPtr->get_fwmeas_peak_cell() << '\n';	
    }
    if (caloDigiHitPtr->has_fwmeas_charge()) {
      hasSomeFirmwareMmeasurements = true;	
      if (_print_udd_data_) std::clog << "        Charge = " << caloDigiHitPtr->get_fwmeas_charge() << '\n';	
    }
    if (caloDigiHitPtr->has_fwmeas_rising_cell()) {
      hasSomeFirmwareMmeasurements = true;	
      if (_print_udd_data_) std::clog << "        Rising cell = " << caloDigiHitPtr->get_fwmeas_rising_cell() << '\n';	
    }
    if (caloDigiHitPtr->has_fwmeas_falling_cell()) {
      hasSomeFirmwareMmeasurements = true;	
      if (_print_udd_data_) std::clog << "        Falling cell = " << caloDigiHitPtr->get_fwmeas_falling_cell() << '\n';	
    }
    if (not hasSomeFirmwareMmeasurements) {
      if (_print_udd_data_) std::clog << "        none available" << '\n';	
    }
    if (caloDigiHitPtr->has_origin()) {
      const snemo::datamodel::calorimeter_digitized_hit::rtd_origin & rtdOrigin
	= caloDigiHitPtr->get_origin();
      if (_print_udd_data_) std::clog << "      RTD origin = " << rtdOrigin.get_hit_number() << '/' << rtdOrigin.get_trigger_id() << '\n';
    }
    caloHitCount++;
  }

  // Details on tracker digitized hits:
  if (_print_udd_data_) std::clog << "  Tracker digitized hits : " << '\n';
  std::size_t trackerHitCount = 0;
  for (const auto & trackerDigiHitPtr : UDD.get_tracker_hits()) {
    // Hit ID:
    int32_t hitId = -1;
    if (trackerDigiHitPtr->has_hit_id()) {
      hitId = trackerDigiHitPtr->get_hit_id();
    }
    // Geometry ID:
    geomtools::geom_id geomId;
    if (trackerDigiHitPtr->has_geom_id()) {
      geomId = trackerDigiHitPtr->get_geom_id();
    }
    if (_print_udd_data_) {      
      std::clog << "    Hit #" << trackerHitCount << '\n';
      std::clog << "      Hit ID = " << hitId << '\n';
      std::clog << "      Geometry ID = " << geomId << '\n';
    }
    int ggId = snemo::datamodel::gg_num(geomId);
    auto & ggRec = _stats_.gg_records[ggId];
    
    // Geiger times:
    std::size_t nbTimes = trackerDigiHitPtr->get_times().size();
    if (_print_udd_data_) std::clog << "      Number of records of Geiger times = " << nbTimes << '\n';
    std::size_t ggTimesCount = 0;
    std::uint32_t hitBits = 0u;
    for (const snemo::datamodel::tracker_digitized_hit::gg_times & ggTimes : trackerDigiHitPtr->get_times()) {
      if (_print_udd_data_) std::clog << "        Time record #" << ggTimesCount << '\n';
      bool hasTimes = false;
      if (ggTimes.has_anode_time(snemo::datamodel::tracker_digitized_hit::ANODE_R0)) {
	hasTimes = true;
	ggRec.ncountsR0++;
	hitBits |= datatools::bit_mask::bit00;
	if (_print_udd_data_) {
	  std::clog << "        Anode R0 = " << ggTimes.get_anode_time(snemo::datamodel::tracker_digitized_hit::ANODE_R0);
	  if (ggTimes.has_anode_origin(snemo::datamodel::tracker_digitized_hit::ANODE_R0)) {
	    const auto & rtdOrigin = ggTimes.get_anode_origin(snemo::datamodel::tracker_digitized_hit::ANODE_R0);
	    std::clog << " (origin=" << rtdOrigin.get_hit_number() << '/' << rtdOrigin.get_trigger_id() << ')';
	  }
	  std::clog << '\n';
	}
      }
      if (ggTimes.has_anode_time(snemo::datamodel::tracker_digitized_hit::ANODE_R1)) {
	hasTimes = true;
	hitBits |= datatools::bit_mask::bit01;
	ggRec.ncountsR1++;

	if (_print_udd_data_) {
	  std::clog << "        Anode R1 = " << ggTimes.get_anode_time(snemo::datamodel::tracker_digitized_hit::ANODE_R1);
	  if (ggTimes.has_anode_origin(snemo::datamodel::tracker_digitized_hit::ANODE_R1)) {
	    const auto & rtdOrigin = ggTimes.get_anode_origin(snemo::datamodel::tracker_digitized_hit::ANODE_R1);
	    std::clog << " (origin=" << rtdOrigin.get_hit_number() << '/' << rtdOrigin.get_trigger_id() << ')';
	  }
	  std::clog << '\n';
	}
      }
      if (ggTimes.has_anode_time(snemo::datamodel::tracker_digitized_hit::ANODE_R2)) {
	hasTimes = true;
	hitBits |= datatools::bit_mask::bit02;
	ggRec.ncountsR2++;
	if (_print_udd_data_) {
	  std::clog << "        Anode R2 = " << ggTimes.get_anode_time(snemo::datamodel::tracker_digitized_hit::ANODE_R2);
	  if (ggTimes.has_anode_origin(snemo::datamodel::tracker_digitized_hit::ANODE_R2)) {
	    const auto & rtdOrigin = ggTimes.get_anode_origin(snemo::datamodel::tracker_digitized_hit::ANODE_R2);
	    std::clog << " (origin=" << rtdOrigin.get_hit_number() << '/' << rtdOrigin.get_trigger_id() << ')';
	  }
	  std::clog << '\n';
	}
      }
      if (ggTimes.has_anode_time(snemo::datamodel::tracker_digitized_hit::ANODE_R3)) {
	hasTimes = true;
	hitBits |= datatools::bit_mask::bit03;
	ggRec.ncountsR3++;
	if (_print_udd_data_) {
	  std::clog << "        Anode R3 = " << ggTimes.get_anode_time(snemo::datamodel::tracker_digitized_hit::ANODE_R3);
	  if (ggTimes.has_anode_origin(snemo::datamodel::tracker_digitized_hit::ANODE_R3)) {
	    const auto & rtdOrigin = ggTimes.get_anode_origin(snemo::datamodel::tracker_digitized_hit::ANODE_R3);
	    std::clog << " (origin=" << rtdOrigin.get_hit_number() << '/' << rtdOrigin.get_trigger_id() << ')';
	  }
	  std::clog << '\n';
	}
      }
      if (ggTimes.has_anode_time(snemo::datamodel::tracker_digitized_hit::ANODE_R4)) {
	hasTimes = true;
	hitBits |= datatools::bit_mask::bit04;
	ggRec.ncountsR4++;
	if (_print_udd_data_) {
	  std::clog << "        Anode R4 = " << ggTimes.get_anode_time(snemo::datamodel::tracker_digitized_hit::ANODE_R4);
	  if (ggTimes.has_anode_origin(snemo::datamodel::tracker_digitized_hit::ANODE_R4)) {
	    const auto & rtdOrigin = ggTimes.get_anode_origin(snemo::datamodel::tracker_digitized_hit::ANODE_R4);
	    std::clog << " (origin=" << rtdOrigin.get_hit_number() << '/' << rtdOrigin.get_trigger_id() << ')';
	  }
	  std::clog << '\n';
	}
      }
      if (ggTimes.has_bottom_cathode_time()) {
	hasTimes = true;
	hitBits |= datatools::bit_mask::bit05;
	ggRec.ncountsR5++;
	if (_print_udd_data_) {
	  std::clog << "        Cathode bottom = " << ggTimes.get_bottom_cathode_time();
	  if (ggTimes.has_bottom_cathode_origin()) {
	    const auto & rtdOrigin = ggTimes.get_bottom_cathode_origin();
	    std::clog << " (origin=" << rtdOrigin.get_hit_number() << '/' << rtdOrigin.get_trigger_id() << ')';
	  }
	  std::clog << '\n';
	}
      }
      if (ggTimes.has_top_cathode_time()) {
	hasTimes = true;
	hitBits |= datatools::bit_mask::bit06;
	ggRec.ncountsR6++;
	if (_print_udd_data_) {
	  std::clog << "        Cathode top    = " << ggTimes.get_top_cathode_time();
	  if (ggTimes.has_top_cathode_origin()) {
	    const auto & rtdOrigin = ggTimes.get_top_cathode_origin();
	    std::clog << " (origin=" << rtdOrigin.get_hit_number() << '/' << rtdOrigin.get_trigger_id() << ')';
	  }
	  std::clog << '\n';
	}
      }
      if (not hasTimes) {
	if (_print_udd_data_) std::clog << "        no time available" << '\n';
      }
      ggTimesCount++;
    }
    trackerHitCount++;
  }

  // Details on origin trigger IDs:
  if (_print_udd_data_) {
    std::clog << "  Origin trigger IDs : " << '\n';
    for (const auto & triggerId : UDD.get_origin_trigger_ids()) {
      std::clog << "    Trigger ID = " << triggerId << '\n';
    }
  }
    
  // Event increment:
  _stats_.eventCounter++;
  _stats_.lastRefTs = uddRefTs;
  
  return dpp::base_module::PROCESS_OK;
}

void UddAnalysis::build_report_oms()
{
  mygsl::arithmetic_mean statNbCounts;
  mygsl::arithmetic_mean statNbCountsLT;
  mygsl::arithmetic_mean statNbCountsNoT;
  for (auto & omRec : _stats_.om_records) {
    auto omId = omRec.first;
    geomtools::geom_id omGid = snemo::datamodel::om_gid(omId);
    auto omCounts = omRec.second.ncounts;
    if (omCounts == 0) {
      omRec.second.status += snemo::rc::calorimeter_om_status::OM_OFF;
    }
    auto omCountsHT  = omRec.second.ncountsHT;
    auto omCountsLTO = omRec.second.ncountsLTO;
    auto omCountsLT  = omCountsHT + omCountsLTO;
    auto omCountsNoT = omRec.second.ncountsNoT;
    if (not snemo::datamodel::is_reference_om(omGid)) {
      if (omCounts == 0) {
	std::cerr << "0-count OM : #" << omId << " (GID=" << omGid << ")\n";
      } else {
	statNbCounts.add(omCounts);
	statNbCountsLT.add(omCountsLT);
	statNbCountsNoT.add(omCountsNoT);
      }
    }
  }
  double meanNCounts    = statNbCounts.get_mean();
  double meanNCountsLT  = statNbCountsLT.get_mean();
  double meanNCountsNoT = statNbCountsNoT.get_mean();
  double sigNCounts     = std::sqrt(statNbCounts.get_variance());
  double sigNCountsLT   = std::sqrt(statNbCountsLT.get_variance());
  double sigNCountsNoT  = std::sqrt(statNbCountsNoT.get_variance());
  std::cerr << "OM mean #counts: " << meanNCounts    << " +/- " << sigNCounts << '\n';
  std::cerr << "OM mean #counts: " << meanNCountsLT  << " +/- " << sigNCountsLT  << " [LT ]" << '\n';
  std::cerr << "OM mean #counts: " << meanNCountsNoT << " +/- " << sigNCountsNoT << " [No thres.]" << '\n';
  
  std::cerr << "OMs with high rates (LT or NoT): " << '\n';
  std::set<int> outliersOmIds;
  std::set<int> outliersOmIdsLT;
  std::set<int> outliersOmIdsNoT;
  for (const auto & omRec : _stats_.om_records) {
    auto omId = omRec.first;
    geomtools::geom_id omGid = snemo::datamodel::om_gid(omId);
    auto omCountsHT = omRec.second.ncountsHT;
    auto omCountsLTO = omRec.second.ncountsLTO;
    auto omCountsLT = omCountsHT + omCountsLTO;
    auto omCountsNoT = omRec.second.ncountsNoT;
    if (not snemo::datamodel::is_reference_om(omGid)) {
      if (omCountsLT > meanNCountsLT + 3 * sigNCountsLT) {
	std::cerr << "LT-count OM : " << omRec.first << " (GID=" << omGid << ")\n";
	outliersOmIdsLT.insert(omId);
      }
      if (omCountsNoT > meanNCountsNoT + 3 * sigNCountsNoT) {
	std::cerr << "NoT-count OM : " << omRec.first << " (GID=" << omGid << ")\n";
	outliersOmIdsNoT.insert(omId);
      }
    }
  }
 
  std::cerr << "\nOMs outliers LT: " << '\n';
  for (auto item : outliersOmIdsLT) {
    std::cerr << "  OM #" << item << " :\n";
    _stats_.om_records.find(item)->second.print(std::cerr);
  }

  std::cerr << "\nOMs outliers no-threshold: " << '\n';
  for (auto item : outliersOmIdsNoT) {
    std::cerr << "  OM #" << item << " :\n";
    _stats_.om_records.find(item)->second.print(std::cerr);
  }

  std::cerr << "\nRecompute stat: " << '\n';
  statNbCountsLT.reset();
  statNbCountsNoT.reset();
  for (const auto & omRec : _stats_.om_records) {
    auto omId = omRec.first;
    geomtools::geom_id omGid = snemo::datamodel::om_gid(omId);
    if (not snemo::datamodel::is_reference_om(omGid)) {
      auto omCountsHT  = omRec.second.ncountsHT;
      auto omCountsLTO = omRec.second.ncountsLTO;
      auto omCountsLT  = omCountsHT + omCountsLTO;
      auto omCountsNoT = omRec.second.ncountsNoT;
      if (omCountsLT > 0 and outliersOmIdsLT.count(omId) == 0) {
	statNbCountsLT.add(omCountsLT);
      }
      if (omCountsNoT > 0 and outliersOmIdsNoT.count(omId) == 0) {
	statNbCountsNoT.add(omCountsNoT);
      }
    }
  }
  meanNCountsLT  = statNbCountsLT.get_mean();
  sigNCountsLT   = std::sqrt(statNbCountsLT.get_variance());
  meanNCountsNoT = statNbCountsNoT.get_mean();
  sigNCountsNoT  = std::sqrt(statNbCountsNoT.get_variance());
  std::cerr << "OM mean #counts: " << meanNCountsLT  << " +/- " << sigNCountsLT  << " [LT ]" << '\n';
  std::cerr << "OM mean #counts: " << meanNCountsNoT << " +/- " << sigNCountsNoT << " [No thres.]" << '\n';
 
  return;
}

void UddAnalysis::build_report_ggs()
{
  return;
}

void UddAnalysis::build_report()
{
  build_report_oms();
  build_report_ggs();
  return;
}

void UddAnalysis::print_report() const
{
  {
    std::ofstream fout("om_scan.csv");
    for (const auto & omRec : _stats_.om_records) {
      omRec.second.print_csv(fout);
    }
    fout.close();
  }

  {
    std::ofstream fout("gg_scan.csv");
    for (const auto & ggRec : _stats_.gg_records) {
      ggRec.second.print_csv(fout);
    }
    fout.close();
  }

  return;
}
