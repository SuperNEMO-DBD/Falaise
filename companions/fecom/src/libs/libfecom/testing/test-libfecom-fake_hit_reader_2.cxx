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

// - Bayeux/brio:
#include <brio/writer.h>
#include <brio/reader.h>

// Third party:
// - Boost:
// Code dedicated to the serialization of the ``std::set`` template class :
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/set.hpp>

int main(int /*argc_*/, char ** /*argv_*/)
{
  datatools::logger::priority logging = datatools::logger::PRIO_DEBUG;
  try {
    fecom::hit_reader reader;
    reader.set_logging(datatools::logger::PRIO_WARNING);
    // reader.set_input_filename("${FECOM_RESOURCES_DIR}/data/samples/fake_run/calo_fake_tracker_hits_2.dat");
    reader.set_input_filename("${FECOM_RESOURCES_DIR}/output_test/test_generate_fake_hit_large.dat");
    reader.initialize();
    fecom::run_header header;
    reader.load_run_header(header);
    header.tree_dump(std::clog, "Run header:");

    fecom::calo_hit chit;
    fecom::tracker_channel_hit tchit;
    fecom::commissioning_event_data commissioning_event_collection;

    std::clog << "Before while " << std::endl;

    std::size_t hit_counter = 0;
    while(reader.has_next_hit()) {
      // std::clog << "Hit counter = " << hit_counter << std::endl;
      chit.reset();
      tchit.reset();
      reader.load_next_hit(chit, tchit);

      std::string valid = "none";
      if (chit.is_valid()) {
	// chit.tree_dump(std::clog, "Calo hit is valid :");
	valid = "calo";
      }

      if (tchit.is_valid()) {
	// tchit.tree_dump(std::clog, "Tracker channel hit is valid :");
	valid = "tracker";
      }

      uint32_t actual_hit_trigger_id = 0xFFFFFF;

      // Do the job if the hit is calo
      if (valid == "calo") {
	actual_hit_trigger_id = chit.trigger_id;
	auto it_set = std::find_if(commissioning_event_collection.get_commissioning_event_collection().begin(),
				   commissioning_event_collection.get_commissioning_event_collection().end(),
				   fecom::commissioning_event::find_by_trigger_id(actual_hit_trigger_id));

	if (it_set != commissioning_event_collection.get_commissioning_event_collection().end()) {
	  // Trigger ID already exist, add the calo hit to the existing commissioning event
	  const_cast<fecom::commissioning_event&>(*it_set).add_calo_hit(chit);
	}

	else {
	  // Trigger ID not exist, create a new commissioning event, add the calo hit then add it to the comm set
	  fecom::commissioning_event a_new_comm_event;
	  a_new_comm_event.set_trigger_id(actual_hit_trigger_id);
	  a_new_comm_event.add_calo_hit(chit);
	  commissioning_event_collection.add_commissioning_event(a_new_comm_event);
	}
      }

      // Do the job if the hit is tracker
      else if (valid == "tracker") {
	actual_hit_trigger_id = tchit.trigger_id;

	auto it_set = std::find_if(commissioning_event_collection.get_commissioning_event_collection().begin(),
				   commissioning_event_collection.get_commissioning_event_collection().end(),
				   fecom::commissioning_event::find_by_trigger_id(actual_hit_trigger_id));

	if (it_set != commissioning_event_collection.get_commissioning_event_collection().end()) {
	  // Trigger ID already exist, add the tracker channel hit to the existing commissioning event
	  const_cast<fecom::commissioning_event&>(*it_set).add_tracker_channel_hit(tchit);
	}

	else {
	  // Trigger ID not exist, create a new commissioning event, add the calo hit then add it to the comm set
	  fecom::commissioning_event a_new_comm_event;
	  a_new_comm_event.set_trigger_id(actual_hit_trigger_id);
	  a_new_comm_event.add_tracker_channel_hit(tchit);
	  commissioning_event_collection.add_commissioning_event(a_new_comm_event);
	}
      }

      else DT_THROW(std::logic_error, "Nor calo and tracker hit are valid ! ");

      hit_counter++;
    } // end of while reader

    reader.reset();


    std::string output_filename = "${FECOM_RESOURCES_DIR}/output_test/commissioning_event_large.data.bz2";
    datatools::fetch_path_with_env(output_filename);
    {
      DT_LOG_DEBUG(logging, "Serialization output file :" + output_filename);
      DT_LOG_DEBUG(logging, "Serialize the commissioning event data...");
      datatools::data_writer serializer(output_filename,
				      datatools::using_multiple_archives);
      serializer.store(commissioning_event_collection);
      DT_LOG_DEBUG(logging, "The commissioning event data has been stored in the '" + output_filename + "' file");
    }

    // DT_LOG_DEBUG(logging, "Serialize the commissioning event data in a brio file...");
    // std::string output_brio_filename = "${FECOM_RESOURCES_DIR}/output_test/commissioning_event.brio";
    // brio::writer my_writer(output_brio_filename, logging);
    // my_writer.store(commissioning_event_collection);
    // my_writer.close();

    fecom::commissioning_event_data deserialize_commissioning_event_collection;
    {
      DT_LOG_DEBUG(logging, "Deserialize the commissioning event data...");

      datatools::data_reader deserializer(output_filename,
					datatools::using_multiple_archives);

      deserializer.load(deserialize_commissioning_event_collection);
      DT_LOG_DEBUG(logging, "The commissioning event data has been loaded");
    }

    std::clog << "Size of deserialized commissioning event data = [" << deserialize_commissioning_event_collection.get_commissioning_event_collection().size() << "]" << std::endl;

    // DT_LOG_DEBUG(logging, "Deserialize the commissioning event data from a brio file...");

    // fecom::commissioning_event_data deserialize_brio_commissioning_event_collection;
    // brio::reader my_brio_reader(output_brio_filename, logging);

    // my_brio_reader.load(deserialize_brio_commissioning_event_collection);
    // DT_LOG_DEBUG(logging, "The commissioning event data has been loaded from a brio file");

    // std::clog << "Size of deserialized commissioning event data from brio file = [" << deserialize_brio_commissioning_event_collection.get_commissioning_event_collection().size() << "]" << std::endl;

    /*
      std::size_t event_counter = 0;
      for (auto it_event =  deserialize_brio_commissioning_event_collection.get_commissioning_event_collection().begin();
      it_event !=  deserialize_brio_commissioning_event_collection.get_commissioning_event_collection().end();
      it_event++)
      {
      std::clog << "****** Event #" << event_counter << " *******" <<std::endl;
      fecom::commissioning_event a_commissioning_event = * it_event;

      std::clog << "Calo size : " << a_commissioning_event.get_calo_hit_collection().size()
      << " tracker size : " <<  a_commissioning_event.get_tracker_channel_hit_collection().size() << std::endl;

      std::size_t calo_counter = 0;
      for (auto it_calo = a_commissioning_event.get_calo_hit_collection().begin();
      it_calo != a_commissioning_event.get_calo_hit_collection().end();
      it_calo++)
      {
      fecom::calo_hit a_calo_hit = * it_calo;
      // a_calo_hit.tree_dump(std::clog, "Read from commissioning event calo #" + std::to_string(calo_counter));
      calo_counter++;
      std::clog << "calo counter = " << calo_counter << std::endl;
      }

      std::size_t tracker_counter = 0;
      for (auto it_tracker = a_commissioning_event.get_tracker_channel_hit_collection().begin();
      it_tracker != a_commissioning_event.get_tracker_channel_hit_collection().end();
      it_tracker++)
      {
      fecom::tracker_channel_hit a_tracker_channel_hit = * it_tracker;
      //	    a_tracker_channel_hit.tree_dump(std::clog, "Read from commissioning event tracker #" + std::to_string(tracker_counter));
      tracker_counter++;
      std::clog << "tracker counter = " << tracker_counter << std::endl;
      }

      event_counter++;
      } // end of it_event
    */
    DT_LOG_DEBUG(logging, "Exiting test-libfecom-fake_hit_reader_2.cxx...");

  } catch (std::exception & error) {
    std::cerr << "error: " << error.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}


/*
  uint16_t associated_channel_1 = -1;
  uint16_t associated_channel_2 = -1;

  fecom::channel_mapping my_channel_mapping;

  for (std::size_t ichannel = 0;
  ichannel < fecom::tracker_constants::NUMBER_OF_CHANNEL_PER_FEAST;
  ichannel++) {
  my_channel_mapping.get_associated_channels(ichannel,
  associated_channel_1,
  associated_channel_2);

  // std::clog << "*** Input channel        = " << ichannel << std::endl;
  // std::clog << "*** Associated channel 1 = " << associated_channel_1 << std::endl;
  // std::clog << "*** Associated channel 2 = " << associated_channel_2 << std::endl;
  // std::clog << "***************************" << std::endl;
  }
*/
