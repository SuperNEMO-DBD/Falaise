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

// Third party:
// - Boost:
// Code dedicated to the serialization of the ``std::set`` template class :
#include <boost/serialization/serialization.hpp>
// #include <boost/serialization/set.hpp>

int main(int /*argc_*/, char ** /*argv_*/)
{
  bool debug = false;
  datatools::logger::priority logging;
  if (debug) logging = datatools::logger::PRIO_DEBUG;
  else logging = datatools::logger::PRIO_INFORMATION;

  try {
    DT_LOG_INFORMATION(logging, "Entering test-libfecom-fake_hit_reader.cxx...");
    std::string input_filename = "${FECOM_RESOURCES_DIR}/data/samples/run_2/Run_SN_Crate_Test_Calo_And_Tracker_Data_2_21_2017_Ascii.dat";

    // Serialization output :
    std::string output_filename("${FECOM_RESOURCES_DIR}/data/samples/run_2/Run_SN_Crate_Test_Calo_And_Tracker_Data_2_21_2017_Ascii.data.bz2");
    // std::string output_filename = "${FECOM_RESOURCES_DIR}/output_test/commissioning_event_10_events.data.bz2";
    datatools::fetch_path_with_env(output_filename);
    DT_LOG_DEBUG(logging, "Serialization output file :" + output_filename);
    datatools::data_writer serializer(output_filename,
				      datatools::using_multiple_archives);

    fecom::hit_reader reader;
    reader.set_logging(datatools::logger::PRIO_INFORMATION);
    reader.set_input_filename(input_filename);
    // reader.set_input_filename("${FECOM_RESOURCES_DIR}/output_test/test_generate_fake_hit_10_events.dat");
    reader.initialize();
    fecom::run_header header;
    reader.load_run_header(header);
    header.tree_dump(std::clog, "Run header:");

    fecom::calo_hit chit;
    fecom::tracker_channel_hit tchit;

    std::set<fecom::commissioning_event, fecom::commissioning_event::compare> open_commissioning_events;

    // Build all tracker hit after the build of all commissioning event :
    std::string input_tracker_mapping_file("${FECOM_RESOURCES_DIR}/data/samples/run_2/mapping_tracker.csv");
    datatools::fetch_path_with_env(input_tracker_mapping_file);
    std::string input_calo_mapping_file("${FECOM_RESOURCES_DIR}/data/samples/run_2/mapping_calo.csv");
    datatools::fetch_path_with_env(input_calo_mapping_file);

    fecom::channel_mapping my_channel_mapping;
    my_channel_mapping.build_tracker_mapping_from_file(input_tracker_mapping_file);
    my_channel_mapping.build_calo_mapping_from_file(input_calo_mapping_file);
    my_channel_mapping.initialize();


    const double GATE_SIZE_IN_NS = 1000000; // = 1ms for the moment (tunable !)

    std::size_t hit_counter = 0;
    std::size_t event_serialized = 0;

    DT_LOG_INFORMATION(logging, "Begin reading of file '" + input_filename + "' !");

    while(reader.has_next_hit()) {

      // std::clog << "Hit counter = " << hit_counter << std::endl;
      chit.reset();
      tchit.reset();
      reader.load_next_hit(chit, tchit);

      std::string valid = "none";
      double actual_time_in_ns = -1;

      uint32_t actual_hit_trigger_id = 0xFFFFFF;

      if (chit.is_valid()) {
	// chit.tree_dump(std::clog, "Calo hit is valid :");
	valid = "calo";
	actual_hit_trigger_id = chit.trigger_id;
	actual_time_in_ns = chit.tdc_ns;
      }

      if (tchit.is_valid()) {
	// tchit.tree_dump(std::clog, "Tracker channel hit is valid :");
	valid = "tracker";
	actual_hit_trigger_id = tchit.trigger_id;
	actual_time_in_ns = tchit.timestamp_time_ns;
      }

      // See hit timestamp and check the last timestamp for each commissioning event in the set
      for (auto it_com_set = open_commissioning_events.begin();
	   it_com_set != open_commissioning_events.end();
	   it_com_set++)
	{
	  if (open_commissioning_events.size() == 0) break;
	  else if (it_com_set != open_commissioning_events.end())
	    {
	      if (actual_time_in_ns > it_com_set->_last_time_in_ns_added_ + GATE_SIZE_IN_NS)
		{
		  DT_LOG_DEBUG(logging, "Begin the Serialization for a commissioning event...");
		  DT_LOG_DEBUG(logging, "Begin the building of tracker hits...");

		  const_cast<fecom::commissioning_event&>(*it_com_set).set_channel_mapping(my_channel_mapping);
		  const_cast<fecom::commissioning_event&>(*it_com_set).build_tracker_hit_from_channels();

		  DT_LOG_DEBUG(logging, "Tree dump the commissioning event...");

		  serializer.store(*it_com_set);
		  DT_LOG_DEBUG(logging, "The commissioning event has been stored in the '" + output_filename + "' file");

		  // After serialization remove it from the set
		  open_commissioning_events.erase(it_com_set);
		  event_serialized++;
		}
	    }
	}

      // Do the job if the hit is calo
      if (valid == "calo") {
	auto it_set = std::find_if(open_commissioning_events.begin(),
				   open_commissioning_events.end(),
				   fecom::commissioning_event::find_by_event_id(actual_hit_trigger_id));

	if (it_set != open_commissioning_events.end()) {
	  // Trigger ID already exist, add the calo hit to the existing commissioning event
	  const_cast<fecom::commissioning_event&>(*it_set).add_calo_hit(chit);
	  if (it_set->_last_time_in_ns_added_ <= actual_time_in_ns) it_set -> _last_time_in_ns_added_ = actual_time_in_ns;
	}

	else {
	  // Trigger ID not exist, create a new commissioning event, add the calo hit then add it to the comm set
	  fecom::commissioning_event a_new_comm_event;
	  a_new_comm_event.grab_event_id().set_event_id(actual_hit_trigger_id);
	  a_new_comm_event.add_calo_hit(chit);
	  a_new_comm_event._last_time_in_ns_added_ = actual_time_in_ns;

	  open_commissioning_events.insert(a_new_comm_event);
	}
      } // if calo

      // Do the job if the hit is tracker
      else if (valid == "tracker") {

	auto it_set = std::find_if(open_commissioning_events.begin(),
				   open_commissioning_events.end(),
				   fecom::commissioning_event::find_by_event_id(actual_hit_trigger_id));

	if (it_set != open_commissioning_events.end()) {
	  // Trigger ID already exist, add the tracker channel hit to the existing commissioning event
	  const_cast<fecom::commissioning_event&>(*it_set).add_tracker_channel_hit(tchit);
	  if (it_set->_last_time_in_ns_added_ <= actual_time_in_ns) it_set -> _last_time_in_ns_added_ = actual_time_in_ns;
	}

	else {
	  // Trigger ID not exist, create a new commissioning event, add the calo hit then add it to the comm set
	  fecom::commissioning_event a_new_comm_event;
	  a_new_comm_event.grab_event_id().set_event_id(actual_hit_trigger_id);
	  a_new_comm_event.add_tracker_channel_hit(tchit);
	  a_new_comm_event._last_time_in_ns_added_ = actual_time_in_ns;

	  open_commissioning_events.insert(a_new_comm_event);
	}
      } // else if tracker

      else DT_THROW(std::logic_error, "Nor calo and tracker hit are valid ! ");

      hit_counter++;
    } // end of while reader

    // Serialize the last(s) commissioning event in the set :
    DT_LOG_DEBUG(logging, "Serialize the last(s) commissioning event in the set");
    for (auto it_com_set = open_commissioning_events.begin();
	 it_com_set != open_commissioning_events.end();
	 it_com_set++)
      {
	if (it_com_set != open_commissioning_events.end() && open_commissioning_events.size() != 0)
	  {
	    DT_LOG_DEBUG(logging, "Begin the Serialization for a commissioning event...");
	    DT_LOG_DEBUG(logging, "Begin the building of tracker hits...");

	    const_cast<fecom::commissioning_event&>(*it_com_set).set_channel_mapping(my_channel_mapping);
	    const_cast<fecom::commissioning_event&>(*it_com_set).build_tracker_hit_from_channels();
	    serializer.store(*it_com_set);
	    event_serialized++;
	  }
      }

    DT_LOG_INFORMATION(logging, "Number of commissioning event serialized : " + std::to_string(event_serialized));

    DT_LOG_INFORMATION(logging, "End of reader file...");
    reader.reset();


    DT_LOG_INFORMATION(logging, "Exiting test-libfecom-fake_hit_reader.cxx...");

  } catch (std::exception & error) {
    std::cerr << "error: " << error.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
