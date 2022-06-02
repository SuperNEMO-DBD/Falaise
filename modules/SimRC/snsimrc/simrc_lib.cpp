#include "simrc_lib.hpp"

// Third party:
// - Bayeux/datatools:
#include <datatools/logger.h>
#include <datatools/kernel.h>
#include <datatools/library_info.h>

// Falaise:
#include "falaise/resource.h"

namespace snsimrc {

  void simrc_initialize()
  {
    datatools::kernel & krnl = datatools::kernel::instance();
    // krnl.tree_dump(std::cerr, "Datatools' kernel", "[DEVEL] ");
    datatools::library_info & lib_info_reg = krnl.grab_library_info_register();
    DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE,
                  "Registration of the SimRC plugin");
    datatools::properties & flsimrc_lib_infos =
      lib_info_reg.registration("flsimrc",
                                "Falaise Simulation Running Condition Module.");
    flsimrc_lib_infos.store_string(datatools::library_info::keys::install_resource_dir(),
                                   falaise::get_resource_dir() + '/' + "modules/SimRC");
    return;
  }
 
  void simrc_terminate()
  {
    datatools::kernel & krnl = datatools::kernel::instance();
    if (krnl.has_library_info_register()) {
      DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE,
                    "Unregistration of the SimRC plugin");
      datatools::library_info & lib_info_reg = krnl.grab_library_info_register();
      if (lib_info_reg.has("flsimrc")) {
        lib_info_reg.unregistration("flsimrc");
      }
    }
    return;
  }

}  // end of namespace snsimrc
