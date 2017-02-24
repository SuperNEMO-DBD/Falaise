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

void get_associated_calorimeter_for_a_zrec(double z_rec,
					   std::vector<std::size_t> & associated_calos);

void get_calo_position(std::size_t calo_number,
		       double & z_begin,
		       double & z_end);

int main(int argc_, char ** argv_)
{
  // Parsing arguments
  int iarg = 1;
  std::string input_filename = "";
  std::string output_path = "";
  bool is_display      = false;
  bool is_help         = false;

  while (iarg < argc_) {
    std::string arg = argv_[iarg];

    if (arg == "-i" || arg == "--input") {
      input_filename = argv_[++iarg];
    }
    else if (arg == "-op" || arg == "--output-path") {
      output_path = argv_[++iarg];
    }

    else if (arg == "-d" || arg == "--display") {
      is_display = true;
    }

    else if (arg =="-h" || arg == "--help") {
      is_help = true;
    }

    else {
      std::cerr << "Unrecognised argument" << std::endl;
      is_help = true;
    }

    iarg++;
  }

  if (is_help)
    {
      std::cerr << std::endl << "Usage :" << std::endl << std::endl
		<< "$ BuildProducts/fecom_programs/fecom_main_decoder_serializer [OPTIONS] [ARGUMENTS]" << std::endl << std::endl
		<< "Allowed options: " << std::endl
		<< "-h    [ --help ]           produce help message" << std::endl
		<< "-i    [ --input ]          set an input file" << std::endl
		<< "-op   [ --output path ]    set a path where all files are store" << std::endl
		<< "-d    [ --display ]        display things for debug" << std::endl << std::endl;
      return 0;
    }

  datatools::logger::priority logging;
  if (is_display) logging = datatools::logger::PRIO_DEBUG;
  else logging = datatools::logger::PRIO_INFORMATION;

  try {
    DT_LOG_INFORMATION(logging, "Entering main_histogram_builder.cxx...");

    // Set the input serialized file available after run  main_decoder_serializer.cxx :
    if (input_filename.empty()) input_filename = "${FECOM_RESOURCES_DIR}/data/samples/fake_run/calo_fake_tracker_hits_1_serialized.data.bz2";
    datatools::fetch_path_with_env(input_filename);
    std::string input_file_basename = input_filename;
    std::size_t found = 0;
    found = input_file_basename.find_last_of("/");
    input_file_basename.erase(0, found+1);
    found = input_file_basename.find("_serialized.data.bz2");
    input_file_basename.erase(found, input_file_basename.size());
    std::string input_path = input_filename;
    found = input_path.find_last_of("/");
    input_path.erase(found+1, input_path.size());

    DT_LOG_INFORMATION(logging, "Input filename      : " + input_filename);
    DT_LOG_INFORMATION(logging, "Input path          : " + input_path);
    DT_LOG_INFORMATION(logging, "Input file basename : " + input_file_basename);

    DT_LOG_INFORMATION(logging, "Deserialization input file :" + input_filename);
    datatools::data_reader deserializer(input_filename,
					datatools::using_multiple_archives);

    std::string input_tracker_mapping_file = input_path + "mapping_tracker.csv";
    datatools::fetch_path_with_env(input_tracker_mapping_file);

    std::string input_calo_mapping_file = input_path + "mapping_calo.csv";
    datatools::fetch_path_with_env(input_calo_mapping_file);

    fecom::channel_mapping my_channel_mapping;
    my_channel_mapping.build_tracker_mapping_from_file(input_tracker_mapping_file);
    my_channel_mapping.build_calo_mapping_from_file(input_calo_mapping_file);
    my_channel_mapping.initialize();

    // Default output path in input path :
    if (output_path.empty())
      {
	output_path = input_path;
	DT_LOG_WARNING(logging, "The output path is empty, did you forget it in the option ? Default directory for output :" + output_path);
      }
    datatools::fetch_path_with_env(output_path);
    DT_LOG_INFORMATION(logging, "Output path : " + output_path);

    std::string output_root_filename = output_path + input_file_basename + "_main_histograms.root";
    DT_LOG_INFORMATION(logging, "Output main root filename :" + output_root_filename);
    TFile * main_root_file = new TFile(output_root_filename.c_str(), "RECREATE");

    output_root_filename = output_path + input_file_basename + "_calo_histograms.root";
    DT_LOG_INFORMATION(logging, "Output calo root filename :" + output_root_filename);
    TFile * calo_root_file = new TFile(output_root_filename.c_str(), "RECREATE");

    output_root_filename = output_path + input_file_basename + "_tracker_histograms.root";
    DT_LOG_INFORMATION(logging, "Output tracker root filename :" + output_root_filename);
    TFile * tracker_root_file = new TFile(output_root_filename.c_str(), "RECREATE");

    /************************************** HISTOS DECLARATION ****************************************************************/

    /************* Calo root file histos : *************/
    std::string string_buffer = "";
    TH1F * calo_raw_charge_trig_row_TH1F[fecom::calo_constants::NUMBERS_OF_CALO_PER_COLUMN];
    TH1F * calo_raw_peak_trig_row_TH1F[fecom::calo_constants::NUMBERS_OF_CALO_PER_COLUMN];
    TH1F * calo_raw_charge_no_trig_row_TH1F[fecom::calo_constants::NUMBERS_OF_CALO_PER_COLUMN];
    TH1F * calo_raw_baseline_no_trig_row_TH1F[fecom::calo_constants::NUMBERS_OF_CALO_PER_COLUMN];
    TH1F * calo_time_distribution_row_TH1F[fecom::calo_constants::NUMBERS_OF_CALO_PER_COLUMN];

    for (unsigned int icalo = 0; icalo < fecom::calo_constants::NUMBERS_OF_CALO_PER_COLUMN; icalo++) {
      string_buffer = "calo_raw_charge_trig_distribution_row_" + std::to_string(icalo);
      calo_raw_charge_trig_row_TH1F[icalo] = new TH1F(string_buffer.c_str(),
						      Form("Calorimeter raw charge if trigger row %i", icalo),
						      1000, 0, 40000);

      string_buffer = "calo_raw_peak_trig_distribution_row_" + std::to_string(icalo);
      calo_raw_peak_trig_row_TH1F[icalo] = new TH1F(string_buffer.c_str(),
						    Form("Calorimeter raw peak if trigger row %i", icalo),
						    1000, 0, 10000);

      string_buffer = "calo_raw_charge_no_trig_distribution_row_" + std::to_string(icalo);
      calo_raw_charge_no_trig_row_TH1F[icalo] = new TH1F(string_buffer.c_str(),
							    Form("Calorimeter raw charge if no trigger row %i", icalo),
							    100, 0, 2000);

      // To see with Jihanne : what is RawBaseline ?
      string_buffer = "calo_raw_baseline_no_trig_distribution_row_" + std::to_string(icalo);
      calo_raw_baseline_no_trig_row_TH1F[icalo] = new TH1F(string_buffer.c_str(),
							   Form("Calorimeter raw baseline if no trigger row %i", icalo),
							   50, 0, 100);

      string_buffer = "calo_time_distribution_row_" + std::to_string(icalo);
      calo_time_distribution_row_TH1F[icalo] = new TH1F(string_buffer.c_str(),
							Form("Calo time distribution (N / seconds) row %i", icalo),
							50, 0, 50);
    }

    /************* Main root file histos : *************/
    string_buffer = "calo_raw_total_charge_distribution_TH1F";
    TH1F * calo_raw_total_charge_TH1F = new TH1F(string_buffer.c_str(),
						 Form("Distribution total raw charge calo"),
						 1000, 0, 100000);

    string_buffer = "calo_raw_charge_trig_distribution_TH1F";
    TH1F * calo_raw_charge_trig_TH1F = new TH1F(string_buffer.c_str(),
						Form("Distribution triger raw charge calo (>HT)"),
						1000, 0, 100000);

    string_buffer = "calo_raw_charge_distribution_two_calos_TH1F";
    TH1F * calo_raw_charge_two_calos_TH1F = new TH1F(string_buffer.c_str(),
						     Form("Distribution raw charge if 2 calos (>HT)"),
						     1000, 0, 100000);

    string_buffer = "calo_delta_time_distribution_two_calos_TH1F";
    TH1F * calo_delta_time_two_calos_TH1F = new TH1F(string_buffer.c_str(),
						     Form("Distribution delta time if 2 calos (>HT)"),
						     100, 0, 10);

    string_buffer = "calo_raw_total_peak_distribution_TH1F";
    TH1F * calo_raw_total_peak_TH1F = new TH1F(string_buffer.c_str(),
					       Form("Distribution total raw peak calo"),
					       1000, 0, 30000);

    string_buffer = "calo_raw_peak_two_calos_distribution_TH1F";
    TH1F * calo_raw_peak_two_calos_TH1F = new TH1F(string_buffer.c_str(),
						   Form("Distribution raw peak if 2 calos (>HT)"),
						   1000, 0, 30000);

    string_buffer = "calo_raw_peak_trig_distribution_TH1F";
    TH1F * calo_raw_peak_trig_TH1F = new TH1F(string_buffer.c_str(),
					      Form("Distribution triger raw peak calo"),
					      1000, 0, 30000);

    string_buffer = "hit_calo_trigger_count_TH2F";
    TH2F * hit_calo_trigger_count_TH2F = new TH2F(string_buffer.c_str(),
						  Form("Hit Calo count if trigger"),
						  20, 0, 20,
						  14, 0, 14);


    string_buffer = "number_of_calos_distribution_TH1F";
    TH1F * number_of_calos_TH1F = new TH1F(string_buffer.c_str(),
					   Form("Distribution of calo (> HT) per event"),
					   13, 0, 13);


    TH1F * tracker_time_distribution_TH1F[fecom::tracker_constants::NUMBER_OF_LAYERS][fecom::tracker_constants::NUMBER_OF_ROWS_PER_BOARD];
    for (unsigned int ilayer = 0; ilayer < fecom::tracker_constants::NUMBER_OF_LAYERS; ilayer++)
      {
    	for (unsigned int irow = 0; irow < fecom::tracker_constants::NUMBER_OF_ROWS_PER_BOARD; irow++)
    	  {
    	    string_buffer = "tracker_time_distribution_layer_" + std::to_string(ilayer) + "_row_" + std::to_string(irow);
    	    tracker_time_distribution_TH1F[ilayer][irow] = new TH1F(string_buffer.c_str(),
								    Form("Tracker time distribution (N / seconds) cell layer %i, row %i", ilayer, irow),
								    50, 0, 50);
    	  }
      }

    string_buffer = "number_of_tracker_distribution_TH1F";
    TH1F * number_of_tracker_TH1F = new TH1F(string_buffer.c_str(),
					     Form("Distribution of tracker cell hit per event"),
					     36, 0, 36);

    string_buffer = "number_of_tracker_1_calo_distribution_TH1F";
    TH1F * number_of_tracker_1_calo_TH1F = new TH1F(string_buffer.c_str(),
						    Form("Distribution of tracker cell hit per event if 1 calo trigger"),
						    36, 0, 36);

    string_buffer = "number_of_tracker_2_calos_distribution_TH1F";
    TH1F * number_of_tracker_2_calos_TH1F = new TH1F(string_buffer.c_str(),
						     Form("Distribution of tracker cell hit per event if 2 calos trigger"),
						     36, 0, 36);


    string_buffer = "number_of_tracker_3p_calos_distribution_TH1F";
    TH1F * number_of_tracker_3p_calos_TH1F = new TH1F(string_buffer.c_str(),
						      Form("Distribution of tracker cell hit per event if 3+ calos trigger"),
						      36, 0, 36);

    string_buffer = "hit_tracker_count_total_TH2F";
    TH2F * hit_tracker_count_total_TH2F = new TH2F(string_buffer.c_str(),
						   Form("Hit tracker GG cell total count"),
						   5, 0, 5,
						   10, 0, 10);

    string_buffer = "tracker_layer_distribution_TH1F";
    TH1F * tracker_layer_distribution_TH1F = new TH1F(string_buffer.c_str(),
						      Form("Tracker hit layers distribution"),
						      11, 0, 11);

    string_buffer = "tracker_bot_cathode_efficiency_TH2F";
    TH2F * tracker_bot_cathode_efficiency_TH2F = new TH2F(string_buffer.c_str(),
							  Form("Bottom cathode efficiency (in %%) "),
							  5, 0, 5,
							  10, 0, 10);

    string_buffer = "tracker_top_cathode_efficiency_TH2F";
    TH2F * tracker_top_cathode_efficiency_TH2F = new TH2F(string_buffer.c_str(),
							  Form("Top cathode efficiency (in %%) "),
							  5, 0, 5,
							  10, 0, 10);


    string_buffer = "hit_tracker_count_if_0_calos_TH2F";
    TH2F * hit_tracker_count_if_0_calos_TH2F = new TH2F(string_buffer.c_str(),
							Form("Hit tracker GG cell count if 0 calos"),
							5, 0, 5,
						       10, 0, 10);

    string_buffer = "hit_tracker_count_if_1_calos_TH2F";
    TH2F * hit_tracker_count_if_1_calos_TH2F = new TH2F(string_buffer.c_str(),
						       Form("Hit tracker GG cell count if 1 calos"),
						       5, 0, 5,
						       10, 0, 10);
    string_buffer = "hit_tracker_count_if_2_calos_TH2F";
    TH2F * hit_tracker_count_if_2_calos_TH2F = new TH2F(string_buffer.c_str(),
						       Form("Hit tracker GG cell count if 2 calos"),
						       5, 0, 5,
						       10, 0, 10);
    string_buffer = "hit_tracker_count_if_3p_calos_TH2F";
    TH2F * hit_tracker_count_if_3p_calos_TH2F = new TH2F(string_buffer.c_str(),
							Form("Hit tracker GG cell count if 3+ calos"),
							5, 0, 5,
							10, 0, 10);

    string_buffer = "tracker_longitudinal_position_distribution_all_cells_TH1F";
    TH1F * tracker_longitudinal_position_distribution_all_cells_TH1F = new TH1F(string_buffer.c_str(),
									Form("Hit tracker Zrec longitudinal mean all cells"),
									300, -1500, 1500);
    /*************************************************************************************************************************/

    std::size_t event_counter = 0;
    const double CLOCK_FREQUENCY = 80e6;
    const double CLOCK_TIME = 1 / CLOCK_FREQUENCY;

    while (deserializer.has_record_tag())
      {
	if (deserializer.record_tag_is(fecom::commissioning_event::SERIAL_TAG)) {
	  fecom::commissioning_event deserialized_com_event;
	  deserializer.load(deserialized_com_event);

	  DT_LOG_DEBUG(logging, "Deserialized com event #" << event_counter);
	  // deserialized_com_event.tree_dump(std::clog, "Deserialized commissioning event ");

	  DT_LOG_DEBUG(logging, "Main histogram builder : Trigger ID #" << deserialized_com_event.get_trigger_id());
	  DT_LOG_DEBUG(logging, "Size of calo hit in the event : " << deserialized_com_event.get_calo_hit_collection().size());

	  std::size_t chit_counter = 0;
	  std::size_t calo_triggered = 0;

	  uint64_t total_charge = 0;
	  uint64_t total_charge_if_trig = 0;
	  uint64_t total_peak = 0;
	  uint64_t total_peak_if_trig = 0;
	  double time_calo_1 = -1;
	  double time_calo_2 = -1;

	  for (auto it_chit = deserialized_com_event.get_calo_hit_collection().begin();
	       it_chit != deserialized_com_event.get_calo_hit_collection().end();
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

	      // hit_calo_count_TH2F->Fill(column, row);
	      total_charge += raw_charge;
	      total_peak += raw_peak;

	      if (it_chit -> low_threshold_trig_count >= 1)
		{
		  total_charge_if_trig += raw_charge;
		  total_peak_if_trig += raw_peak;

		  // Fill calo trigger histo
		  calo_raw_charge_trig_row_TH1F[row]->Fill(raw_charge);
		  calo_raw_peak_trig_row_TH1F[row]->Fill(raw_peak);

		  double calo_time_in_ns = it_chit -> tdc_ns * 1e-9;
		  calo_time_distribution_row_TH1F[row]->Fill(calo_time_in_ns);

		  hit_calo_trigger_count_TH2F->Fill(column, row);

		  if (calo_triggered == 0) time_calo_1 = calo_time_in_ns;
		  else if (calo_triggered == 1) time_calo_2 = calo_time_in_ns;

		  calo_triggered++;
		}
	      else
		{
		  // Fill calo baseline histos
		  calo_raw_charge_no_trig_row_TH1F[row]->Fill(raw_charge);
		  calo_raw_baseline_no_trig_row_TH1F[row]->Fill(raw_baseline);
		}
	      chit_counter++;
	    }
	  number_of_calos_TH1F->Fill(calo_triggered);

	  DT_LOG_DEBUG(logging, "Size of tracker hit in the event : " << deserialized_com_event.get_tracker_hit_collection().size());
	  std::size_t thit_counter = 0;

	  std::size_t tracker_triggered = 0;
	  char display_matrix[fecom::tracker_constants::NUMBER_OF_ROWS_PER_BOARD][fecom::tracker_constants::NUMBER_OF_LAYERS];

	  for (std::size_t irow = 0; irow < fecom::tracker_constants::NUMBER_OF_ROWS_PER_BOARD; irow++) {
	    for (std::size_t ilayer = 0; ilayer < fecom::tracker_constants::NUMBER_OF_LAYERS; ilayer++) {
	      display_matrix[irow][ilayer] = '.';
	    }
	  }

	  std::bitset<fecom::tracker_constants::NUMBER_OF_LAYERS> tracker_layers_hit (0x0);

	  std::vector<double> cells_last_layer_z_reco;

	  for (auto it_thit = deserialized_com_event.get_tracker_hit_collection().begin();
	       it_thit != deserialized_com_event.get_tracker_hit_collection().end();
	       it_thit++)
	    {
	      // it_thit -> tree_dump(std::clog, "Tracker hit #" + std::to_string(thit_counter));
	      uint16_t layer = it_thit -> cell_geometric_id.get(fecom::tracker_constants::LAYER_INDEX);
	      uint16_t row =  it_thit -> cell_geometric_id.get(fecom::tracker_constants::ROW_INDEX);
	      // hit_tracker_channel_TH1F[layer][row]->Fill(1);
	      display_matrix[row][layer] = '*';

	      tracker_layers_hit.set(layer, true);

	      hit_tracker_count_total_TH2F->Fill(row, layer);

	      if (calo_triggered == 1)
		{
		  hit_tracker_count_if_1_calos_TH2F->Fill(row, layer);
		}
	      else if (calo_triggered == 2)
		{
		  hit_tracker_count_if_2_calos_TH2F->Fill(row, layer);
		}
	      else if (calo_triggered >= 3)
		{
		  hit_tracker_count_if_3p_calos_TH2F->Fill(row, layer);
		}
	      else
		{
		  // Strange behaviour, 0 calo X tracker is not possible (normally)
		  // It's just for check
		  hit_tracker_count_if_0_calos_TH2F->Fill(row, layer);
		}

	      if (it_thit -> has_bot_cathodic_time() && it_thit -> has_top_cathodic_time())
		{
		  // Longitudinal position : zrec = (Leff / 2) * ((t1 - t2) / (t1 + t2)) HDR C.Augier p.210
		  // Top cathode position : + Leff / 2
		  // Bot cathode position : - Leff / 2

		  double bot_cathode_time = (it_thit -> get_bot_cathodic_time() - it_thit -> anodic_t0);
		  double top_cathode_time = (it_thit -> get_top_cathodic_time() - it_thit -> anodic_t0);

		  // See Jihanne for cathodic register in datas. R5 / R6
		  double z_rec = (fecom::tracker_constants::geiger_cell_Leff() / (CLHEP::mm * 2)) * (int(bot_cathode_time - top_cathode_time)) / (bot_cathode_time + top_cathode_time);
		  // double z_rec = -1;
		  tracker_longitudinal_position_distribution_all_cells_TH1F->Fill(z_rec);

		  DT_LOG_DEBUG(logging, "Time top cathode = " << top_cathode_time << " Time bot cathode = " <<  bot_cathode_time);
		  DT_LOG_DEBUG(logging, "Z_Reconstructed = " << z_rec << " mm");

		  if (layer == fecom::tracker_constants::NUMBER_OF_LAYERS - 1)
		    {
		      cells_last_layer_z_reco.push_back(z_rec);
		    }
		}

	      double anodic_t0_ns = -1;
	      // double anodic_t1_ns = -1;
	      // double anodic_t2_ns = -1;
	      // double anodic_t3_ns = -1;
	      // double anodic_t4_ns = -1;
	      // double bot_cathodic_time = -1;
	      // double top_cathodic_time  = -1;

	      if (it_thit -> has_anodic_t0()) anodic_t0_ns = it_thit -> anodic_t0 * CLOCK_TIME;
	      // if (it_thit -> has_anodic_t1()) anodic_t1_ns = it_thit->anodic_t1 * CLOCK_TIME;
	      // if (it_thit -> has_anodic_t2()) anodic_t2_ns = it_thit->anodic_t2 * CLOCK_TIME;
	      // if (it_thit -> has_anodic_t3()) anodic_t3_ns = it_thit->anodic_t3 * CLOCK_TIME;
	      // if (it_thit -> has_anodic_t4()) anodic_t4_ns = it_thit->anodic_t4 * CLOCK_TIME;
	      // if (it_thit -> has_bot_cathodic_time()) bot_cathodic_time = it_thit->bot_cathodic_time * CLOCK_TIME;
	      // if (it_thit -> has_top_cathodic_time()) top_cathodic_time = it_thit->top_cathodic_time * CLOCK_TIME;

	      if (it_thit -> has_bot_cathodic_time()) tracker_bot_cathode_efficiency_TH2F->Fill(row, layer);
	      if (it_thit -> has_top_cathodic_time()) tracker_top_cathode_efficiency_TH2F->Fill(row, layer);

	      tracker_time_distribution_TH1F[layer][row]->Fill(anodic_t0_ns);

	      tracker_triggered++;
	      thit_counter++;
	    } // end of it_hit

	  // If full track :
	  if (tracker_layers_hit.count() == fecom::tracker_constants::NUMBER_OF_LAYERS)
	    {
	      DT_LOG_DEBUG(logging, "Vector size of last layer Z :" << cells_last_layer_z_reco.size());

	      for (auto it_z_vector = cells_last_layer_z_reco.begin();
		   it_z_vector != cells_last_layer_z_reco.end();
		   it_z_vector++)
		{
		  std::vector<std::size_t> associated_calo_vector;

		  get_associated_calorimeter_for_a_zrec(*it_z_vector,
							associated_calo_vector);

		  double first_calo_z_begin = 999999;
		  double first_calo_z_end = 999999;
		  get_calo_position(associated_calo_vector[0],
				    first_calo_z_begin,
				    first_calo_z_end);
		  DT_LOG_DEBUG(logging, "Calo #" << associated_calo_vector[0] << " Zbegin = " << first_calo_z_begin << " Zend = " << first_calo_z_end);

		  if (associated_calo_vector.size() == 2)
		    {


		    }

		}

	    }


	  tracker_layer_distribution_TH1F->Fill(tracker_layers_hit.count());

	  number_of_tracker_TH1F->Fill(tracker_triggered);

	  calo_raw_total_charge_TH1F->Fill(total_charge);
	  calo_raw_charge_trig_TH1F->Fill(total_charge_if_trig);
	  calo_raw_total_peak_TH1F->Fill(total_peak);
	  calo_raw_peak_trig_TH1F->Fill(total_peak_if_trig);

	  if (calo_triggered == 1)
	    {
	      number_of_tracker_1_calo_TH1F->Fill(tracker_triggered);
	    }
	  else if (calo_triggered == 2)
	    {
	      calo_raw_charge_two_calos_TH1F->Fill(total_charge_if_trig);
	      calo_delta_time_two_calos_TH1F->Fill(std::abs(time_calo_2 - time_calo_1) / CLOCK_TIME);
	      calo_raw_peak_two_calos_TH1F->Fill(total_peak_if_trig);
	      number_of_tracker_2_calos_TH1F->Fill(tracker_triggered);
	    }
	  else if (calo_triggered >= 3)
	    {
	      number_of_tracker_3p_calos_TH1F->Fill(tracker_triggered);
	    }
	  else {}

	  if (is_display) {
	    std::clog << "Display tracker event #" << deserialized_com_event.get_trigger_id() << std::endl;
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
	  }
	  event_counter++;

	} // end of if record tag is com event

      } // end of has record tag


    // Fill main ROOT file :
    main_root_file->cd();
    number_of_calos_TH1F->Write();
    number_of_tracker_TH1F->Write();
    number_of_tracker_1_calo_TH1F->Write();
    number_of_tracker_2_calos_TH1F->Write();
    number_of_tracker_3p_calos_TH1F->Write();
    hit_calo_trigger_count_TH2F->Write();
    calo_raw_total_charge_TH1F->Write();
    calo_raw_charge_trig_TH1F->Write();
    calo_raw_total_peak_TH1F->Write();
    calo_raw_peak_trig_TH1F->Write();
    calo_raw_charge_two_calos_TH1F->Write();
    calo_raw_peak_two_calos_TH1F->Write();
    calo_delta_time_two_calos_TH1F->Write();
    hit_tracker_count_total_TH2F->Write();
    tracker_layer_distribution_TH1F->Write();
    hit_tracker_count_if_0_calos_TH2F->Write();
    hit_tracker_count_if_1_calos_TH2F->Write();
    hit_tracker_count_if_2_calos_TH2F->Write();
    hit_tracker_count_if_3p_calos_TH2F->Write();
    tracker_longitudinal_position_distribution_all_cells_TH1F->Write();
    tracker_bot_cathode_efficiency_TH2F->Scale(100./event_counter);
    tracker_bot_cathode_efficiency_TH2F->Write();
    tracker_top_cathode_efficiency_TH2F->Scale(100./event_counter);
    tracker_top_cathode_efficiency_TH2F->Write();

    main_root_file->Close();

    // Fill calo ROOT file :
    calo_root_file->cd();
    for (unsigned int icalo = 0; icalo < fecom::calo_constants::NUMBERS_OF_CALO_PER_COLUMN; icalo++)
      {
	calo_raw_charge_trig_row_TH1F[icalo]->Write();
	calo_raw_peak_trig_row_TH1F[icalo]->Write();
	calo_raw_charge_no_trig_row_TH1F[icalo]->Write();
	calo_raw_baseline_no_trig_row_TH1F[icalo]->Write();
	calo_time_distribution_row_TH1F[icalo]->Write();
      }
    calo_root_file->Close();

    // Fill tracker ROOT file :
    tracker_root_file->cd();
    for (unsigned int ilayer = 0; ilayer < fecom::tracker_constants::NUMBER_OF_LAYERS; ilayer++)
      {
    	for (unsigned int irow = 0; irow < fecom::tracker_constants::NUMBER_OF_ROWS_PER_BOARD; irow++)
    	  {
	    tracker_time_distribution_TH1F[ilayer][irow]->Write();
	  }
      }
    tracker_root_file->Close();

    DT_LOG_INFORMATION(logging, "Exiting main_histogram_builder.cxx...");
    DT_LOG_INFORMATION(logging, "EXIT_STATUS : SUCCESS");

  } catch (std::exception & error) {
    std::cerr << "error: " << error.what() << std::endl;
    DT_LOG_FATAL(logging, "EXIT_STATUS : FAILURE");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}


void get_associated_calorimeter_for_a_zrec(double z_rec,
					   std::vector<std::size_t> & associated_calos)
{
  // Z_rec in mm
  const double om_size = 259; // mm

  double om_fraction_incertitude_association = 0.15; // ex : 0.15 * 259 = +- 38.85 mm for calorimeter association

  double calo_position_shifted = (z_rec + (om_size / 2)) / om_size;
  double decimal_fraction =  std::abs(calo_position_shifted) - std::floor(std::abs(calo_position_shifted));

  std::size_t first_associated_calo = 6 + std::floor(calo_position_shifted);

  associated_calos.push_back(first_associated_calo);

  if (z_rec >= 0)
    {
      if (decimal_fraction > 1 - om_fraction_incertitude_association)
	{
	  // Incertitude on calorimeter, add the next one (+1) in the potential associate calo
	  std::size_t second_associated_calos = first_associated_calo + 1;
	  associated_calos.push_back(second_associated_calos);
	}
      else if (decimal_fraction < om_fraction_incertitude_association)
	{
	  // Incertitude on calorimeter, add the previous one (-1) in the potential associate calo
	  std::size_t second_associated_calos = first_associated_calo - 1;
	  associated_calos.push_back(second_associated_calos);
	}
    }
  else
    {
      if (decimal_fraction > 1 - om_fraction_incertitude_association)
	{
	  // Incertitude on calorimeter, add the previous one (-1) in the potential associate calo
	  std::size_t second_associated_calos = first_associated_calo - 1;
	  associated_calos.push_back(second_associated_calos);
	}
      else if (decimal_fraction < om_fraction_incertitude_association)
	{
	  // Incertitude on calorimeter, add the next one (+1) in the potential associate calo
	  std::size_t second_associated_calos = first_associated_calo + 1;
	  associated_calos.push_back(second_associated_calos);
	}
    }

  return;
}

void get_calo_position(std::size_t calo_number,
		       double & z_begin,
		       double & z_end)
{
  const double om_size = 259; // mm
  const double z_min = -((om_size / 2) + (om_size * 6));

  z_begin = z_min + om_size * calo_number;
  z_end = z_min + om_size * (calo_number + 1);

  return;
}
