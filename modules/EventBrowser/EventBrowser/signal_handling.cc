/* event_server.cc
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

// Ourselves:
#include <EventBrowser/view/signal_handling.h>

// This project:
#include <EventBrowser/detector/detector_manager.h>
#include <EventBrowser/event_browser_version.h>
#include <EventBrowser/io/event_server.h>
#include <EventBrowser/view/event_browser.h>
#include <EventBrowser/view/event_browser_signals.h>
#include <EventBrowser/view/options_manager.h>
#include <EventBrowser/view/style_manager.h>

// Standard library
#include <iostream>
#include <sstream>
#include <stdexcept>

// Third party
// - ROOT
#include <KeySymbols.h>
#include <TGFileDialog.h>
#include <TGMsgBox.h>
#include <TGWidget.h>
#include <TROOT.h>
#include <TSystem.h>
// - Falaise
#include <falaise/resource.h>

namespace snemo {

namespace visualization {

namespace view {

const unsigned int g__binded_keys[] = {
    kKey_Escape,  kKey_PageDown, kKey_PageUp, kKey_Home, kKey_End, kKey_Space, kKey_Tab,
    kKey_Backtab, kKey_P,        kKey_A,      kKey_D,    kKey_o,   kKey_r};
const unsigned int g__nbr_binded_keys = sizeof(g__binded_keys) / sizeof(unsigned int);

// ctor:
signal_handling::signal_handling(TGMainFrame* main_) : _browser_(0) {
  // get pointer to browser for button connections
  _browser_ = dynamic_cast<event_browser*>(main_);
  DT_THROW_IF(!_browser_, std::logic_error, "Event_browser can't be cast from frame!");

  // define keyboard strokes to be handled by the event_browser_options
  for (unsigned int i_key = 0; i_key < g__nbr_binded_keys; ++i_key) {
    _browser_->BindKey((const TGWindow*)_browser_,
                       gVirtualX->KeysymToKeycode(g__binded_keys[i_key]), kAnyModifier);
  }
  return;
}

// dtor:
signal_handling::~signal_handling() {
  // remove binding of keys
  for (unsigned int i_key = 0; i_key < g__nbr_binded_keys; ++i_key) {
    _browser_->RemoveBind((const TGWindow*)_browser_,
                          gVirtualX->KeysymToKeycode(g__binded_keys[i_key]), kAnyModifier);
  }
  return;
}

bool signal_handling::handle_key(Event_t* event_) {
  char tmp[2];
  unsigned int keysym;
  gVirtualX->LookupString(event_, tmp, sizeof(tmp), keysym);

  if (event_->fType != kGKeyPress) return false;

  EKeySym key_pressed = (EKeySym)keysym;

  if (event_->fState & kKeyControlMask) {
    switch (key_pressed) {
      case kKey_PageDown:
        _browser_->change_tab(PREVIOUS_TAB);
        break;
      case kKey_PageUp:
        _browser_->change_tab(NEXT_TAB);
        break;
      case kKey_o:
        _process_menu_(FILE_OPEN);
        break;
      default:
        break;
    }
  } else {
    switch (key_pressed) {
      case kKey_Escape:
        _browser_->close_window();
        break;
      case kKey_PageDown:
        _browser_->change_event(NEXT_EVENT);
        break;
      case kKey_PageUp:
        _browser_->change_event(PREVIOUS_EVENT);
        break;
      case kKey_Home:
        _browser_->change_event(FIRST_EVENT);
        break;
      case kKey_End:
        _browser_->change_event(LAST_EVENT);
        break;
      case kKey_d:
        _browser_->update_menu(DUMP_EVENT);
        break;
      case kKey_r:
        _browser_->initialize_event_server();
        break;
      default:
        break;
    }
  }
  return true;
}

bool signal_handling::process_message(long msg_, long parm1_, long parm2_) {
  // Process messages coming from widgets associated with the dialog.
  DT_LOG_TRACE(
      options_manager::get_instance().get_logging_priority(),
      msg_ << " " << parm1_ << " " << parm2_ << " " << GET_MSG(msg_) << " " << GET_SUBMSG(msg_));

  const int get_msg = GET_MSG(msg_);
  const int get_sub_msg = GET_SUBMSG(msg_);

  if (get_msg == kC_COMMAND) {
    if (get_sub_msg == kCM_COMBOBOX) {
      if (parm1_ == STATUS_BAR) {
        _browser_->change_event(CURRENT_EVENT, parm2_);
      }
    } else if (get_sub_msg == kCM_TAB) {
      _browser_->update_tab((event_browser::tab_id_index_type)parm1_);
    } else if (get_sub_msg == kCM_BUTTON) {
      this->_process_button_((button_signals_type)parm1_);
    } else if (get_sub_msg == kCM_CHECKBUTTON) {
      // Nothing here for the time being
    } else if (get_sub_msg == kCM_MENU) {
      this->_process_menu_((button_signals_type)parm1_);
    }
  }

  if (get_msg == kC_TEXTENTRY) {
    if (get_sub_msg == kTE_ENTER) {
      if (parm1_ == UPDATE_SELECTION) {
        this->_process_button_((button_signals_type)parm1_);
      } else if (parm1_ == GOTO_EVENT) {
        _browser_->change_event(CURRENT_EVENT);
      }
    }
  }
  return true;
}

void signal_handling::_process_button_(const button_signals_type signal_) {
  switch (signal_) {
    case NEXT_EVENT: {
      // use it to stop the auto event reading mode
      options_manager& options_mgr = options_manager::get_instance();
      if (options_mgr.is_automatic_event_reading_mode()) {
        options_mgr.set_automatic_event_reading_mode(false);
      } else {
        _browser_->change_event(NEXT_EVENT);
      }
    } break;
    case PREVIOUS_EVENT:
    case FIRST_EVENT:
    case LAST_EVENT:
      _browser_->change_event(signal_);
      break;
    default:
      break;
  }
  return;
}

void signal_handling::_process_menu_(const button_signals_type signal_) {
  switch (signal_) {
    // -------------------- THE FILE MENU ---------------
    case FILE_OPEN: {
      const std::string& filename = _browser_->get_event_server().get_current_filename();
      TString directory(gSystem->DirName(filename.c_str()));
      TGFileInfo file_info;
      file_info.fIniDir = StrDup(directory.Data());
      new TGFileDialog(gClient->GetRoot(), _browser_, kFDOpen, &file_info);
      if (file_info.fFilename) {
        options_manager::get_instance().set_default_options();
        std::vector<std::string> tmp;
        tmp.push_back(std::string(file_info.fFilename));
        options_manager::get_instance().set_input_files(tmp);
        _browser_->initialize_event_server();
      }
    } break;
    case FILE_RELOAD: {
      _browser_->initialize_event_server();
    } break;
    // case CONNECT_TO_DAQ:
    //   {
    //     string server_name("");
    //     new SelectDAQServer(__main, server_name);
    //     if (!server_name.empty ()) {
    //       vector<string> tmp;
    //       tmp.push_back(server_name);
    //       _browser_->open_data_file (tmp);
    //     }
    //   }
    //   break;
    case SAVE_EVENT_TO_XML: {
      _browser_->get_event_server().store_event();
    } break;
    case SAVE_EVENT_AS: {
      const std::string dir = std::string(gSystem->ExpandPathName("$(PWD)"));
      TString directory(dir.c_str());
      TGFileInfo file_info;
      const char* config_file_types[] = {"BRIO (Boost to ROOT) archives",
                                         "*.brio",
                                         "Binary archives",
                                         "*.data",
                                         "Text archives",
                                         "*.txt",
                                         "XML archives",
                                         "*.xml",
                                         "All files",
                                         "*",
                                         0,
                                         0};
      file_info.fFileTypes = config_file_types;
      file_info.fIniDir = StrDup(directory.Data());
      new TGFileDialog(gClient->GetRoot(), _browser_, kFDSave, &file_info);
      if (!file_info.fFilename) break;

      std::string file_name = file_info.fFilename;
      std::string file_type = file_info.fFileTypes[file_info.fFileTypeIdx + 1];
      // remove asterisk
      file_type.erase(0, 1);

      // if extension is missing to filename
      if (!TString(file_name).EndsWith(file_type.c_str())) {
        file_name += file_type;
      }
      _browser_->get_event_server().store_event(file_name);
    } break;
    case LOAD_DETECTOR: {
      const std::string dir = falaise::get_resource_dir();
      TString directory(dir.c_str());
      TGFileInfo file_info;
      const char* config_file_types[] = {"Geometry config files", "*.conf", "All files", "*", 0, 0};
      file_info.fFileTypes = config_file_types;
      file_info.fIniDir = StrDup(directory.Data());
      new TGFileDialog(gClient->GetRoot(), _browser_, kFDOpen, &file_info);
      if (file_info.fFilename) {
        // options_manager & options_mgr = options_manager::get_instance ();
        // options_mgr.set_detector_config_file (file_info.fFilename);
        detector::detector_manager& detector_mgr = detector::detector_manager::get_instance();
        detector_mgr.reset();
        detector_mgr.initialize(file_info.fFilename);
        detector_mgr.construct();
        _browser_->update_browser();
      }
    } break;
    case LOAD_STYLE: {
      const std::string dir = falaise::get_resource_dir() + "/modules/EventBrowser/styles";
      TString directory(dir.c_str());
      TGFileInfo file_info;
      const char* style_file_types[] = {"Style files", "*.sty", "All files", "*", 0, 0};
      file_info.fFileTypes = style_file_types;
      file_info.fIniDir = StrDup(directory.Data());
      new TGFileDialog(gClient->GetRoot(), _browser_, kFDOpen, &file_info);
      if (file_info.fFilename) {
        style_manager& style_mgr = style_manager::get_instance();
        style_mgr.reset();
        style_mgr.initialize(file_info.fFilename);

        detector::detector_manager::get_instance().update();
        _browser_->update_browser();
      }
    } break;
    case FILE_QUIT:
      _browser_->close_window();
      break;

    // -------------------- THE VIEW MENU -----------------
    case FULL_2D_VIEW:
    case VIEW_X3D:
    case VIEW_OGL:
    case PRINT_3D:
    case PRINT_3D_AS_EPS:
    case PRINT_2D:
    case PRINT_2D_AS_EPS:

    // -------------------- THE MISC MENU ---------------
    case SHOW_ALL_VERTICES:
    case SHOW_ALL_MC_TRACKS:
    case SHOW_ALL_MC_HITS:
    case SHOW_ALL_MC_TRACKS_AND_HITS:
    case SHOW_ALL_CALIBRATED_HITS:
    case DUMP_EVENT:
    case DUMP_INTO_TOOLTIP:
    case DUMP_INTO_TERMINAL:
    case DUMP_INTO_WINDOW: {
      _browser_->update_menu(signal_);
    } break;

    // ------------------- THE HELP MENU ------------------
    case ABOUT: {
      EMsgBoxIcon icon_type = kMBIconAsterisk;
      EMsgBoxButton button_type = kMBOk;
      int retval;
      std::ostringstream info;
      info << "SuperNEMO Event Browser" << std::endl
           << std::endl
           << "Version " << EventBrowser::version::get_version() << std::endl
           << "Compiled with ROOT v" << gROOT->GetVersion() << std::endl
           << "Authors: SuperNEMO collaboration" << std::endl
           << std::endl
           << "Have fun browsing !" << std::endl
           << std::endl;
      new TGMsgBox(gClient->GetRoot(), gClient->GetRoot(), "About", info.str().c_str(), icon_type,
                   button_type, &retval);
    } break;
    default:
      _process_menu_options_(signal_);
      break;
  }
  return;
}

void signal_handling::_process_menu_options_(const button_signals_type signal_) {
  // -------------------- THE OPTIONS MENU ---------------
  const options_manager& options_mgr = options_manager::get_instance();
  const std::map<button_signals_type, bool>& option_dict = options_mgr.get_options_dictionnary();

  if (option_dict.find(signal_) == option_dict.end()) return;

  _browser_->update_menu(signal_);
  bool reset_view = false;

  if (signal_ == FOCUS_ROI) reset_view = true;

  _browser_->update_browser(reset_view);
  return;
}

}  // end of namespace view

}  // end of namespace visualization

}  // end of namespace snemo

// end of signal_handling.cc

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
