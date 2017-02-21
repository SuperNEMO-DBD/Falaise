// Standard library:
#include <cstdlib>
#include <string>
#include <iostream>
#include <stdexcept>

// This project:
#include <fecom/hit_reader.hpp>
#include <fecom/commissioning_event_data.hpp>
#include <fecom/channel_mapping.hpp>

// - Bayeux/datatools:
#include <datatools/logger.h>
#include <datatools/utils.h>
#include <datatools/io_factory.h>
#include <datatools/clhep_units.h>

// Root :
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"


int main(int /*argc_*/, char ** /*argv_*/)
{
  datatools::logger::priority logging = datatools::logger::PRIO_DEBUG;
  try {
    DT_LOG_DEBUG(logging, "Entering test-libfecom-event_builder.cxx...");

    std::string input_filename("${FECOM_RESOURCES_DIR}/data/samples/run_1/Run_SN_Crate_Test_Tracker_And_Calo_Data_2_17_2017_Ascii.data.bz2");
    // std::string input_filename("${FECOM_RESOURCES_DIR}/data/samples/fake_run/calo_fake_tracker_hits_1.data.bz2");
    // std::string input_filename = "${FECOM_RESOURCES_DIR}/output_test/commissioning_event_10_events.data.bz2";
    datatools::fetch_path_with_env(input_filename);    // Deserialization of data :
    fecom::commissioning_event_data deserialize_commissioning_event_collection;
    {
      DT_LOG_DEBUG(logging, "Deserialize the commissioning event data...");
      datatools::data_reader deserializer(input_filename,
					  datatools::using_multiple_archives);

      deserializer.load(deserialize_commissioning_event_collection);
      DT_LOG_DEBUG(logging, "The commissioning event data has been loaded");
    }

    std::clog << "Size of deserialized commissioning event data = [" << deserialize_commissioning_event_collection.get_commissioning_event_collection().size() << "]" << std::endl;


    std::string input_tracker_mapping_file("${FECOM_RESOURCES_DIR}/data/samples/run_1/mapping_tracker.csv");
    datatools::fetch_path_with_env(input_tracker_mapping_file);

    std::string input_calo_mapping_file("${FECOM_RESOURCES_DIR}/data/samples/run_1/mapping_calo.csv");
    datatools::fetch_path_with_env(input_calo_mapping_file);


    // See if mapping is really useful ?
    fecom::channel_mapping my_channel_mapping;
    my_channel_mapping.build_tracker_mapping_from_file(input_tracker_mapping_file);
    my_channel_mapping.build_calo_mapping_from_file(input_calo_mapping_file);
    my_channel_mapping.initialize();


    std::string string_buffer = "${FECOM_RESOURCES_DIR}/output_test/test_histograms_main.root";
    datatools::fetch_path_with_env(string_buffer);
    TFile * main_root_file = new TFile(string_buffer.c_str(), "RECREATE");

    string_buffer = "${FECOM_RESOURCES_DIR}/output_test/test_histograms_calo.root";
    datatools::fetch_path_with_env(string_buffer);
    TFile * calo_root_file = new TFile(string_buffer.c_str(), "RECREATE");

    string_buffer = "${FECOM_RESOURCES_DIR}/output_test/test_histograms_tracker.root";
    datatools::fetch_path_with_env(string_buffer);
    TFile * tracker_root_file = new TFile(string_buffer.c_str(), "RECREATE");

    TH1F * raw_charge_trig_calo_channel_TH1F[fecom::calo_constants::NUMBERS_OF_CALO_PER_COLUMN];
    TH1F * raw_peak_calo_channel_TH1F[fecom::calo_constants::NUMBERS_OF_CALO_PER_COLUMN];

    TH1F * raw_charge_baseline_calo_channel_TH1F[fecom::calo_constants::NUMBERS_OF_CALO_PER_COLUMN];
    TH1F * raw_baseline_calo_channel_TH1F[fecom::calo_constants::NUMBERS_OF_CALO_PER_COLUMN];

    for (unsigned int icalo_channel = 0; icalo_channel < fecom::calo_constants::NUMBERS_OF_CALO_PER_COLUMN; icalo_channel++) {
      string_buffer = "raw_charge_trig_calo_channel_" + std::to_string(icalo_channel);
      raw_charge_trig_calo_channel_TH1F[icalo_channel] = new TH1F(string_buffer.c_str(),
								  Form("Raw charge triggered calorimeter %i", icalo_channel),
								  1000, 0, 40000);

      string_buffer = "raw_peak_calo_channel_" + std::to_string(icalo_channel);
      raw_peak_calo_channel_TH1F[icalo_channel] = new TH1F(string_buffer.c_str(),
							   Form("Raw peak calorimeter %i", icalo_channel),
							   1000, 0, 10000);

      string_buffer = "raw_charge_baseline_calo_channel_" + std::to_string(icalo_channel);
      raw_charge_baseline_calo_channel_TH1F[icalo_channel] = new TH1F(string_buffer.c_str(),
								      Form("Raw charge baseline calorimeter %i", icalo_channel),
								      100, 0, 2000);

      // To see with Jihanne : what is RawBaseline ?
      string_buffer = "raw_baseline_calo_channel_" + std::to_string(icalo_channel);
      raw_baseline_calo_channel_TH1F[icalo_channel] = new TH1F(string_buffer.c_str(),
							       Form("Raw baseline calorimeter %i", icalo_channel),
							       50, 0, 100);

    }

    string_buffer = "hit_calo_rate_TH2F";
    TH2F * hit_calo_rate_TH2F = new TH2F(string_buffer.c_str(),
					 Form("Hit Calo rate"),
					 20, 0, 20,
					 14, 0, 14);

    string_buffer = "hit_calo_trigger_rate_TH2F";
    TH2F * hit_calo_trigger_rate_TH2F = new TH2F(string_buffer.c_str(),
						 Form("Hit Calo rate if trigger"),
						 20, 0, 20,
						 14, 0, 14);


    string_buffer = "number_of_calos_TH1F";
    TH1F * number_of_calos_TH1F = new TH1F(string_buffer.c_str(),
					  Form("Number of calo (> HT) per event"),
					  13, 0, 13);


    // TH1F * hit_tracker_channel_TH1F[fecom::tracker_constants::NUMBER_OF_LAYERS][fecom::tracker_constants::NUMBER_OF_ROWS_PER_BOARD];
    // for (unsigned int ilayer = 0; ilayer < fecom::tracker_constants::NUMBER_OF_LAYERS; ilayer++)
    //   {
    // 	for (unsigned int irow = 0; irow < fecom::tracker_constants::NUMBER_OF_ROWS_PER_BOARD; irow++)
    // 	  {
    // 	    string_buffer = "hit_tracker_layer_" + std::to_string(ilayer) + "_row_" + std::to_string(irow);
    // 	    hit_tracker_channel_TH1F[ilayer][irow] = new TH1F(string_buffer.c_str(),
    // 							      Form("Hit tracker GG cell layer %i, row %i", ilayer, irow),
    // 							      5, 0, 5);
    // 	  }
    //   }

    // TH1F * hit_tracker_time_rate_TH1F[fecom::tracker_constants::NUMBER_OF_LAYERS][fecom::tracker_constants::NUMBER_OF_ROWS_PER_BOARD][fecom::tracker_constants::NUMBER_OF_TIMES];
    // for (unsigned int ilayer = 0; ilayer < fecom::tracker_constants::NUMBER_OF_LAYERS; ilayer++)
    //   {
    // 	for (unsigned int irow = 0; irow < fecom::tracker_constants::NUMBER_OF_ROWS_PER_BOARD; irow++)
    // 	  {
    // 	    for (unsigned int itime = 0; itime < fecom::tracker_constants::NUMBER_OF_TIMES; itime++)
    // 	      {
    // 		string_buffer = "hit_tracker_layer_" + std::to_string(ilayer) + "_row_" + std::to_string(irow) + "_timestamp_t" + std::to_string(itime);
    // 		hit_tracker_time_rate_TH1F[ilayer][irow][itime] = new TH1F(string_buffer.c_str(),
    // 									   Form("Hit tracker GG cell timestamp rate layer %i, row %i, timestamp %i", ilayer, irow, itime),
    // 									   5, 0, 5);
    // 	      }
    // 	  }
    //   }


    string_buffer = "number_of_tracker_TH1F";
    TH1F * number_of_tracker_TH1F = new TH1F(string_buffer.c_str(),
					     Form("Number of tracker cell hit per event"),
					     36, 0, 36);

    string_buffer = "number_of_tracker_1_calo_TH1F";
    TH1F * number_of_tracker_1_calo_TH1F = new TH1F(string_buffer.c_str(),
						    Form("Number of tracker cell hit per event if 1 calo trigger"),
						    36, 0, 36);

    string_buffer = "number_of_tracker_2_calos_TH1F";
    TH1F * number_of_tracker_2_calos_TH1F = new TH1F(string_buffer.c_str(),
						     Form("Number of tracker cell hit per event if 2 calos trigger"),
						     36, 0, 36);


    string_buffer = "number_of_tracker_3p_calos_TH1F";
    TH1F * number_of_tracker_3p_calos_TH1F = new TH1F(string_buffer.c_str(),
						      Form("Number of tracker cell hit per event if 3+ calos trigger"),
						      36, 0, 36);

    string_buffer = "hit_tracker_rate_total_TH2F";
    TH2F * hit_tracker_rate_total_TH2F = new TH2F(string_buffer.c_str(),
						  Form("Hit tracker GG cell total rate"),
						  5, 0, 5,
						  10, 0, 10);


    string_buffer = "hit_tracker_rate_if_0_calos_TH2F";
    TH2F * hit_tracker_rate_if_0_calos_TH2F = new TH2F(string_buffer.c_str(),
						       Form("Hit tracker GG cell rate if 0 calos"),
						       5, 0, 5,
						       10, 0, 10);

    string_buffer = "hit_tracker_rate_if_1_calos_TH2F";
    TH2F * hit_tracker_rate_if_1_calos_TH2F = new TH2F(string_buffer.c_str(),
						       Form("Hit tracker GG cell rate if 1 calos"),
						       5, 0, 5,
						       10, 0, 10);
    string_buffer = "hit_tracker_rate_if_2_calos_TH2F";
    TH2F * hit_tracker_rate_if_2_calos_TH2F = new TH2F(string_buffer.c_str(),
						       Form("Hit tracker GG cell rate if 2 calos"),
						       5, 0, 5,
						       10, 0, 10);
    string_buffer = "hit_tracker_rate_if_3p_calos_TH2F";
    TH2F * hit_tracker_rate_if_3p_calos_TH2F = new TH2F(string_buffer.c_str(),
						       Form("Hit tracker GG cell rate if 3+ calos"),
						       5, 0, 5,
						       10, 0, 10);

    string_buffer = "tracker_longitudinal_position_mean_all_cells_TH1F";
    TH1F * tracker_longitudinal_position_mean_all_cells_TH1F = new TH1F(string_buffer.c_str(),
									Form("Hit tracker Zrec longitudinal mean all cells"),
									300, -1500, 1500);

    std::size_t event_counter = 0;
    for (std::set<fecom::commissioning_event>::iterator it_event = deserialize_commissioning_event_collection.grab_commissioning_event_collection().begin();
    	 it_event !=  deserialize_commissioning_event_collection.grab_commissioning_event_collection().end();
    	 it_event++)
      {
	std::clog << "DEBUG : TEST HISTOGRAM BUILDER : TRIGGER_ID #" << it_event -> get_trigger_id() << std::endl;

	std::clog << "Size of calo hit in the event : " << it_event->get_calo_hit_collection().size() << std::endl;
	std::size_t chit_counter = 0;

	std::size_t calo_triggered = 0;
	for (auto it_chit = it_event->get_calo_hit_collection().begin();
	     it_chit != it_event->get_calo_hit_collection().end();
	     it_chit++)
	  {
	    // it_chit -> tree_dump(std::clog, "Calo hit #" + std::to_string(chit_counter));
	    geomtools::geom_id electronic_id = it_chit -> electronic_id;
	    geomtools::geom_id geometric_id;

	    my_channel_mapping.get_geometric_id_from_electronic_id(electronic_id,
								   geometric_id);

	    uint16_t column = geometric_id.get(fecom::calo_constants::COLUMN_INDEX);
	    uint16_t row = geometric_id.get(fecom::calo_constants::ROW_INDEX);
	    int32_t raw_charge =  it_chit -> raw_charge;
	    if (raw_charge < 0) raw_charge *= -1;
	    int16_t raw_peak = it_chit -> raw_peak;
	    if (raw_peak < 0) raw_peak *= -1;
	    int16_t raw_baseline = it_chit -> raw_baseline;
	    if (raw_baseline < 0) raw_baseline *= -1;

	    hit_calo_rate_TH2F->Fill(column, row);

	    if (it_chit -> low_threshold_trig_count >= 1)
	      {
		// Fill calo trigger histo
		hit_calo_trigger_rate_TH2F->Fill(column, row);
		raw_charge_trig_calo_channel_TH1F[row]->Fill(raw_charge);
		raw_peak_calo_channel_TH1F[row]->Fill(raw_peak);
		calo_triggered++;
	      }
	    else
	      {
		// Fill calo baseline histos
		raw_baseline_calo_channel_TH1F[row]->Fill(raw_baseline);
		raw_charge_baseline_calo_channel_TH1F[row]->Fill(raw_charge);
	      }
	    chit_counter++;
	  }
	number_of_calos_TH1F->Fill(calo_triggered);

	std::clog << "Size of tracker hit in the event : " << it_event->get_tracker_hit_collection().size() << std::endl;
	std::size_t thit_counter = 0;

	std::size_t tracker_triggered = 0;
	char display_matrix[fecom::tracker_constants::NUMBER_OF_ROWS_PER_BOARD][fecom::tracker_constants::NUMBER_OF_LAYERS];

	for (std::size_t irow = 0; irow < fecom::tracker_constants::NUMBER_OF_ROWS_PER_BOARD; irow++) {
	  for (std::size_t ilayer = 0; ilayer < fecom::tracker_constants::NUMBER_OF_LAYERS; ilayer++) {
	    display_matrix[irow][ilayer] = '.';
	  }
	}


	for (auto it_thit = it_event->get_tracker_hit_collection().begin();
	     it_thit != it_event->get_tracker_hit_collection().end();
	     it_thit++)
	  {
	    // it_thit -> tree_dump(std::clog, "Tracker hit #" + std::to_string(thit_counter));
	    uint16_t layer = it_thit -> cell_geometric_id.get(fecom::tracker_constants::LAYER_INDEX);
	    uint16_t row =  it_thit -> cell_geometric_id.get(fecom::tracker_constants::ROW_INDEX);
	    // hit_tracker_channel_TH1F[layer][row]->Fill(1);
	    display_matrix[row][layer] = '*';

	    hit_tracker_rate_total_TH2F->Fill(row, layer);

	    if (calo_triggered == 1)
	      {
		hit_tracker_rate_if_1_calos_TH2F->Fill(row, layer);
	      }
	    else if (calo_triggered == 2)
	      {
		hit_tracker_rate_if_2_calos_TH2F->Fill(row, layer);
	      }
	    else if (calo_triggered >= 3)
	      {
		hit_tracker_rate_if_3p_calos_TH2F->Fill(row, layer);
	      }
	    else
	      {
		// Strange behaviour, 0 calo X tracker is not possible (normally)
		// It's just for check
		hit_tracker_rate_if_0_calos_TH2F->Fill(row, layer);
	      }

	    if (it_thit -> has_bot_cathodic_time() && it_thit -> has_top_cathodic_time())
	      {
		// Longitudinal position : zrec = (Leff / 2) * ((t1 - t2) / (t1 + t2)) HDR C.Augier p.210
		// Top cathode position : + Leff / 2
		// Bot cathode position : - Leff / 2

		uint64_t bot_cathode_time = it_thit -> get_bot_cathodic_time();
		uint64_t top_cathode_time = it_thit -> get_top_cathodic_time();

		// See Jihanne for cathodic register in datas. R5 / R6

		double z_rec = (fecom::tracker_constants::geiger_cell_Leff() / (CLHEP::mm * 2)) * (int(bot_cathode_time - top_cathode_time)) / (bot_cathode_time + top_cathode_time);
		tracker_longitudinal_position_mean_all_cells_TH1F->Fill(z_rec);

		std::clog << "DEBUG : Time top cathode = " << top_cathode_time << " Time bot cathode = " <<  bot_cathode_time<< std::endl;
		std::clog << "DEBUG : Z_REC = " << z_rec << std::endl;
	      }

	    // if (it_thit -> has_anodic_t0()) hit_tracker_time_rate_TH1F[layer][row][0]->Fill(1);
	    // if (it_thit -> has_anodic_t1()) hit_tracker_time_rate_TH1F[layer][row][1]->Fill(1);
	    // if (it_thit -> has_anodic_t2()) hit_tracker_time_rate_TH1F[layer][row][2]->Fill(1);
	    // if (it_thit -> has_anodic_t3()) hit_tracker_time_rate_TH1F[layer][row][3]->Fill(1);
	    // if (it_thit -> has_anodic_t4()) hit_tracker_time_rate_TH1F[layer][row][4]->Fill(1);
	    // if (it_thit -> has_cathodic_t5()) hit_tracker_time_rate_TH1F[layer][row][5]->Fill(1);
	    // if (it_thit -> has_cathodic_t6()) hit_tracker_time_rate_TH1F[layer][row][6]->Fill(1);

	    tracker_triggered++;
	    thit_counter++;
	  } // end of it_hit

	number_of_tracker_TH1F->Fill(tracker_triggered);

	if (calo_triggered == 1)
	  {
	    number_of_tracker_1_calo_TH1F->Fill(tracker_triggered);
	  }
	else if (calo_triggered == 2)
	  {
	    number_of_tracker_2_calos_TH1F->Fill(tracker_triggered);
	  }
	else if (calo_triggered >= 3)
	  {
	    number_of_tracker_3p_calos_TH1F->Fill(tracker_triggered);
	  }
	else {}


	std::clog << "L |[CALO]" << std::endl;
	std::clog << "- |      " << std::endl;
	for (unsigned int ilayer = fecom::tracker_constants::NUMBER_OF_LAYERS - 1; ilayer != (unsigned) 0-1; ilayer--) {
	  for (std::size_t irow = 0; irow < fecom::tracker_constants::NUMBER_OF_ROWS_PER_BOARD; irow++) {
	    if (irow == 0) std::clog << ilayer << " | " << display_matrix[irow][ilayer];
	    else std::clog << display_matrix[irow][ilayer];
	  }
	  std::clog << std::endl;
	}
	std::clog << "------- tracker plate" << std::endl;
	std::clog << "    0123 row" << std::endl;

	event_counter++;
      }

    main_root_file->cd();
    hit_calo_rate_TH2F->Write();
    hit_calo_trigger_rate_TH2F->Write();
    number_of_calos_TH1F->Write();
    number_of_tracker_TH1F->Write();
    number_of_tracker_1_calo_TH1F->Write();
    number_of_tracker_2_calos_TH1F->Write();
    number_of_tracker_3p_calos_TH1F->Write();
    hit_tracker_rate_total_TH2F->Write();
    hit_tracker_rate_if_0_calos_TH2F->Write();
    hit_tracker_rate_if_1_calos_TH2F->Write();
    hit_tracker_rate_if_2_calos_TH2F->Write();
    hit_tracker_rate_if_3p_calos_TH2F->Write();
    tracker_longitudinal_position_mean_all_cells_TH1F->Write();
    main_root_file->Close();


    calo_root_file->cd();
    for (unsigned int icalo_channel = 0; icalo_channel < fecom::calo_constants::NUMBERS_OF_CALO_PER_COLUMN; icalo_channel++)
      {
	raw_charge_trig_calo_channel_TH1F[icalo_channel]->Write();
	raw_peak_calo_channel_TH1F[icalo_channel]->Write();
	raw_charge_baseline_calo_channel_TH1F[icalo_channel]->Write();
	raw_baseline_calo_channel_TH1F[icalo_channel] ->Write();
      }
    calo_root_file->Close();


    tracker_root_file->Close();

    DT_LOG_DEBUG(logging, "Exiting test-libfecom-histogram_builder.cxx...");

  } catch (std::exception & error) {
    std::cerr << "error: " << error.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
