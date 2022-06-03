// test_calo_digitized_hit.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

// Third party:
// - Bayeux/datatools:
#include <bayeux/datatools/io_factory.h>

// This project:
#include <snemo/datamodels/calo_digitized_hit.h>

int main(/* int argc_, char ** argv_ */) {
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::datamodel::calo_digitized_hit'" << std::endl;

    namespace sdm = snemo::datamodel;
    
    {
      sdm::calo_digitized_hit caloDigiHit;
      caloDigiHit.set_hit_id(42);
      sncabling::om_id omId(sncabling::OM_MAIN, 0, 9, 4);
      geomtools::geom_id geomId;
      sdm::sncabling_bridge::convert_to_geom_id(omId, geomId);
      caloDigiHit.set_geom_id(geomId);
      caloDigiHit.set_reference_time(snfee::data::timestamp(snfee::data::CLOCK_160MHz, 1234567));
      caloDigiHit.set_low_threshold_only(false);
      caloDigiHit.set_high_threshold(true);
      caloDigiHit.set_fcr(132);
      std::vector<int16_t> & wf = caloDigiHit.grab_waveform();
      wf.assign(1024, 700);
      for (int i = 120; i < 200; i++) {
        wf[i] = 650;
      }
      caloDigiHit.set_fwmeas_baseline(2893);
      caloDigiHit.set_fwmeas_charge(65361);
      caloDigiHit.set_origin(sdm::calo_digitized_hit::rtd_origin(234, 213223));
      caloDigiHit.set_om_id(omId);

      std::clog << "A calorimeter digitized (raw) hit:\n";
      caloDigiHit.print_tree(std::clog);
      std::clog << "\n";
 
      {
        datatools::data_writer xout("test_calo_digitized_hit.xml", datatools::using_multi_archives);
        datatools::data_writer bout("test_calo_digitized_hit.data", datatools::using_multi_archives);
        xout.store(caloDigiHit);
        bout.store(caloDigiHit);
      }

    }
    {
      sdm::calo_digitized_hit caloDigiHit;
      datatools::data_reader xin("test_calo_digitized_hit.xml", datatools::using_multi_archives);
      xin.load(caloDigiHit);
      std::clog << "A calorimeter digitized (raw) hit (from XML file):\n";
      caloDigiHit.print_tree(std::clog);
      std::clog << "\n";
    }
    {
      sdm::calo_digitized_hit caloDigiHit;
      datatools::data_reader bin("test_calo_digitized_hit.data", datatools::using_multi_archives);
      bin.load(caloDigiHit);
      std::clog << "A calorimeter digitized (raw) hit (from binary file):\n";
      caloDigiHit.print_tree(std::clog);
      std::clog << "\n";
    }
 
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
