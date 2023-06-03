// - Implementation of UddAnalysis

// Ourselves
#include "UddAnalysis.hpp"

#include <iostream>

// - Falaise
#include "falaise/snemo/datamodels/event_header.h"
#include "falaise/snemo/datamodels/unified_digitized_data.h"

DPP_MODULE_REGISTRATION_IMPLEMENT(UddAnalysis, "UddAnalysis");

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
  this->_set_initialized(false);
  _print_udd_data_ = false;
  return;
}

// Process
dpp::base_module::process_status UddAnalysis::process(datatools::things & event_record_)
{
  // Check if the event record hosts a "UDD" bank :
  if (not event_record_.has("UDD")) {
    std::cerr << "No UDD bank found in this event" << '\n';
  } else {

    // Access to the "UDD" bank contents as a reference to
    // a non mutable "snemo::datamodel::unified_digitized_data" instance:
    const snemo::datamodel::unified_digitized_data & UDD
      = event_record_.get<snemo::datamodel::unified_digitized_data>("UDD");    
    
    // Extract raw run ID, if any:
    int32_t runId = -1;
    if (UDD.has_run_id()) {
      runId = UDD.get_run_id();
    }
 
    // Extract raw event ID, if any:
    int32_t eventId = -1;
    if (UDD.has_event_id()) {
      eventId = UDD.get_event_id();
    }
  
    // Extract reference timestamp (in clock tick unit), if any:
    int64_t refTimestamp = -1;
    if (UDD.has_reference_timestamp()) {
      refTimestamp = UDD.get_reference_timestamp();
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
      
      int64_t timestamp = caloDigiHitPtr->get_timestamp();
      bool low_threshold_only = caloDigiHitPtr->is_low_threshold_only();
      bool high_threshold = caloDigiHitPtr->is_high_threshold();
      uint16_t lt_trigger_counter = caloDigiHitPtr->get_lt_trigger_counter();
      uint16_t lt_time_counter = caloDigiHitPtr->get_lt_time_counter();
      std::size_t nbOfWaveformSamples = caloDigiHitPtr->get_waveform().size();
      
      if (_print_udd_data_) {
	std::clog << "    Hit #" << caloHitCount << '\n';
	std::clog << "      Hit ID = " << hitId << '\n';
	std::clog << "      Geometry ID = " << geomId << '\n';
	std::clog << "      Timestamp = " << timestamp << " (@40 MHz)" << '\n';
	std::clog << "      Low threshold only = " << std::boolalpha << low_threshold_only << '\n';
	std::clog << "      High threshold = " << std::boolalpha << high_threshold << '\n';
	std::clog << "      LT trigger counter = " << lt_trigger_counter << '\n';
	std::clog << "      LT time counter = " << lt_time_counter << '\n';
	std::clog << "      Number of waveform samples = " << nbOfWaveformSamples << '\n';
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
      
      // Geiger times:
      std::size_t nbTimes = trackerDigiHitPtr->get_times().size();
      if (_print_udd_data_) std::clog << "      Number of records of Geiger times = " << nbTimes << '\n';
      std::size_t ggTimesCount = 0;
      for (const snemo::datamodel::tracker_digitized_hit::gg_times & ggTimes : trackerDigiHitPtr->get_times()) {
	if (_print_udd_data_) std::clog << "        Time record #" << ggTimesCount << '\n';
	bool hasTimes = false;
	if (ggTimes.has_anode_time(snemo::datamodel::tracker_digitized_hit::ANODE_R0)) {
	  hasTimes = true;
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
    
  } // has UDD
  
  return dpp::base_module::PROCESS_OK;
}
