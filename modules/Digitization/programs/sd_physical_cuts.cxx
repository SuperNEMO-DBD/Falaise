// Standard libraries :
#include <fstream>
#include <iostream>

// - Bayeux/datatools:
#include <datatools/clhep_units.h>
#include <datatools/io_factory.h>
#include <datatools/properties.h>
#include <datatools/utils.h>

// - Bayeux/geomtools:
#include <bayeux/geomtools/manager.h>

// - Bayeux/mctools:
#include <mctools/simulated_data.h>

// - Bayeux/dpp:
#include <dpp/input_module.h>
#include <dpp/output_module.h>

// Falaise:
#include <falaise/falaise.h>

// Third part :
// Root :
#include "TFile.h"
#include "TTree.h"

/*
  PHYSICALLY NOT WORKING : problems on geiger cells number !!!! Take care
  Process can be launched but the computing for geiger cells number is false
  If You want a Fake trigger, use the fake trigger cut / module

  -> sd_physical_cuts : maybe to delete !!
 */

int main(int argc_, char** argv_) {
  falaise::initialize(argc_, argv_);
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;

  // Parsing arguments
  int iarg = 1;
  bool is_input_file = false;
  bool is_event_number = false;
  bool is_output_file = false;
  bool is_help = false;

  std::string input_filename;
  std::string output_filename;
  int arg_event_number = -1;

  while (iarg < argc_) {
    std::string arg = argv_[iarg];
    if (arg == "-i" || arg == "--input") {
      is_input_file = true;
      input_filename = argv_[++iarg];
    }

    else if (arg == "-o" || arg == "--output") {
      is_output_file = true;
      output_filename = argv_[++iarg];
    }

    else if (arg == "-n" || arg == "--number") {
      is_event_number = true;
      arg_event_number = atoi(argv_[++iarg]);
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
              << "$ BuildProducts/bin/sd_physical_cuts [OPTIONS] [ARGUMENTS]" << std::endl
              << std::endl;
  }

  // Process
  try {
    std::clog << "Little test program to do some basic cuts on simulated datas !" << std::endl;
    // Geometry manager
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

    std::string pipeline_simulated_data_filename;

    // Simulated Data "SD" bank label :
    std::string SD_bank_label = "SD";

    datatools::fetch_path_with_env(input_filename);
    if (is_input_file)
      pipeline_simulated_data_filename = input_filename;
    else {
      pipeline_simulated_data_filename =
          "${FALAISE_DIGITIZATION_TESTING_DIR}/data/"
          "Se82_0nubb-source_strips_bulk_SD_10_events.brio";
      // pipeline_simulated_data_filename =
      // "${DATA_NEMO_PERSO_DIR}/raw_simulated_data_brio/Bi214_Po214_100000-field_wire_surface_SD.brio";
    }
    datatools::fetch_path_with_env(pipeline_simulated_data_filename);

    // Number of events :
    int event_number = -1;
    if (is_event_number)
      event_number = arg_event_number;
    else
      event_number = 50;
    // Event reader :
    dpp::input_module reader;
    datatools::properties reader_config;
    reader_config.store("logging.priority", "debug");
    reader_config.store("max_record_total", event_number);
    reader_config.store("files.mode", "single");
    reader_config.store("files.single.filename", pipeline_simulated_data_filename);
    reader.initialize_standalone(reader_config);
    // reader.tree_dump (std::clog, "Simulated data reader module");

    datatools::fetch_path_with_env(output_filename);
    if (is_output_file)
      output_filename = output_filename;
    else
      output_filename = "default";

    // Event writer :
    dpp::output_module writer;
    datatools::properties writer_config;
    writer_config.store("logging.priority", "debug");
    writer_config.store("files.mode", "single");
    writer_config.store(
        "files.single.filename",
        "${DATA_NEMO_PERSO_DIR}/trigger/cuted_simulated_data_brio/" + output_filename + ".brio");
    writer.initialize_standalone(writer_config);

    // Event record :
    datatools::things ER;

    std::string root_filename = "${DATA_NEMO_PERSO_DIR}/trigger/root_files_cuted_simulated_data/" +
                                output_filename + ".root";
    datatools::fetch_path_with_env(root_filename);
    TFile* root_file = new TFile(root_filename.c_str(), "RECREATE");
    TTree* cuts_tree = new TTree("CutsTree", "SD_cuts");

    // Variables definitions :
    Int_t event_id = 0;
    Bool_t validated_event = false;
    Int_t gg_cells_hits = 0;
    Int_t pm_hits = 0;

    // Branch definitions :
    cuts_tree->Branch("event", &event_id, "event_id/I");
    cuts_tree->Branch("validated_event", &validated_event);
    cuts_tree->Branch("gg_cells_hits", &gg_cells_hits, "gg_cells_hits/I");
    cuts_tree->Branch("pm_hits", &pm_hits, "pm_hits_id/I");

    bool debug = true;

    // Internal counters
    int psd_count = 0;  // Event counter
    while (!reader.is_terminated()) {
      reader.process(ER);

      event_id = psd_count;
      validated_event = false;
      gg_cells_hits = 0;
      pm_hits = 0;

      std::size_t number_of_gg_cells = 0;
      std::size_t number_of_delayed_gg_cells = 0;
      std::size_t number_of_calo = 0;
      std::size_t number_of_main_wall_calo = 0;
      std::size_t number_of_x_wall_calo = 0;
      std::size_t number_of_gveto_calo = 0;

      // A plain `mctools::simulated_data' object is stored here :
      if (ER.has(SD_bank_label) && ER.is_a<mctools::simulated_data>(SD_bank_label)) {
        // Access to the "SD" bank with a stored `mctools::simulated_data' :
        const mctools::simulated_data& SD = ER.get<mctools::simulated_data>(SD_bank_label);

        if (debug) SD.tree_dump(std::clog, "Simulated Data : ", "INFO : ");

        if (SD.has_step_hits("gg")) {
          mctools::simulated_data::hit_handle_collection_type BSHC = SD.get_step_hits("gg");
          std::set<geomtools::geom_id> gg_cells_gid_set;
          std::set<geomtools::geom_id> delayed_gg_cells_gid_set;
          for (mctools::simulated_data::hit_handle_collection_type::const_iterator i = BSHC.begin();
               i != BSHC.end(); i++) {
            const mctools::base_step_hit& BSH = i->get();
            const geomtools::geom_id& a_gg_gid = BSH.get_geom_id();
            const double time_start = BSH.get_time_start();

            gg_cells_gid_set.insert(a_gg_gid);
            if (time_start > 50) {
              delayed_gg_cells_gid_set.insert(a_gg_gid);
            }
          }

          number_of_gg_cells = gg_cells_gid_set.size();
          number_of_delayed_gg_cells = delayed_gg_cells_gid_set.size();

          // std::clog << "GG cells size = " << number_of_gg_cells << " Delayed gg cells = " <<
          // number_of_delayed_gg_cells << std::endl;
        }

        if (SD.has_step_hits("calo")) {
          mctools::simulated_data::hit_handle_collection_type BSHC = SD.get_step_hits("calo");
          std::set<geomtools::geom_id> calo_gid_set;
          for (mctools::simulated_data::hit_handle_collection_type::const_iterator i = BSHC.begin();
               i != BSHC.end(); i++) {
            const mctools::base_step_hit& BSH = i->get();
            const geomtools::geom_id& a_calo_gid = BSH.get_geom_id();
            calo_gid_set.insert(a_calo_gid);
          }
          number_of_main_wall_calo = calo_gid_set.size();
        }

        if (SD.has_step_hits("xcalo")) {
          mctools::simulated_data::hit_handle_collection_type BSHC = SD.get_step_hits("xcalo");
          std::set<geomtools::geom_id> xcalo_gid_set;
          for (mctools::simulated_data::hit_handle_collection_type::const_iterator i = BSHC.begin();
               i != BSHC.end(); i++) {
            const mctools::base_step_hit& BSH = i->get();
            const geomtools::geom_id& a_xcalo_gid = BSH.get_geom_id();
            xcalo_gid_set.insert(a_xcalo_gid);
          }
          number_of_x_wall_calo = xcalo_gid_set.size();
        }

        if (SD.has_step_hits("gveto")) {
          mctools::simulated_data::hit_handle_collection_type BSHC = SD.get_step_hits("gveto");
          std::set<geomtools::geom_id> gveto_gid_set;
          for (mctools::simulated_data::hit_handle_collection_type::const_iterator i = BSHC.begin();
               i != BSHC.end(); i++) {
            const mctools::base_step_hit& BSH = i->get();
            const geomtools::geom_id& a_gveto_gid = BSH.get_geom_id();
            gveto_gid_set.insert(a_gveto_gid);
          }
          number_of_gveto_calo = gveto_gid_set.size();
        }

        number_of_calo = number_of_main_wall_calo + number_of_x_wall_calo + number_of_gveto_calo;

        // std::clog << "Nmbr of calo = " << number_of_calo << " Nmbr of gg cells = " <<
        // number_of_gg_cells << std::endl;

      }  // end of if has bank label

      if (number_of_calo >= 1 && number_of_gg_cells >= 3 && number_of_delayed_gg_cells >= 1) {
        writer.process(ER);
        gg_cells_hits = number_of_gg_cells;
        pm_hits = number_of_calo;
        validated_event = true;
      }
      cuts_tree->Fill();

      ER.clear();
      psd_count++;
      // std::clog << "DEBUG : psd count " << psd_count << std::endl;
      // std::clog << "\r" << "DEBUG : psd count " << psd_count << std::flush;
      DT_LOG_NOTICE(logging, "Simulated data #" << psd_count);

    }  // end of while

    root_file->Write();
    root_file->Close();
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
