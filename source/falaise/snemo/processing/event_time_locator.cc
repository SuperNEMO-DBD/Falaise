/// \file falaise/snemo/processing/event_time_locator.cc

// Ourselves:
#include <falaise/snemo/processing/event_time_locator.h>

// Bayeux:
#include <bayeux/datatools/exception.h>

namespace snemo {
  namespace processing {
     
    time::time_point event_time_locator::get_time(const datamodel::event_header & eh_) const
    {
      if (eh_.is_real()) {
	return eh_.get_timestamp().to_timepoint();
      }
      DT_THROW_IF(not eh_.has_mc_timestamp(), std::logic_error,
		  "Simulated data has no MC timestamps. Consider to run the SimRC module on the simulated data sample this event belongs to.");
      return eh_.get_mc_timestamp();
    }

    rc::run_id_type event_time_locator::get_run_id(const datamodel::event_header & eh_) const
    {
      if (eh_.is_real()) {
	return eh_.get_id().get_run_number();
      }
      DT_THROW_IF(not eh_.has_mc_run_id(), std::logic_error,
		  "Simulated data has no MC run ID. Consider to run the SimRC module on the simulated data sample this event belongs to.");
      return eh_.get_mc_run_id();
    }
    
  } // end of namespace processing
} // end of namespace snemo
