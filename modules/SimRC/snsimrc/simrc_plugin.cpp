// Third party:
// - Bayeux/datatools:
#include <datatools/library_info.h>
#include <datatools/kernel.h>
#include <datatools/logger.h>

// - Falaise:
#include <falaise/resource.h>

//- This project:
#include "simrc_lib.hpp"

namespace {
  
  struct simrc_plugin
  {
    simrc_plugin()
    {
      DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE, "Initialization of the SimRC plugin");
      if (datatools::kernel::is_instantiated()) {
        snsimrc::simrc_initialize();
      }
      return;
    }
    ~simrc_plugin()
    {
      DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE, "Termination of the SimRC plugin");
      if (datatools::kernel::is_instantiated()) {
        snsimrc::simrc_terminate();
      }
      return;
    }
  };
  static simrc_plugin _initPlugin_pluginModule;
}
