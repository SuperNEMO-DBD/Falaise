// test_signal_to_tp_process.cxx

// Standard libraries :
#include <iostream>

// GSL:
#include <bayeux/mygsl/rng.h>

// - Bayeux/datatools:
#include <datatools/clhep_units.h>
#include <datatools/io_factory.h>
#include <datatools/utils.h>
// - Bayeux/mctools:
#include <mctools/simulated_data.h>
// - Bayeux/dpp:
#include <dpp/input_module.h>

// Falaise:
#include <falaise/falaise.h>

// This project :
#include <snemo/digitization/clock_utils.h>
#include <snemo/digitization/electronic_mapping.h>
#include <snemo/digitization/mapping.h>
#include <snemo/digitization/sd_to_calo_signal_algo.h>
#include <snemo/digitization/sd_to_geiger_signal_algo.h>
#include <snemo/digitization/signal_to_calo_tp_algo.h>
#include <snemo/digitization/signal_to_geiger_tp_algo.h>

int main(int argc_, char** argv_) {
  falaise::initialize(argc_, argv_);
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  int iarg = 1;
  bool is_input_file = false;
  std::string input_filename;
  while (iarg < argc_) {
    std::string arg = argv_[iarg];
    if (arg == "-i" || arg == "--input") {
      is_input_file = true;
      input_filename = argv_[++iarg];
    } else if (arg == "-f" || arg == "--filename") {
      input_filename = argv_[++iarg];
    }
    iarg++;
  }

  try {
    std::clog << "Test program for class 'snemo::digitization::sd_to_calo_tp_algo' !" << std::endl;
    int32_t seed = 314159;
    mygsl::rng random_generator;
    random_generator.initialize(seed);

    std::string manager_config_file;

    manager_config_file = "@falaise:config/snemo/demonstrator/geometry/3.0/manager.conf";
    datatools::fetch_path_with_env(manager_config_file);
    datatools::properties manager_config;
    datatools::properties::read_config(manager_config_file, manager_config);
    geomtools::manager my_manager;

    manager_config.update("build_mapping", true);
    if (manager_config.has_key("mapping.excluded_categories")) {
      manager_config.erase("mapping.excluded_categories");
    }
    my_manager.initialize(manager_config);

    std::string pipeline_simulated_data_filename = "";
    // std::string SD_bank_label = "SD";

    if (is_input_file) {
      pipeline_simulated_data_filename = input_filename;
    }
    // else{
    //    pipeline_simulated_data_filename =
    //    "${FALAISE_DIGITIZATION_TESTING_DIR}/data/Se82_0nubb-source_strips_bulk_SD_10_events.brio";
    //  }

    // dpp::input_module reader;
    // datatools::properties reader_config;
    // reader_config.store ("logging.priority", "debug");
    // reader_config.store ("max_record_total", 1);
    // reader_config.store ("files.mode", "single");
    // reader_config.store ("files.single.filename", pipeline_simulated_data_filename);
    // reader.initialize_standalone (reader_config);
    // reader.tree_dump (std::clog, "Simulated data reader module");

    snemo::digitization::clock_utils my_clock_manager;
    my_clock_manager.initialize();
    my_clock_manager.compute_clockticks_ref(random_generator);
    uint32_t clocktick_800_reference = my_clock_manager.get_clocktick_800_ref();
    double clocktick_800_shift = my_clock_manager.get_shift_800();

    datatools::things ER;

    snemo::digitization::electronic_mapping my_e_mapping;
    my_e_mapping.set_geo_manager(my_manager);
    my_e_mapping.set_module_number(0);
    my_e_mapping.add_preconstructed_type(snemo::digitization::mapping::GEIGER_CATEGORY_TYPE);
    my_e_mapping.initialize();

    snemo::digitization::signal_to_geiger_tp_algo signal_2_geiger_tp;
    signal_2_geiger_tp.initialize(my_e_mapping);
    signal_2_geiger_tp.set_clocktick_reference(clocktick_800_reference);
    signal_2_geiger_tp.set_clocktick_shift(clocktick_800_shift);

    const geomtools::geom_id GID1(1204, 0, 0, 3, 106);
    const geomtools::geom_id GID2(1204, 0, 0, 6, 95);
    const geomtools::geom_id GID3(1204, 0, 0, 5, 57);
    const double anode_avalanche_time1 = 1200 * CLHEP::nanosecond;
    const double anode_avalanche_time2 = 850 * CLHEP::nanosecond;
    const double anode_avalanche_time3 = 4500 * CLHEP::nanosecond;

    snemo::digitization::signal_data signal_data;
    snemo::digitization::geiger_signal& my_gg_signal = signal_data.add_geiger_signal();
    my_gg_signal.set_header(0, GID1);
    my_gg_signal.set_anode_avalanche_time(anode_avalanche_time1);

    snemo::digitization::geiger_signal& my_gg_signal2 = signal_data.add_geiger_signal();
    my_gg_signal2.set_header(1, GID2);
    my_gg_signal2.set_anode_avalanche_time(anode_avalanche_time2);

    snemo::digitization::geiger_signal& my_gg_signal3 = signal_data.add_geiger_signal();
    my_gg_signal3.set_header(3, GID3);
    my_gg_signal3.set_anode_avalanche_time(anode_avalanche_time3);

    std::clog << "DEBUG : size of signal data : " << signal_data.get_geiger_signals().size()
              << std::endl;
    snemo::digitization::geiger_tp_data my_geiger_tp_data;

    if (signal_data.has_geiger_signals()) {
      signal_2_geiger_tp.process(signal_data, my_geiger_tp_data);
      my_geiger_tp_data.tree_dump(std::clog, "Geiger TP(s) data : ", "INFO : ");
    }

    std::clog << "The end." << std::endl;
  }

  catch (std::exception& error) {
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
