// Ourselves
#include "FLSimulateUtils.h"

// Third party:
// - Bayeux:
#include "bayeux/datatools/kernel.h"
#include "bayeux/datatools/urn_query_service.h"

// This Project:
#include "falaise/detail/falaise_sys.h"

namespace FLSimulate {

std::string default_simulation_setup() { return "urn:snemo:demonstrator:simulation:2.1"; }

std::map<std::string, std::string> list_of_simulation_setups() {
  std::map<std::string, std::string> m;
  datatools::logger::priority logging =
      falaise::detail::falaise_sys::const_instance().get_logging();
  datatools::kernel& dtk = ::datatools::kernel::instance();
  if (dtk.has_urn_query()) {
    const datatools::urn_query_service& dtkUrnQuery = dtk.get_urn_query();
    if (datatools::logger::is_debug(logging)) {
      dtkUrnQuery.tree_dump(std::cerr, "Bayeux/datatools's kernel URN query service:", "[debug] ");
    }
    std::vector<std::string> flsim_urn_infos;
    if (dtkUrnQuery.find_urn_info(flsim_urn_infos, falaise::detail::falaise_sys::fl_setup_db_name(),
                                  // Format : "urn:{namespace}:{experiment}:simulation:{version}"
                                  "(urn:)([^:]*)(:)([^:]*)(:simulation:)([^:]*)", "simsetup")) {
      for (size_t i = 0; i < flsim_urn_infos.size(); i++) {
        const datatools::urn_info& ui = dtkUrnQuery.get_urn_info(flsim_urn_infos[i]);
        m[ui.get_urn()] = ui.get_description();
      }
    }
  }
  return m;
}

}  // end of namespace FLSimulate
