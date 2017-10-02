// Standard library:
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

// This project:
#include <fecom/channel_mapping.hpp>

// - Bayeux/datatools:
#include <datatools/clhep_units.h>
#include <datatools/io_factory.h>
#include <datatools/logger.h>
#include <datatools/utils.h>

int main(int /*argc_*/, char** /*argv_*/) {
  datatools::logger::priority logging = datatools::logger::PRIO_DEBUG;
  try {
    DT_LOG_DEBUG(logging, "Entering test-libfecom-channel_mapping.cxx...");

    std::string input_tracker_mapping_file("${FECOM_RESOURCES_DIR}/config/mapping_tracker.csv");
    datatools::fetch_path_with_env(input_tracker_mapping_file);

    std::string input_calo_mapping_file("${FECOM_RESOURCES_DIR}/config/mapping_calo.csv");
    datatools::fetch_path_with_env(input_calo_mapping_file);

    fecom::channel_mapping my_channel_mapping;
    my_channel_mapping.build_tracker_mapping_from_file(input_tracker_mapping_file);
    my_channel_mapping.build_calo_mapping_from_file(input_calo_mapping_file);
    my_channel_mapping.initialize();

    for (auto it_bimap = my_channel_mapping.tracker_bimap.right.begin();
         it_bimap != my_channel_mapping.tracker_bimap.right.end(); it_bimap++) {
      std::clog << "EID FEB : " << it_bimap->first << " <-> GID Cell : " << it_bimap->second
                << std::endl;
    }

    for (auto it_bimap = my_channel_mapping.calo_bimap.right.begin();
         it_bimap != my_channel_mapping.calo_bimap.right.end(); it_bimap++) {
      std::clog << "EID FEB : " << it_bimap->first << " <-> GID Calo OM : " << it_bimap->second
                << std::endl;
    }

    // for (std::size_t ichan = 0; ichan < 5; ichan++)
    //   {
    // 	uint16_t layer = -1;
    // 	uint16_t row = -1;
    // 	my_channel_mapping.get_cell_layer_row_for_a_channel(0,
    // 							    ichan,
    // 							    layer,
    // 							    row);

    // 	std::clog << "Layer = " << layer << " Row = " << row << std::endl;
    // 	std::clog << "is anodic ? : " << my_channel_mapping.is_anodic_channel(0, ichan) <<
    // std::endl;;
    //   }

    // fecom::channel_mapping my_channel_mapping;
    // my_channel_mapping.build_mapping_from_file(input_mapping_file);
    // my_channel_mapping.initialize();

    // std::size_t cell_counter = 0;
    // for (auto it_vector = my_channel_mapping._channel_triplet_collection_.begin();
    // 	 it_vector != my_channel_mapping._channel_triplet_collection_.end();
    // 	 it_vector++)
    //   {
    // 	std::clog << "Cell #" << cell_counter << " Anodic       F" <<
    // it_vector->anodic_channel.feast_id << " Ch " << it_vector->anodic_channel.channel_id <<
    // std::endl; 	std::clog << "Cell #" << cell_counter << " Cathodic Bot F" <<
    // it_vector->bottom_cathode_channel.feast_id << " Ch " <<
    // it_vector->bottom_cathode_channel.channel_id << std::endl; 	std::clog << "Cell #" <<
    // cell_counter << " Cathodic Top F" << it_vector->top_cathode_channel.feast_id << " Ch " <<
    // it_vector->top_cathode_channel.channel_id << std::endl; 	std::clog <<
    // "********************************************************" << std::endl;
    // 	cell_counter++;
    //   }

    // for (auto it_map = my_channel_mapping._cell_channel_mapping_.begin();
    // 	 it_map != my_channel_mapping._cell_channel_mapping_.end();
    // 	 it_map++)
    //   {
    // 	std::clog << "Channel #" << it_map->first << " <-> Cell #" << it_map->second << std::endl;
    //   }

    DT_LOG_DEBUG(logging, "Exiting test-libfecom-channel_mapping.cxx...");

  } catch (std::exception& error) {
    std::cerr << "error: " << error.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
