// -*- mode: c++ ; -*-
/* event_browser.h
 * Author (s) :     Xavier Garrido <<garrido@lal.in2p3.fr>>
 * Creation date: 2010-06-11
 * Last modified: 2010-06-11
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
 *   main class for visualization
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_VISUALIZATION_VIEW_EVENT_BROWSER_H
#define FALAISE_SNEMO_VISUALIZATION_VIEW_EVENT_BROWSER_H 1

#include <EventBrowser/view/event_browser_signals.h>

#include <map>
#include <string>
#include <vector>

#include <KeySymbols.h>
#include <TGFrame.h>

class TGWindow;
class TGTab;

namespace snemo {

namespace visualization {

namespace io {
class event_server;
}

namespace view {

class event_browser_menu;
class event_display;
class full_2d_display;
class status_bar;
class signal_handling;
class event_browser_ctrl;

class event_browser : public TGMainFrame {
 public:
  /// Tab enumeration
  enum tab_id_index_type { EVENT_DISPLAY = 0, ONLINE_DISPLAY = 1, FULL_2D_DISPLAY = 2 };

  /// Default constructor
  event_browser(const TGWindow *window_, const unsigned int width_, const unsigned int height_);

  /// Destructor
  virtual ~event_browser();

  /// Return initialization status
  bool is_initialized() const;

  /// Initialize event browser
  void initialize();

  /// Initialization of all GUI components
  void initialize_gui();

  /// Initialization of the event server
  void initialize_event_server();

  /// Reset event browser
  void reset();

  /// Update event browser
  void update_browser(const bool reset_view_ = true);

  /// Update a given browser tab
  void update_tab(const tab_id_index_type index_, const bool reset_view_ = true);

  /// Update browser menus
  void update_menu(const button_signals_type signal_);

  /// Change tab signal
  void change_tab(const button_signals_type signal_ = NEXT_TAB);

  /// Change event signal
  void change_event(const button_signals_type signal_ = NEXT_EVENT, const int event_selected_ = -1);

  /// Track selection signal
  void track_select();

  /// Add a tab to simultanously see the three 2D views
  void add_full_2d_view();

  /// Print welcome message
  void welcome() const;

  /// Get a non-mutable reference to event server
  const io::event_server &get_event_server() const;

  /// Get a mutable reference to event server
  io::event_server &grab_event_server();

  /// Closing operation
  void close_window();

  /// Overload handling key event
  bool handle_key(Event_t *event_);

  /// Overload processing of callback message
  bool process_message(long msg_, long parm1_, long parm2_);

  /// Return thread status
  bool has_thread_ctrl() const;

  /// Set thread control status
  void set_thread_ctrl(event_browser_ctrl &thread_ctrl_);

  /// Get a non-mutable reference to control thred
  const event_browser_ctrl &get_thread_ctrl() const;

  /// Get a mutable reference to control thred
  event_browser_ctrl &grab_thread_ctrl();

  /// Lock thread
  void lock_thread();

  /// Unlock thread
  void unlock_thread();

  // Wait for thread
  void wait();

  /// Start thread process
  void start_threading();

  /// Stop thread process
  void stop_threading();

 private:
  /// Main initialization of event browser
  void _at_init_();

  /// Forbid use of default constructor
  event_browser();

  /// Forbid constructor copy of event browser
  event_browser(const event_browser &);

  /// Forbid copy of event browser
  event_browser &operator=(const event_browser &);

 private:
  bool _initialized_;  //!< Initialization flag

  event_browser_menu *_menu_;        //!< GUI menu
  event_display *_display_;          //!< Event display frame
  event_display *_full_2d_display_;  //!< 2D event displays
  status_bar *_status_;              //!< GUI status bar
  signal_handling *_handlers_;       //!< Signal handler

  TGTab *_tabs_;                                        //!< GUI tabs
  std::map<tab_id_index_type, bool> _tab_is_uptodate_;  //!< GUI tab status

  io::event_server *_event_server_;  //!< Event server

  event_browser_ctrl *_thread_ctrl_;  //!< Thread controller

 public:
  // ROOT Interface
  bool ProcessMessage(long msg_, long parm1_, long parm2_) {
    return process_message(msg_, parm1_, parm2_);
  }
  bool HandleKey(Event_t *event_) { return handle_key(event_); }
  void CloseWindow() { close_window(); }

  // No I/O so ClassDefVersionID = 0
  ClassDef(event_browser, 0);
};
}  // end of namespace view

}  // end of namespace visualization

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_VISUALIZATION_VIEW_EVENT_BROWSER_H

// end of event_browser.h

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
