// test_trigger_digitized_hit.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

// Third party:
// - Bayeux/datatools:
#include <bayeux/datatools/io_factory.h>

// This project:
#include <falaise/snemo/datamodels/trigger_digitized_hit.h>

int main(/* int argc_, char ** argv_ */) {
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::datamodel::trigger_digitized_hit'" << std::endl;

    namespace sdm = snemo::datamodel;

    {
      sdm::trigger_digitized_hit triggerDigiHit;
      triggerDigiHit.set_hit_id(42);
      geomtools::geom_id geomId(0, 7, 52);;
      triggerDigiHit.set_geom_id(geomId);

      std::clog << "A trigger digitized (raw) hit:\n";
      triggerDigiHit.print_tree(std::clog);
      std::clog << "\n";

      // TODO:  Fill trigger digi data with some examples


      {
        datatools::data_writer xout("test_trigger_digitized_hit.xml", datatools::using_multi_archives);
        datatools::data_writer bout("test_trigger_digitized_hit.data", datatools::using_multi_archives);
        xout.store(triggerDigiHit);
        bout.store(triggerDigiHit);
      }

    }

    {
      sdm::trigger_digitized_hit triggerDigiHit;
      datatools::data_reader xin("test_trigger_digitized_hit.xml", datatools::using_multi_archives);
      xin.load(triggerDigiHit);
      std::clog << "A trigger digitized (raw) hit (from XML file):\n";
      triggerDigiHit.print_tree(std::clog);
      std::clog << "\n";
    }

    {
      sdm::trigger_digitized_hit triggerDigiHit;
      datatools::data_reader bin("test_trigger_digitized_hit.data", datatools::using_multi_archives);
      bin.load(triggerDigiHit);
      std::clog << "A trigger digitized (raw) hit (from binary file):\n";
      triggerDigiHit.print_tree(std::clog);
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
