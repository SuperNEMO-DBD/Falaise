/* event_display.cc
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
 */

// This project
#include <falaise/snemo/view/browser_tracks.h>
#include <falaise/snemo/view/display_2d.h>
#include <falaise/snemo/view/display_3d.h>
#include <falaise/snemo/view/display_options.h>
#include <falaise/snemo/view/event_display.h>
#include <falaise/snemo/view/event_selection.h>
#include <falaise/snemo/view/options_manager.h>
#include <falaise/snemo/view/status_bar.h>

#include <falaise/snemo/view/default_draw_manager.h>
#include <falaise/snemo/view/snemo_draw_manager.h>

#include <falaise/snemo/io/event_server.h>

#include <falaise/snemo/detector/detector_manager.h>

// Third party:
// ROOT:
#include <TGFrame.h>
#include <TGSplitFrame.h>
#include <TGTab.h>
#include <TSystem.h>

namespace snemo {

namespace visualization {

namespace view {

bool event_display::is_initialized() const { return _initialized_; }

// ctor:
event_display::event_display() {
  _server_ = 0;
  _status_ = 0;
  _display_3d_ = 0;
  _display_2d_ = 0;
  _browser_tracks_ = 0;
  _options_ = 0;
  _selection_ = 0;
  _draw_manager_ = 0;
  _tabs_ = 0;
  _full_2d_display_ = false;
  _top_2d_ = 0;
  _front_2d_ = 0;
  _side_2d_ = 0;
  _initialized_ = false;
  return;
}

// dtor:
event_display::~event_display() {
  this->clear();
  this->reset();
  return;
}

void event_display::set_full_2d_display(const bool full_) {
  _full_2d_display_ = full_;
  return;
}

void event_display::set_event_server(io::event_server* server_) {
  DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized !");
  _server_ = server_;
  return;
}

void event_display::set_status_bar(view::status_bar* status_) {
  DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized !");
  _status_ = status_;
  return;
}

void event_display::initialize(TGCompositeFrame* main_) {
  DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized !");
  this->_at_init_(main_);
  _initialized_ = true;
  return;
}

void event_display::_at_init_(TGCompositeFrame* main_) {
  // Get detector setup label to instantiate the right drawer
  const detector::detector_manager& detector_mgr = detector::detector_manager::get_instance();

  switch (detector_mgr.get_setup_label()) {
    case detector::detector_manager::SNEMO:
    case detector::detector_manager::TRACKER_COMMISSIONING:
    case detector::detector_manager::SNEMO_DEMONSTRATOR:
      _draw_manager_ = new snemo_draw_manager(_server_);
      break;

    case detector::detector_manager::UNDEFINED:
    default:
      DT_LOG_WARNING(options_manager::get_instance().get_logging_priority(),
                     "Detector setup '"
                         << detector_mgr.get_setup_label_name()
                         << "' not yet supported by any drawing manager ! Use default one");
      _draw_manager_ = new default_draw_manager(_server_);
      break;
  }

  TGSplitFrame* main_frame = new TGSplitFrame(main_, main_->GetWidth(), main_->GetHeight());
  if (_full_2d_display_) {
    main_frame->SplitHorizontal();
    main_->AddFrame(main_frame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

    // 2D top display view
    TGSplitFrame* top_part = main_frame->GetFirst();
    top_part->SplitVertical();

    TGSplitFrame* top_right_part = top_part->GetFirst();
    _top_2d_ = new display_2d(top_right_part, _server_, false);
    _top_2d_->set_drawer(_draw_manager_);
    _top_2d_->update_detector();
    _top_2d_->set_view_type(TOP_VIEW);

    TGSplitFrame* top_left_part = top_part->GetSecond();
    _front_2d_ = new display_2d(top_left_part, _server_, false);
    _front_2d_->set_drawer(_draw_manager_);
    _front_2d_->update_detector();
    _front_2d_->set_view_type(FRONT_VIEW);

    TGSplitFrame* bottom_part = main_frame->GetSecond();
    _side_2d_ = new display_2d(bottom_part, _server_, false);
    _side_2d_->set_drawer(_draw_manager_);
    _side_2d_->update_detector();
    _side_2d_->set_view_type(SIDE_VIEW);
  } else {
    main_frame->SplitVertical();
    main_->AddFrame(main_frame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

    // Left part of the display:
    TGSplitFrame* left_part = main_frame->GetFirst();

    // Add the tabs
    TGSplitFrame* right_part = main_frame->GetSecond();
    _tabs_ = new TGTab(right_part, right_part->GetWidth(), right_part->GetHeight());
    right_part->AddFrame(_tabs_, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

    TGCompositeFrame* display_frame = _tabs_->AddTab("Overview");
    TGCompositeFrame* tracks_frame = _tabs_->AddTab("Tracks");
    TGCompositeFrame* options_frame = _tabs_->AddTab("Options");
    TGCompositeFrame* selection_frame = _tabs_->AddTab("Selection");
    _tab_is_uptodate_[OVERVIEW_TAB] = false;
    _tab_is_uptodate_[TRACK_BROWSER_TAB] = false;
    _tab_is_uptodate_[OPTIONS_TAB] = true;
    _tab_is_uptodate_[SELECTION_TAB] = true;

    // 2D/3D display view
    if (options_manager::get_instance().is_2d_display_on_left()) {
      _display_2d_ = new display_2d(left_part, _server_);
      _display_3d_ = new display_3d(display_frame, _server_);
    } else {
      _display_2d_ = new display_2d(display_frame, _server_);
      _display_3d_ = new display_3d(left_part, _server_);
    }

    _display_2d_->set_drawer(_draw_manager_);
    _display_2d_->update_detector();
    _display_3d_->set_drawer(_draw_manager_);
    _display_3d_->update_detector();

    // Track browser
    _browser_tracks_ = new browser_tracks(tracks_frame, _server_);

    // Display options
    _options_ = new display_options;
    _options_->initialize(options_frame);

    // Selection options
    _selection_ = new event_selection;
    _selection_->set_event_server(_server_);
    _selection_->set_status_bar(_status_);
    _selection_->initialize(selection_frame);
  }

  return;
}

void event_display::clear() {
  if (_draw_manager_) _draw_manager_->clear();
  return;
}

void event_display::reset() {
  DT_THROW_IF(!is_initialized(), std::logic_error, "Not initialized !");

  if (_full_2d_display_) {
    delete _top_2d_;
    _top_2d_ = 0;

    delete _front_2d_;
    _front_2d_ = 0;

    delete _side_2d_;
    _side_2d_ = 0;
  } else {
    delete _display_3d_;
    _display_3d_ = 0;

    delete _display_2d_;
    _display_2d_ = 0;

    delete _browser_tracks_;
    _browser_tracks_ = 0;

    delete _options_;
    _options_ = 0;

    delete _selection_;
    _selection_ = 0;

    delete _tabs_;
    _tabs_ = 0;
  }

  if (_draw_manager_) {
    delete _draw_manager_;
    _draw_manager_ = 0;
  }

  _initialized_ = false;
  return;
}

void event_display::update(const bool reset_view_, const bool reset_track_) {
  this->clear();
  // No draw manager, no update
  if (!_draw_manager_) {
    DT_LOG_WARNING(options_manager::get_instance().get_logging_priority(),
                   "No event will be shown since no 'draw manager' has been set!");
    return;
  }
  _draw_manager_->update();

  if (!_full_2d_display_) {
    _tab_is_uptodate_[OVERVIEW_TAB] = false;
    _tab_is_uptodate_[OPTIONS_TAB] = false;
    _tab_is_uptodate_[TRACK_BROWSER_TAB] = !reset_track_;

    const tab_id_index_type current_tab_id = (tab_id_index_type)_tabs_->GetCurrent();
    this->_update_tab_(current_tab_id, reset_view_);

    if (reset_track_) _browser_tracks_->update();
  }
  this->_update_view_(reset_view_);
  return;
}

void event_display::_update_tab_(const tab_id_index_type index_, const bool /*reset_view_*/) {
  if (!_tab_is_uptodate_[index_]) {
    switch (index_) {
      case OPTIONS_TAB:
        _options_->update();
        break;
      case OVERVIEW_TAB:
      case TRACK_BROWSER_TAB:
        //_browser_tracks_->update();
        break;
      default:
        break;
    }
    _tab_is_uptodate_[index_] = true;
  }
  return;
}

void event_display::_update_view_(const bool reset_view_) {
  if (reset_view_) {
    if (_full_2d_display_) {
      _top_2d_->reset();
      _front_2d_->reset();
      _side_2d_->reset();
    } else {
      _display_2d_->reset();
      _display_3d_->reset();
    }
  }

  if (_full_2d_display_) {
    _top_2d_->update_scene();
    _front_2d_->update_scene();
    _side_2d_->update_scene();
  } else {
    _display_2d_->update_scene();
    _display_3d_->update_scene();
  }
  return;
}

void event_display::show_all(const button_signals_type signal_) {
  options_manager& options_mgr = options_manager::get_instance();
  DT_LOG_NOTICE(options_mgr.get_logging_priority(), "Processing all events... please wait...");

  // First: clear
  this->clear();

  // Second: reset event server
  if (_server_->is_opened()) _server_->rewind();

  // Third: unset all over options after copying their values
  std::map<button_signals_type, bool>& option_dict = options_mgr.grab_options_dictionnary();
  std::map<button_signals_type, bool> tmp_option_dict(option_dict);
  for (std::map<button_signals_type, bool>::iterator i = option_dict.begin();
       i != option_dict.end(); ++i)
    i->second = false;

  switch (signal_) {
    case SHOW_ALL_VERTICES:
      options_mgr.set_option_flag(SHOW_MC_VERTEX, true);
      break;
    case SHOW_ALL_MC_TRACKS:
      options_mgr.set_option_flag(SHOW_MC_VERTEX, true);
      options_mgr.set_option_flag(SHOW_MC_TRACKS, true);
      break;
    case SHOW_ALL_MC_HITS:
      options_mgr.set_option_flag(SHOW_MC_VERTEX, true);
      options_mgr.set_option_flag(SHOW_MC_HITS, true);
      options_mgr.set_option_flag(SHOW_MC_CALORIMETER_HITS, true);
      options_mgr.set_option_flag(SHOW_MC_TRACKER_HITS, true);
      options_mgr.set_option_flag(SHOW_GG_CIRCLE, true);
      options_mgr.set_option_flag(SHOW_GG_TIME_GRADIENT, true);
      break;
    case SHOW_ALL_MC_TRACKS_AND_HITS:
      options_mgr.set_option_flag(SHOW_MC_VERTEX, true);
      options_mgr.set_option_flag(SHOW_MC_TRACKS, true);
      options_mgr.set_option_flag(SHOW_MC_HITS, true);
      options_mgr.set_option_flag(SHOW_MC_CALORIMETER_HITS, true);
      options_mgr.set_option_flag(SHOW_MC_TRACKER_HITS, true);
      options_mgr.set_option_flag(SHOW_GG_CIRCLE, true);
      options_mgr.set_option_flag(SHOW_GG_TIME_GRADIENT, true);
      break;
    case SHOW_ALL_CALIBRATED_HITS:
      options_mgr.set_option_flag(SHOW_CALIBRATED_HITS, true);
    default:
      break;
  }

  // Four: loop over event
  // // Sanity check otherwise processing of events take too long
  // const size_t nevent = 200;
  while (_server_->next_event()) {
    _draw_manager_->update();
    _selection_->select_events(NEXT_EVENT);

    // if ((size_t)_server_->get_current_event_number() > nevent) {
    //   DT_LOG_WARNING(options_mgr.get_logging_priority(),
    //                  "Stop after " << nevent << " to avoid CPU overload !");
    //   break;
    // }
    // Avoid event browser lagging
    gSystem->ProcessEvents();
  }

  this->_update_view_(true);

  // Get back to default options
  option_dict = tmp_option_dict;

  return;
}

void event_display::handle_button_signals(const button_signals_type signal_,
                                          const int event_selected_) {
  switch (signal_) {
    case VIEW_X3D:
    case VIEW_OGL:
    case PRINT_3D:
    case PRINT_3D_AS_EPS:
      _display_3d_->handle_button_signals(signal_);
      break;
    case PRINT_2D:
    case PRINT_2D_AS_EPS:
      _display_2d_->handle_button_signals(signal_);
      break;
    case SHOW_ALL_VERTICES:
    case SHOW_ALL_MC_TRACKS:
    case SHOW_ALL_MC_HITS:
    case SHOW_ALL_MC_TRACKS_AND_HITS:
    case SHOW_ALL_CALIBRATED_HITS:
      this->show_all(signal_);
      break;
    case CURRENT_EVENT:
    case NEXT_EVENT:
    case PREVIOUS_EVENT:
    case LAST_EVENT:
    case FIRST_EVENT: {
      _selection_->select_events(signal_, event_selected_);
      break;
    }
    default:
      break;
  }
  return;
}

}  // end of namespace view

}  // end of namespace visualization

}  // end of namespace snemo

// end of event_display.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
