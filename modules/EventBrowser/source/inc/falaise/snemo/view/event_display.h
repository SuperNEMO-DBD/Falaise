// -*- mode: c++ ; -*-
/* event_display.h
 * Author (s) :   Xavier Garrido <<garrido@lal.in2p3.fr>>
 * Creation date: 2010-06-12
 * Last modified: 2014-07-12
 *
 * Copyright (C) 2011-2014 Xavier Garrido <garrido@lal.in2p3.fr>
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
 *
 *   Event display view
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_VISUALIZATION_VIEW_EVENT_DISPLAY_H
#define FALAISE_SNEMO_VISUALIZATION_VIEW_EVENT_DISPLAY_H 1

#include <map>

#include <KeySymbols.h>

#include <falaise/snemo/view/event_browser_signals.h>

class TGCompositeFrame;
class TGTab;

namespace snemo {

namespace visualization {

namespace io {
class event_server;
}

namespace view {

class display_3d;
class display_2d;
class browser_tracks;
class display_options;
class i_draw_manager;
class event_selection;
class status_bar;

/// \brief A general display class to visualize event within ROOT TCanvas
class event_display {
 public:
  /// Tab index
  enum tab_id_index_type {
    OVERVIEW_TAB = 0,
    TRACK_BROWSER_TAB = 1,
    OPTIONS_TAB = 2,
    SELECTION_TAB = 3
  };

  /// Default constructor
  event_display();

  /// Destructor
  ~event_display();

  /// Return initialization flag
  bool is_initialized() const;

  /// Initialization method
  void initialize(TGCompositeFrame* main_);

  /// Update display
  void update(const bool reset_view_ = true, const bool reset_track_ = true);

  /// Clear display
  void clear();

  /// Reset display
  void reset();

  /// Visualize all event given the signal argument
  void show_all(const button_signals_type signal_);

  /// Set full 2D display mode
  void set_full_2d_display(const bool full_ = true);

  /// Set event server
  void set_event_server(io::event_server* server_);

  /// Set status bar
  void set_status_bar(view::status_bar* status_);

  /// Handle button signal
  void handle_button_signals(const button_signals_type signal_, const int event_selected_ = -1);

 private:
  /// Main initialization method
  void _at_init_(TGCompositeFrame* main_);

  /// Update view status
  void _update_view_(const bool reset_view_ = true);

  /// Update tab status
  void _update_tab_(const tab_id_index_type index_, const bool reset_view_ = true);

 private:
  bool _initialized_;  //!< Initialization flag

  io::event_server* _server_;  //!< Event server
  status_bar* _status_;        //!< Event status

  display_3d* _display_3d_;  //!< 3D panel
  display_2d* _display_2d_;  //!< 2D panel
  bool _full_2d_display_;    //!< Flag for 2D full display
  display_2d* _top_2d_;      //!< 2D top view (full display)
  display_2d* _front_2d_;    //!< 2D front view (full display)
  display_2d* _side_2d_;     //!< 2D side view (full display)

  browser_tracks* _browser_tracks_;  //!< Browser panel
  display_options* _options_;        //!< Option panel
  event_selection* _selection_;      //!< Event selection

  i_draw_manager* _draw_manager_;  //!< Drawer manager

  TGTab* _tabs_;                                        //!< Tabs
  std::map<tab_id_index_type, bool> _tab_is_uptodate_;  //!< Tabs status
};

}  // end of namespace view

}  // end of namespace visualization

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_VISUALIZATION_VIEW_EVENT_DISPLAY_H

// end of event_display.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
