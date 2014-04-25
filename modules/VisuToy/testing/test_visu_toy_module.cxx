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
// - Bayeux/dpp:
#include <dpp/input_module.h>

// Falaise:
#include <falaise/falaise.h>
#include <falaise/snemo/datamodels/tracker_clustering_data.h>
#include <falaise/snemo/geometry/locator_plugin.h>
#include <falaise/snemo/geometry/gg_locator.h>

// This project:
#include <snemo/visualization/visu_toy_module.h>

int main(int argc_, char ** argv_)
{
  FALAISE_INIT();
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  try {
    std::clog << "Hello, World!\n";

    bool dump = false;
    bool draw = false;
    std::string input_file;
    int nevents = 7;

    int iarg = 1;
    while (iarg < argc_) {
      std::string token = argv_[iarg];
      if (token[0] == '-') {
        std::string option = token;
        if (option == "--dump") {
          dump = true;
        } else if (option == "--draw") {
          draw = true;
        } else if (option == "--no-limit") {
          nevents = 0;
        } else {
          std::clog << "warning: ignoring option '" << option << "'!" << std::endl;
        }
      } else {
        std::string argument = token;
        if (input_file.empty()) {
          input_file = argument;
        } else {
          std::clog << "warning: ignoring argument '" << argument << "'!" << std::endl;
        }
      }
      iarg++;
    }

    // DT_THROW_IF (input_file.empty(), std::logic_error, "Missing input filename!");
    if (input_file.empty()) {
      input_file = "@falaise.resources:modules/VisuToy/examples/data/Se82.0nubb-source_strips_bulk_SD.brio";
      datatools::fetch_path_with_env(input_file);
      std::cerr << "NOTICE: Input file is '" << input_file << "'" << std::endl;
    }

    srand48(314159);

    dpp::input_module Imod;
    Imod.set_single_input_file(input_file);
    Imod.initialize_simple();
    // Imod.tree_dump(std::clog, "Input module: ", "DEBUG: ");

    // Parameters for the TD driver:
    datatools::properties TDconfig;
    TDconfig.store("interactive", true);
    TDconfig.store("min_event_count", 2);
    TDconfig.store("max_event_count", 5);

    // Geometry manager:
    geomtools::manager Geo;
    std::string GeoConfigFile = "@falaise:config/snemo/demonstrator/geometry/3.0/manager.conf";
    datatools::fetch_path_with_env (GeoConfigFile);
    datatools::properties GeoConfig;
    datatools::properties::read_config(GeoConfigFile, GeoConfig);
    Geo.initialize(GeoConfig);

    // The TD module:
    snemo::visualization::visu_toy_module TDmod;
    TDmod.set_logging_priority(logging);
    TDmod.set_geometry_manager(Geo);
    TDmod.initialize_standalone(TDconfig);

    // Event loop:
    int event_count = 0;
    while (true) {
      if (nevents > 0 && (event_count > nevents)) break;
      std::clog << "Processing event #" << event_count << "\n";
      datatools::things eventRecord;

      dpp::base_module::process_status status = dpp::base_module::PROCESS_OK;
      status = Imod.process(eventRecord);
      if (status != dpp::base_module::PROCESS_OK) {
        DT_LOG_FATAL(logging, "No more available event record!");
        break;
      }
      if (draw) {
        status = TDmod.process(eventRecord);
        if (status != dpp::base_module::PROCESS_OK) {
          DT_LOG_FATAL(logging, "Toy display module has terminated!");
          break;
        }
      }
      if (dump) {
        eventRecord.tree_dump(std::clog, "Event record: ", "DEVEL: ");
      }
      event_count++;
    }

    // Terminate the event display module:
    TDmod.reset();

    // Terminate the input module:
    Imod.reset();

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
