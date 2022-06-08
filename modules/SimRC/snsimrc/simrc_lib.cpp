#include "simrc_lib.hpp"

// Third party:
// - Bayeux/datatools:
#include <datatools/logger.h>
#include <datatools/kernel.h>
#include <datatools/library_info.h>

namespace snsimrc {

  void simrc_initialize()
  {
    datatools::kernel & krnl = datatools::kernel::instance();
    datatools::library_info & lib_info_reg = krnl.grab_library_info_register();
    datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
    DT_LOG_NOTICE(logging, "Registration of the SimRC plugin");
    datatools::properties & flsimrc_lib_infos =
      lib_info_reg.registration("flsimrc",
                                "Falaise Simulation Running Condition Module.");
    
    // Note: because of the mechanism used in binreloc, we cannot use here
    // the 'falaise::get_resource_dir()' to locate the base directory of falaise.
    // So we use the Bayeux/datatools' kernel library registry to pickup this
    // information
    
    DT_THROW_IF(not lib_info_reg.has("falaise"), std::logic_error,
                "Falaise library is not registered!");
    const datatools::properties & falaise_lib_infos = lib_info_reg.get("falaise");
    std::string falaise_resource_path;
    if (falaise_lib_infos.has_key(datatools::library_info::keys::install_resource_dir())) {
      falaise_resource_path = falaise_lib_infos.fetch_string(datatools::library_info::keys::install_resource_dir());
    }
    if (falaise_lib_infos.has_key(datatools::library_info::keys::env_resource_dir())) {
      std::string falaise_resource_env = falaise_lib_infos.fetch_string(datatools::library_info::keys::env_resource_dir());
      if (!falaise_resource_env.empty()) {
        char * c = getenv(falaise_resource_env.c_str());
        if (c != nullptr) {
          falaise_resource_path = std::string(c);
        }
      }
    }
    DT_THROW_IF(falaise_resource_path.empty(), std::logic_error,
                "Cannot locate Falaise resource directory!");
    DT_LOG_NOTICE(logging,
                  "Falaise resource directory = '" << falaise_resource_path << "'");
    std::string flsimrc_resource_path = falaise_resource_path + '/' + "modules/SimRC";
    DT_LOG_NOTICE(logging,
                  "Falaise SimRC plugin's resource directory = '" << flsimrc_resource_path << "'");
    flsimrc_lib_infos.store_string(datatools::library_info::keys::install_resource_dir(),
                                   flsimrc_resource_path);
    DT_LOG_NOTICE(logging,
                  "Registration of the SimRC plugin resource directory");
    return;
  }
 
  void simrc_terminate()
  {
    datatools::kernel & krnl = datatools::kernel::instance();
    if (krnl.has_library_info_register()) {
      datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
      DT_LOG_NOTICE(logging, "Unregistration of the SimRC plugin");
      datatools::library_info & lib_info_reg = krnl.grab_library_info_register();
      if (lib_info_reg.has("flsimrc")) {
        lib_info_reg.unregistration("flsimrc");
      }
    }
    return;
  }

} // end of namespace snsimrc
