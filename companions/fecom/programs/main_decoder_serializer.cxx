// Standard library:
#include <cstdlib>
#include <string>
#include <iostream>
#include <stdexcept>

// This project:
#include <fecom/hit_reader.hpp>
#include <fecom/commissioning_event.hpp>
#include <fecom/channel_mapping.hpp>

// - Bayeux/datatools:
#include <datatools/logger.h>
#include <datatools/utils.h>
#include <datatools/io_factory.h>
#include <datatools/clhep_units.h>
#include <datatools/things.h>

// Third party:
// - Boost:
// Code dedicated to the serialization of the ``std::set`` template class :
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/set.hpp>

int main(int argc_, char ** argv_)
{
  // Parsing arguments
  int iarg = 1;
  std::string input_filename = "";
  std::string output_path = "";
  bool is_plain    = false;
  bool is_pipeline = false;
  bool is_display  = false;
  bool is_help     = false;

  while (iarg < argc_) {
    std::string arg = argv_[iarg];

    if (arg == "-i" || arg == "--input") {
      input_filename = argv_[++iarg];
    }

    else if (arg == "-op" || arg == "--output-path") {
      output_path = argv_[++iarg];
    }

    else if (arg == "--plain") {
      is_plain = true;
    }

    else if (arg == "--pipeline") {
      is_pipeline = true;
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
		<< "-h         [ --help ]        produce help message" << std::endl
		<< "-i         [ --input ]       set an input file" << std::endl
		<< "-op        [ --output path ] set a path where all files are store" << std::endl
		<< "--plain    [ --plain ]       set the serialization mode to plain -> no datatools::things" << std::endl
		<< "--pipeline [ --pipeline ]    set the serialization mode to pipeline -> in a datatools::things -> HCE bank" << std::endl
		<< "-d    [ --display ]        display things for debug" << std::endl << std::endl;
      return 0;
    }
  DT_THROW_IF(!is_plain && !is_pipeline, std::logic_error, "The serialization mode is not defined, it has to be defined as plain or pipeline, check your options ! ");
  DT_THROW_IF(is_plain && is_pipeline, std::logic_error, "The serialization mode can't be plain and pipeline at the same time, check your options ! ");

  datatools::logger::priority logging;
  if (is_display) logging = datatools::logger::PRIO_DEBUG;
  else logging = datatools::logger::PRIO_INFORMATION;

  try {
    DT_LOG_INFORMATION(logging, "Entering main_decoder_serializer.cxx...");

    // Set the input file from Jihanne :
    if (input_filename.empty()) input_filename = "${FECOM_RESOURCES_DIR}/data/samples/fake_run/calo_fake_tracker_hits_1.dat";
    datatools::fetch_path_with_env(input_filename);
    // std::string input_file_basename = input_filename;
    // std::size_t found = 0;
    // found = input_file_basename.find_last_of("/");
    // input_file_basename.erase(0, found+1);
    // found = input_file_basename.find(".dat");
    // input_file_basename.erase(found, input_file_basename.size());
    std::string input_path = input_filename;
    std::size_t found = input_path.find_last_of("/");
    input_path.erase(found+1, input_path.size());

    DT_LOG_INFORMATION(logging, "Input filename      : " + input_filename);
    DT_LOG_INFORMATION(logging, "Input path          : " + input_path);
    // DT_LOG_INFORMATION(logging, "Input file basename : " + input_file_basename);

    // Default output path in input path :
    if (output_path.empty())
      {
	output_path = input_path;
	DT_LOG_WARNING(logging, "The output path is empty, did you forget it in the option ? Default directory for output :" + output_path);
      }
    datatools::fetch_path_with_env(output_path);
    DT_LOG_INFORMATION(logging, "Output path : " + output_path);

    std::string output_filename = output_path + "output_serialized.data.bz2";
    DT_LOG_INFORMATION(logging, "Serialization output file :" + output_filename);
    datatools::data_writer serializer(output_filename,
				      datatools::using_multiple_archives);

    // Input file (from Jihanne) hits reader :
    fecom::hit_reader reader;
    reader.set_logging(datatools::logger::PRIO_FATAL);
    reader.set_input_filename(input_filename);
    reader.initialize();
    fecom::run_header header;
    reader.load_run_header(header);
    DT_LOG_INFORMATION(logging, "File [" << input_filename << "]   header : ");
    header.tree_dump(std::clog, "Run header:");

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

    const double SERIALIZATION_GATE_SIZE_IN_NS = 100000000; // = 50 ms for the moment (tunable !)

    std::size_t hit_counter = 0;
    std::size_t event_serialized = 0;
    fecom::calo_hit chit;
    fecom::tracker_channel_hit tchit;

    std::vector<fecom::commissioning_event> open_commissioning_events;

    DT_LOG_INFORMATION(logging, "Do the job...");
    DT_LOG_INFORMATION(logging, "...");

    while(reader.has_next_hit()) {
      DT_LOG_DEBUG(logging, "Hit counter = " << hit_counter);
      chit.reset();
      tchit.reset();
      reader.load_next_hit(chit, tchit);

      std::string valid = "none";
      double actual_time_in_ns = 0;
      uint32_t actual_hit_trigger_id = 0xFFFFFF;

      if (chit.is_valid()) {
	// chit.tree_dump(std::clog, "Calo hit is valid :");
	valid = "calo";
	actual_hit_trigger_id = chit.trigger_id;
	if (chit.tdc_ns != 0) actual_time_in_ns = chit.tdc_ns;
      }

      if (tchit.is_valid()) {
	// tchit.tree_dump(std::clog, "Tracker channel hit is valid :");
	valid = "tracker";
	actual_hit_trigger_id = tchit.trigger_id;
	if (tchit.timestamp_time_ns != 0 && tchit.timestamp_time_ns != 0) actual_time_in_ns = tchit.timestamp_time_ns;
      }

      for (auto it_com_set = open_commissioning_events.begin();
	   it_com_set != open_commissioning_events.end();)
	{
	  // it_com_set->tree_dump(std::clog, "BEGIN LOOP COM EVENT : ");
	  /*std::clog << "Actual time = " << actual_time_in_ns << std::endl;
	  std::clog << "Last time added = " <<  it_com_set -> _last_time_in_ns_added_ << std::endl;
	  std::clog << "Last time added + gate = " <<  it_com_set -> _last_time_in_ns_added_ + SERIALIZATION_GATE_SIZE_IN_NS << std::endl;
	  std::clog << "Size of open com event = " <<  open_commissioning_events.size() << std::endl;*/

	  if (actual_time_in_ns > it_com_set -> _last_time_in_ns_added_ + SERIALIZATION_GATE_SIZE_IN_NS)
	    {
	      DT_LOG_DEBUG(logging, "Begin the Serialization for a commissioning event...");

	      if (it_com_set->get_tracker_channel_hit_collection().size() != 0)
		{
		  DT_LOG_DEBUG(logging, "Begin the building of tracker hits...");
		  const_cast<fecom::commissioning_event&>(*it_com_set).set_channel_mapping(my_channel_mapping);
		  const_cast<fecom::commissioning_event&>(*it_com_set).build_tracker_hit_from_channels();
		}
	      if (is_plain) serializer.store(*it_com_set);
	      else if (is_pipeline) {
		datatools::things commissioning_event_record;
		commissioning_event_record.set_name("CER");
		commissioning_event_record.set_description("A single data record with banks in it");
		fecom::commissioning_event & CE = commissioning_event_record.add<fecom::commissioning_event>("RHCE", "The Raw Half Commissioning Event bank");
		CE = *it_com_set;
		serializer.store(CE);
	      }
	      //std::clog << "Com event Trigger ID = " << it_com_set -> get_trigger_id() << std::endl;

	      DT_LOG_DEBUG(logging, "The commissioning event has been stored in the '" + output_filename + "' file");
	      // After serialization remove it from the vector
	      open_commissioning_events.erase(it_com_set);
	      event_serialized++;
	    }
	  else it_com_set++;
	} // end of for

	  // Do the job if the hit is calo
      if (valid == "calo") {
	auto it_set = std::find_if(open_commissioning_events.begin(),
				   open_commissioning_events.end(),
				   fecom::commissioning_event::find_by_trigger_id(actual_hit_trigger_id));
	if (it_set != open_commissioning_events.end()) {
	  // Trigger ID already exist, add the calo hit to the existing commissioning event
	  const_cast<fecom::commissioning_event&>(*it_set).add_calo_hit(chit);
	  if (actual_time_in_ns > it_set -> _last_time_in_ns_added_) it_set -> _last_time_in_ns_added_ = actual_time_in_ns;

	}

	else {
	  // Trigger ID not exist, create a new commissioning event, add the calo hit then add it to the open comm set
	  fecom::commissioning_event a_new_comm_event;
	  //	  a_new_comm_event.set_trigger_id(actual_hit_trigger_id);
	  a_new_comm_event.add_calo_hit(chit);
	  a_new_comm_event._last_time_in_ns_added_ = actual_time_in_ns;
	  open_commissioning_events.push_back(a_new_comm_event);
	}
      } // end of if calo

      // Do the job if the hit is tracker
      else if (valid == "tracker") {
	auto it_set = std::find_if(open_commissioning_events.begin(),
				   open_commissioning_events.end(),
				   fecom::commissioning_event::find_by_trigger_id(actual_hit_trigger_id));

	if (it_set != open_commissioning_events.end()) {
	  // Trigger ID already exist, add the tracker channel hit to the existing commissioning event if the timestamp is not zero
	  if (tchit.timestamp_value != 0 && tchit.timestamp_time_ns != 0)
	    {
	      // std::clog << "Tracker channel hit timestamp value = " << tchit.timestamp_time_ns << std::endl;
	      const_cast<fecom::commissioning_event&>(*it_set).add_tracker_channel_hit(tchit);
	      if (actual_time_in_ns > it_set->_last_time_in_ns_added_) it_set -> _last_time_in_ns_added_ = actual_time_in_ns;
	    }
	}

	else {
	  if (tchit.timestamp_value != 0 && tchit.timestamp_time_ns != 0)
	    {
	      // Trigger ID not exist, create a new commissioning event, add the calo hit then add it to the open comm set
	      fecom::commissioning_event a_new_comm_event;
	      // a_new_comm_event.set_event_id(actual_hit_trigger_id);
	      a_new_comm_event.add_tracker_channel_hit(tchit);
	      a_new_comm_event._last_time_in_ns_added_ = actual_time_in_ns;
	      open_commissioning_events.push_back(a_new_comm_event);
	    }
	}
      } // end of else if tracker

      else
	{
	  // std::clog << "DEBUG : Print hits before throw : " << std::endl;
	  // chit.tree_dump(std::clog, "Calo hit is not valid and throwed :");
	  // tchit.tree_dump(std::clog, "Tracker channel hit is not valid and throwed :");
	  DT_THROW(std::logic_error, "Nor calo and tracker hit are valid ! ");
	}

      // std::clog << "Event serialized = " << event_serialized << std::endl;
      // std::clog << "Hit counter      = " << hit_counter << std::endl;

      hit_counter++;
    } // end of while reader

    // Serialize the last(s) commissioning event present in the set :
    DT_LOG_DEBUG(logging, "Serialize the last(s) commissioning event in the set");
    for (auto it_com_set = open_commissioning_events.begin();
	 it_com_set != open_commissioning_events.end();
	 it_com_set++)
      {
	std::clog << "Size at the end of the open com event :" << open_commissioning_events.size() << std::endl;
	if (it_com_set != open_commissioning_events.end() && open_commissioning_events.size() != 0)
	  {
	    DT_LOG_DEBUG(logging, "Begin the Serialization for a commissioning event...");

	    if (it_com_set->get_tracker_channel_hit_collection().size() != 0)
	      {
		DT_LOG_DEBUG(logging, "Begin the building of tracker hits...");
		const_cast<fecom::commissioning_event&>(*it_com_set).set_channel_mapping(my_channel_mapping);
		const_cast<fecom::commissioning_event&>(*it_com_set).build_tracker_hit_from_channels();
	      }
	    if (is_plain) serializer.store(*it_com_set);
	    else if (is_pipeline) {
	      datatools::things commissioning_event_record;
	      commissioning_event_record.set_name("CER");
	      commissioning_event_record.set_description("A single data record with banks in it");
	      fecom::commissioning_event & CE = commissioning_event_record.add<fecom::commissioning_event>("HCRD", "The Half Commissioning Raw Data bank");
	      CE = *it_com_set;
	      serializer.store(CE);
	    }
	    // std::clog << "Com event Trigger ID (at the end)  = " << it_com_set -> get_trigger_id() << std::endl;
	    event_serialized++;
	  }
      }

    DT_LOG_INFORMATION(logging, "Job done !");

    if (is_plain) {
      DT_LOG_INFORMATION(logging, "Serialization mode : PLAIN MODE");
    }
    else if (is_pipeline) {
      DT_LOG_INFORMATION(logging, "Serialization mode : PIPELINE MODE");
    }

    DT_LOG_INFORMATION(logging, "Number of commissioning event serialized : " + std::to_string(event_serialized));


    DT_LOG_INFORMATION(logging, "Output file : " << output_filename);
    DT_LOG_INFORMATION(logging, "End of reader file...");
    reader.reset();


    DT_LOG_INFORMATION(logging, "Exiting main_decoder_serializer.cxx...");
    DT_LOG_INFORMATION(logging, "EXIT_STATUS : SUCCESS");

  } catch (std::exception & error) {
    std::cerr << "error: " << error.what() << std::endl;
    DT_LOG_FATAL(logging, "EXIT_STATUS : FAILURE");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
