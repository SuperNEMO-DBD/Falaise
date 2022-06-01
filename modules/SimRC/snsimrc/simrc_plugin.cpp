// Third party:
// - Bayeux/datatools:
#include <datatools/library_info.h>
#include <datatools/kernel.h>
#include <datatools/logger.h>

// - Falaise:
#include <falaise/resource.h>

namespace {
  
  struct simrc_plugin
  {
    
    simrc_plugin()
    {
      DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE, "Initialization of the SimRC plugin");
      datatools::kernel & krnl = datatools::kernel::instance();
      datatools::library_info & lib_info_reg = krnl.grab_library_info_register();
      DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE, "Registration of the SimRC plugin");
      datatools::properties & flsimrc_lib_infos =
        lib_info_reg.registration("flsimrc",
                                  "Falaise Simulation Running Condition Module.");
      flsimrc_lib_infos.store_string(datatools::library_info::keys::install_resource_dir(),
                                     falaise::get_resource_dir() + '/' + "modules/SimRC");
      return;
    }
    ~simrc_plugin()
    {
      DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE, "Termination of the SimRC plugin");
      datatools::kernel & krnl = datatools::kernel::instance();
      if (krnl.has_library_info_register()) {
        DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE, "Unregistration of the SimRC plugin");
        datatools::library_info & lib_info_reg = krnl.grab_library_info_register();
        if (lib_info_reg.has("flsimrc")) {
          lib_info_reg.unregistration("flsimrc");
        }
      }
      return;
    }
  };
  static simrc_plugin _initPlugin_pluginModule;
}
