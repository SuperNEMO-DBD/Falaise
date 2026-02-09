// test_snemo_rc_calorimeter_om_status.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

// This project:
#include <falaise/snemo/rc/calorimeter_om_status.h>

void test1();
void test2();

int main(int /* argc_ */, char** /* argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::rc::calorimeter_om_status'!" << std::endl;

    test1();
    test2();
   
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
  std::clog << "\n\ntest1\n";
  namespace snrc = snemo::rc;
  namespace snt = snemo::time;

  snrc::calorimeter_om_status_history omHistory;

  std::uint32_t status1 = snrc::calorimeter_om_status::OM_DEAD;
  std::clog << "Status #1 = " << snrc::calorimeter_om_status::status_to_string(status1) << '\n';
  snt::time_period p1(snt::time_point(snt::date(2022, 5, 27), snt::hours(2) + snt::minutes(12)),
                      snt::time_duration(snt::hours(100))
                      );
  omHistory.add(p1, status1);

  std::uint32_t status2 = snrc::calorimeter_om_status::OM_OFF;
  std::clog << "Status #2 = " << snrc::calorimeter_om_status::status_to_string(status2) << '\n';
  snt::time_period p2(p1.end(),
                      snt::time_duration(snt::hours(50))
                      );
  omHistory.add(p2, status2);

  std::uint32_t status3 = snrc::calorimeter_om_status::OM_DEAD;
  std::clog << "Status #3 = " << snrc::calorimeter_om_status::status_to_string(status3) << '\n';
  snt::time_period p3(p2.end(),
                      snt::time_duration(snt::hours(250))
                      );
  omHistory.add(p3, status3);
  
  omHistory.print(std::clog);

  std::clog << "Calorimeter OM history : \n";
  for (const auto & rec : omHistory.records()) {
    std::clog << "- Period : " << snt::to_string(rec.period) << " : status=" << snrc::calorimeter_om_status::status_to_string(rec.status) << '\n';
  }

  return;
}
 
void test2()
{
  std::clog << "\n\ntest2\n";
  namespace snrc = snemo::rc;
  namespace snt = snemo::time;

  auto ev1 =
    snrc::calorimeter_om_status_change_event::make_set_bit(snt::time_point_from_string("2026-02-02 10:00:00.000000"),
							   snrc::calorimeter_om_status::OM_OFF);
  auto ev2 =
    snrc::calorimeter_om_status_change_event::make_unset_bit(snt::time_point_from_string("2026-02-02 11:00:00.000000"),
							     snrc::calorimeter_om_status::OM_OFF);
  auto ev3 =
    snrc::calorimeter_om_status_change_event::make_set_bit(snt::time_point_from_string("2026-02-02 14:00:00.000000"),
							   snrc::calorimeter_om_status::OM_NOISY);
  auto ev4 =
    snrc::calorimeter_om_status_change_event::make_unset_bit(snt::time_point_from_string("2026-02-02 14:30:00.000000"),
							     snrc::calorimeter_om_status::OM_NOISY);
  auto ev5 =
    snrc::calorimeter_om_status_change_event::make_set_bit(snt::time_point_from_string("2026-02-02 16:00:00.000000"),
							   snrc::calorimeter_om_status::OM_OFF);
  auto ev6 =
    snrc::calorimeter_om_status_change_event::make_unset_bit(snt::time_point_from_string("2026-02-02 17:00:00.000000"),
							     snrc::calorimeter_om_status::OM_OFF);
  auto ev7 =
    snrc::calorimeter_om_status_change_event::make_set_bit(snt::time_point_from_string("2026-02-02 18:00:00.000000"),
							   snrc::calorimeter_om_status::OM_NOISY);
  auto ev8 =
    snrc::calorimeter_om_status_change_event::make_set_bit(snt::time_point_from_string("2026-02-02 18:00:00.000000"),
							   snrc::calorimeter_om_status::OM_OTHER_ISSUES);
  auto ev9 =
    snrc::calorimeter_om_status_change_event::make_reset(snt::time_point_from_string("2026-02-02 18:30:00.000000"));

  auto ev10 =
    snrc::calorimeter_om_status_change_event::make_set_bit(snt::time_point_from_string("2026-02-02 18:30:00.000000"),
							   snrc::calorimeter_om_status::OM_DEAD);
  
  snrc::calorimeter_om_status_change_event_list eventList;
  eventList.add_event(ev1);
  eventList.add_event(ev2);
  eventList.add_event(ev3);
  eventList.add_event(ev4);
  eventList.add_event(ev5);
  eventList.add_event(ev6);
  eventList.add_event(ev7);
  eventList.add_event(ev8);
  eventList.add_event(ev9);
  eventList.add_event(ev10);

  for (auto iEvent = 0u; iEvent < eventList.size(); iEvent++) {
    const auto & event = eventList.event(iEvent);
    std::clog << "event#" << iEvent << " = " << event << '\n';
  }

  std::clog << "\n";
  snrc::calorimeter_om_status_history omHistory;
  snrc::build_calorimeter_om_status_history_from_event_list(eventList, omHistory);
  omHistory.print(std::clog);
  
  return;
}
