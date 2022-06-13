// test_snemo_rc_run_event.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

// This project:
#include <falaise/snemo/rc/run_event.h>

int main(int /* argc_ */, char** /* argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::rc::run_event'!" << std::endl;

    namespace snrc = snemo::rc;
    namespace snt = snemo::time;

    std::int32_t runId = 42;
    snt::time_point runStart(snt::date(2022, 5, 27), snt::hours(2) + snt::minutes(12));
    snrc::run_event runEvent1(runStart, snrc::run_event_category::START, runId);
    snrc::run_event runEvent2(runStart + snt::minutes(12),
                              snrc::run_event_category::PAUSE, runId);
    snrc::run_event runEvent3(runStart + snt::minutes(15),
                              snrc::run_event_category::RESUME, runId);
    snrc::run_event runEvent4(runStart + snt::minutes(45),
                              snrc::run_event_category::STOP, runId);

    std::clog << "Event #1:\n";
    runEvent1.print_tree(std::clog);
    std::clog << "\nEvent #2:\n";
    runEvent2.print_tree(std::clog);
    std::clog << "\nEvent #3:\n";
    runEvent3.print_tree(std::clog);
    std::clog << "\nEvent #4:\n";
    runEvent4.print_tree(std::clog);

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
