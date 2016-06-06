// Standard library:
#include <iostream>
#include <exception>
#include <cstdlib>
#include <string>

// Third party:
// - Bayeux/datatools:
#include <datatools/logger.h>
#include <datatools/properties.h>
#include <datatools/utils.h>
#include <datatools/clhep_units.h>
// - Bayeux/geomtools:
#include <geomtools/manager.h>

// Falaise:
#include <falaise/falaise.h>
#include <falaise/snemo/datamodels/tracker_clustering_data.h>
#include <falaise/snemo/geometry/locator_plugin.h>
#include <falaise/snemo/geometry/gg_locator.h>

// This project:
#include <snemo/reconstruction/mock_tracker_clustering_driver.h>

// Testing resources:
#include <utilities.h>

int main(int argc_, char ** argv_)
{
  falaise::initialize(argc_, argv_);
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  try {
    std::clog << "Hello, World!\n";
    bool draw = false;
    int iarg = 1;
    while (iarg < argc_) {
      std::string token = argv_[iarg];
      if (token[0] == '-') {
        std::string option = token;
        if ((option == "-D") || (option == "--draw")) {
          draw = true;
        } else {
          std::clog << "warning: ignoring option '" << option << "'!" << std::endl;
        }
      } else {
        std::string argument = token;
        {
          std::clog << "warning: ignoring argument '" << argument << "'!" << std::endl;
        }
      }
      iarg++;
    }

    srand48(314159);

    // Parameters for the MTC driver:
    datatools::properties MTCconfig;
    MTCconfig.store("MTC.max_layer_distance", 2);
    MTCconfig.store("MTC.max_row_distance", 2);
    MTCconfig.store("MTC.max_sum_distance", 3);

    // Geometry manager:
    geomtools::manager Geo;
    std::string GeoConfigFile = "@falaise:config/snemo/demonstrator/geometry/3.0/manager.conf";
    datatools::fetch_path_with_env (GeoConfigFile);
    datatools::properties GeoConfig;
    datatools::properties::read_config(GeoConfigFile, GeoConfig);
    Geo.initialize(GeoConfig);

    // Extract Geiger locator:
    const snemo::geometry::gg_locator * gg_locator = 0;
    std::string locator_plugin_name = "locators_driver";
    if (Geo.has_plugin(locator_plugin_name)
        && Geo.is_plugin_a<snemo::geometry::locator_plugin>(locator_plugin_name)) {
      DT_LOG_NOTICE(logging, "Found locator plugin named '" << locator_plugin_name << "'");
      const snemo::geometry::locator_plugin & lp
        = Geo.get_plugin<snemo::geometry::locator_plugin>(locator_plugin_name);
      // Set the Geiger cell locator :
      gg_locator = dynamic_cast<const snemo::geometry::gg_locator*>(&(lp.get_gg_locator ()));
    }

    // The MTC driver:
    snemo::reconstruction::mock_tracker_clustering_driver MTC;
    MTC.set_logging_priority(logging);
    MTC.set_geometry_manager(Geo);
    MTC.initialize(MTCconfig);

    // Event loop:
    for (int i = 0; i < 3; i++) {
      std::clog << "Processing event #" << i << "\n";
      snemo::reconstruction::mock_tracker_clustering_driver::hit_collection_type gghits;
      generate_gg_hits(*gg_locator, gghits);
      snemo::reconstruction::mock_tracker_clustering_driver::calo_hit_collection_type calohits;
      snemo::datamodel::tracker_clustering_data clustering_data;
      int code = MTC.process(gghits, calohits, clustering_data);
      if (code != 0) {
        break;
      }
      clustering_data.tree_dump(std::clog, "Clustering data: ");
      if (draw) display_event(*gg_locator, gghits, clustering_data);
    }

    // Terminate the MTC driver:
    MTC.reset();

    std::clog << "The end.\n";
  }
  catch (std::exception & error) {
    DT_LOG_FATAL(logging, error.what());
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    DT_LOG_FATAL(logging, "Unexpected error!");
    error_code = EXIT_FAILURE;
  }
  falaise::terminate();
  return error_code;
}
