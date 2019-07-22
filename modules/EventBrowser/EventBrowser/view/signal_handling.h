// -*- mode: c++ ; -*-
/* signal_handling.h
 * Author (s) :     Xavier Garrido <<garrido@lal.in2p3.fr>>
 * Creation date: 2010-07-07
 * Last modified: 2010-07-07
 *
 * Copyright (C) 2011 Xavier Garrido <garrido@lal.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 *
 * Description:
 *   Bindings for event browser
 *   used to catch and handle events
 *   in event browser canvas
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_VISUALIZATION_VIEW_SIGNAL_HANDLING_H
#define FALAISE_SNEMO_VISUALIZATION_VIEW_SIGNAL_HANDLING_H 1

#include <EventBrowser/view/event_browser_signals.h>

class TGMainFrame;
struct Event_t;

namespace snemo {

namespace visualization {

namespace view {

class event_browser;

class signal_handling {
 public:
  signal_handling(TGMainFrame* main_);
  ~signal_handling();

  bool handle_key(Event_t* event_);

  bool process_message(long msg_, long parm1_, long parm2_);

 private:
  void _process_button_(const button_signals_type signal_);

  void _process_menu_(const button_signals_type signal_);

  void _process_menu_options_(const button_signals_type signal_);

 private:
  event_browser* _browser_;
};

}  // end of namespace view

}  // end of namespace visualization

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_VISUALIZATION_VIEW_SIGNAL_HANDLING_H

// end of signal_handling.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
