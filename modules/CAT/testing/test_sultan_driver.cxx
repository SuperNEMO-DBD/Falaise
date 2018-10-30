// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

// Third party:
// - Bayeux/datatools:
#include <datatools/clhep_units.h>
#include <datatools/logger.h>
#include <datatools/properties.h>
#include <datatools/utils.h>
// - Bayeux/geomtools:
#include <geomtools/manager.h>

// Falaise:
#include <falaise/falaise.h>
#include <falaise/snemo/datamodels/tracker_clustering_data.h>
#include <falaise/snemo/geometry/gg_locator.h>
#include <falaise/snemo/geometry/locator_plugin.h>

// This project:
#include <snemo/reconstruction/sultan_driver.h>

// Testing resources:
#include <utilities.h>

int main(int argc_, char** argv_) {
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
        { std::clog << "warning: ignoring argument '" << argument << "'!" << std::endl; }
      }
      iarg++;
    }

    srand48(314159);

    // Parameters for the SULTAN driver:
    datatools::properties SULTANconfig;
    SULTANconfig.store_real("SULTAN.magnetic_field", 25 * CLHEP::gauss);
    SULTANconfig.store_string("SULTAN.clusterizer_level", "normal");
    SULTANconfig.store_string("SULTAN.sequentiator_level", "normal");
    SULTANconfig.store_real("SULTAN.max_time", 10000.0 * CLHEP::ms);
    SULTANconfig.store_boolean("SULTAN.print_event_display", 0);
    SULTANconfig.store_real("SULTAN.Emin", 0.2 * CLHEP::MeV);
    SULTANconfig.store_real("SULTAN.Emax", 7.0 * CLHEP::MeV);
    SULTANconfig.store_real("SULTAN.probmin", 0.0);
    SULTANconfig.store_real("SULTAN.nsigma_r", 5.0);
    SULTANconfig.store_real("SULTAN.nsigma_z", 3.0);
    SULTANconfig.store_integer("SULTAN.nofflayers", 0);
    SULTANconfig.store_integer("SULTAN.first_event", -1);
    SULTANconfig.store_integer("SULTAN.min_ncells_in_cluster", 0);
    SULTANconfig.store_integer("SULTAN.ncells_between_triplet_min", 0);
    SULTANconfig.store_integer("SULTAN.ncells_between_triplet_range", 0);
    SULTANconfig.store_real("SULTAN.nsigmas", 1.0);
    SULTANconfig.store_real("SULTAN.driver.sigma_z_factor", 1.0);

    // Geometry manager:
    geomtools::manager Geo;
    std::string GeoConfigFile = "@falaise:snemo/demonstrator/geometry/GeometryManager.conf";
    datatools::fetch_path_with_env(GeoConfigFile);
    datatools::properties GeoConfig;
    datatools::properties::read_config(GeoConfigFile, GeoConfig);
    Geo.initialize(GeoConfig);

    // Extract Geiger locator:
    const snemo::geometry::gg_locator* gg_locator = 0;
    std::string locator_plugin_name = "locators_driver";
    if (Geo.has_plugin(locator_plugin_name) &&
        Geo.is_plugin_a<snemo::geometry::locator_plugin>(locator_plugin_name)) {
      DT_LOG_NOTICE(logging, "Found locator plugin named '" << locator_plugin_name << "'");
      const snemo::geometry::locator_plugin& lp =
          Geo.get_plugin<snemo::geometry::locator_plugin>(locator_plugin_name);
      // Set the Geiger cell locator :
      gg_locator = dynamic_cast<const snemo::geometry::gg_locator*>(&(lp.get_gg_locator()));
    }

    // The SULTAN driver:
    snemo::reconstruction::sultan_driver SULTAN;
    SULTAN.set_logging_priority(logging);
    SULTAN.set_geometry_manager(Geo);
    SULTAN.initialize(SULTANconfig);

    // Event loop:
    for (int i = 0; i < 3; i++) {
      std::clog << "Processing event #" << i << "\n";
      snemo::reconstruction::sultan_driver::hit_collection_type gghits;
      generate_gg_hits(*gg_locator, gghits);
      snemo::reconstruction::sultan_driver::calo_hit_collection_type calohits;
      snemo::datamodel::tracker_clustering_data clustering_data;
      int code = SULTAN.process(gghits, calohits, clustering_data);
      if (code != 0) {
        break;
      }
      clustering_data.tree_dump(std::clog, "Clustering data: ");
      if (draw) display_event(*gg_locator, gghits, clustering_data);
    }

    // Terminate the SULTAN driver:
    SULTAN.reset();

    std::clog << "The end.\n";
  } catch (std::exception& error) {
    DT_LOG_FATAL(logging, error.what());
    error_code = EXIT_FAILURE;
  } catch (...) {
    DT_LOG_FATAL(logging, "Unexpected error!");
    error_code = EXIT_FAILURE;
  }
  falaise::terminate();
  return error_code;
}
