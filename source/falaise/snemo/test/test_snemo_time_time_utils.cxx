// test_snemo_time_time_utils.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

// Bayeux:
#include <bayeux/datatools/clhep_units.h>

// This project:
#include <falaise/snemo/time/time_utils.h>

int main(int /* argc_ */, char** /* argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::time::time_utils'!" << std::endl;

    namespace snt = snemo::time;
    
    snt::time_point runStart(snt::date(2025, 5, 27),
                             snt::hours(2) +  snt::minutes(13)
                             +  snt::seconds(42) + snt::milliseconds(678));
    std::cout << "Run start : " << snt::to_string(runStart) << std::endl;

    snt::time_duration runDuration(snt::hours(1) +  snt::minutes(3)
                                   +  snt::seconds(17) + snt::milliseconds(123));
    std::cout << "Run duration : " << snt::to_string(runDuration) << std::endl;

    snt::time_point runStop = runStart + runDuration;
    snt::time_period runPeriod(runStart, runStop);
    std::cout << "Run period : " << snt::to_string(runPeriod) << std::endl;

    std::string runStartRepr("2025-05-27 02:13:42.678000");
    runStart = snt::time_point_from_string(runStartRepr);
    std::cout << "\nInput run start : " << snt::to_string(runStart) << std::endl;
    std::string runDurationRepr("01:03:17.123042");
    runDuration = snt::time_duration_from_string(runDurationRepr);
    std::cout << "Input run duration : " << snt::to_string(runDuration) << std::endl;
    std::string runPeriodRepr("[2025-05-27 02:13:42.678000/2025-05-27 03:16:59.801000]");
    runPeriod = snt::time_period_from_string(runPeriodRepr);
    DT_THROW_IF(not snt::is_valid(runPeriod), std::logic_error,
                "Invalid run period '" << runPeriod  << "'!");
    std::cout << "Input run period : " << snt::to_string(runPeriod) << std::endl;

    double qRunDuration = snt::to_quantity(runDuration);
    std::cout.precision(15);
    std::cout << "qRunDuration = " << qRunDuration / CLHEP::second << " s" << std::endl;

    snt::time_point snEpoch = snt::sn_epoch();
    std::cout << "\nSuperNEMO epoch : " << snt::to_string(snEpoch) << std::endl;

    snt::time_duration runStartFromEpoch = snt::elapsed_since_sn_epoch(runStart);
    double qRunStartFromEpoch = snt::to_quantity(runStartFromEpoch);
    std::cout << "qRunStartFromEpoch = " << qRunStartFromEpoch / CLHEP::second << " s" << std::endl;

    snt::time_duration runStopFromEpoch = snt::elapsed_since_sn_epoch(runStop);
    double qRunStopFromEpoch = snt::to_quantity(runStopFromEpoch);
    std::cout << "qRunStopFromEpoch = " << qRunStopFromEpoch / CLHEP::second << " s" << std::endl;

    double qRunDuration2 = qRunStopFromEpoch - qRunStartFromEpoch;
    std::cout << "qRunDuration2 = " << qRunDuration2 / CLHEP::second << " s" << std::endl;
 
    snt::time_point epoch = snt::epoch();
    std::cout << "\nepoch : " << snt::to_string(epoch) << std::endl;

    double qRun1546Start = 1744303364.847308 * CLHEP::second;
    snt::time_point run1546Start = snt::time_point_from_epoch_sec(qRun1546Start);
    std::cout << "\nrun1546Start : " << snt::to_string(run1546Start) << std::endl;

    double qRun1546Duration = 39963.9 * CLHEP::second;
    snt::time_duration run1546Duration = snt::time_duration_from_sec(qRun1546Duration);
    std::cout << "Run duration : " << snt::to_string(run1546Duration) << std::endl;

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
