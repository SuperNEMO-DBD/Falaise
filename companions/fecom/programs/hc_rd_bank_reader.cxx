// programs/hc_rd_bank_reader.
// Standard library:
#include <cstdlib>
#include <string>
#include <iostream>
#include <stdexcept>
#include <limits>
#include <algorithm>

// Third party:
// - Boost:
#include <boost/program_options.hpp>

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

// Falaise:
#include <falaise/falaise.h>
#include <falaise/snemo/datamodels/calibrated_data.h>

// This project:
#include <fecom/mock_hc2cd_module.hpp>

// Root :
#include "TError.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"

// This project:
#include <fecom/commissioning_event.hpp>
// #include <fecom/channel_mapping.hpp>

int main(int argc_, char ** argv_)
{
  falaise::initialize(argc_, argv_);
  int error_code = EXIT_SUCCESS;

  gErrorAbortLevel = 2000;

  try {

    bool is_debug = false;
    std::vector<std::string> input_filenames;
    std::string output_path = "";
    std::size_t max_events     = 0;

    // Parse options:
    namespace po = boost::program_options;
    po::options_description opts("Allowed options");
    opts.add_options()
      ("help,h", "produce help message")
      ("debug,d", "debug mode")
      ("input,i",
       po::value<std::vector<std::string> >(& input_filenames)->multitoken(),
       "set a list of input files")
      ("output,o",
       po::value<std::string>(& output_path),
       "set the output path")
      ("max-events,M",
       po::value<std::size_t>(& max_events)->default_value(10),
       "set the maximum number of events")
      ; // end of options description

    // Describe command line arguments :
    po::variables_map vm;
    po::store(po::command_line_parser(argc_, argv_)
              .options(opts)
              .run(), vm);
    po::notify(vm);

    // Use command line arguments :
    if (vm.count("help")) {
      std::cout << "Usage : " << std::endl;
      std::cout << opts << std::endl;
      return(1);
    }
    // Use command line arguments :
    if (vm.count("debug")) {
      is_debug = true;
    }

    datatools::logger::priority logging;
    if (is_debug) logging = datatools::logger::PRIO_DEBUG;
    else logging = datatools::logger::PRIO_INFORMATION;
    DT_LOG_INFORMATION(logging, "Entering hc_rd_bank_reader.cxx...");

    DT_LOG_INFORMATION(logging, "List of input file(s) : ");
    for (auto file = input_filenames.begin();
    	 file != input_filenames.end();
    	 file++) std::clog << *file << ' ';
    std::clog << std::endl;

    DT_THROW_IF(input_filenames.size() == 0, std::logic_error, "No input file(s) ! ");
    DT_LOG_INFORMATION(logging, "Total number of files = " << input_filenames.size());

    DT_LOG_INFORMATION(logging, "Output path for files = " + output_path);
    if (output_path.empty()) {
      output_path = "/tmp/";
      DT_LOG_INFORMATION(logging, "No output path, default output path is = " + output_path);
    }
    datatools::fetch_path_with_env(output_path);
    DT_LOG_INFORMATION(logging, "Output path : " + output_path);



    int max_record_total = static_cast<int>(max_events) * static_cast<int>(input_filenames.size());
    std::clog << "Max event for a file     = " << max_events << std::endl;
    std::clog << "Max events for all files = " << max_record_total << std::endl;

    // Event reader :
    dpp::input_module reader;
    datatools::properties reader_config;
    reader_config.store("logging.priority", "debug");
    reader_config.store("files.mode", "list");
    reader_config.store("files.list.filenames", input_filenames);
    reader_config.store("max_record_total", max_record_total);
    reader_config.store("max_record_per_file", static_cast<int>(max_events));
    reader_config.tree_dump(std::clog, "Input module configuration parameters: ");
    reader.initialize_standalone(reader_config);
    reader.tree_dump(std::clog, "Input module 'reader' dump");

    // Output module to dump selected event in a brio file (for sorting event purpose)
    // Usecase : save in a file all 'calo + tracker' HCRD events

    // std::string output_brio_filename = output_path + "/output_file.brio";
    // dpp::output_module serializer;
    // datatools::properties writer_config;
    // writer_config.store ("logging.priority", "debug");
    // writer_config.store ("files.mode", "single");
    // writer_config.store ("files.single.filename", output_brio_filename);
    // serializer.initialize_standalone(writer_config);

    // ROOT output file :
    std::string output_hcrd_reader_histograms = output_path + "/output_hcrd_reader_histograms.root";
    DT_LOG_INFORMATION(logging, "Output histograms for analysis :" + output_hcrd_reader_histograms);
    TFile * histogram_file = new TFile(output_hcrd_reader_histograms.c_str(), "RECREATE");

    // Event record :
    datatools::things ER;

    // Half Commissioning Raw Data "HCRD" bank label :
    std::string HCRD_bank_label = "HCRD";

    int modulo = 1000;
    DT_LOG_INFORMATION(logging, "Reading commisioning events...");
    DT_LOG_INFORMATION(logging, "...");

    uint64_t psd_count = 0;

    while (!reader.is_terminated()) {

      reader.process(ER);

      // A plain `fecom::commissioning_event' object is stored here :
      if (ER.has(HCRD_bank_label) && ER.is_a<fecom::commissioning_event>(HCRD_bank_label))
	{
	  if ((psd_count % modulo) == 1 ){
	    DT_LOG_DEBUG(logging, "Event number = " << psd_count);
	  }
	  const fecom::commissioning_event & CE = ER.grab<fecom::commissioning_event>(HCRD_bank_label);

	  if (is_debug) CE.tree_dump(std::clog, "A com event");

	  std::size_t event_number = CE.get_event_id().get_event_number();
	  double event_time_start_ns = CE.get_time_start_ns();

	  std::size_t number_of_calo = CE.get_number_of_calo();
	  std::size_t number_of_geiger_cells = CE.get_number_of_tracker();
	  std::size_t number_of_calo_ht = CE.get_number_of_calo_ht();

	  if (is_debug) {
	    std::clog << "Event # " << event_number
		      << " Tstart (ns) =" << event_time_start_ns
		      << " #Calo = "      << number_of_calo
		      << " #CaloHT = "    << number_of_calo_ht
		      << " #Cells = "     << number_of_geiger_cells << std::endl << std::endl;;
	  }

	  // Test to retrieve the calorimeter collection of HT hits only :
	  fecom::commissioning_event::calo_hit_collection calo_ht_hit_collection;
	  CE.get_calo_ht_hit_collection(calo_ht_hit_collection);
	  if (is_debug) {
	    std::clog << "Number of calo HT = " << calo_ht_hit_collection.size() << std::endl;

	    for (auto icalo = calo_ht_hit_collection.begin();
		 icalo != calo_ht_hit_collection.end();
		 icalo++) {
	      icalo->tree_dump(std::clog, "A calo HT");
	    }
	  }
	} // end of has HCRD bank label

      ER.clear();
      psd_count++;
    } // end of while is reader


    // Save histograms in an other file
    histogram_file->cd();
    //cts.save_in_root_file(histogram_file);
    histogram_file->Close();

    // Output for statistics in txt file
    std::string output_hcrd_bank_reader_filename = output_path + "output_hcrd_bank_reader.txt";
    DT_LOG_INFORMATION(logging, "Output HCRD bank reader file : " + output_hcrd_bank_reader_filename);

    std::ofstream ofstat(output_hcrd_bank_reader_filename.c_str());
    //da.print(ofstat);
    ofstat.close();

    DT_LOG_INFORMATION(logging, "The end.");
  } catch (std::exception & error) {
    std::cerr << "error: " << error.what() << std::endl;
    error_code = EXIT_FAILURE;
  }
  falaise::terminate();
  return error_code;
}
