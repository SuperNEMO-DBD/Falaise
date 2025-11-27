// test_snemo_rc_run_phase.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

// This project:
#include <falaise/snemo/rc/run_phase.h>

void test1();

int main(int /* argc_ */, char** /* argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::rc::run_phase'!" << std::endl;
    test1();
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
  snrc::run_phase phase0;
  phase0.set_id(0);
  auto startTime0 = snt::time_point_from_string("2025-04-10 00:00");
  auto stopTime0  = snt::time_point_from_string("2025-05-22 23:59");
  phase0.set_start_time(startTime0);
  phase0.set_stop_time(stopTime0);
  phase0.set_status(snrc::run_phase::other_issues);
  {
    boost::property_tree::ptree props;
    props.put("title", "Phase 0");
    phase0.print_tree(std::clog, props);
  }
  
  snrc::run_phase phase1;
  phase1.set_id(1);
  auto startTime1 = snt::time_point_from_string("2025-05-23 00:00");
  auto stopTime1  = snt::time_point_from_string("2025-06-20 23:59");
  phase1.set_start_time(startTime1);
  phase1.set_stop_time(stopTime1);
  phase1.set_status(snrc::run_phase::good);
  {
    boost::property_tree::ptree props;
    props.put("title", "Phase 1");
    phase1.print_tree(std::clog, props);
  }
  
  snrc::run_phase phaseCurrent;
  phaseCurrent.set_id(2);
  auto startTime2 = snt::time_point_from_string("2025-06-21 00:00");
  phaseCurrent.set_start_time(startTime2);
  phaseCurrent.set_status(snrc::run_phase::good);
  {
    boost::property_tree::ptree props;
    props.put("title", "Current phase");
    phaseCurrent.print_tree(std::clog, props);
  }
  return;
}
