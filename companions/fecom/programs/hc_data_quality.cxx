// Standard library:
#include <cstdlib>
#include <string>
#include <iostream>
#include <stdexcept>

// Third party:
// - Boost:
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/operations.hpp>

// - Bayeux/datatools:
#include <datatools/logger.h>
#include <datatools/exception.h>
#include <datatools/utils.h>
#include <datatools/io_factory.h>
#include <datatools/clhep_units.h>
#include <datatools/things.h>
// - Bayeux/dpp:
#include <dpp/input_module.h>
#include <dpp/output_module.h>

// - Bayeux/mygsl:
#include <mygsl/histogram.h>

// This project:
#include <fecom/commissioning_event.hpp>
#include <fecom/channel_mapping.hpp>

int main(int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  std::string input_filename = "";
  std::string output_filename = "";
  std::string input_tracker_mapping_file = "";
  std::string input_calo_mapping_file = "";
  std::size_t max_events     = 0;
  bool        is_debug       = false;
  bool        is_help        = false;

  try {
    // Parsing arguments:
    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];

      if (arg == "-i" || arg == "--input") {
        input_filename = argv_[++iarg];
      }

      else if (arg == "-o" || arg == "--output") {
        output_filename = argv_[++iarg];
      }

      else if (arg == "-cm" || arg == "--calo-map") {
	input_calo_mapping_file = argv_[++iarg];
      }

      else if (arg == "-tm" || arg == "--tracker-map") {
	input_tracker_mapping_file = argv_[++iarg];
      }

      else if (arg == "-M" || arg == "--max-events") {
        max_events = boost::lexical_cast<std::size_t>(argv_[++iarg]);
      }

      else if (arg == "-d" || arg == "--debug") {
        is_debug = true;
      }

      else if (arg == "-h" || arg == "--help") {
        is_help = true;
      }

      else {
        DT_THROW(std::logic_error, "Unrecognised argument '" << arg << "'!");
      }

      iarg++;
    }

    if (is_help) {
      std::cerr << std::endl << "Usage :" << std::endl << std::endl
                << "fecom-hc_data_quality [OPTIONS]" << std::endl << std::endl
                << "Allowed options: " << std::endl
                << "-h    [ --help ]        produce help message" << std::endl
                << "-i    [ --input ] file  set an input file" << std::endl
                << "-o    [ --output ] file set an output file" << std::endl
                << "-d    [ --debug ]       debug mode" << std::endl << std::endl;
      return 0;
    }

    datatools::logger::priority logging;
    if (is_debug) logging = datatools::logger::PRIO_DEBUG;
    else logging = datatools::logger::PRIO_INFORMATION;
    DT_LOG_INFORMATION(logging, "Entering hc_data_quality.cxx...");

    if (input_filename.empty()) input_filename = "${FECOM_RESOURCES_DIR}/data/samples/fake_run/calo_fake_tracker_hits_1.dat";
    datatools::fetch_path_with_env(input_filename);
    std::string input_path = input_filename;
    std::size_t found = input_path.find_last_of("/");
    input_path.erase(found+1, input_path.size());

    std::string output_path = input_path;
    if (output_filename.empty()) {
      datatools::fetch_path_with_env(output_path);
      output_filename = output_path + "output_hc_data_quality.txt";
    }
    datatools::fetch_path_with_env(output_filename);

    DT_LOG_INFORMATION(logging, "Input filename           : " + input_filename)
      DT_LOG_INFORMATION(logging, "Input path               : " + input_path);
    DT_LOG_INFORMATION(logging, "Data quality output file : " + output_filename);
    DT_LOG_INFORMATION(logging, "Output path              : " + output_path);

    // Event reader :
    dpp::input_module reader;
    datatools::properties reader_config;
    reader_config.store ("logging.priority", "debug");
    reader_config.store ("max_record_total", static_cast<int>(max_events));
    reader_config.store ("files.mode", "single");
    reader_config.store_path("files.single.filename", input_filename);
    reader.initialize_standalone (reader_config);

    if (input_tracker_mapping_file.empty()) {
      input_tracker_mapping_file = input_path + '/' + "mapping_tracker.csv";
    }
    datatools::fetch_path_with_env(input_tracker_mapping_file);
    if (input_calo_mapping_file.empty()) {
      input_calo_mapping_file = input_path + '/' + "mapping_calo.csv";
    }
    datatools::fetch_path_with_env(input_calo_mapping_file);

    DT_LOG_INFORMATION(logging, "Mapping tracker file :" + input_tracker_mapping_file);
    DT_LOG_INFORMATION(logging, "Mapping calo file :" + input_calo_mapping_file);

    // Channel mapping :
    fecom::channel_mapping my_channel_mapping;
    my_channel_mapping.build_tracker_mapping_from_file(input_tracker_mapping_file);
    my_channel_mapping.build_calo_mapping_from_file(input_calo_mapping_file);
    my_channel_mapping.initialize();

    // Data quality :
    // std::size_t number_of_cathode_wo_anode = 0;
    // std::size_t number_of_cathode_wo_anode_in_event = 0;

    mygsl::histogram delta_t_calo_tref(100, 0., 10.);
    mygsl::histogram delta_t_anode_tref(100, 0., 150000.);
    mygsl::histogram delta_t_cathode_tref(100, 0., 150000.);
    mygsl::histogram delta_t_anode_anode(100, 0., 100.);
    mygsl::histogram delta_t_cathode_cathode(100, 0., 100.);
    mygsl::histogram delta_t_anode_cathode_same_hit(100, 0., 20000.);
    // h0.fill (0.5, -5.0);

    // General statistics :
    uint64_t event_number = 0;
    uint64_t event_only_calo = 0;
    uint64_t event_only_tracker = 0;
    uint64_t event_calo_tracker = 0;

    // Calo statistics :
    uint64_t calo_hit_number = 0;

    // Tracker statistics :
    uint64_t tracker_hit_number = 0;
    // uint64_t anode_only_counter = 0;
    uint64_t anode_t0_counter = 0;
    uint64_t anode_t0_only_counter = 0;
    uint64_t bot_cathode_counter = 0;
    uint64_t bot_cathode_only_counter = 0;

    // Calo tracker statistics :


    // Event record :
    datatools::things ER;

    // Simulated Data "SD" bank label :
    std::string HCRD_bank_label = "HCRD";

    int modulo = 1000;

    DT_LOG_INFORMATION(logging, "Reading commisioning events...");
    DT_LOG_INFORMATION(logging, "...");
    while (!reader.is_terminated()) {
      if ((event_number % modulo) == 0){
	DT_LOG_DEBUG(logging, "Event number = " << event_number);
      }
      reader.process(ER);
      // A plain `fecom::commissioning' object is stored here :
      if (ER.has(HCRD_bank_label) && ER.is_a<fecom::commissioning_event>(HCRD_bank_label))
	{
	  const fecom::commissioning_event & CE = ER.get<fecom::commissioning_event>(HCRD_bank_label);
	  // Fill data quality :
	  double event_time_ref = CE.get_time_start_ns();

	  if (CE.is_only_calo()) event_only_calo++;
	  else if (CE.is_only_tracker()) event_only_tracker++;
	  else event_calo_tracker++;

	  if (CE.has_calo_hits()
	      && CE.get_calo_hit_collection().size() > 1) {
	    for (auto icalo = CE.get_calo_hit_collection().begin();
		 icalo != CE.get_calo_hit_collection().end();
		 icalo++) {
	      if (icalo->low_threshold || icalo->high_threshold) {
		double DT_calo_tref = icalo->tdc_ns + icalo->falling_time_ns - event_time_ref;
		if (DT_calo_tref != 0) delta_t_calo_tref.fill(DT_calo_tref);
		calo_hit_number++;
	      }
	    } // end of icalo
	  }// end of if has calo hits

	  if (CE.has_tracker_hits()
	      && !CE.is_only_tracker()) {
	    // First loop to search anode minimum and cathode minimum :
	    double anode_min_time_ns = 0;
	    double cathode_min_time_ns = 0;
	    for (auto itrack = CE.get_tracker_hit_collection().begin();
		 itrack != CE.get_tracker_hit_collection().end();
		 itrack++) {
	      if (itrack->has_anodic_t0()) {
		if (anode_min_time_ns == 0 || itrack->anodic_t0_ns < anode_min_time_ns) anode_min_time_ns = itrack->anodic_t0_ns;
	      }
	      if (itrack->has_bot_cathodic_time()) {
		if (cathode_min_time_ns == 0 || itrack->bot_cathodic_time_ns < cathode_min_time_ns) cathode_min_time_ns = itrack->bot_cathodic_time_ns;
	      }
	    } // end of itrack

	    for (auto itrack = CE.get_tracker_hit_collection().begin();
		 itrack != CE.get_tracker_hit_collection().end();
		 itrack++) {

	      if (itrack->has_anodic_t0()) {

		double DT_anode_tref = itrack->anodic_t0_ns - event_time_ref;
		if (DT_anode_tref != 0) {
		  DT_LOG_DEBUG(logging, "DT(anode-tref) = " << DT_anode_tref);
		  delta_t_anode_tref.fill(DT_anode_tref);
		}

		double DT_anode_anode = itrack->anodic_t0_ns - anode_min_time_ns;
		if (DT_anode_anode != 0) {
		  DT_LOG_DEBUG(logging, "DT(anode-anode)=" << DT_anode_anode);
		  delta_t_anode_anode.fill(DT_anode_anode);
		}
		anode_t0_counter++;
		if (!itrack->has_bot_cathodic_time()) anode_t0_only_counter++;
	      }

	      if (itrack->has_bot_cathodic_time()) {

		double DT_cathode_tref = itrack->bot_cathodic_time_ns - event_time_ref;
		DT_LOG_DEBUG(logging, "DT(cathode-tref)=" << DT_cathode_tref);
		delta_t_cathode_tref.fill(DT_cathode_tref);

		if (itrack->has_anodic_t0()) {
		  double DT_cathode_anode_same_hit = itrack->bot_cathodic_time_ns - itrack->anodic_t0_ns;
		  DT_LOG_DEBUG(logging,"DT(cathode-anode)=" << DT_cathode_anode_same_hit);
		  delta_t_anode_cathode_same_hit.fill(DT_cathode_anode_same_hit);
		}

		double DT_cathode_cathode = itrack->bot_cathodic_time_ns - cathode_min_time_ns;
		if (DT_cathode_cathode != 0) {
		  DT_LOG_DEBUG(logging, "DT(cathode-cathode)=" << DT_cathode_cathode);
		  delta_t_cathode_cathode.fill(DT_cathode_cathode);
		}
		bot_cathode_counter++;
		if (!itrack->has_anodic_t0()) bot_cathode_only_counter++;
	      }

	      tracker_hit_number++;

	    }
	    if (is_debug) std::clog << std::endl;


	  } // end of if has tracker hits

	} // end of has HCRD bank label


      ER.clear();
      event_number++;
    } // end of while is reader

    // General statistics :
    std::string sname = "/tmp/hc_data_quality_general.stat";
    std::ofstream ofhist(sname.c_str());
    double eff_only_calo    = (static_cast<double>(event_only_calo) / static_cast<double>(event_number)) * 100.;
    double eff_only_tracker = (static_cast<double>(event_only_tracker) /  static_cast<double>(event_number)) * 100.;
    double eff_calo_tracker = (static_cast<double>(event_calo_tracker) /  static_cast<double>(event_number)) * 100.;

    ofhist << "***GENERAL STATISTICS***" << std::endl;
    ofhist << "Total number of commissioning events : " << event_number <<  std::endl;
    ofhist << "Total number of calo only events     : " << event_only_calo    << " Eff : " << eff_only_calo    << "%" << std::endl;
    ofhist << "Total number of tracker only events  : " << event_only_tracker << " Eff : " << eff_only_tracker << "%" <<std::endl;
    ofhist << "Total number of calo tracker events  : " << event_calo_tracker << " Eff : " << eff_calo_tracker << "%" <<std::endl << std::endl;

    ofhist << "***CALO STATISTICS***" << std::endl;
    ofhist << "Total Calo hits (if 2+ calos HT)         : " << calo_hit_number << std::endl << std::endl;

    ofhist << "***TRACKER STATISTICS***" << std::endl;
    ofhist << "Total Tracker hits if calo + tracker     : " << tracker_hit_number << std::endl;
    ofhist << "Total anode t0 hits if calo + tracker    : " << anode_t0_counter << std::endl;
    ofhist << "Total anode t0 only if calo + tracker    : " << anode_t0_only_counter << std::endl;
    ofhist << "Total bot cathode hits if calo + tracker : " << bot_cathode_counter << std::endl;
    ofhist << "Total bot cathode only if calo + tracker : " << bot_cathode_only_counter << std::endl << std::endl;

    ofhist.close();
    ofhist.clear();


    sname = "/tmp/delta_t_calo_ref.hist";
    ofhist.open(sname.c_str());
    delta_t_calo_tref.print(ofhist);
    ofhist.close();
    ofhist.clear();

    sname = "/tmp/delta_t_anode_tref.hist";
    ofhist.open(sname.c_str());
    delta_t_anode_tref.print(ofhist);
    ofhist.close();
    ofhist.clear();

    sname = "/tmp/delta_t_cathode_tref.hist";
    ofhist.open(sname.c_str());
    delta_t_cathode_tref.print(ofhist);
    ofhist.close();
    ofhist.clear();

    sname = "/tmp/delta_t_anode_cathode_same_hit.hist";
    ofhist.open(sname.c_str());
    delta_t_anode_cathode_same_hit.print(ofhist);
    ofhist.close();
    ofhist.clear();

    sname = "/tmp/delta_t_anode_anode.hist";
    ofhist.open(sname.c_str());
    delta_t_anode_anode.print(ofhist);
    ofhist.close();
    ofhist.clear();

    sname = "/tmp/delta_t_cathode_cathode.hist";
    ofhist.open(sname.c_str());
    delta_t_cathode_cathode.print(ofhist);
    ofhist.close();
    ofhist.clear();

    DT_LOG_INFORMATION(logging, "The end.");
  } catch (std::exception & error) {
    std::cerr << "error: " << error.what() << std::endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}
