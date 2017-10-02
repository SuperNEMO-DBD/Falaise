// hc_rate_analysis.cxx
// Standard libraries :
#include <iostream>
// - Bayeux/datatools:
#include <datatools/clhep_units.h>
#include <datatools/io_factory.h>
#include <datatools/utils.h>
// - Bayeux/mctools:
#include <mctools/simulated_data.h>
// - Bayeux/geomtools:
#include <bayeux/geomtools/id_mgr.h>
#include <bayeux/geomtools/id_selector.h>
#include <bayeux/geomtools/manager.h>

// - Bayeux/dpp:
#include <dpp/input_module.h>
#include <dpp/output_module.h>

// Falaise:
#include <falaise/falaise.h>

// Third part :
// Root :
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TTree.h"

int column_to_hc_half_zone(const int& column_);

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
              << "$ BuildProducts/bin/half_commissioning-hc_rate_analysis [OPTIONS] [ARGUMENTS]"
              << std::endl
              << std::endl
              << "Allowed options: " << std::endl
              << "-h  [ --help ]           produce help message" << std::endl
              << "-i  [ --input ]          set an input file" << std::endl
              << "-op [ --output path ]    set a path where all files are stored" << std::endl
              << "-d  [ --display ]        display things for debug" << std::endl
              << "-n  [ --number ]         set the number of events" << std::endl
              << "Example : " << std::endl
              << std::endl;
    return 0;
  }

  try {
    // boolean for debugging (display etc)
    bool debug = false;

    if (is_display) debug = true;

    std::clog << "Program for half commissioning rate analysis" << std::endl;

    std::string manager_config_file;
    manager_config_file = "@falaise:config/snemo/demonstrator/geometry/4.0/manager.conf";
    datatools::fetch_path_with_env(manager_config_file);
    datatools::properties manager_config;
    datatools::properties::read_config(manager_config_file, manager_config);
    geomtools::manager my_geom_manager;
    manager_config.update("build_mapping", true);
    if (manager_config.has_key("mapping.excluded_categories")) {
      manager_config.erase("mapping.excluded_categories");
    }
    my_geom_manager.initialize(manager_config);

    std::string pipeline_simulated_data_filename;

    // Simulated Data "SD" bank label :
    std::string SD_bank_label = "SD";
    datatools::fetch_path_with_env(input_filename);
    if (is_input_file) {
      pipeline_simulated_data_filename = input_filename;
    } else {
      pipeline_simulated_data_filename =
          "${FALAISE_DIGITIZATION_TESTING_DIR}/data/Co60-100_row_0_column_0_SD.brio";
    }
    datatools::fetch_path_with_env(pipeline_simulated_data_filename);

    // Number of events :
    int event_number = -1;
    if (is_event_number)
      event_number = arg_event_number;
    else
      event_number = 10;

    // Vertex in the filename :
    std::string vtx_filename = pipeline_simulated_data_filename;
    std::size_t found = 0;
    found = vtx_filename.find_last_of("/");
    vtx_filename.erase(0, found + 1);
    found = vtx_filename.find(".brio");
    vtx_filename.erase(found, vtx_filename.size());
    std::clog << "vertex = " << vtx_filename << std::endl;

    // Found row i and column j :
    std::string temporary_filename = vtx_filename;
    std::size_t found_1 = 0;
    int row = 0;
    int column = 0;
    // Find the row :
    found_1 = temporary_filename.find("row_");
    temporary_filename.erase(0, found_1 + 4);
    found_1 = temporary_filename.find_first_of("_");
    temporary_filename.erase(found_1, temporary_filename.size());
    row = std::stoi(temporary_filename);

    // Find the column :
    temporary_filename = vtx_filename;
    found_1 = temporary_filename.find("column_");
    temporary_filename.erase(0, found_1 + 7);
    found_1 = temporary_filename.find_first_of("_");
    temporary_filename.erase(found_1, temporary_filename.size());
    column = std::stoi(temporary_filename);

    // Check if a column count for 2 half zones :
    int process_size = 1;
    if ((column >= 3 && column <= 5) || column == 62 || (column >= 107 && column <= 109)) {
      process_size = 2;
    }

    for (int i = 0; i < process_size; i++) {
      // Event reader :
      dpp::input_module reader;
      datatools::properties reader_config;
      reader_config.store("logging.priority", "debug");
      reader_config.store("max_record_total", event_number);
      reader_config.store("files.mode", "single");
      reader_config.store("files.single.filename", pipeline_simulated_data_filename);
      reader.initialize_standalone(reader_config);
      if (debug) reader.tree_dump(std::clog, "Simulated data reader module");

      // Event record :
      datatools::things ER;

      // Internal counters
      int psd_count = 0;  // Event counter

      // Selection rules depending on half zone for commissioning :

      int hc_half_zone = column_to_hc_half_zone(column) + i;
      std::string hc_main_calo_half_zone_rules = " ";
      // Invalidate the rule for hc xwall (all zones except 0 and 19 :
      std::string hc_xwall_calo_half_zone_rules =
          "category='xcalo_block' module={1} side={0} wall={*} column={*} row={*}";

      std::string hc_geiger_half_zone_rules = " ";
      int geiger_hc_zone_inf_limit = 0;
      int geiger_hc_zone_sup_limit = 0;

      if (hc_half_zone == 0) {
        geiger_hc_zone_inf_limit = 0;
        geiger_hc_zone_sup_limit = 5;
        hc_xwall_calo_half_zone_rules =
            "category='xcalo_block' module={0} side={1} wall={0} column={*} row={*}";
      } else if (hc_half_zone >= 1 && hc_half_zone <= 10) {
        geiger_hc_zone_inf_limit = hc_half_zone * 6 - 3;
        geiger_hc_zone_sup_limit = geiger_hc_zone_inf_limit + 5;
      } else if (hc_half_zone >= 11 && hc_half_zone <= 18) {
        geiger_hc_zone_inf_limit = hc_half_zone * 6 - 4;
        geiger_hc_zone_sup_limit = geiger_hc_zone_inf_limit + 5;
      } else if (hc_half_zone == 19) {
        geiger_hc_zone_inf_limit = 107;
        geiger_hc_zone_sup_limit = 112;
        hc_xwall_calo_half_zone_rules =
            "category='xcalo_block' module={0} side={1} wall={1} column={*} row={*}";
      }

      // Id selector rules :
      hc_main_calo_half_zone_rules = "category='calorimeter_block' module={0} side={1} column={" +
                                     std::to_string(hc_half_zone) + "} row={*} part={*}";
      hc_geiger_half_zone_rules = "category='drift_cell_core' module={0} side={1} layer={*} row={" +
                                  std::to_string(geiger_hc_zone_inf_limit) + ";" +
                                  std::to_string(geiger_hc_zone_inf_limit + 1) + ";" +
                                  std::to_string(geiger_hc_zone_inf_limit + 2) + ";" +
                                  std::to_string(geiger_hc_zone_inf_limit + 3) + ";" +
                                  std::to_string(geiger_hc_zone_inf_limit + 4) + ";" +
                                  std::to_string(geiger_hc_zone_sup_limit) + "}";

      std::map<geomtools::geom_id, int> hit_rate_map;

      // Output path :
      std::string output_path_dir = "";
      datatools::fetch_path_with_env(output_path);
      if (is_output_path)
        output_path_dir = output_path + "hc_half_zone_" + std::to_string(hc_half_zone) + "/";
      else
        output_path = "${FALAISE_DIGITIZATION_TESTING_DIR}/output_default/";
      datatools::fetch_path_with_env(output_path);

      std::clog << "OUTPUT PATH = " << output_path_dir << std::endl;

      // Name of SD output files
      std::string HC_writer_1 = output_path_dir + "HC_" + vtx_filename + "_match_rules" + ".brio";

      // Event writer :
      dpp::output_module writer_1;
      datatools::properties writer_config_1;
      writer_config_1.store("logging.priority", "debug");
      writer_config_1.store("files.mode", "single");
      writer_config_1.store("files.single.filename", HC_writer_1);
      writer_1.initialize_standalone(writer_config_1);

      // Output ROOT file :
      std::string root_filename = output_path_dir + "HC_" + vtx_filename + "_analysis.root";
      datatools::fetch_path_with_env(root_filename);
      TFile* root_file = new TFile(root_filename.c_str(), "RECREATE");
      // TTree* hc_analysis_tree = new TTree("HC_analysis_tree", "Half Commissioning analysis
      // tree");

      TH1F* calo_half_zone_energy_spectrum_TH1F =
          new TH1F("Calo half zone energy spectrum TH1F",
                   Form("Calo half zone energy spectrum row %i, col %i, zone %i;", row, column,
                        hc_half_zone),
                   100, 0, 3000);

      TH2F* main_calo_half_zone_distribution_TH2F =
          new TH2F("Main calo half zone distribution TH2F",
                   Form("Main calo half zone distribution row %i, col %i, zone %i;", row, column,
                        hc_half_zone),
                   20, 0, 20, 14, 0, 14);

      TH2F* xwall_calo_half_zone_distribution_TH2F =
          new TH2F("Xwall calo half zone distribution TH2F",
                   Form("Xwall calo half zone distribution row %i, col %i, zone %i;", row, column,
                        hc_half_zone),
                   5, 0, 5, 17, 0, 17);

      TH2F* geiger_cells_half_zone_distribution_TH2F =
          new TH2F("GG cells half zone distribution",

                   Form("GG cells half zone distribution row %i, col %i, zone %i;", row, column,
                        hc_half_zone),
                   7, 0, 7, 10, 0, 10);
      while (!reader.is_terminated()) {
        if (is_display)
          std::clog
              << "********************************************************************************"
              << std::endl;
        if (is_display)
          std::clog << "****************************** EVENT #" << psd_count
                    << " **************************************" << std::endl;
        reader.process(ER);

        bool write_event = false;
        std::set<geomtools::geom_id> set_of_already_hit_gid_for_an_event;

        // A plain `mctools::simulated_data' object is stored here :
        if (ER.has(SD_bank_label) && ER.is_a<mctools::simulated_data>(SD_bank_label)) {
          // Access to the "SD" bank with a stored `mctools::simulated_data' :
          const mctools::simulated_data& SD = ER.get<mctools::simulated_data>(SD_bank_label);

          std::map<geomtools::geom_id, double> calo_gid_energy_map;

          // If main calo hits :
          if (SD.has_step_hits("calo")) {
            // const size_t number_of_main_calo_hits = SD.get_number_of_step_hits("calo");
            // Rules have to change depending on row / column input
            geomtools::id_selector my_hc_main_calo_id_selector(my_geom_manager.get_id_mgr());
            my_hc_main_calo_id_selector.initialize(hc_main_calo_half_zone_rules);
            if (is_display) my_hc_main_calo_id_selector.dump(std::clog, "Main calo ID selector: ");

            mctools::simulated_data::hit_handle_collection_type BSHC = SD.get_step_hits("calo");
            if (is_display) std::clog << "BSCH step hits # = " << BSHC.size() << std::endl;
            // int count = 0;
            for (mctools::simulated_data::hit_handle_collection_type::const_iterator the_it =
                     BSHC.begin();
                 the_it != BSHC.end(); the_it++) {
              const mctools::base_step_hit& BSH = the_it->get();
              // extract the corresponding geom ID:
              const geomtools::geom_id& main_calo_gid = BSH.get_geom_id();
              if (my_hc_main_calo_id_selector.match(main_calo_gid)) {
                const bool is_in_set = set_of_already_hit_gid_for_an_event.find(main_calo_gid) !=
                                       set_of_already_hit_gid_for_an_event.end();
                const bool is_in_map = hit_rate_map.find(main_calo_gid) != hit_rate_map.end();
                const bool is_in_energy_map =
                    calo_gid_energy_map.find(main_calo_gid) != calo_gid_energy_map.end();

                int main_calo_gid_column = main_calo_gid.get(2);
                int main_calo_gid_row = main_calo_gid.get(3);
                double energy = BSH.get_energy_deposit();
                if (!is_in_set && !is_in_map) {
                  hit_rate_map.insert(std::pair<geomtools::geom_id, int>(main_calo_gid, 1));
                  main_calo_half_zone_distribution_TH2F->Fill(main_calo_gid_column,
                                                              main_calo_gid_row);
                } else if (!is_in_set && is_in_map) {
                  hit_rate_map.find(main_calo_gid)->second++;
                  main_calo_half_zone_distribution_TH2F->Fill(main_calo_gid_column,
                                                              main_calo_gid_row);
                }

                if (!is_in_energy_map) {
                  calo_gid_energy_map.insert(
                      std::pair<geomtools::geom_id, double>(main_calo_gid, energy));
                }

                else {
                  calo_gid_energy_map.find(main_calo_gid)->second += energy;
                }

                write_event = true;
                set_of_already_hit_gid_for_an_event.insert(main_calo_gid);

                if (is_display)
                  std::clog << "ID=" << main_calo_gid << " matches the selector rules !"
                            << std::endl;
                if (is_display) BSH.tree_dump(std::clog, "A Main calo Base Step Hit : ", "INFO : ");
              }
            }

          }  // end of if has step hits calo

          // If xcalo hits :
          if (SD.has_step_hits("xcalo")) {
            // const size_t number_of_xcalo_hits = SD.get_number_of_step_hits("xcalo");
            // Rules have to change depending on row / column input
            geomtools::id_selector my_hc_xwall_calo_id_selector(my_geom_manager.get_id_mgr());
            my_hc_xwall_calo_id_selector.initialize(hc_xwall_calo_half_zone_rules);
            if (is_display)
              my_hc_xwall_calo_id_selector.dump(std::clog, "Xwall calo ID selector: ");

            mctools::simulated_data::hit_handle_collection_type BSHC = SD.get_step_hits("xcalo");

            if (is_display) std::clog << "BSCH step hits # = " << BSHC.size() << std::endl;
            // int count = 0;
            for (mctools::simulated_data::hit_handle_collection_type::const_iterator my_it =
                     BSHC.begin();
                 my_it != BSHC.end(); my_it++) {
              const mctools::base_step_hit& BSH = my_it->get();
              const geomtools::geom_id& xwall_calo_gid = BSH.get_geom_id();
              if (my_hc_xwall_calo_id_selector.match(xwall_calo_gid)) {
                const bool is_in_set = set_of_already_hit_gid_for_an_event.find(xwall_calo_gid) !=
                                       set_of_already_hit_gid_for_an_event.end();
                const bool is_in_map = hit_rate_map.find(xwall_calo_gid) != hit_rate_map.end();
                const bool is_in_energy_map =
                    calo_gid_energy_map.find(xwall_calo_gid) != calo_gid_energy_map.end();

                int wall = xwall_calo_gid.get(2);
                int xwall_gid_column = xwall_calo_gid.get(3);
                int xwall_gid_row = xwall_calo_gid.get(4);

                double energy = BSH.get_energy_deposit();

                if (wall == 1) {
                  column += 2;
                }

                if (!is_in_set && !is_in_map) {
                  hit_rate_map.insert(std::pair<geomtools::geom_id, int>(xwall_calo_gid, 1));
                  xwall_calo_half_zone_distribution_TH2F->Fill(xwall_gid_column, xwall_gid_row);
                } else if (!is_in_set && is_in_map) {
                  hit_rate_map.find(xwall_calo_gid)->second++;
                  xwall_calo_half_zone_distribution_TH2F->Fill(xwall_gid_column, xwall_gid_row);
                }

                if (!is_in_energy_map) {
                  calo_gid_energy_map.insert(
                      std::pair<geomtools::geom_id, double>(xwall_calo_gid, energy));
                }

                else {
                  calo_gid_energy_map.find(xwall_calo_gid)->second += energy;
                }

                write_event = true;
                set_of_already_hit_gid_for_an_event.insert(xwall_calo_gid);

                if (is_display)
                  std::clog << "ID=" << xwall_calo_gid << " matches the selector rules !"
                            << std::endl;
                if (is_display)
                  BSH.tree_dump(std::clog, "A Xwall calo Base Step Hit : ", "INFO : ");
              }
            }

          }  // end of if has step hits xcalo

          std::map<geomtools::geom_id, double>::iterator it_energy;
          for (it_energy = calo_gid_energy_map.begin(); it_energy != calo_gid_energy_map.end();
               it_energy++) {
            double energy = it_energy->second;
            energy *= 1000;
            calo_half_zone_energy_spectrum_TH1F->Fill(energy);
          }

          // If Geiger hits :
          if (SD.has_step_hits("gg")) {
            const size_t number_of_hits = SD.get_number_of_step_hits("gg");
            // Rules have to change depending on row / column input
            geomtools::id_selector my_hc_geiger_id_selector(my_geom_manager.get_id_mgr());
            my_hc_geiger_id_selector.initialize(hc_geiger_half_zone_rules);

            if (is_display) my_hc_geiger_id_selector.dump(std::clog, "Geiger ID selector: ");

            // New sd bank
            mctools::simulated_data flaged_sd = SD;

            // We have to flag the gg cells already hit before (maybe take into account the dead
            // time of a GG cell)
            for (size_t ihit = 0; ihit < number_of_hits; ihit++) {
              mctools::base_step_hit& geiger_hit = flaged_sd.grab_step_hit("gg", ihit);
              for (size_t jhit = ihit + 1; jhit < number_of_hits; jhit++) {
                mctools::base_step_hit& other_geiger_hit = flaged_sd.grab_step_hit("gg", jhit);
                if (geiger_hit.get_geom_id() == other_geiger_hit.get_geom_id()) {
                  const double gg_hit_time = geiger_hit.get_time_start();
                  const double other_gg_hit_time = other_geiger_hit.get_time_start();
                  if (gg_hit_time > other_gg_hit_time) {
                    bool geiger_already_hit = true;
                    if (!geiger_hit.get_auxiliaries().has_flag("geiger_already_hit"))
                      geiger_hit.grab_auxiliaries().store("geiger_already_hit", geiger_already_hit);
                  } else {
                    bool geiger_already_hit = true;
                    if (!other_geiger_hit.get_auxiliaries().has_flag("geiger_already_hit"))
                      other_geiger_hit.grab_auxiliaries().store("geiger_already_hit",
                                                                geiger_already_hit);
                  }
                }  // end of if get_geom_id
              }    // end of jhit
            }      // end of ihit
            mctools::simulated_data::hit_handle_collection_type BSHC =
                flaged_sd.get_step_hits("gg");
            if (is_display) std::clog << "BSCH step hits # = " << BSHC.size() << std::endl;
            // for (mctools::simulated_data::hit_handle_collection_type::const_iterator i =
            // BSHC.begin();
            for (mctools::simulated_data::hit_handle_collection_type::const_iterator it =
                     BSHC.begin();
                 it != BSHC.end(); it++) {
              const mctools::base_step_hit& BSH = it->get();
              if (is_display) BSH.tree_dump(std::clog, "A Geiger Base Step Hit : ", "INFO : ");
              if (BSH.get_auxiliaries().has_flag("geiger_already_hit") ||
                  BSH.get_auxiliaries().has_flag("other_geiger_already_hit")) {
              } else {
                const geomtools::geom_id& geiger_gid = BSH.get_geom_id();

                int geiger_gid_layer = geiger_gid.get(2);
                int geiger_gid_row = geiger_gid.get(3) - geiger_hc_zone_inf_limit;

                if (my_hc_geiger_id_selector.match(geiger_gid)) {
                  if (is_display)
                    std::clog << "ID=" << geiger_gid << " matches the selector rules !"
                              << std::endl;

                  const bool is_in_map = hit_rate_map.find(geiger_gid) != hit_rate_map.end();

                  if (!is_in_map) {
                    hit_rate_map.insert(std::pair<geomtools::geom_id, int>(geiger_gid, 1));
                    geiger_cells_half_zone_distribution_TH2F->Fill(geiger_gid_row,
                                                                   geiger_gid_layer);
                  } else {
                    hit_rate_map.find(geiger_gid)->second++;
                    geiger_cells_half_zone_distribution_TH2F->Fill(geiger_gid_row,
                                                                   geiger_gid_layer);
                  }
                  write_event = true;
                }
              }
            }  // end of for
          }    // end of if has step hits "gg"
        }      // end of ER

        if (write_event == true) writer_1.process(ER);
        ER.clear();
        psd_count++;
        if (is_display) std::clog << "DEBUG : psd count " << psd_count << std::endl;
      }  // end of while reader

      std::map<geomtools::geom_id, int>::iterator it;

      if (is_display) {
        for (it = hit_rate_map.begin(); it != hit_rate_map.end(); it++) {
          std::clog << "Elements in mymap:" << std::endl;
          std::clog << "GID => " << it->first << " Nhits : " << it->second << std::endl;
        }
      }

      root_file->cd();
      calo_half_zone_energy_spectrum_TH1F->GetXaxis()->SetTitle("Energy [keV]");
      calo_half_zone_energy_spectrum_TH1F->Write();
      geiger_cells_half_zone_distribution_TH2F->Write();
      main_calo_half_zone_distribution_TH2F->Write();
      xwall_calo_half_zone_distribution_TH2F->Write();
      root_file->Write();
      root_file->Close();

    }  // end of for i = process size

    std::clog << "The end." << std::endl;
  }  // end of try

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

int column_to_hc_half_zone(const int& column_) {
  int hc_zone = -1;
  if (column_ >= 0 && column_ <= 5) {
    hc_zone = 0;
  }

  else if (column_ >= 6 && column_ <= 62) {
    hc_zone = (column_ + 3) / 6;
  }

  else if (column_ >= 63 && column_ <= 106) {
    hc_zone = (column_ + 4) / 6;
  } else if (column_ >= 107 && column_ <= 109) {
    hc_zone = 18;
  }

  else if (column_ >= 110 && column_ <= 112) {
    hc_zone = 19;
  }

  return hc_zone;
}
