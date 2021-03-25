#include "falaise/snemo/datamodels/event_header.h"
#include "falaise/snemo/processing/filter.h"
#include "falaise/snemo/processing/module.h"

namespace flp = falaise::processing;

// Always o.k. module
// Just to provide an accept branch for filtering
class YesModule {
 public:
  YesModule() = default;
  YesModule(falaise::property_set const& /*unused*/, datatools::service_manager& /*unused*/) {}

  flp::status process(datatools::things& /*unused*/) { return flp::status::PROCESS_OK; }
};
FALAISE_REGISTER_MODULE(YesModule)

// Module to do the filtering
// Simple check on event id odd/even: accept ODD event ids
class EventIDFilter {
 public:
  EventIDFilter() = default;
  EventIDFilter(falaise::property_set const& /*unused*/, datatools::service_manager& /*unused*/) {}

  bool filter(const datatools::things& data) {
    // Extract event id
    auto& eventHeader = data.get<snemo::datamodel::event_header>("EH");
    int n = eventHeader.get_id().get_event_number();
    return n % 2 != 0;
  }
};
FALAISE_REGISTER_FILTER(EventIDFilter)

// Module to check the filter was applied correctly
// Expect only odd numbered events, 5 events total
class CheckFilter {
 public:
  CheckFilter() = default;
  CheckFilter(falaise::property_set const& /*unused*/, datatools::service_manager& /*unused*/) {}

  flp::status process(datatools::things& data) {
    // Extract event id
    auto& eventHeader = data.get<snemo::datamodel::event_header>("EH");
    int n = eventHeader.get_id().get_event_number();
    count_ += 1;

    DT_THROW_IF(count_ > 5, std::logic_error, "More than 5 events in output stream");
    DT_THROW_IF(n%2 == 0, std::logic_error, "Even numbered event in output stream")
    return flp::status::PROCESS_OK;
  }

 private:
  int count_ = 0;
};
FALAISE_REGISTER_MODULE(CheckFilter)
