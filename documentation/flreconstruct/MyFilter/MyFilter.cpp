// Interface from Falaise
#include "falaise/snemo/processing/filter.h"

class MyFilter {
 public:
  MyModule() = default;

  MyFilter(falaise::property_set const& ps, datatools::service_manager& /*services*/)
      : yesOrNo_(ps.get<bool>("yes_or_no")) {}

  bool filter(datatools::things& /*workItem*/) {
    return yesOrNo_;
  }

 private:
  bool yesOrNo_ = true;
};

FALAISE_REGISTER_FILTER(MyFilter);