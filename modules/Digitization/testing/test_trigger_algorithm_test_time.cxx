// test_trigger_algorithm_test_time.cxx
// Standard libraries :
#include <iostream>

// - Bayeux/datatools:
#include <datatools/clhep_units.h>
#include <datatools/io_factory.h>
#include <datatools/utils.h>
// - Bayeux/mctools:
#include <mctools/simulated_data.h>
// - Bayeux/dpp:
#include <dpp/input_module.h>
#include <dpp/output_module.h>

// Falaise:
#include <falaise/falaise.h>

// Third part :
// GSL:
#include <bayeux/mygsl/rng.h>
// Root :
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TTree.h"

// This project :
#include <snemo/digitization/clock_utils.h>
#include <snemo/digitization/mapping.h>

#include <snemo/digitization/calo_tp_to_ctw_algo.h>
#include <snemo/digitization/sd_to_calo_signal_algo.h>
#include <snemo/digitization/signal_to_calo_tp_algo.h>

#include <snemo/digitization/geiger_tp_to_ctw_algo.h>
#include <snemo/digitization/sd_to_geiger_signal_algo.h>
#include <snemo/digitization/signal_to_geiger_tp_algo.h>

#include <snemo/digitization/trigger_algorithm_test_time.h>

int main(int argc_, char** argv_) {
  falaise::initialize(argc_, argv_);
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;

  // Parsing arguments
  int iarg = 1;
  bool is_input_file = false;
  bool is_event_number = false;
  bool is_output_path = false;
  bool is_display = false;
  bool is_help = false;

  std::string input_filename;
  std::string output_path;
  int arg_event_number = -1;

  while (iarg < argc_) {
    std::string arg = argv_[iarg];
    if (arg == "-i" || arg == "--input") {
      is_input_file = true;
      input_filename = argv_[++iarg];
    }

    else if (arg == "-op" || arg == "--output-path") {
      is_output_path = true;
      output_path = argv_[++iarg];
    }

    else if (arg == "-n" || arg == "--number") {
      is_event_number = true;
      arg_event_number = atoi(argv_[++iarg]);
    }

    else if (arg == "-d" || arg == "--display") {
      is_display = true;
    }

    else if (arg == "-h" || arg == "--help") {
      is_help = true;
    }

    iarg++;
  }

  if (is_help) {
    std::cerr << std::endl
              << "Usage :" << std::endl
              << std::endl
              << "$ BuildProducts/bin/falaisedigitizationplugin-test_trigger_algorithm_test_time "
                 "[OPTIONS] [ARGUMENTS]"
              << std::endl
              << std::endl
              << "Allowed options: " << std::endl
              << "-h  [ --help ]           produce help message" << std::endl
              << "-i  [ --input ]          set an input file" << std::endl
              << "-op [ --output path ]    set a path where all files are stored" << std::endl
              << "-n  [ --number ]         set the number of events" << std::endl
              << "Example : " << std::endl
              << std::endl
              << "$ BuildProducts/bin/falaisedigitizationplugin-test_trigger_algorithm_test_time "
                 "--input "
                 "${FALAISE_DIGITIZATION_TESTING_DIR}/data/"
                 "Se82_0nubb-source_strips_bulk_SD_10_events.brio -op "
                 "${FALAISE_DIGITIZATION_TESTING_DIR}/output_default"
              << " --number 5" << std::endl
              << std::endl
              << "If no options are set, programs have default values :" << std::endl
              << std::endl
              << "input file           = "
                 "${FALAISE_DIGITIZATION_TESTING_DIR}/data/"
                 "Se82_0nubb-source_strips_bulk_SD_10_events.brio"
              << std::endl
              << "output path          = ${FALAISE_DIGITIZATION_TESTING_DIR}/output_default/"
              << std::endl
              << "number of events     = 10" << std::endl
              << std::endl;
    return 0;
  }

  try {
    // boolean for debugging (display etc)
    bool debug = false;

    if (is_display) debug = true;

    std::clog << "Test program for class 'snemo::digitization::test_trigger_algorithm_test_time' !"
              << std::endl;
    int32_t seed = 314159;
    mygsl::rng random_generator;
    random_generator.initialize(seed);

    std::string manager_config_file;

    manager_config_file = "@falaise:config/snemo/demonstrator/geometry/GeometryManager.conf";
    datatools::fetch_path_with_env(manager_config_file);
    datatools::properties manager_config;
    datatools::properties::read_config(manager_config_file, manager_config);
    geomtools::manager my_manager;
    manager_config.update("build_mapping", true);
    if (manager_config.has_key("mapping.excluded_categories")) {
      manager_config.erase("mapping.excluded_categories");
    }
    my_manager.initialize(manager_config);

    std::string pipeline_simulated_data_filename;

    // Simulated Data "SD" bank label :
    std::string SD_bank_label = "SD";

    datatools::fetch_path_with_env(input_filename);
    if (is_input_file) {
      pipeline_simulated_data_filename = input_filename;
    } else {
      pipeline_simulated_data_filename =
          "${FALAISE_DIGITIZATION_TESTING_DIR}/data/"
          "Se82_0nubb-source_strips_bulk_SD_10_events.brio";
    }
    datatools::fetch_path_with_env(pipeline_simulated_data_filename);

    // Number of events :
    int event_number = -1;
    if (is_event_number)
      event_number = arg_event_number;
    else
      event_number = 10;

    // Event record :
    datatools::things ER;

    // Event reader :
    dpp::input_module reader;
    datatools::properties reader_config;
    reader_config.store("logging.priority", "debug");
    reader_config.store("max_record_total", event_number);
    reader_config.store("files.mode", "single");
    reader_config.store("files.single.filename", pipeline_simulated_data_filename);
    reader.initialize_standalone(reader_config);
    // reader.tree_dump(std::clog, "Simulated data reader module");

    datatools::fetch_path_with_env(output_path);
    if (is_output_path)
      output_path = output_path;
    else
      output_path = "${FALAISE_DIGITIZATION_TESTING_DIR}/output_default/";
    datatools::fetch_path_with_env(output_path);

    // Electronic mapping :
    snemo::digitization::electronic_mapping my_e_mapping;
    my_e_mapping.set_geo_manager(my_manager);
    my_e_mapping.set_module_number(snemo::digitization::mapping::DEMONSTRATOR_MODULE_NUMBER);
    my_e_mapping.initialize();

    // Clock manager :
    snemo::digitization::clock_utils my_clock_manager;
    my_clock_manager.initialize();

    // Initializing SD to calo signal algo :
    snemo::digitization::sd_to_calo_signal_algo sd_2_calo_signal(my_manager);
    sd_2_calo_signal.initialize();

    // Initializing SD to geiger signal algo :
    snemo::digitization::sd_to_geiger_signal_algo sd_2_geiger_signal(my_manager);
    sd_2_geiger_signal.initialize();

    // Initializing signal to calo tp algo :
    snemo::digitization::signal_to_calo_tp_algo signal_2_calo_tp;
    signal_2_calo_tp.initialize(my_e_mapping);

    // Initializing signal to geiger tp algo :
    snemo::digitization::signal_to_geiger_tp_algo signal_2_geiger_tp;
    signal_2_geiger_tp.initialize(my_e_mapping);

    // Initializing calo tp to calo ctw algorithms for each crate :
    snemo::digitization::calo_tp_to_ctw_algo calo_tp_2_ctw_0;
    calo_tp_2_ctw_0.set_crate_number(snemo::digitization::mapping::MAIN_CALO_SIDE_0_CRATE);
    calo_tp_2_ctw_0.initialize();
    snemo::digitization::calo_tp_to_ctw_algo calo_tp_2_ctw_1;
    calo_tp_2_ctw_1.set_crate_number(snemo::digitization::mapping::MAIN_CALO_SIDE_1_CRATE);
    calo_tp_2_ctw_1.initialize();
    snemo::digitization::calo_tp_to_ctw_algo calo_tp_2_ctw_2;
    calo_tp_2_ctw_2.set_crate_number(snemo::digitization::mapping::XWALL_GVETO_CALO_CRATE);
    calo_tp_2_ctw_2.initialize();

    // Initializing geiger tp to geiger ctw :
    snemo::digitization::geiger_tp_to_ctw_algo geiger_tp_2_ctw;
    geiger_tp_2_ctw.initialize();

    // Loading memory from external files
    std::string mem1 = "${FALAISE_DIGITIZATION_TESTING_DIR}/config/trigger/tracker/mem1.conf";
    std::string mem2 = "${FALAISE_DIGITIZATION_TESTING_DIR}/config/trigger/tracker/mem2.conf";
    std::string mem3 = "${FALAISE_DIGITIZATION_TESTING_DIR}/config/trigger/tracker/mem3.conf";
    std::string mem4 = "${FALAISE_DIGITIZATION_TESTING_DIR}/config/trigger/tracker/mem4.conf";
    std::string mem5 = "${FALAISE_DIGITIZATION_TESTING_DIR}/config/trigger/tracker/mem5.conf";

    datatools::fetch_path_with_env(mem1);
    datatools::fetch_path_with_env(mem2);
    datatools::fetch_path_with_env(mem3);
    datatools::fetch_path_with_env(mem4);
    datatools::fetch_path_with_env(mem5);

    // Properties to configure trigger algorithm :
    datatools::properties trigger_config;
    int calo_circular_buffer_depth = 4;
    int calo_threshold = 1;
    bool inhibit_both_side_coinc = false;
    bool inhibit_single_side_coinc = false;
    int coincidence_calorimeter_gate_size = 5;  // Gate for the calorimeter gate size at 1600ns
    int L2_decision_coincidence_gate_size =
        5;  // Gate for calorimeter / tracker coincidence (5 x 1600 ns)
    int previous_event_buffer_depth =
        10;  // Maximum number of PER record (with an internal counter of 1 ms)
    bool activate_any_coincidences = true;
    // bool activate_calorimeter_only = false;

    trigger_config.store("calo.circular_buffer_depth", calo_circular_buffer_depth);
    trigger_config.store("calo.total_multiplicity_threshold", calo_threshold);
    trigger_config.store("calo.inhibit_both_side", inhibit_both_side_coinc);
    trigger_config.store("calo.inhibit_single_side", inhibit_single_side_coinc);
    trigger_config.store("tracker.mem1_file", mem1);
    trigger_config.store("tracker.mem2_file", mem2);
    trigger_config.store("tracker.mem3_file", mem3);
    trigger_config.store("tracker.mem4_file", mem4);
    trigger_config.store("tracker.mem5_file", mem5);
    trigger_config.store("coincidence_calorimeter_gate_size", coincidence_calorimeter_gate_size);
    trigger_config.store("L2_decision_coincidence_gate_size", L2_decision_coincidence_gate_size);
    trigger_config.store("previous_event_buffer_depth", previous_event_buffer_depth);
    trigger_config.store("activate_any_coincidences", activate_any_coincidences);
    // trigger_config.store("activate_calorimeter_only", activate_calorimeter_only);

    // Creation of trigger display manager :
    snemo::digitization::trigger_display_manager my_trigger_display;
    datatools::properties trigger_display_config;
    bool calo_25ns = true;
    bool calo_1600ns = true;
    bool tracker_1600ns = true;
    bool coinc_1600ns = true;
    trigger_display_config.store("calo_25ns", calo_25ns);
    trigger_display_config.store("calo_1600ns", calo_1600ns);
    trigger_display_config.store("tracker_1600ns", tracker_1600ns);
    trigger_display_config.store("coinc_1600ns", coinc_1600ns);
    my_trigger_display.initialize(trigger_display_config);

    // Creation and initialization of trigger algorithm :
    snemo::digitization::trigger_algorithm_test_time my_trigger_algo;
    my_trigger_algo.set_electronic_mapping(my_e_mapping);
    my_trigger_algo.set_clock_manager(my_clock_manager);
    my_trigger_algo.initialize(trigger_config);

    // Root file output :
    std::string root_output_filename = output_path + "test_trigger_algorithm_time_output.root";
    datatools::fetch_path_with_env(root_output_filename);
    TFile* root_output_file = new TFile(root_output_filename.c_str(), "RECREATE");

    std::string string_buffer = "number_of_L2_decision_TH1F";
    TH1F* number_of_L2_decision_TH1F =
        new TH1F(string_buffer.c_str(), Form("Number of L2 decision"), 10, 0, 10);

    string_buffer = "CARACO_decision_TH1F";
    TH1F* CARACO_decision_TH1F = new TH1F(string_buffer.c_str(), Form("CARACO decision"), 2, 0, 2);

    string_buffer = "CARACO_CT_decision_1600ns_TH1F";
    TH1F* CARACO_CT_decision_1600ns_TH1F =
        new TH1F(string_buffer.c_str(), Form("CARACO CT decision 1600"), 10, 0, 10);

    string_buffer = "delayed_decision_TH1F";
    TH1F* delayed_decision_TH1F =
        new TH1F(string_buffer.c_str(), Form("delayed decision"), 2, 0, 2);

    string_buffer = "delayed_CT_decision_1600ns_TH1F";
    TH1F* delayed_CT_decision_1600ns_TH1F =
        new TH1F(string_buffer.c_str(), Form("delayed CT decision 1600"), 650, 0, 650);

    string_buffer = "delayed_L2_trigger_mode_TH1F";
    TH1F* delayed_L2_trigger_mode_TH1F =
        new TH1F(string_buffer.c_str(), Form("delayed delayed_L2_trigger_mode_TH1F"), 10, 0, 10);

    // Internal counters
    int psd_count = 0;  // Event counter

    while (!reader.is_terminated()) {
      reader.process(ER);
      // A plain `mctools::simulated_data' object is stored here :
      if (ER.has(SD_bank_label) && ER.is_a<mctools::simulated_data>(SD_bank_label)) {
        // Access to the "SD" bank with a stored `mctools::simulated_data' :
        const mctools::simulated_data& SD = ER.get<mctools::simulated_data>(SD_bank_label);

        my_clock_manager.compute_clockticks_ref(random_generator);
        int32_t clocktick_25_reference = my_clock_manager.get_clocktick_25_ref();
        double clocktick_25_shift = my_clock_manager.get_shift_25();
        int32_t clocktick_800_reference = my_clock_manager.get_clocktick_800_ref();
        double clocktick_800_shift = my_clock_manager.get_shift_800();

        // Creation of calo ctw data :
        snemo::digitization::calo_ctw_data my_calo_ctw_data;

        // Creation of geiger ctw data :
        snemo::digitization::geiger_ctw_data my_geiger_ctw_data;

        if (SD.has_step_hits("calo") || SD.has_step_hits("xcalo") || SD.has_step_hits("gveto") ||
            SD.has_step_hits("gg")) {
          // Creation of a signal data object to store calo & geiger signals :
          snemo::digitization::signal_data signal_data;

          // Processing Calo signal :
          sd_2_calo_signal.process(SD, signal_data);

          // Processing Geiger signal :
          sd_2_geiger_signal.process(SD, signal_data);

          // signal_data.tree_dump(std::clog, "*** Signal Data ***", "INFO : ");

          // my_clock_manager.tree_dump(std::clog, "Clock utils : ", "INFO : ");

          snemo::digitization::calo_tp_data my_calo_tp_data;
          // Calo signal to calo TP :
          if (signal_data.has_calo_signals()) {
            // Set calo clockticks :
            signal_2_calo_tp.set_clocktick_reference(clocktick_25_reference);
            signal_2_calo_tp.set_clocktick_shift(clocktick_25_shift);
            // Signal to calo TP process :
            signal_2_calo_tp.process(signal_data, my_calo_tp_data);

            // my_calo_tp_data.tree_dump(std::clog, "Calorimeter TP(s) data : ", "INFO : ");
            // my_calo_tp_data.get_calo_tps()[0].get().tree_dump(std::clog, "Calo TP data [0] : ",
            // "INFO : "); my_calo_tp_data.get_calo_tps()[1].get().tree_dump(std::clog, "Calo TP
            // data [1] : ", "INFO : ");

            // Calo TP to geiger CTW process :
            calo_tp_2_ctw_0.process(my_calo_tp_data, my_calo_ctw_data);
            calo_tp_2_ctw_1.process(my_calo_tp_data, my_calo_ctw_data);
            calo_tp_2_ctw_2.process(my_calo_tp_data, my_calo_ctw_data);
            // my_calo_ctw_data.tree_dump(std::clog, "Calorimeter CTW(s) data : ", "INFO : ");

          }  // end of if has calo signal

          snemo::digitization::geiger_tp_data my_geiger_tp_data;
          if (signal_data.has_geiger_signals()) {
            // Set geiger clockticks :
            signal_2_geiger_tp.set_clocktick_reference(clocktick_800_reference);
            signal_2_geiger_tp.set_clocktick_shift(clocktick_800_shift);
            // Signal to geiger TP process
            signal_2_geiger_tp.process(signal_data, my_geiger_tp_data);

            // Geiger TP to geiger CTW process
            geiger_tp_2_ctw.process(my_geiger_tp_data, my_geiger_ctw_data);
            // my_geiger_ctw_data.tree_dump(std::clog, "Geiger CTW(s) data : ", "INFO : ");

          }  // end of if has geiger signal

        }  // end of if has "calo" || "xcalo" || "gveto" || "gg" step hits

        // Creation of outputs collection structures for calo and tracker
        std::vector<snemo::digitization::trigger_structures::calo_summary_record>
            calo_collection_records;
        std::vector<snemo::digitization::trigger_structures::coincidence_calo_record>
            coincidence_collection_calo_records;
        std::vector<snemo::digitization::trigger_structures::tracker_record>
            tracker_collection_records;
        std::vector<snemo::digitization::trigger_structures::coincidence_event_record>
            coincidence_collection_records;

        std::vector<snemo::digitization::trigger_structures::L2_decision> L2_decision_record;

        // Reseting trigger display
        my_trigger_display.reset_matrix_pattern();

        // Trigger process
        my_trigger_algo.process(my_calo_ctw_data, my_geiger_ctw_data);

        // Finale structures :
        calo_collection_records = my_trigger_algo.get_calo_records_25ns_vector();
        coincidence_collection_calo_records =
            my_trigger_algo.get_coincidence_calo_records_1600ns_vector();
        tracker_collection_records = my_trigger_algo.get_tracker_records_vector();
        coincidence_collection_records = my_trigger_algo.get_coincidence_records_vector();
        L2_decision_record = my_trigger_algo.get_L2_decision_records_vector();

        uint16_t number_of_L2_decision = L2_decision_record.size();
        bool caraco_decision = false;
        uint32_t caraco_clocktick_1600ns = snemo::digitization::clock_utils::INVALID_CLOCKTICK;
        bool delayed_decision = false;
        uint32_t delayed_clocktick_1600ns = snemo::digitization::clock_utils::INVALID_CLOCKTICK;
        bool already_delayed_trig = false;
        snemo::digitization::trigger_structures::L2_trigger_mode delayed_trigger_mode =
            snemo::digitization::trigger_structures::L2_trigger_mode::INVALID;

        if (number_of_L2_decision != 0) {
          for (unsigned int isize = 0; isize < number_of_L2_decision; isize++) {
            if (L2_decision_record[isize].L2_decision_bool &&
                L2_decision_record[isize].L2_trigger_mode ==
                    snemo::digitization::trigger_structures::L2_trigger_mode::CARACO) {
              caraco_decision = L2_decision_record[isize].L2_decision_bool;
              caraco_clocktick_1600ns = L2_decision_record[isize].L2_ct_decision;
            } else if (L2_decision_record[isize].L2_decision_bool &&
                       (L2_decision_record[isize].L2_trigger_mode ==
                            snemo::digitization::trigger_structures::L2_trigger_mode::APE ||
                        L2_decision_record[isize].L2_trigger_mode ==
                            snemo::digitization::trigger_structures::L2_trigger_mode::DAVE) &&
                       already_delayed_trig == false) {
              delayed_decision = L2_decision_record[isize].L2_decision_bool;
              delayed_clocktick_1600ns = L2_decision_record[isize].L2_ct_decision;
              delayed_trigger_mode = L2_decision_record[isize].L2_trigger_mode;
              already_delayed_trig = true;
            }
          }
        }

        // if (number_of_L2_decision == 4) std::cin.get();
        number_of_L2_decision_TH1F->Fill(number_of_L2_decision);
        CARACO_decision_TH1F->Fill(caraco_decision);
        if (caraco_clocktick_1600ns != snemo::digitization::clock_utils::INVALID_CLOCKTICK)
          CARACO_CT_decision_1600ns_TH1F->Fill(caraco_clocktick_1600ns);
        delayed_decision_TH1F->Fill(delayed_decision);
        if (delayed_decision &&
            delayed_clocktick_1600ns != snemo::digitization::clock_utils::INVALID_CLOCKTICK)
          delayed_CT_decision_1600ns_TH1F->Fill(delayed_clocktick_1600ns);
        if (delayed_decision &&
            delayed_trigger_mode !=
                snemo::digitization::trigger_structures::L2_trigger_mode::INVALID)
          delayed_L2_trigger_mode_TH1F->Fill(delayed_trigger_mode);

        // std::clog << "Number of L2 decision : " << number_of_L2_decision << std::endl;
        // std::clog << "CARACO decision :       " << caraco_decision << std::endl;
        // std::clog << "CARACO CT1600ns :       " << caraco_clocktick_1600ns << std::endl;
        // std::clog << "Delayed decision :      " << delayed_decision << std::endl;
        // std::clog << "Delayed CT1600ns :      " << delayed_clocktick_1600ns << std::endl;
        // std::clog << "Delayed trigger mode :  " << delayed_trigger_mode << std::endl;

        my_trigger_algo.reset_data();

      }  // end of if has bank label "SD"

      ER.clear();
      psd_count++;
      std::clog << "\r"
                << "DEBUG : psd count " << psd_count << std::flush;
      if (debug) std::clog << "DEBUG : psd count " << psd_count << std::endl;
      DT_LOG_NOTICE(logging, "Simulated data #" << psd_count);
    }  // end of reader is terminated

    root_output_file->Write();
    root_output_file->Close();

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
