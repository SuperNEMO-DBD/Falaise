// Standard library:
#include <cstdlib>
#include <string>
#include <iostream>
#include <stdexcept>

// Third party:
// - Boost:
// // Code dedicated to the serialization of the ``std::set`` template class :
// #include <boost/serialization/serialization.hpp>
// #include <boost/serialization/set.hpp>
// - Bayeux/datatools:
#include <datatools/logger.h>
#include <datatools/utils.h>
#include <datatools/io_factory.h>
#include <datatools/clhep_units.h>
#include <datatools/things.h>
// - Bayeux/dpp:
#include <dpp/output_module.h>

// This project:
#include <fecom/hit_reader.hpp>
#include <fecom/commissioning_event.hpp>
#include <fecom/channel_mapping.hpp>


class event_builder
{

public :

  event_builder() {};
  virtual ~event_builder() {};

  void set_times(const double time_start_)
  {
    time_start = time_start_;
    time_stop = time_start + EVENT_BUILDING_GATE_IN_NS;
    //std::clog << "Tstart = " << time_start << " Tstop = " << time_stop << std::endl;
    working_commissioning_event.set_time_start_ns(time_start);
    return;
  }

  void build_tracker_hits_from_channels()
  {
    if (commissioning_event_for_serialization.get_tracker_channel_hit_collection().size() != 0)
      {
	commissioning_event_for_serialization.set_channel_mapping(*channel_mapping);
	commissioning_event_for_serialization.build_tracker_hit_from_channels();
      }
  }

  fecom::channel_mapping * channel_mapping = nullptr;
  fecom::commissioning_event commissioning_event_for_serialization;
  fecom::commissioning_event working_commissioning_event;
  double time_start = 0.0;
  double time_stop = 0.0;
  bool com_event_is_active = false;
  bool ready_for_serialization = false;
  bool begin_by_calorimeter = false;

  const double EVENT_BUILDING_GATE_IN_NS = 150000.0; // 150µs to build a commissioning event (from a valid calo_hit)

};


int main(int argc_, char ** argv_)
{
  // Parsing arguments
  int iarg = 1;
  std::string input_filename = "";
  std::string output_filename = "";
  std::string output_path = "";
  // std::string run_number_str = "";
  int32_t run_number = -1;
  bool is_display  = false;
  bool is_help     = false;

  while (iarg < argc_) {
    std::string arg = argv_[iarg];

    if (arg == "-i" || arg == "--input") {
      input_filename = argv_[++iarg];
    }

    else if (arg == "-o" || arg == "--output") {
      output_filename = argv_[++iarg];
    }

    else if (arg == "-op" || arg == "--output-path") {
      output_path = argv_[++iarg];
    }

    else if (arg == "-r" || arg == "--run-number") {
      run_number = std::stoi(argv_[++iarg]);
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
		<< "-h    [ --help ]        produce help message" << std::endl
		<< "-i    [ --input ] file  set an input file" << std::endl
		<< "-o    [ --output ] file set an output file" << std::endl
		<< "-op   [ --output ] path set a path where all files are store" << std::endl
		<< "-d    [ --display ]     display things for debug" << std::endl << std::endl;
      return 0;
    }

  datatools::logger::priority logging;
  if (is_display) logging = datatools::logger::PRIO_DEBUG;
  else logging = datatools::logger::PRIO_INFORMATION;

  try {
    DT_LOG_INFORMATION(logging, "Entering HC Event Builder...");

    // Set the input file :
    if (input_filename.empty()) input_filename = "${FECOM_RESOURCES_DIR}/data/samples/fake_run/toto.xml";
    datatools::fetch_path_with_env(input_filename);
    std::string input_path = input_filename;
    std::size_t found = input_path.find_last_of("/");
    input_path.erase(found+1, input_path.size());

    DT_LOG_INFORMATION(logging, "Input filename      : " + input_filename);
    DT_LOG_INFORMATION(logging, "Input path          : " + input_path);

    // Default output path in input path :
    if (output_path.empty())
      {
	output_path = input_path;
	DT_LOG_WARNING(logging, "The output path is empty, did you forget it in the option ? Default directory for output :" + output_path);
      }
    datatools::fetch_path_with_env(output_path);
    DT_LOG_INFORMATION(logging, "Output path : " + output_path);

    DT_LOG_INFORMATION(logging, "Serialization input file :" + input_filename);
    datatools::data_reader deserializer(input_filename,
					datatools::using_multiple_archives);


    if (output_filename.empty()) {
      output_filename = output_path + "hc_event_builder_serialized.xml"; //.data.bz2
    }
    DT_LOG_INFORMATION(logging, "Serialization output file :" + output_filename);

    // Event writer :
    dpp::output_module serializer;
    datatools::properties writer_config;
    writer_config.store ("logging.priority", "debug");
    writer_config.store ("files.mode", "single");
    writer_config.store ("files.single.filename", output_filename);
    serializer.initialize_standalone(writer_config);
    serializer.tree_dump(std::clog, "HC Event builder writer module");

    // Build all tracker hit after the build of all commissioning event :
    std::string input_tracker_mapping_file = input_path + "mapping_tracker.csv";
    datatools::fetch_path_with_env(input_tracker_mapping_file);
    std::string input_calo_mapping_file = input_path + "mapping_calo.csv";
    datatools::fetch_path_with_env(input_calo_mapping_file);

    DT_LOG_INFORMATION(logging, "Mapping tracker file :" + input_tracker_mapping_file);
    DT_LOG_INFORMATION(logging, "Mapping calo file :" + input_calo_mapping_file);

    fecom::channel_mapping my_channel_mapping;
    my_channel_mapping.build_tracker_mapping_from_file(input_tracker_mapping_file);
    my_channel_mapping.build_calo_mapping_from_file(input_calo_mapping_file);
    my_channel_mapping.initialize();

    // const double SERIALIZATION_GATE_SIZE_IN_NS = 100000000; // = 50 ms for the moment (tunable !)

    std::size_t hit_counter = 0;
    std::size_t event_serialized = 0;
    fecom::calo_hit chit;
    fecom::tracker_channel_hit tchit;

    std::vector<fecom::commissioning_event> open_commissioning_events;

    DT_LOG_INFORMATION(logging, "Building the commisioning events...");
    DT_LOG_INFORMATION(logging, "...");

    event_builder eb;
    eb.channel_mapping = & my_channel_mapping;

    int64_t event_number = 0;
    int modulo = 100;
    while (deserializer.has_record_tag()) {
      chit.reset();
      tchit.reset();
      if ((event_number % modulo) == 0){
	std::clog << "Event #" << event_number << std::endl;
      }
      //DT_LOG_INFORMATION(logging, "Entering has record tag...");
      std::string deserialized_hit = "none";
      if (deserializer.record_tag_is(fecom::calo_hit::SERIAL_TAG)) {
	// DT_LOG_INFORMATION(logging, "Entering record tag is calo_hit...");
	deserializer.load(chit);
	// chit.tree_dump(std::clog, "A Calo Hit");
	DT_THROW_IF(!chit.is_valid(), std::logic_error,
		    "The deserialized calo_hit is not valid ! ");
	deserialized_hit = "calo";
      } else if (deserializer.record_tag_is(fecom::tracker_channel_hit::SERIAL_TAG)) {
	// DT_LOG_INFORMATION(logging, "Entering record tag is tracker_channel_hit...");
	deserializer.load(tchit);
	// tchit.tree_dump(std::clog, "A Tracker Channel Hit");
	DT_THROW_IF(!tchit.is_valid(), std::logic_error,
		    "The deserialized tracker_channel_hit is not valid ! ");
	deserialized_hit = "tracker";
      }

      // Check if the working commissioning event is ready or not for serialization
      if (eb.ready_for_serialization) {
	// Build tracker hits from tracker channels
	// DT_LOG_INFORMATION(logging, "Build tracker hits from channels");
	eb.build_tracker_hits_from_channels();

	datatools::event_id event_id;
	event_id.set_run_number(run_number);
	event_id.set_event_number(event_number);
	eb.commissioning_event_for_serialization.set_event_id(event_id);
	event_number++;
	// Store it in a datatools::things and reset it :
	datatools::things commissioning_event_record;
	commissioning_event_record.set_name("CER");
	commissioning_event_record.set_description("A single data record with banks in it");

	datatools::properties & props = commissioning_event_record.add<datatools::properties>("Props", "Props bank");
	props.store_flag("test");

	fecom::commissioning_event & CE = commissioning_event_record.add<fecom::commissioning_event>("HCRD", "The Half Commissioning Raw Data bank");

	CE = eb.commissioning_event_for_serialization;
	CE.tree_dump(std::clog, "Commissioning event before serialization");
	commissioning_event_record.tree_dump(std::clog, "Dump CER things");
	serializer.process(commissioning_event_record);
	event_serialized++;
	eb.commissioning_event_for_serialization.reset();
	eb.ready_for_serialization = false;
	break;
      }

      if (deserialized_hit == "calo") {
	// First calo hit of the event
	if (!eb.com_event_is_active) {
	  eb.working_commissioning_event.add_calo_hit(chit);
	  double time_start = chit.tdc_ns + chit.falling_time_ns;
	  eb.set_times(time_start);
	  eb.com_event_is_active = true;
	  eb.begin_by_calorimeter = true;
	}
	else {
	  if (eb.begin_by_calorimeter) {
	    // Check if the calo hit is in the gate
	    if (chit.tdc_ns < eb.time_stop) {
	      // Add the calo hit in the working commissioning event :
	      eb.working_commissioning_event.add_calo_hit(chit);
	    }
	    // Outside gate, close the working com event and internal copy for serialization
	    else {
	      eb.ready_for_serialization = true;
	      eb.commissioning_event_for_serialization = eb.working_commissioning_event;
	      eb.working_commissioning_event.reset();
	      eb.working_commissioning_event.add_calo_hit(chit);
	      double time_start = chit.tdc_ns + chit.falling_time_ns;
	      eb.set_times(time_start);
	      eb.begin_by_calorimeter = true;
	    }
	  } // end of begin by calo
	  // if not begin by calo, close actual commissioning event, serialize, reset and fill it with the new calo hit
	  else {
	    eb.ready_for_serialization = true;
	    eb.commissioning_event_for_serialization = eb.working_commissioning_event;
	    eb.working_commissioning_event.reset();
	    eb.working_commissioning_event.add_calo_hit(chit);
	    double time_start = chit.tdc_ns + chit.falling_time_ns;
	    eb.set_times(time_start);
	    eb.begin_by_calorimeter = true;
	  } // end of else not begin by calo
	}
      } // end of if "calo"

      else if (deserialized_hit == "tracker") {
	// If the commissioning event begin by a tracker hit, skip it
	// It must begin by a calo hit
	if (!eb.com_event_is_active) break;
	else {
	  if (eb.begin_by_calorimeter) {
	    // Check if the tracker channel hit is in the gate
	    if (tchit.timestamp_time_ns < eb.time_stop) {
	      // Add the tracker channel hit in the working commissioning event :
	      eb.working_commissioning_event.add_tracker_channel_hit(tchit);
	    }
	    // Outside gate, close the working com event and internal copy for serialization
	    else {
	      eb.ready_for_serialization = true;
	      eb.commissioning_event_for_serialization = eb.working_commissioning_event;
	      eb.working_commissioning_event.reset();
	      eb.working_commissioning_event.add_tracker_channel_hit(tchit);
	      double time_start = tchit.timestamp_time_ns;
	      eb.set_times(time_start);
	      eb.begin_by_calorimeter = false;
	      // Event tracker only, add it to the first bit of the traits :
	      eb.working_commissioning_event.grab_traits().set(0, true);
	    }
	  } // end of if begin by calo
	  else {
	    // If com event not begin by a calo add the tracker channel hit until we found a new calo hit :
	    eb.working_commissioning_event.add_tracker_channel_hit(tchit);
	  }

	}
      } // end of if tracker

      hit_counter++;
    } // end of while deserializer

    // Serialize the last commissioning event :
    eb.commissioning_event_for_serialization = eb.working_commissioning_event;
    eb.build_tracker_hits_from_channels();
    datatools::things commissioning_event_record;
    commissioning_event_record.set_name("CER");
    commissioning_event_record.set_description("Half commissioning event record");
    fecom::commissioning_event & CE = commissioning_event_record.add<fecom::commissioning_event>("HCRD", "The Half Commissioning Raw Data bank");
    CE = eb.commissioning_event_for_serialization;
    serializer.process(commissioning_event_record);
    event_serialized++;

    DT_LOG_INFORMATION(logging, "Job done !");

    DT_LOG_INFORMATION(logging, "Run serialized : " + std::to_string(run_number));
    DT_LOG_INFORMATION(logging, "Number of commissioning event serialized : " + std::to_string(event_number));

    DT_LOG_INFORMATION(logging, "Output file : " << output_filename);
    DT_LOG_INFORMATION(logging, "End of reader file...");
    deserializer.reset();


    DT_LOG_INFORMATION(logging, "Exiting main_decoder_serializer.cxx...");
    DT_LOG_INFORMATION(logging, "EXIT_STATUS : SUCCESS");

  } catch (std::exception & error) {
    std::cerr << "error: " << error.what() << std::endl;
    DT_LOG_FATAL(logging, "EXIT_STATUS : FAILURE");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
