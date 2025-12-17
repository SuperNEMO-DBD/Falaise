// -*- mode: c++ ; -*-
//// \file falaise/snemo/processing/event_time_locator.h

#ifndef FALAISE_SNEMO_PROCESSING_EVENT_TIME_LOCATOR_H
#define FALAISE_SNEMO_PROCESSING_EVENT_TIME_LOCATOR_H

// This project:
#include <falaise/snemo/time/time_utils.h>
#include <falaise/snemo/datamodels/event_header.h>
#include <falaise/snemo/rc/run_utils.h>

namespace snemo {
  namespace processing {

    /// \brief Extract 
    class event_time_locator
    {
    public:

      event_time_locator() = default;
      
      time::time_point get_time(const datamodel::event_header & eh_) const;

      rc::run_id_type get_run_id(const datamodel::event_header & eh_) const;
      
    private:

      
    };

  } // end of namespace processing
} // end of namespace snemo

#endif  // FALAISE_SNEMO_PROCESSING_EVENT_TIME_LOCATOR_H
