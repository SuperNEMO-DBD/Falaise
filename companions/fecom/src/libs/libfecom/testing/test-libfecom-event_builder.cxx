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

// Third party:
// - Boost:
// Code dedicated to the serialization of the ``std::set`` template class :
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/set.hpp>

int main(int /*argc_*/, char ** /*argv_*/)
{
  datatools::logger::priority logging = datatools::logger::PRIO_DEBUG;
  try {
    DT_LOG_DEBUG(logging, "Entering test-libfecom-event_builder.cxx...");

    std::string input_filename("${FECOM_RESOURCES_DIR}/data/samples/fake_run/calo_fake_tracker_hits_2.data.bz2");
    // std::string input_filename = "${FECOM_RESOURCES_DIR}/output_test/commissioning_event_10_events.data.bz2";
    datatools::fetch_path_with_env(input_filename);
    fecom::commissioning_event_data deserialize_commissioning_event_collection;
    {
      DT_LOG_DEBUG(logging, "Deserialize the commissioning event data...");
      datatools::data_reader deserializer(input_filename,
					  datatools::using_multiple_archives);

      deserializer.load(deserialize_commissioning_event_collection);
      DT_LOG_DEBUG(logging, "The commissioning event data has been loaded");
    }

    std::clog << "Size of deserialized commissioning event data = [" << deserialize_commissioning_event_collection.get_commissioning_event_collection().size() << "]" << std::endl;

    fecom::channel_mapping my_channel_mapping;
    my_channel_mapping.initialize();

    std::size_t event_counter = 0;
    for (std::set<fecom::commissioning_event>::iterator it_event = deserialize_commissioning_event_collection.grab_commissioning_event_collection().begin();
    	 it_event !=  deserialize_commissioning_event_collection.grab_commissioning_event_collection().end();
    	 it_event++)
      {
	std::clog << "DEBUG : TEST EVENT BUILDER : EVENT #" << event_counter << std::endl;
	std::clog << "Size of tracker channel hit in the event : " << it_event->get_tracker_channel_hit_collection().size() << std::endl;

	const_cast<fecom::commissioning_event&>(*it_event).set_channel_mapping(my_channel_mapping);

	// std::size_t tchit_counter = 0;
	// for (auto it_tchit = it_event->get_tracker_channel_hit_collection().begin(); it_tchit != it_event->get_tracker_channel_hit_collection().end(); it_tchit++)
	//   {
	//     it_tchit -> tree_dump(std::clog, "Tracker Channel hit #" + std::to_string(tchit_counter));
	//     tchit_counter++;
	//   };

	const_cast<fecom::commissioning_event&>(*it_event).build_tracker_hit_from_channels();

	std::clog << "Size of tracker hit in the event : " << it_event->get_tracker_hit_collection().size() << std::endl;
	std::size_t thit_counter = 0;
	for (auto it_thit = it_event->get_tracker_hit_collection().begin(); it_thit != it_event->get_tracker_hit_collection().end(); it_thit++)
	  {
	    it_thit -> tree_dump(std::clog, "Tracker hit #" + std::to_string(thit_counter));
	    thit_counter++;
	  }

	event_counter++;
      }

    // std::size_t cell_counter = 0;
    // for (auto it_vector = my_channel_mapping._channel_triplet_collection_.begin();
    // 	 it_vector != my_channel_mapping._channel_triplet_collection_.end();
    // 	 it_vector++)
    //   {
    // 	std::clog << "Cell #" << cell_counter << " Anodic       F" << it_vector->anodic_channel.feast_id << " Ch " << it_vector->anodic_channel.channel_id << std::endl;
    // 	std::clog << "Cell #" << cell_counter << " Cathodic Bot F" << it_vector->bottom_cathode_channel.feast_id << " Ch " << it_vector->bottom_cathode_channel.channel_id << std::endl;
    // 	std::clog << "Cell #" << cell_counter << " Cathodic Top F" << it_vector->top_cathode_channel.feast_id << " Ch " << it_vector->top_cathode_channel.channel_id << std::endl;
    // 	std::clog << "********************************************************" << std::endl;
    // 	cell_counter++;
    //   }

    // my_channel_mapping.cell_to_channel_mapping();
    // for (auto it_map = my_channel_mapping._cell_channel_mapping_.begin();
    // 	 it_map != my_channel_mapping._cell_channel_mapping_.end();
    // 	 it_map++)
    //   {
    // 	std::clog << "Channel #" << it_map->first << " <-> Cell #" << it_map->second << std::endl;
    //   }


    DT_LOG_DEBUG(logging, "Exiting test-libfecom-event_builder.cxx...");

  } catch (std::exception & error) {
    std::cerr << "error: " << error.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
