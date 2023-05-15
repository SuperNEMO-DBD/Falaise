// -*- mode: c++ ; -*-
/* browser_tracks.h
 * Author (s) :     Xavier Garrido <<garrido@lal.in2p3.fr>>
 * Creation date: 2010-06-15
 * Last modified: 2010-06-15
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
 *   browser tracks panel
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_VISUALIZATION_VIEW_BROWSER_TRACKS_H
#define FALAISE_SNEMO_VISUALIZATION_VIEW_BROWSER_TRACKS_H 1

#include <map>
#include <vector>

#include <Rtypes.h>

class TGCompositeFrame;
class TGListTree;
class TGListTreeItem;
class TGPicture;
class TObject;

namespace datatools {
class properties;
}

namespace geomtools {
class base_hit;
}

namespace snemo {

namespace visualization {

namespace io {
class event_server;
}

namespace view {

class event_browser;

class browser_tracks {
 public:
  /// Name of the property flag to mark an item as checked (make it private)
  static const std::string CHECKED_FLAG;

  /// Name of the property flag to set a color to 'hit' (make it private)
  static const std::string COLOR_FLAG;

  /// Name of the property flag to highlight 'hit' (make it private)
  static const std::string HIGHLIGHT_FLAG;

  /// Enum for icon entry
  enum icon_type {
    TRACK_CLOSED = 0,
    TRACK_OPEN = 1,
    CLUSTER_CLOSED = 2,
    CLUSTER_OPEN = 3,
    GEIGER_CLOSED = 4,
    GEIGER_OPEN = 5,
    BLANK = 6
  };

  /// Return initialization flag
  bool is_initialized() const;

  /// Default constructor
  browser_tracks(TGCompositeFrame *main_, io::event_server *server_);

  /// Destructor
  virtual ~browser_tracks();

  /// Initialization method
  void initialize(TGCompositeFrame *main_);

  /// Update browser panel
  void update();

  /// Clear browser panel
  void clear();

  /// Reset browser panel
  void reset();

  /// Callback method when an item is checked
  void item_checked(TObject *object_, bool is_checked_);

  /// Callback method when an item is selected
  void item_selected(TGListTreeItem *item_, int button_);

  /// Return pointer to 'datatools::properties' object given the item id
  datatools::properties *get_item_properties(const int id_);

  /// Return point to 'geomtools::bast_hit' object given item id
  geomtools::base_hit *get_base_hit(const int id_);

 protected:
  /// Update event header data bank
  void _update_event_header();

  /// Update simulated data bank
  void _update_simulated_data();

  /// Update digitized data bank
  void _update_digitized_data();

  /// Update calibrated data bank
  void _update_calibrated_data();

  /// Update tracker clustering data bank
  void _update_tracker_clustering_data();

  /// Update tracker trajectory data bank
  void _update_tracker_trajectory_data();

  /// Update particle track data bank
  void _update_particle_track_data();

 private:
  /// Main initialization method
  void _at_init_(TGCompositeFrame *main_);

  /// Panel construction
  void _at_construct_();

  /// Return TGPicture given icon type name
  const TGPicture *_get_colored_icon_(const std::string &icon_type,
                                      const std::string &hex_color_ = "",
                                      const bool reverse_color_ = false);

 private:
  bool _initialized_;  //!< Initialization flag

  io::event_server *_server_;  //!< Pointer to event server

  event_browser *_browser_;  //!< Pointer to event browser
  TGCompositeFrame *_main_;  //!< Main panel frame

  TGListTree *_tracks_list_box_;  //!< List of panel items

  TGListTreeItem *_top_item_;  //!< Top item

  std::map<std::string, const TGPicture *> _icons_;  //!< Dictionary of ROOT::TGPicture

  std::vector<TGListTreeItem *> _item_list_;  //! List of items

  // Dictionnary matching a pointer to an item id
  int _item_id_;  //!< Internal item id counter
  std::map<int, datatools::properties *>
      _properties_dictionnary_;                                 //!<
                                                                //! Dictionary
                                                                //! of
                                                                //!'datatools::properties' pointers
  std::map<int, geomtools::base_hit *> _base_hit_dictionnary_;  //!<
                                                                //! Dictionary
                                                                //! of
                                                                //!'geomtools::base_hit' pointers

  // No I/O so ClassDefVersionID = 0
  ClassDef(browser_tracks, 0);
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
