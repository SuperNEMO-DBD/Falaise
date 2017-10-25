// Standard library:
#include <cstdlib>
#include <iostream>
#include <stdexcept>

// This project:
#include <wctools/raw_event.hpp>
#include <wctools/raw_reader.hpp>
#include <wctools/utils.hpp>

/*
  From the build directory:
  \code
  $ WCTOOLS_RESOURCE_FILES_DIR=../resources ./wctools_tests/wctools-test-libwctools-raw_reader
  \endcode
*/

int main(int /*argc_*/, char** /*argv_*/) {
  try {
    wctools::raw_reader reader;
    std::string input_filename = "${WCTOOLS_RESOURCE_FILES_DIR}/data/samples/wc_1000keV_1.data";

    reader.set_input_filename(input_filename);
    reader.initialize();

    wctools::raw_event event;
    while (reader.has_next_event()) {
      reader.load_next_event(event);
      event.print(std::clog, "Event: ");
    }

    reader.reset();

  } catch (std::exception& error) {
    std::cerr << "error: " << error.what() << std::endl;
    return EXIT_FAILURE;
  } catch (...) {
    std::cerr << "error: "
              << "An unexpected error was caught!" << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
