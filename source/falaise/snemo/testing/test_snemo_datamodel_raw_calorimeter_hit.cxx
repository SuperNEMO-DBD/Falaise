// test_snemo_datamodel_raw_calorimeter_hit.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third party:
// - Bayeux/datatools:
#include <datatools/smart_ref.h>
#include <datatools/clhep_units.h>

// This project:
#include <falaise/snemo/datamodels/raw_calorimeter_hit.h>

int main(/* int argc_, char ** argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::datamodel::raw_calorimeter_hit'!" << std::endl;

    namespace sdm = snemo::datamodel;

    sdm::raw_calorimeter_hit my_calo_hit;
    my_calo_hit.set_hit_id(1);
    my_calo_hit.grab_auxiliaries().store_flag("mock_hit");
    my_calo_hit.set_trigger_id(12345);
    uint16_t channel = 7;
    uint16_t board   = 3;
    uint16_t crate   = 1;
    uint16_t module  = 0;
    uint16_t address = (module << 12) | (crate << 10) | (board << 5) | channel;
    my_calo_hit.set_channel_address(address);
    uint16_t data_desc;
    data_desc = sdm::raw_calorimeter_hit::DATA_DESC_SIG_HASWAVEFORM;
    my_calo_hit.set_data_description(data_desc);
    my_calo_hit.set_waveform_first_sample(87);
    my_calo_hit.set_waveform_last_sample(268);
    sdm::raw_calorimeter_hit::waveform_type & waveform = my_calo_hit.grab_waveform();
    for (int sample = my_calo_hit.get_waveform_first_sample();
         sample <= my_calo_hit.get_waveform_last_sample();
         sample++) {
      waveform.push_back(3280);
    }
    // my_calo_hit.set_aux_baseline(2);
    // my_calo_hit.set_aux_peak_charge(3458);
    // my_calo_hit.set_aux_peak_amplitude(68);
    // my_calo_hit.set_aux_peak_time_max_amplitude(63);
    // my_calo_hit.set_aux_underflow_tot(0);
    // my_calo_hit.set_aux_time_cfd_interp(0);
    my_calo_hit.tree_dump(std::clog, "Raw calorimeter hit: ");

    std::clog << "The end." << std::endl;
  }
  catch(std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch(...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return(error_code);
}
