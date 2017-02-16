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


int main(int /*argc_*/, char ** /*argv_*/)
{
  datatools::logger::priority logging = datatools::logger::PRIO_DEBUG;
  try {
    DT_LOG_DEBUG(logging, "Entering test-libfecom-event_builder.cxx...");

    std::string input_filename("${FECOM_RESOURCES_DIR}/data/samples/fake_run/calo_fake_tracker_hits_2.data.bz2");
    // std::string input_filename = "${FECOM_RESOURCES_DIR}/output_test/commissioning_event_10_events.data.bz2";
    datatools::fetch_path_with_env(input_filename);

    std::string input_mapping_file("${FECOM_RESOURCES_DIR}/config/mapping_tracker.csv");
    datatools::fetch_path_with_env(input_mapping_file);

    // Deserialization of data :
    fecom::commissioning_event_data deserialize_commissioning_event_collection;
    {
      DT_LOG_DEBUG(logging, "Deserialize the commissioning event data...");
      datatools::data_reader deserializer(input_filename,
					  datatools::using_multiple_archives);

      deserializer.load(deserialize_commissioning_event_collection);
      DT_LOG_DEBUG(logging, "The commissioning event data has been loaded");
    }

    std::clog << "Size of deserialized commissioning event data = [" << deserialize_commissioning_event_collection.get_commissioning_event_collection().size() << "]" << std::endl;

    // See if mapping is really useful ?
    fecom::channel_mapping my_channel_mapping;
    my_channel_mapping.build_mapping_from_file(input_mapping_file);
    my_channel_mapping.initialize();


    std::string string_buffer = "${FECOM_RESOURCES_DIR}/output_test/test_histograms.root";
    datatools::fetch_path_with_env(string_buffer);
    TFile* root_file = new TFile(string_buffer.c_str(), "RECREATE");

    const std::size_t CALO_MAX_NUMBER_OF_CHANNELS = 13;
    const std::size_t TRACKER_MAX_NUMBER_OF_CHANNELS = 36;
    // const std::size_t TRACKER_NUMBER_OF_TIMES = 7;

    TH1F * hit_calo_channel_TH1F[CALO_MAX_NUMBER_OF_CHANNELS];
    for (unsigned int icalo_channel = 0; icalo_channel < CALO_MAX_NUMBER_OF_CHANNELS; icalo_channel++) {
      string_buffer = "hit_calo_channel_" + std::to_string(icalo_channel);

      hit_calo_channel_TH1F[icalo_channel] = new TH1F(string_buffer.c_str(),
    						      Form("Hit calorimeter %i", icalo_channel),
    						      5, 0, 5);
    }

    TH1F * hit_tracker_channel_TH1F[TRACKER_MAX_NUMBER_OF_CHANNELS];
    for (unsigned int itracker_channel = 0; itracker_channel < TRACKER_MAX_NUMBER_OF_CHANNELS; itracker_channel++) {
      string_buffer = "hit_tracker_channel_" + std::to_string(itracker_channel);
      hit_tracker_channel_TH1F[itracker_channel] = new TH1F(string_buffer.c_str(),
    							    Form("Hit tracker GG cell %i", itracker_channel),
    							    5, 0, 5);
    }


    std::size_t event_counter = 0;
    for (std::set<fecom::commissioning_event>::iterator it_event = deserialize_commissioning_event_collection.grab_commissioning_event_collection().begin();
    	 it_event !=  deserialize_commissioning_event_collection.grab_commissioning_event_collection().end();
    	 it_event++)
      {
	std::clog << "DEBUG : TEST HISTOGRAM BUILDER : EVENT #" << event_counter << std::endl;

    	std::clog << "Size of calo hit in the event : " << it_event->get_calo_hit_collection().size() << std::endl;
	std::size_t chit_counter = 0;
	for (auto it_chit = it_event->get_calo_hit_collection().begin();
	     it_chit != it_event->get_calo_hit_collection().end();
	     it_chit++)
	  {
	    // it_chit -> tree_dump(std::clog, "Calo hit #" + std::to_string(chit_counter));
	    uint8_t hit_channel = it_chit -> channel;
	    hit_calo_channel_TH1F[hit_channel]->Fill(1);


	    chit_counter++;
	  }
       	std::clog << "Size of tracker hit in the event : " << it_event->get_tracker_hit_collection().size() << std::endl;
	std::size_t thit_counter = 0;
	for (auto it_thit = it_event->get_tracker_hit_collection().begin();
	     it_thit != it_event->get_tracker_hit_collection().end();
	     it_thit++)
	  {
	    // it_thit -> tree_dump(std::clog, "Tracker hit #" + std::to_string(thit_counter));
	    uint8_t hit_channel = it_thit -> _cell_id_;
	    hit_tracker_channel_TH1F[hit_channel]->Fill(1);

	    thit_counter++;
	  }

	event_counter++;
      }

    root_file->Write();
    root_file->Close();


    DT_LOG_DEBUG(logging, "Exiting test-libfecom-event_builder.cxx...");

  } catch (std::exception & error) {
    std::cerr << "error: " << error.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
