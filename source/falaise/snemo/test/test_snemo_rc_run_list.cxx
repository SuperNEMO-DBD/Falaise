// test_snemo_rc_run_list.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

// This project:
#include <falaise/snemo/rc/run_list.h>

int main(int /* argc_ */, char** /* argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::rc::run_list'!" << std::endl;

    namespace snrc = snemo::rc;
    namespace snt = snemo::time;

    snrc::run_list runList;
    
    snt::time_point runStart(snt::date(2022, 5, 27), snt::hours(2) + snt::minutes(12));
    snt::time_duration runDuration(snt::hours(10) + snt::minutes(23) + snt::seconds(42));

    std::int32_t run_id = 42;
    runList.add_run(snrc::run_description::make(run_id++,
                                                snrc::run_category::PRODUCTION,
                                                snt::time_period(runStart, runDuration),
                                                1234));
    
    runStart    += runDuration + snt::minutes(7);
    runDuration += snt::hours(2) + snt::minutes(37);
    
    runList.add_run(snrc::run_description::make(run_id++,
                                                snrc::run_category::PRODUCTION,
                                                snt::time_period(runStart, runDuration),
                                                2454));
    
    runStart    += runDuration + snt::minutes(13);
    runDuration -= snt::minutes(23);
    
    runList.add_run(snrc::run_description::make(run_id++,
                                                snrc::run_category::PRODUCTION,
                                                snt::time_period(runStart, runDuration),
                                                1431));
                      
    runList.print_tree(std::clog);

    std::clog << "\nList of run IDs:\n";
    std::set<std::int32_t> runIds;
    runList.build_run_ids(runIds);
    for (auto runId : runIds) {
      std::clog << runId << std::endl;
    }

    std::clog << "\nExtract run 43:\n";
    if (runList.has_run(43)) {
      runList.get_run(43).print_tree(std::clog);
    }

    snt::time_point eventTime = runStart + snt::hours(1);
    if (runList.contains(eventTime)) {
      auto runId = runList.locate_run(eventTime);
      std::clog << "\nEvent @" << snt::to_string(eventTime) << " is contained in run " << runId << "\n";
    }
    eventTime = runList.get_run(43).end() + snt::seconds(10);
    if (! runList.contains(eventTime)) {
      std::clog << "Event @" <<  snt::to_string(eventTime) << " is not contained\n";
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
