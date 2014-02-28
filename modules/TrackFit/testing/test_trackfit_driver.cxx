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
#include <falaise/snemo/datamodels/tracker_trajectory_data.h>
#include <falaise/snemo/geometry/locator_plugin.h>
#include <falaise/snemo/geometry/gg_locator.h>

// This project:
#include <snemo/reconstruction/trackfit_driver.h>

// Testing resources:
#include <utilities.h>

int main(int argc_, char ** argv_)
{
  FALAISE_INIT();
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

    // Parameters for the TrackFit driver:
    datatools::properties TrackFitconfig;
    TrackFitconfig.store_string("trackfit.drift_time_calibration_label", "");
    std::vector<std::string> models;
    models.push_back("line");
    models.push_back("helix");
    TrackFitconfig.store("trackfit.models", models);
    std::vector<std::string> line_only_guess;
    TrackFitconfig.store("trackfit.line.only_guess", line_only_guess);
    std::vector<std::string> helix_only_guess;
    TrackFitconfig.store("trackfit.helix.only_guess", helix_only_guess);

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

    // The TrackFit driver:
    snemo::reconstruction::trackfit_driver TF;
    TF.set_logging_priority(logging);
    TF.set_geometry_manager(Geo);
    TF.initialize(TrackFitconfig);

    // Event loop:
    for (int i = 0; i < 3; i++) {
      std::clog << "Processing event #" << i << "\n";
      snemo::datamodel::calibrated_data::tracker_hit_collection_type CTH;
      snemo::datamodel::tracker_clustering_data TCD;
      generate_tcd(*gg_locator, CTH, TCD);
      snemo::datamodel::tracker_trajectory_data TTD;
      int code = TF.process(TCD, TTD);
      if (code != 0) {
        break;
      }
      TTD.tree_dump(std::clog, "Trajectory data: ", "", true);
      for (int j = 0; j < (int) TTD.get_solutions().size(); j++) {
        std::string indent = "|   ";
        if (j == (int) TTD.get_solutions().size()-1) {
          std::cerr << "`-- ";
          indent = "    ";
        } else {
          std::cerr << "|-- ";
        }
        std::cerr << "TrackFit solution 0: " << std::endl;
        TTD.get_solutions()[j].get().tree_dump(std::clog, "TrackFit solution: ", indent);
      }
      if (draw) display_event(*gg_locator, CTH, TCD, TTD);
    }

    // Terminate the TrackFit driver:
    TF.reset();

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
