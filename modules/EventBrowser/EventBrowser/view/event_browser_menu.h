// -*- mode: c++ ; -*-
/* event_browser_menu.h
 * Author (s) :   Xavier Garrido <<garrido@lal.in2p3.fr>>
 * Creation date: 2010-06-11
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
 *   Menu for event browser
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_VISUALIZATION_VIEW_EVENT_BROWSER_MENU_H
#define FALAISE_SNEMO_VISUALIZATION_VIEW_EVENT_BROWSER_MENU_H 1

#include <EventBrowser/view/event_browser_signals.h>

#include <TGMenu.h>

#include <map>

class TGCompositeFrame;

namespace snemo {

namespace visualization {

namespace io {
class event_server;
}

namespace view {

/// \brief Homemade TGPopupMenu class to ease the use of popup submenu
/// especially enabling check/uncheck, enable/disable, hide/unhide
/// functionalities. Following
/// http://root.cern.ch/phpBB3/viewtopic.php?f=3&t=3919&p=15408&hilit=TGPopupMenu#p15408
/// http://root.cern.ch/phpBB3/viewtopic.php?f=3&t=14423
class TGMenuEntryPlus : public TGMenuEntry {
 public:
  // Public setters to TGMenuEntry members
  void SetLabel(TGHotString *label_);
  void SetPicture(const TGPicture *pic_);
  void SetType(const EMenuEntryType type_);
  void SetEntryId(const int id_);
  void SetPopup(TGPopupMenu *popup_);
  void SetStatus(const int status_);
  void SetEx(const unsigned int ex_);
  void SetEy(const unsigned int ey_);
  void SetEh(const unsigned int eh_);
  void SetEw(const unsigned int ew_);
  void SetUserData(void *user_data_);
};

class TGPopupMenuPlus : public TGPopupMenu {
 public:
  TGPopupMenuPlus(const TGWindow *p = 0);
  virtual ~TGPopupMenuPlus();

  // Hack AddPopup to allow the use of an id (the check, disable mechanism
  // uses this id to check, enable/disable, hide ... the entry).
  virtual void AddPopupPlus(TGHotString *s, TGPopupMenu *popup, Int_t id = -2,
                            TGMenuEntry *before = 0, const TGPicture *p = 0);
  virtual void AddPopupPlus(const char *s, TGPopupMenu *popup, Int_t id = -2,
                            TGMenuEntry *before = 0, const TGPicture *p = 0);

  virtual void PlaceMenu(Int_t x, Int_t y, Bool_t stick_mode, Bool_t grab_pointer);

  virtual Int_t EndMenu(void *&userData);

  virtual void UpdateDictionnary(std::map<button_signals_type, TGPopupMenuPlus *> &dict_);

 private:
  int fPreviousId;  //!< To be used and remember when popup menu is (un)checked
};

/// \brief A menu manager for event browser
class event_browser_menu {
 public:
  /// Menu status
  enum status_type {
    DISABLED = 0,
    ENABLED = 1,
    CHECKED = 2,
    UNCHECKED = 3,
    HIDDEN = 4,
    UNHIDDEN = 5
  };

  /// Default constructor
  event_browser_menu(TGCompositeFrame *main_);

  /// Destructor
  ~event_browser_menu();

  /// Refresh menus
  void update(const button_signals_type signal_);

  /// Set default menu status given the event data bank
  void set_default_option(const io::event_server &server_);

  /// Check if menu button exists
  bool has_option(const button_signals_type signal_) const;

  /// Change menu button status
  void change_option(const button_signals_type signal_, const int status_);

  /// Check menu button
  void check_option(const button_signals_type signal_);

  /// Uncheck menu button
  void uncheck_option(const button_signals_type signal_);

  /// Enable menu button
  void enable_option(const button_signals_type signal_);

  /// Disable menu button
  void disable_option(const button_signals_type signal_);

  /// Hide menu button
  void hide_option(const button_signals_type signal_);

  /// Recursively check menu button
  bool rcheck_option(const button_signals_type signal_, const int status_);

 private:
  std::map<button_signals_type, TGPopupMenuPlus *> _popup_dict_;  //!< Menu dictionnary
};

}  // end of namespace view

}  // end of namespace visualization

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_VISUALIZATION_VIEW_EVENT_BROWSER_MENU_H

// end of event_browser_menu.h

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
