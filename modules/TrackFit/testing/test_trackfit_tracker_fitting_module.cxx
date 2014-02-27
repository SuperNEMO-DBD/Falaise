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
#include <snemo/reconstruction/cat_tracker_clustering_module.h>

// Testing resources:
#include <utilities.h>

int main()
{
  FALAISE_INIT();
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  try {
    std::clog << "Hello, World!\n";
    bool draw = false;

    srand48(314159);

    // Parameters for the CAT driver:
    datatools::properties CATconfig;
    CATconfig.store_real("CAT.magnetic_field", 25 * CLHEP::gauss);
    CATconfig.store_string("CAT.level",           "normal");
    CATconfig.store_real("CAT.max_time",          5000.0 * CLHEP::ms);
    CATconfig.store_real("CAT.small_radius",      2.0 * CLHEP::mm);
    CATconfig.store_real("CAT.probmin",           0.0);
    CATconfig.store_integer("CAT.nofflayers",     1);
    CATconfig.store_integer("CAT.first_event",    -1);
    CATconfig.store_real("CAT.ratio",             10000.0);
    CATconfig.store_real("CAT.driver.sigma_z_factor", 1.0);

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

    // The CAT module:
    snemo::reconstruction::cat_tracker_clustering_module CATmod;
    CATmod.set_logging_priority(logging);
    CATmod.set_cd_label("CD");
    CATmod.set_tcd_label("TCD");
    CATmod.set_geometry_manager(Geo);
    CATmod.initialize_standalone(CATconfig);

    // Event loop:
    for (int i = 0; i < 3; i++) {
      std::clog << "Processing event #" << i << "\n";
      datatools::things eventRecord;
      snemo::datamodel::calibrated_data & CD
        = eventRecord.add<snemo::datamodel::calibrated_data>("CD");
      snemo::datamodel::calibrated_data::tracker_hit_collection_type & gghits = CD.calibrated_tracker_hits();
      generate_gg_hits(*gg_locator, gghits);
      dpp::base_module::process_status status = CATmod.process(eventRecord);
      if (status != 0) {
        DT_LOG_FATAL(logging, "CAT module failed!");
        break;
      }
      const snemo::datamodel::tracker_clustering_data & TCD
        = eventRecord.get<snemo::datamodel::tracker_clustering_data>("TCD");
      TCD.tree_dump(std::clog, "Tracker clustering data: ", "DEVEL: ");
      if (draw) display_event(*gg_locator, gghits, TCD);
      eventRecord.tree_dump(std::clog, "Event record: ", "DEVEL: ");
    }

    // Terminate the CAT module:
    CATmod.reset();

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
  FALAISE_FINI();
  return error_code;
}
