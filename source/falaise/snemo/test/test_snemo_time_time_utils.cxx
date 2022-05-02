// test_snemo_time_time_utils.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

// This project:
#include <falaise/snemo/time/time_utils.h>

int main(int /* argc_ */, char** /* argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::time::time_utils'!" << std::endl;

    namespace snt = snemo::time;
    snt::time_point runStart(snt::date(2022, 5, 27),
                             snt::hours(2) +  snt::minutes(13)
                             +  snt::seconds(42) + snt::milliseconds(678));
    std::cout << "Run start : " << snt::to_string(runStart) << std::endl;

    snt::time_duration runDuration(snt::hours(1) +  snt::minutes(3)
                                   +  snt::seconds(17) + snt::milliseconds(123));
    std::cout << "Run duration : " << snt::to_string(runDuration) << std::endl;

    snt::time_point runStop = runStart + runDuration;
    snt::time_period runPeriod(runStart, runStop);
    std::cout << "Run period : " << snt::to_string(runPeriod) << std::endl;

    std::string runStartRepr("2022-05-27 02:13:42.678000");
    runStart = snt::time_point_from_string(runStartRepr);
    std::cout << "\nInput run start : " << snt::to_string(runStart) << std::endl;
    std::string runDurationRepr("01:03:17.123000");
    runDuration = snt::time_duration_from_string(runDurationRepr);
    std::cout << "Input run duration : " << snt::to_string(runDuration) << std::endl;
    std::string runPeriodRepr("[2022-05-27 02:13:42.678000/2022-05-27 03:16:59.801000]");
    runPeriod = snt::time_period_from_string(runPeriodRepr);
    std::cout << "Input run period : " << snt::to_string(runPeriod) << std::endl;
 
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
