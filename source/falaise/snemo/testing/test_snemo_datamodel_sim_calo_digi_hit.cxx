// test_snemo_datamodel_sim_calo_digi_hit.cxx

// Standard libraries :
#include <cstdlib>
#include <exception>
#include <iostream>

// Third party:
// - Bayeux/datatools:
#include <datatools/clhep_units.h>
#include <datatools/io_factory.h>
#include <datatools/logger.h>

// Falaise:
#include <falaise/falaise.h>

// This project :
#include <snemo/datamodels/sim_calo_digi_hit.h>

int main(int argc_, char** argv_) {
  falaise::initialize(argc_, argv_);
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  long int seed = 314159;
  srand48(seed);

  try {
    std::clog << "Test program for class 'snemo::datamodel::sim_calo_digi_hit' !" << std::endl;

    {
      snemo::datamodel::sim_calo_digi_hit my_hit;
      my_hit.set_hit_id(23);
      my_hit.grab_geom_id().set_type(1234);
      my_hit.grab_geom_id().set_address(3, 0, 10);
      my_hit.grab_auxiliaries().store("test", "hello");
      std::clog << "Number of samples: " << my_hit.get_waveform().size() << std::endl;
      for (std::size_t ichannel = 0; ichannel < my_hit.get_waveform().size(); ichannel++) {
        // std::clog << "Set sample: " << ichannel << std::endl;
        double p = drand48();

	int16_t adc = 234 + p * 30;
	my_hit.grab_waveform().push_back(adc);
        }

      my_hit.tree_dump(std::clog, "Simulated calo digitized hit: ");

      {
        datatools::data_writer writer("test_snemo_datamodel_sim_calo_digi_hit.xml",
                                      datatools::using_multi_archives);
        writer.store(my_hit);
      }
    }

    {
      snemo::datamodel::sim_calo_digi_hit my_hit;
      {
        datatools::data_reader reader("test_snemo_datamodel_sim_calo_digi_hit.xml",
                                      datatools::using_multi_archives);
        reader.load(my_hit);
      }
      my_hit.tree_dump(std::clog, "Loaded simulated calo digitized hit: ");
    }

    std::clog << "The end." << std::endl;
  } catch (std::exception& error) {
    DT_LOG_FATAL(logging, error.what());
    error_code = EXIT_FAILURE;
  } catch (...) {
    DT_LOG_FATAL(logging, "Unexpected error!");
    error_code = EXIT_FAILURE;
  }
  falaise::terminate();
  return error_code;
}
