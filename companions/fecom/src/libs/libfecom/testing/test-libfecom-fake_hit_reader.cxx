// Standard library:
#include <cstdlib>
#include <string>
#include <iostream>
#include <stdexcept>

// This project:
#include <fecom/hit_reader.hpp>

// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/io_factory.h>
#include <datatools/clhep_units.h>

// Third part :
// Root :
#include "TFile.h"
#include "TH1F.h"

int main(int /*argc_*/, char ** /*argv_*/)
{
  try {

    fecom::hit_reader reader;
    reader.set_logging(datatools::logger::PRIO_TRACE);
    reader.set_input_filename("${FECOM_RESOURCES_DIR}/data/samples/fake_run/calo_fake_tracker_hits_1.dat");
    reader.initialize();
    fecom::run_header header;
    reader.load_run_header(header);
    header.tree_dump(std::clog, "Run header:");

    fecom::calo_hit chit;
    fecom::tracker_hit thit;
    fecom::tracker_channel_hit tchit;

    std::map<uint32_t, std::pair<std::set<fecom::calo_hit, fecom::base_hit::compare>, std::set<fecom::tracker_hit, fecom::base_hit::compare> > > map_hit_collection;

    std::size_t hit_counter = 0;
    while(reader.has_next_hit()) {
      std::clog << "Hit counter = " << hit_counter << std::endl;
      chit.reset();
      thit.reset();
      reader.load_next_hit(chit, tchit);

      std::string valid = "none";
      if (chit.is_valid())
	{
	  // chit.tree_dump(std::clog, "Calo hit:");
	  valid = "calo";
	}
      if (tchit.is_valid())
	{
	  // thit.tree_dump(std::clog, "Tracker hit:");
	  valid = "tracker";
	}

      // Voir si la map contient le trigger ID (12 par exemple), si oui -> ajouterle calo / tracker hit
      // si non, crée une nouvelle collection et l'ajouter dans la map.

      uint32_t actual_hit_trigger_id = 0xFFFFFF;

      if (valid == "calo") {
	actual_hit_trigger_id = chit.trigger_id;
	auto it_map = map_hit_collection.find(actual_hit_trigger_id);
	if (it_map != map_hit_collection.end()) {
	  // Trigger ID already exist, add the calo hit to the calo collection (first of the pair)
	  it_map -> second.first.insert(chit);
	}

	else {
	  // Trigger ID does not exist, create a pair of a calo collection and an empty tracker collection and add the hit
	  std::pair<std::set<fecom::calo_hit, fecom::base_hit::compare>, std::set<fecom::tracker_hit, fecom::base_hit::compare> > a_pair_of_calo_tracker_collection;
	  std::set<fecom::calo_hit, fecom::base_hit::compare> calo_hit_collection_per_trigger_id;
	  std::set<fecom::tracker_hit, fecom::base_hit::compare> empty_tracker_hit_collection;
	  calo_hit_collection_per_trigger_id.insert(chit);
	  a_pair_of_calo_tracker_collection = std::make_pair(calo_hit_collection_per_trigger_id, empty_tracker_hit_collection);

	  map_hit_collection.insert(std::pair<uint32_t, std::pair<std::set<fecom::calo_hit, fecom::base_hit::compare>, std::set<fecom::tracker_hit, fecom::base_hit::compare> > > (actual_hit_trigger_id, a_pair_of_calo_tracker_collection) );
	}
      } // end of valid == 'calo'

      // else  if (valid == "tracker") {
      // 	actual_hit_trigger_id = thit.trigger_id;
      // 	auto it_map = map_hit_collection.find(actual_hit_trigger_id);
      // 	if (it_map != map_hit_collection.end()) {
      // 	  // Trigger ID already exist, add the tracker hit to the tracker collection (second of the pair)
      // 	  it_map -> second.second.insert(thit);
      // 	}

      // 	else {
      // 	  // Trigger ID does not exist, create a pair of a tracker collection and an empty calo collection and add the hit
      // 	  std::pair<std::set<fecom::calo_hit, fecom::base_hit::compare>, std::set<fecom::tracker_hit, fecom::base_hit::compare> > a_pair_of_calo_tracker_collection;
      // 	  std::set<fecom::calo_hit, fecom::base_hit::compare> empty_calo_hit_collection;
      // 	  std::set<fecom::tracker_hit, fecom::base_hit::compare> tracker_hit_collection_per_trigger_id;
      // 	  tracker_hit_collection_per_trigger_id.insert(thit);
      // 	  a_pair_of_calo_tracker_collection = std::make_pair(empty_calo_hit_collection, tracker_hit_collection_per_trigger_id);

      // 	  map_hit_collection.insert(std::pair<uint32_t, std::pair<std::set<fecom::calo_hit, fecom::base_hit::compare>, std::set<fecom::tracker_hit, fecom::base_hit::compare> > > (actual_hit_trigger_id, a_pair_of_calo_tracker_collection) );
      // 	}
      // } // end of valid == 'tracker'

      else DT_THROW(std::logic_error, "Nor calo and tracker hit are valid ! ");


      if (hit_counter++ > 20) break;
    } // end of while reader

    // Create and fill histograms :

    std::string string_buffer = "${FECOM_RESOURCES_DIR}/data/samples/fake_run/test_histograms.root";
    datatools::fetch_path_with_env(string_buffer);
    TFile* root_file = new TFile(string_buffer.c_str(), "RECREATE");

    // OLD WAY because tracker hits in data file are by channel and not by tracker hits !
    // See fake_hit_reader_2 for a 'good' decode.



    // const std::size_t CALO_MAX_NUMBER_OF_CHANNELS = 13;
    // const std::size_t TRACKER_MAX_NUMBER_OF_CHANNELS = 18;
    // const std::size_t TRACKER_NUMBER_OF_TIMES = 7;

    // TH1F * hit_calo_channel_TH1F[CALO_MAX_NUMBER_OF_CHANNELS];
    // for (unsigned int icalo_channel = 0; icalo_channel < CALO_MAX_NUMBER_OF_CHANNELS; icalo_channel++) {
    //   string_buffer = "hit_calo_channel_" + std::to_string(icalo_channel);

    //   hit_calo_channel_TH1F[icalo_channel] = new TH1F(string_buffer.c_str(),
    // 						      Form("Hit calo channel %i", icalo_channel),
    // 						      100, 0, 100);
    // }

    // TH1F * hit_tracker_channel_TH1F[TRACKER_MAX_NUMBER_OF_CHANNELS];
    // for (unsigned int itracker_channel = 0; itracker_channel < TRACKER_MAX_NUMBER_OF_CHANNELS; itracker_channel++) {
    //   string_buffer = "hit_tracker_channel_" + std::to_string(itracker_channel);
    //   hit_tracker_channel_TH1F[itracker_channel] = new TH1F(string_buffer.c_str(),
    // 							    Form("Hit tracker channel %i", itracker_channel),
    // 							    100, 0, 100);
    // }

    std::cout << "My Hit map contains" << std::endl;
    for (auto it=map_hit_collection.begin();
    	 it!=map_hit_collection.end();
    	 it++) {
      std::clog << "-------------------- Trigger ID = " << it->first << " --------------------" <<  std::endl;
      std::clog << it->first << " => Calo size : " << it->second.first.size() << "  Tracker size : " <<  it->second.second.size() << std::endl;

      for (auto it_calo = it->second.first.begin(); it_calo != it->second.first.end(); it_calo++) {
      	fecom::calo_hit a_calo_hit = *it_calo;
      	a_calo_hit.tree_dump(std::clog, "A Calo hit");
      }

      for (auto it_tracker = it->second.second.begin(); it_tracker != it->second.second.end(); it_tracker++) {
      	fecom::tracker_hit a_tracker_hit = *it_tracker;
	a_tracker_hit.tree_dump(std::clog, "A Tracker hit");
      }
    }

    root_file->cd();
    root_file->Write();
    root_file->Close();

    reader.reset();

  } catch (std::exception & error) {
    std::cerr << "error: " << error.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
