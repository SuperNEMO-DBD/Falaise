/* event_browser_ctrl.cc */

// http://drdobbs.com/cpp/184401518

#include <EventBrowser/view/event_browser.h>
#include <EventBrowser/view/event_browser_ctrl.h>
#include <EventBrowser/view/options_manager.h>

#include <iostream>

#include <boost/bind.hpp>

namespace snemo {

namespace visualization {

namespace view {

void event_browser_ctrl::set_event_browser(event_browser& browser_) {
  browser = &browser_;
  return;
}

event_browser_ctrl::event_browser_ctrl(event_browser& browser_, size_t max_counts_) {
  DT_LOG_TRACE(options_manager::get_instance().get_logging_priority(), "Entering...");

  // test
  browser = 0;
  browser_thread = 0;
  event_mutex = 0;
  event_available_condition = 0;

  event_availability_status = event_browser_ctrl::NOT_AVAILABLE_FOR_ROOT;
  stop_requested = false;
  counts = 0;
  max_counts = max_counts_;

  set_event_browser(browser_);

  event_mutex = new boost::mutex;
  event_available_condition = new boost::condition;

  DT_LOG_TRACE(options_manager::get_instance().get_logging_priority(), "Exiting.");
  return;
}

event_browser_ctrl::~event_browser_ctrl() {
  DT_LOG_TRACE(options_manager::get_instance().get_logging_priority(), "Entering...");
  set_stop_requested();
  if (event_mutex != 0) {
    DT_LOG_TRACE(options_manager::get_instance().get_logging_priority(),
                 "Acquire the event control lock...");
    boost::mutex::scoped_lock lock(*event_mutex);
    event_availability_status = event_browser_ctrl::ABORT;
    event_available_condition->notify_one();
  }
  if (browser_thread != 0) {
    browser_thread->join();
  }

  if (event_available_condition != 0) {
    delete event_available_condition;
    event_available_condition = 0;
  }
  if (event_mutex != 0) {
    delete event_mutex;
    event_mutex = 0;
  }

  if (browser_thread) {
    delete browser_thread;
    browser_thread = 0;
  }
  browser = 0;
  DT_LOG_TRACE(options_manager::get_instance().get_logging_priority(), "Exiting.");
  return;
}

void event_browser_ctrl::start() {
  DT_LOG_TRACE(options_manager::get_instance().get_logging_priority(), "Entering...");
  DT_LOG_TRACE(options_manager::get_instance().get_logging_priority(),
               "Allocating the event browser thread...");

  browser_thread = new boost::thread(boost::bind(&event_browser_ctrl::start_browsing_event, this));
  DT_LOG_TRACE(options_manager::get_instance().get_logging_priority(),
               "Event browser thread is allocated...");
  DT_LOG_TRACE(options_manager::get_instance().get_logging_priority(), "Exiting.");
  return;
}

void event_browser_ctrl::start_browsing_event() {
  DT_LOG_TRACE(options_manager::get_instance().get_logging_priority(), "Entering...");
  // 2014-09-25 XG : lock variable must be enclosed within {} in order to
  // kill them at the end of the check.
  {
    boost::mutex::scoped_lock lock(*event_mutex);
    while (event_availability_status == event_browser_ctrl::NOT_AVAILABLE_FOR_ROOT) {
      event_available_condition->wait(*event_mutex);
    }
  }
  browser->start_threading();
  DT_LOG_TRACE(options_manager::get_instance().get_logging_priority(), "Exiting.");
  return;
}

void event_browser_ctrl::set_stop_requested() {
  stop_requested = true;
  return;
}

bool event_browser_ctrl::is_stop_requested() const { return stop_requested; }

}  // end of namespace view

}  // end of namespace visualization

}  // end of namespace snemo

// end of event_browser_ctrl.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
