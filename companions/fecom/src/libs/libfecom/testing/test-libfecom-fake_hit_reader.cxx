// Standard library:
#include <cstdlib>
#include <string>
#include <iostream>
#include <stdexcept>

// This project:
#include <fecom/hit_reader.hpp>

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

    std::map<uint32_t, std::pair<std::vector<fecom::calo_hit>, std::vector<fecom::tracker_hit> > > map_hit_collection;

    std::size_t hit_counter = 0;
    while(reader.has_next_hit()) {
      std::clog << "Hit counter = " << hit_counter << std::endl;
      chit.reset();
      thit.reset();
      reader.load_next_hit(chit, thit);

      std::string valid = "none";
      if (chit.is_valid())
	{
	  chit.tree_dump(std::clog, "Calo hit:");
	  valid = "calo";
	}
      if (thit.is_valid())
	{
	  thit.tree_dump(std::clog, "Tracker hit:");
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
	  it_map -> second.first.push_back(chit);
	}

	else {
	  // Trigger ID does not exist, create a pair of a calo collection and an empty tracker collection and add the hit
	  std::pair<std::vector<fecom::calo_hit>, std::vector<fecom::tracker_hit> > a_pair_of_calo_tracker_collection;
	  std::vector<fecom::calo_hit> calo_hit_collection_per_trigger_id;
	  std::vector<fecom::tracker_hit> empty_tracker_hit_collection;
	  calo_hit_collection_per_trigger_id.push_back(chit);
	  a_pair_of_calo_tracker_collection = std::make_pair(calo_hit_collection_per_trigger_id, empty_tracker_hit_collection);

	  map_hit_collection.insert(std::pair<uint32_t, std::pair<std::vector<fecom::calo_hit>, std::vector<fecom::tracker_hit> > > (actual_hit_trigger_id, a_pair_of_calo_tracker_collection) );
	}
      } // end of valid == 'calo'

      else  if (valid == "tracker") {
	actual_hit_trigger_id = thit.trigger_id;
	auto it_map = map_hit_collection.find(actual_hit_trigger_id);
	if (it_map != map_hit_collection.end()) {
	  // Trigger ID already exist, add the tracker hit to the tracker collection (second of the pair)
	  it_map -> second.second.push_back(thit);
	}

	else {
	  // Trigger ID does not exist, create a pair of a tracker collection and an empty calo collection and add the hit
	  std::pair<std::vector<fecom::calo_hit>, std::vector<fecom::tracker_hit> > a_pair_of_calo_tracker_collection;
	  std::vector<fecom::calo_hit> empty_calo_hit_collection;
	  std::vector<fecom::tracker_hit> tracker_hit_collection_per_trigger_id;
	  tracker_hit_collection_per_trigger_id.push_back(thit);
	  a_pair_of_calo_tracker_collection = std::make_pair(empty_calo_hit_collection, tracker_hit_collection_per_trigger_id);

	  map_hit_collection.insert(std::pair<uint32_t, std::pair<std::vector<fecom::calo_hit>, std::vector<fecom::tracker_hit> > > (actual_hit_trigger_id, a_pair_of_calo_tracker_collection) );
	}
      } // end of valid == 'tracker'

      else DT_THROW(std::logic_error, "Nor calo and tracker hit are valid ! ");


      if (hit_counter++ > 10) break;
    }


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


    reader.reset();

  } catch (std::exception & error) {
    std::cerr << "error: " << error.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
