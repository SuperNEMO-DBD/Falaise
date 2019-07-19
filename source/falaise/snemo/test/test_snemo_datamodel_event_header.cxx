// test_event_header.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

// This project:
#include <falaise/snemo/datamodels/event_header.h>

int main(int /* argc_ */, char** /* argv_ */) {
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::datamodel::event_header'!" << std::endl;

    snemo::datamodel::event_header EH;
    EH.get_id().set_run_number(4);
    EH.get_id().set_event_number(11);
    EH.set_generation(snemo::datamodel::event_header::GENERATION_SIMULATED);
    EH.get_timestamp().set_seconds(1268644034);
    EH.get_timestamp().set_picoseconds(666);
    EH.get_properties().store("simulation.bundle", "falaise");
    EH.get_properties().store("simulation.version", "0.1");
    EH.get_properties().store("simulation.config", "0.1");
    EH.get_properties().store("author", std::string(getenv("USER")));
    EH.tree_dump(std::clog, "Event header: ", "NOTICE: ");

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
