#ifndef FLUDDSCAN_HH
#define FLUDDSCAN_HH

// Standard Library
#include <string>
#include <cstdint>

// Bayeux:
#include "bayeux/datatools/things.h"

// This Project
#include <falaise/snemo/datamodels/timestamp.h>

namespace FLUddStat {
  
  struct FLUddScanner
  {
  public:
    
    static const uint32_t CLOCK_TICK_NS = 25;
    
    FLUddScanner() = default;

    void process(const datatools::things & event_);

  public:

    datatools::logger::priority verbosity = datatools::logger::PRIO_FATAL;
    uint32_t eventCounter = 0;
    snemo::datamodel::timestamp firstEventTimeStamp;
    int64_t runRefTs = 0;
    int64_t lastRefTs = 0;
    
  };

} // FLUddStat

#endif // FLUDDSCAN_HH
