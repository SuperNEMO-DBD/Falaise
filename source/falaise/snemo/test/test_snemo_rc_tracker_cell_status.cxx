// test_snemo_rc_tracker_cell_status.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

// This project:
#include <falaise/snemo/rc/tracker_cell_status.h>

void test1();

int main(int /* argc_ */, char** /* argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::rc::tracker_cell_status'!" << std::endl;

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
  namespace snrc = snemo::rc;
  namespace snt = snemo::time;

  snrc::tracker_cell_status_history cellHistory;

  std::uint32_t status1 = snrc::tracker_cell_status::CELL_DEAD;
  std::clog << "Status #1 = " << snrc::tracker_cell_status::status_to_string(status1) << '\n';
  snt::time_period p1(snt::time_point(snt::date(2022, 5, 27), snt::hours(2) + snt::minutes(12)),
                      snt::time_duration(snt::hours(100))
                      );
  cellHistory.add(p1, status1);

  std::uint32_t status2 = snrc::tracker_cell_status::CELL_NO_BOTTOM_CATHODE;
  std::clog << "Status #2 = " << snrc::tracker_cell_status::status_to_string(status2) << '\n';
  snt::time_period p2(p1.end(),
                      snt::time_duration(snt::hours(50))
                      );
  cellHistory.add(p2, status2);

  std::uint32_t status3 = snrc::tracker_cell_status::CELL_NO_BOTTOM_CATHODE | snrc::tracker_cell_status::CELL_NO_TOP_CATHODE;
  std::clog << "Status #3 = " << snrc::tracker_cell_status::status_to_string(status3) << '\n';
  snt::time_period p3(p2.end(),
                      snt::time_duration(snt::hours(250))
                      );
  cellHistory.add(p3, status3);

  std::clog << "Tracker cell history : \n";
  for (const auto & rec : cellHistory.records()) {
    std::clog << "- Period : " << snt::to_string(rec.period) << " : status=" << snrc::tracker_cell_status::status_to_string(rec.status) << '\n';
  }
  
  return;
}
