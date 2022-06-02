// Ourselves
#include "app_params.hpp"

namespace FLSimRC {

  // static
  app_params app_params::make_default()
  {
    app_params acl;
    acl.log_level = datatools::logger::PRIO_FATAL;
    acl.activity_model_factory_conf_path.clear();
    acl.activity_model_name.clear();
    acl.run_list_path.clear();
    acl.number_of_mc_events = 0;
    acl.event_timestamps_path.clear();
    acl.timestamp_generation_mode = "regular";
    acl.timestamp_generation_random_seed = 0;
    return acl;
  }

} // namespace FLSimRC

