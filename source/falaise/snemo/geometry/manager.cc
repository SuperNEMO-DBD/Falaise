// Ourselves:
#include <falaise/snemo/geometry/manager.h>

// Standard library:
#include <string>

// Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/properties.h>

namespace snemo {

  namespace geometry {

    void initialize_manager(geomtools::manager & gmgr_)
    {
      if (gmgr_.is_initialized()) {
        gmgr_.reset();
      }
      std::string managerConfigFile = "@falaise:snemo/demonstrator/geometry/GeometryManager.conf";
      datatools::fetch_path_with_env(managerConfigFile);
      datatools::properties managerConfig;
      datatools::properties::read_config(managerConfigFile, managerConfig);
      gmgr_.initialize(managerConfig);
      return;
    }

    std::unique_ptr<geomtools::manager> create_manager()
    {
      auto mgr = std::make_unique<geomtools::manager>();
      initialize_manager(*mgr);
      return mgr;
    }
    
  }  // end of namespace geometry

}  // end of namespace snemo

