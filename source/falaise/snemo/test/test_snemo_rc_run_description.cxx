// test_snemo_rc_run_description.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

// This project:
#include <falaise/snemo/rc/run_description.h>

int main(int /* argc_ */, char** /* argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::rc::run_description'!" << std::endl;

    namespace snrc = snemo::rc;
    namespace snt = snemo::time;
    snt::time_point runStart(snt::date(2022, 5, 27), snt::hours(2));
    snt::time_duration runDuration =
      snt::hours(4) + snt::minutes(23) + snt::seconds(42);
    snt::time_point runStop = runStart + runDuration;
    snt::time_period runPeriod(runStart, runStop);
    
    snrc::run_description runDesc = snrc::run_description::make(42,
                                                                snrc::run_category::TEST,
                                                                runPeriod,
                                                                1234);
    runDesc.add_break(snt::minutes(35), snt::minutes(12));
    runDesc.add_break(snt::hours(1) + snt::minutes(17),
                      snt::minutes(17));
                       
    runDesc.print_tree(std::clog);

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
