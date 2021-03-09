// Interface from Falaise
#include "falaise/snemo/processing/filter.h"

class MyFilter {
 public:
  MyFilter() = default;

  MyFilter(falaise::property_set const& /*ps*/, datatools::service_manager& /*services*/){}

  bool filter(const datatools::things& /*workItem*/) {
    bool isOddEvent = (counter_ % 2 != 0);
    counter_++;
    return isOddEvent;
  }

  private:
    size_t counter_ = 0;
};

FALAISE_REGISTER_FILTER(MyFilter)
