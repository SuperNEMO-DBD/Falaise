// Ourselves
#include "FlUddScan.hh"

// Bayeux:
#include "bayeux/datatools/logger.h"

// This Project
#include <falaise/snemo/datamodels/calorimeter_digitized_hit.h>
#include <falaise/snemo/datamodels/tracker_digitized_hit.h>
#include <falaise/snemo/datamodels/udd_utils.h>
#include <falaise/snemo/datamodels/unified_digitized_data.h>
#include <falaise/metadata_utils.h>
#include <falaise/snemo/datamodels/event_header.h>

namespace FLUddStat {

  void FLUddScanner::process(const datatools::things & event_)
  {
    DT_THROW_IF(not event_.has("EH"), std::logic_error, "Missing event header");
    DT_THROW_IF(not event_.has("UDD"), std::logic_error, "Missing UDD bank");

    const snemo::datamodel::event_header & EH
	= event_.get<snemo::datamodel::event_header>("EH");
    const datatools::event_id & ehID = EH.get_id();
    auto ehRunID = ehID.get_run_number();
    auto ehEventID = ehID.get_event_number();
    DT_LOG_DEBUG(verbosity, "EH run ID   = " << ehRunID);
    DT_LOG_DEBUG(verbosity, "EH event ID = " << ehEventID);
    
    const snemo::datamodel::unified_digitized_data & UDD
	= event_.get<snemo::datamodel::unified_digitized_data>("UDD");
    int32_t uddRunId = UDD.get_run_id();
    int32_t uddEventId = UDD.get_event_id();
    DT_LOG_DEBUG(verbosity, "UDD run ID   = " << uddRunId);
    DT_LOG_DEBUG(verbosity, "UDD event ID = " << uddEventId);

    DT_THROW_IF(ehRunID != uddRunId, std::logic_error, "EH vs UDD run ID issue");
    DT_THROW_IF(ehEventID != uddEventId, std::logic_error, "EH vs UDD event ID issue");

    const std::set<int32_t> & uddTriggerIds = UDD.get_origin_trigger_ids();
    auto nbTriggerIds = uddTriggerIds.size();
    // if (nbTriggerIds > 4) {
    //   DT_THROW(std::logic_error, "Many trigger IDs : " << nbTriggerIds);
    // }
    DT_LOG_DEBUG(verbosity, "#UDD trigger IDs = " << nbTriggerIds);
    for (const auto triggerId : uddTriggerIds) {
      DT_LOG_DEBUG(verbosity, " - UDD trigger ID = " << triggerId);
    }
    const snemo::datamodel::timestamp & ehTs = EH.get_timestamp();
    DT_LOG_DEBUG(verbosity, "EH timestamp = " << ehTs);
    int64_t uddRefTs = UDD.get_reference_timestamp(); // * CLOCK_TICK_NS;
    DT_LOG_DEBUG(verbosity, "UDD reference timestamp = " << uddRefTs);

    if (eventCounter == 0) {
      firstEventTimeStamp = ehTs;
      runRefTs = uddRefTs;
      DT_LOG_DEBUG(verbosity, "UDD run reference timestamp = " << runRefTs);
    }
    int64_t uddRelTs = (UDD.get_reference_timestamp() - runRefTs) * CLOCK_TICK_NS;
    DT_LOG_DEBUG(verbosity, "UDD relative timestamp = " << uddRelTs);

    // if (uddRefTs < lastRefTs) {
    //   DT_THROW(std::logic_error, "uddRefTs = " << uddRefTs << " < lastRefTs = " << lastRefTs);
    // }

    // Event increment:
    eventCounter++;
    lastRefTs = uddRefTs;
    return;
  }
 
} // FLUddStat

