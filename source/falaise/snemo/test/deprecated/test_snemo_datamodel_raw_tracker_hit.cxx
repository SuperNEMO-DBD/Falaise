// test_snemo_datamodel_raw_tracker_hit.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

// Third party:
// - Bayeux/datatools:
#include <datatools/clhep_units.h>
#include <datatools/smart_ref.h>

// This project:
#include <falaise/snemo/datamodels/raw_tracker_hit.h>

int main(/* int argc_, char ** argv_ */) {
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::datamodel::raw_tracker_hit'!" << std::endl;

    namespace sdm = snemo::datamodel;

    sdm::raw_tracker_hit my_gg_hit;
    my_gg_hit.set_hit_id(1);
    my_gg_hit.grab_auxiliaries().store_flag("mock_hit");
    my_gg_hit.set_trigger_id(12345);
    uint16_t channel = 4;
    uint16_t board = 3;
    uint16_t crate = 1;
    uint16_t module = 0;
    uint16_t address = (module << 12) | (crate << 10) | (board << 5) | channel;
    my_gg_hit.set_channel_address(address);
    uint16_t data_desc;
    data_desc = sdm::raw_tracker_hit::DATA_DESC_HAS_ANODE_T0 |
                sdm::raw_tracker_hit::DATA_DESC_HAS_ANODE_T1 |
                sdm::raw_tracker_hit::DATA_DESC_HAS_ANODE_T2 |
                sdm::raw_tracker_hit::DATA_DESC_HAS_ANODE_T3 |
                sdm::raw_tracker_hit::DATA_DESC_HAS_ANODE_T4 |
                sdm::raw_tracker_hit::DATA_DESC_HAS_CATHODE_T6 |
                (sdm::raw_tracker_hit::DATA_DESC_MODE | sdm::raw_tracker_hit::MODE_NEMO3);
    my_gg_hit.set_data_description(data_desc);
    my_gg_hit.set_anode_t0(0);
    my_gg_hit.set_anode_t1(801);
    my_gg_hit.set_anode_t2(3198);
    my_gg_hit.set_anode_t3(820);
    my_gg_hit.set_anode_t4(3174);
    // my_gg_hit.set_cathode_t5(0);
    my_gg_hit.set_cathode_t6(3193);
    my_gg_hit.tree_dump(std::clog, "Raw tracker hit: ");

    std::clog << "The end." << std::endl;
  } catch (std::exception& x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "error: "
              << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}
