// test_snemo_datamodel_sim_tracker_digi_hit.cxx

// Standard libraries :
#include <cstdlib>
#include <exception>
#include <iostream>

// Third party:
// - Bayeux/datatools:
#include <datatools/io_factory.h>
#include <datatools/logger.h>

// Falaise:
#include <falaise/falaise.h>

// This project :
#include <snemo/datamodels/sim_tracker_digi_hit.h>

int main(int argc_, char** argv_) {
  falaise::initialize(argc_, argv_);
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  try {
    std::clog << "Test program for class 'snemo::datamodel::sim_tracker_digi_hit' !" << std::endl;

    {
      snemo::datamodel::sim_tracker_digi_hit my_hit;
      my_hit.set_hit_id(23);
      my_hit.grab_geom_id().set_type(1234);
      my_hit.grab_geom_id().set_address(3, 0, 10);
      my_hit.set_anode_t0(323);
      my_hit.set_anode_t1(423);
      my_hit.set_anode_t2(432);
      my_hit.set_anode_t3(673);
      my_hit.set_cathode_t6(683);
      my_hit.tree_dump(std::clog, "Simulated tracker digitized hit: ");

      {
        datatools::data_writer writer("test_snemo_datamodel_sim_tracker_digi_hit.xml",
                                      datatools::using_multi_archives);
        writer.store(my_hit);
      }
    }

    {
      snemo::datamodel::sim_tracker_digi_hit my_hit;
      {
        datatools::data_reader reader("test_snemo_datamodel_sim_tracker_digi_hit.xml",
                                      datatools::using_multi_archives);
        reader.load(my_hit);
      }
      my_hit.tree_dump(std::clog, "Loaded simulated tracker digitized hit: ");
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
