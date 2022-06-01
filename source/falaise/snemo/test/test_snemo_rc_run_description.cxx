// test_snemo_rc_run_description.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

// This project:
#include <falaise/snemo/rc/run_description.h>

void test1();
void test2();
void test3();

int main(int /* argc_ */, char** /* argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::rc::run_description'!" << std::endl;
    test1();
    test2();
    test3();
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

void test1()
{
  std::clog << "\nTest 1:\n";
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
  return;
}

void test2()
{
  std::clog << "\nTest 2:\n";
  namespace snrc = snemo::rc;
  namespace snt = snemo::time;
  snrc::run_description runDesc;
  runDesc.set_run_id(42);
  datatools::properties runDescConfig;
  runDescConfig.store("debug", true);
  runDescConfig.store("category", "commissioning");
  runDescConfig.store("period", "{2022-05-27 02:00:00 + 04:23:42}");
  runDescConfig.store("number_of_events", 1234);
  runDescConfig.store("number_of_breaks", 2);
  runDescConfig.store("break_0.period", "{2022-05-27 02:35:00 + 00:13:00}");
  runDescConfig.store("break_1.period", "{2022-05-27 04:04:00 + 00:17:10}");
  runDesc.load(runDescConfig);
  runDesc.print_tree(std::clog);
  return;
}

void test3()
{
  std::clog << "\nTest 3:\n";
  namespace snrc = snemo::rc;
  namespace snt = snemo::time;
  snrc::run_description runDesc;
  runDesc.set_run_id(42);
  datatools::properties runDescConfig;
  runDescConfig.store("debug", true);
  runDescConfig.store("category", "commissioning");
  runDescConfig.store("period", "[2022-05-27 02:00:00/2022-05-27 06:23:41.999999]");
  runDescConfig.store("number_of_events", 1234);
  std::vector<std::string> breakPeriodReprs = {"[2022-05-27 02:35:00/2022-05-27 02:47:59.999999]",
                                               "[2022-05-27 04:04:00/2022-05-27 04:20:69.999999]",
                                               "[2022-05-27 05:13:00/2022-05-27 05:21:28.999999]"
  };
  runDescConfig.store("breaks", breakPeriodReprs);
  runDesc.load(runDescConfig);
  runDesc.print_tree(std::clog);
  return;
}
