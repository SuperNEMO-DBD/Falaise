/* event_browser_ctrl.h
 * Author(s)     : Xavier Garrido <garrido@lal.in2p3.fr>
 * Creation date : 2011-11-10
 * Last modified : 2011-11-10
 *
 * Description:
 *
 * Copyright (C) 2011 Xavier Garrido <garrido@lal.in2p3.fr>
 *
 */

#ifndef FALAISE_SNEMO_VISUALIZATION_VIEW_EVENT_BROWSER_CTRL_H
#define FALAISE_SNEMO_VISUALIZATION_VIEW_EVENT_BROWSER_CTRL_H 1

#define BOOST_SYSTEM_NO_DEPRECATED 1
#include <boost/thread/thread.hpp>
#undef BOOST_SYSTEM_NO_DEPRECATED
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

namespace snemo {

  namespace visualization {

    namespace view {

      class event_browser;

      class event_browser_ctrl
      {
      public :

        enum event_availability_status_type
          {
            NOT_AVAILABLE_FOR_ROOT = 0,
            AVAILABLE_FOR_ROOT     = 1,
            ABORT                  = 2
          };

        // test
        event_browser    * browser;
        boost::thread    * browser_thread;
        boost::mutex     * event_mutex;
        boost::condition * event_available_condition;
        int                event_availability_status;
        bool               stop_requested;
        size_t             counts;
        size_t             max_counts;

        void set_stop_requested ();

        bool is_stop_requested () const;

        void set_event_browser (event_browser & browser_);

        // ctor:
        event_browser_ctrl (event_browser & browser_, size_t max_counts_ = 0);

        // dtor:
        ~event_browser_ctrl ();

        void start ();

        void start_browsing_event ();

      };

    } // end of namespace view

  }  // end of namespace visualization

}  // end of namespace snemo

#endif // FALAISE_SNEMO_VISUALIZATION_VIEW_EVENT_BROWSER_CTRL_H

// end of event_browser_ctrl.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
