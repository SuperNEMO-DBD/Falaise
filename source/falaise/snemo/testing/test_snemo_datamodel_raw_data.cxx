// test_snemo_datamodel_raw_data.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

// Third party:
// - Bayeux/datatools:
#include <datatools/clhep_units.h>
#include <datatools/smart_ref.h>
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#endif
#include <datatools/io_factory.h>
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

// This project:
#include <falaise/snemo/datamodels/raw_data.h>

int main(/* int argc_, char ** argv_ */) {
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::datamodel::raw_data'!" << std::endl;

    namespace sdm = snemo::datamodel;
    sdm::raw_data RD;

    // Trigger informations:
    sdm::raw_trigger_infos& RTI = RD.grab_raw_trigger_infos();
    RTI.set_trigger_number(1234567);

    // Calorimeter hits:
    for (int i = 0; i < 3; i++) {
      {
        sdm::raw_calorimeter_hit dummy;
        RD.grab_raw_calorimeter_hits().push_back(dummy);
      }
      sdm::raw_calorimeter_hit& my_calo_hit = RD.grab_raw_calorimeter_hits().back();
      my_calo_hit.set_hit_id(1);
      my_calo_hit.grab_auxiliaries().store_flag("mock_hit");
      my_calo_hit.set_trigger_id(12345);
      uint16_t channel = 7 + i;
      uint16_t board = 3 + i;
      uint16_t crate = 1;
      uint16_t module = 0;
      uint16_t address = (module << 12) | (crate << 10) | (board << 5) | channel;
      my_calo_hit.set_channel_address(address);
      uint16_t data_desc;
      data_desc = sdm::raw_calorimeter_hit::DATA_DESC_SIG_HASWAVEFORM;
      my_calo_hit.set_data_description(data_desc);
      my_calo_hit.set_waveform_first_sample(87);
      my_calo_hit.set_waveform_last_sample(268);
      sdm::raw_calorimeter_hit::waveform_type& waveform = my_calo_hit.grab_waveform();
      for (int sample = my_calo_hit.get_waveform_first_sample();
           sample <= my_calo_hit.get_waveform_last_sample(); sample++) {
        waveform.push_back(3280);
      }
    }

    // Tracker hits:
    for (int i = 0; i < 7; i++) {
      {
        sdm::raw_tracker_hit dummy;
        RD.grab_raw_tracker_hits().push_back(dummy);
      }
      sdm::raw_tracker_hit& my_gg_hit = RD.grab_raw_tracker_hits().back();
      my_gg_hit.set_hit_id(1);
      my_gg_hit.grab_auxiliaries().store_flag("mock_hit");
      my_gg_hit.set_trigger_id(12345);
      uint16_t channel = 4 + i;
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
      my_gg_hit.set_cathode_t6(3193);
    }

    RD.tree_dump(std::clog, "Raw data: ");

    datatools::data_writer writer("test_snemo_datamodel_raw_data.xml");
    writer.store(RD);

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
