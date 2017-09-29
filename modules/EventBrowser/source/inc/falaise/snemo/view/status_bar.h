// -*- mode: c++ ; -*-
/* status_bar.h
 * Author (s) :   Xavier Garrido <<garrido@lal.in2p3.fr>>
 * Creation date: 2010-06-17
 * Last modified: 2017-06-28
 *
 * Copyright (C) 2011-2017 Xavier Garrido <garrido@lal.in2p3.fr>
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
 *   Status bar to display run information and navigate through events
 *
 * History:
 *
 *   28/06/2017 : add "Goto event number" widget
 *
 */

#ifndef FALAISE_SNEMO_VISUALIZATION_VIEW_STATUS_BAR_H
#define FALAISE_SNEMO_VISUALIZATION_VIEW_STATUS_BAR_H 1

// - ROOT:
#include <Rtypes.h>

// - This project:
#include <falaise/snemo/view/event_browser_signals.h>

// Forward declaration
class TGCompositeFrame;
class TGComboBox;
class TGPictureButton;
class TGNumberEntryField;

namespace snemo {

namespace visualization {

namespace io {
class event_server;
}

namespace view {

/// \brief Status bar to display run information and navigate through
/// events
class status_bar {
 public:
  /// Default constructor
  status_bar();

  /// Destructor
  virtual ~status_bar();

  /// Set event server reference
  void set_event_server(io::event_server* server_);

  /// Return initialization flag
  bool is_initialized() const;

  /// Initialize status bar
  void initialize(TGCompositeFrame* main_);

  /// Update method
  void update(const bool reset_ = false, const bool disable_ = false);

  /// Reset method
  void reset();

  /// Update GUI buttons state
  void update_buttons(const button_signals_type signal_ = UNDEFINED);

  /// Reset GUI buttons state
  void reset_buttons();

  /// Process callback signals
  void process();

 private:
  /// Main initialization method
  void _at_init_(TGCompositeFrame* main_);

 private:
  bool _initialized_;  //!< Initialization flag

  io::event_server* _server_;  //!< Event server reference

  TGComboBox* _event_list_;            //!< Event list combo box
  TGNumberEntryField* _goto_event_;    //!< Goto event number box
  TGPictureButton* _button_first_;     //!< First event button
  TGPictureButton* _button_previous_;  //!< Previous event button
  TGPictureButton* _button_next_;      //!< Next event button
  TGPictureButton* _button_last_;      //!< Last event button

  // No I/O so ClassDefVersionID = 0
  ClassDef(status_bar, 0);
};

}  // end of namespace view

}  // end of namespace visualization

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_VISUALIZATION_VIEW_EVENT_DISPLAY_H

// end of status_bar.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
