/* event_browser.cc
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

// Standard libraries
#include <fstream>
#include <iostream>
#include <sstream>

// This project:
#include <EventBrowser/event_browser_version.h>
#include <EventBrowser/view/event_browser.h>
#include <EventBrowser/view/event_browser_ctrl.h>
#include <EventBrowser/view/event_browser_menu.h>
#include <EventBrowser/view/event_display.h>
#include <EventBrowser/view/options_manager.h>
#include <EventBrowser/view/signal_handling.h>
#include <EventBrowser/view/status_bar.h>
#include <EventBrowser/view/style_manager.h>
#include <EventBrowser/io/event_server.h>
#include <EventBrowser/log.h>

// Third party:
// Boost:
#include <boost/filesystem.hpp>
// ROOT:
#include <TApplication.h>
#include <TGMsgBox.h>
#include <TGTab.h>
#include <TROOT.h>
#include <TSystem.h>

// Need trailing ; to satisfy clang-format, but leads to -pedantic error, ignore
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
ClassImp(snemo::visualization::view::event_browser);
#pragma GCC diagnostic pop

namespace snemo {
	
  namespace visualization {

    namespace view {

      const io::event_server& event_browser::get_event_server() const { return *_event_server_; }

      io::event_server& event_browser::grab_event_server() { return *_event_server_; }

      bool event_browser::is_initialized() const { return _initialized_; }

      void event_browser::welcome() const {
	std::cout << std::endl;
	std::cout << "\tSuperNEMO Event Browser                          " << std::endl
		  << "\tVersion " << EventBrowser::version::get_version() << std::endl
		  << "                                                   " << std::endl
		  << "\tCopyright (C) 2010-2017                          " << std::endl
		  << "\tSuperNEMO Collaboration                          " << std::endl
		  << "                                                   " << std::endl
		  << "\tCompiled with ROOT v" << gROOT->GetVersion() << std::endl;
	std::cout << std::endl;
      }

      // ctor:
      event_browser::event_browser(const TGWindow* window_, const unsigned int width_,
				   const unsigned int height_)
	: TGMainFrame(window_, width_, height_) {
	_menu_ = nullptr;
	_display_ = nullptr;
	_full_2d_display_ = nullptr;
	_status_ = nullptr;
	_handlers_ = nullptr;
	_tabs_ = nullptr;
	_event_server_ = nullptr;
	_thread_ctrl_ = nullptr;
	_initialized_ = false;
      }

      // dtor:
      event_browser::~event_browser() { this->reset(); }

      void event_browser::initialize() {
	FL_LOG_DEVEL("Entering...");
	DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized !");
	this->_at_init_();
	_initialized_ = true;
	FL_LOG_DEVEL("Exiting...");
      }

      void event_browser::reset() {
	FL_LOG_DEVEL("Entering...");
	DT_THROW_IF(!is_initialized(), std::logic_error, "Not initialized !");
	_initialized_ = false;
	if (_event_server_) {
	  delete _event_server_;
	  _event_server_ = nullptr;
	}

	if (_display_) {
	  delete _display_;
	  _display_ = nullptr;
	}
				
	if (_full_2d_display_ != nullptr) {
	  delete _full_2d_display_;
	  _full_2d_display_ = nullptr;
	}

	if (_status_) {
	  delete _status_;
	  _status_ = nullptr;
	}

	if (_handlers_) {
	  delete _handlers_;
	  _status_ = nullptr;
	}

	if (_tabs_) {
	  delete _tabs_;
	  _tabs_ = nullptr;
	}

	if (_menu_) {
	  delete _menu_;
	  _menu_ = nullptr;
	}

	FL_LOG_DEVEL("Exiting...");
      }

      void event_browser::_at_init_() {
	FL_LOG_DEVEL("Entering...");
	// Welcome printout
	this->welcome();

	// Event server
	_event_server_ = new io::event_server;

	// Initialize browser components
	this->initialize_gui();
	this->initialize_event_server();
	FL_LOG_DEVEL("Exiting...");
      }

      void event_browser::initialize_gui() {
	FL_LOG_DEVEL("Entering...");
	// Connect virtual method CloseWindow to close_window
	this->Connect("CloseWindow()", "snemo::visualization::view::event_browser", this,
		      "close_window()");

	// Avoid double deletions.
	this->DontCallClose();

	// Use hierarchical cleaning
	this->SetCleanup(kDeepCleanup);

	// Add the menu
	_menu_ = new event_browser_menu(this);

	// Add the tabs
	_tabs_ = new TGTab(this, this->GetWidth(), this->GetHeight());

	TGCompositeFrame* event_display_frame = _tabs_->AddTab("Event Display");
	// TGCompositeFrame * raw_data_frame      = _tabs_->AddTab("Raw Data");

	// Raw data off by default
	_tabs_->SetEnabled(ONLINE_DISPLAY, false);

	// Set startup tab
	_tabs_->SetTab(style_manager::get_instance().get_startup_tab());

	_tab_is_uptodate_[EVENT_DISPLAY] = false;
	_tab_is_uptodate_[ONLINE_DISPLAY] = false;

	this->AddFrame(_tabs_, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 3, 3, 3, 3));

	// Status bar
	_status_ = new status_bar;
	_status_->set_event_server(_event_server_);
	_status_->initialize(this);

	// Create plot objects
	_display_ = new event_display;
	_display_->set_event_server(_event_server_);
	_display_->set_status_bar(_status_);
	_display_->initialize(event_display_frame);
	if (options_manager::get_instance().get_option_flag(FULL_2D_VIEW)) {
	  this->add_full_2d_view();
	}

	// Bindings
	_handlers_ = new signal_handling(this);

	this->SetIconPixmap("");
	this->SetWindowName("SuperNEMO Event Browser - no input data ");
	this->MapSubwindows();
	this->Resize(this->GetDefaultSize());
	this->MapWindow();
	FL_LOG_DEVEL("Exiting...");
      }

      void event_browser::initialize_event_server() {
	FL_LOG_DEVEL("Entering...");
	const options_manager& options_mgr = options_manager::get_instance();
	const std::vector<std::string>& filenames = options_mgr.get_input_files();
	// Check here the existence of file
	std::vector<std::string> existing_files;
	for (const auto& a_file : filenames) {
	  if (!boost::filesystem::exists(a_file)) {
	    DT_LOG_WARNING(options_mgr.get_logging_priority(), a_file << " does not exist");
	    continue;
	  }
	  existing_files.push_back(a_file);
	}

	// No file bye bye
	if (existing_files.empty()) {
	  DT_LOG_WARNING(options_mgr.get_logging_priority(), "No existing file specified!");
	  FL_LOG_DEVEL("Exiting...");
	  return;
	}

	// Initialize event server
	io::event_server& server = *_event_server_;
	if (server.is_initialized()) {
	  server.reset();
	}
	if (!server.initialize(existing_files)) {
	  DT_LOG_WARNING(options_mgr.get_logging_priority(), "Cannot open data source!");
	  FL_LOG_DEVEL("Exiting...");
	  return;
	}

	std::ostringstream message;
	message << "Identified input data as: '" << server.get_file_type_as_string() << "' ";
	if (server.has_sequential_data()) {
	  message << " (sequential reading mode)";
	} else {
	  message << "(" << server.get_number_of_events() << " entries)";
	}
	DT_LOG_NOTICE(options_mgr.get_logging_priority(), message.str());

	// Window title
	std::ostringstream info;
	info << "SuperNEMO snvisualization - reading from: ";
	if (existing_files.empty()) {
	  info << "unknown...";
	} else {
	  info << existing_files[0];
	  if (existing_files.size() > 1) {
	    info << " (and others)";
	  }
	}

	this->SetWindowName(info.str().c_str());
	if (!server.has_sequential_data() && server.get_number_of_events() == 0) {
	  DT_LOG_WARNING(options_mgr.get_logging_priority(), "No events found in the given file!");
	  return;
	}
	_status_->update(true);

	this->change_event(server.has_sequential_data() ? NEXT_EVENT : FIRST_EVENT);
	FL_LOG_DEVEL("Exiting...");
      }

      void event_browser::track_select() { _display_->update(false, false); }

      void event_browser::add_full_2d_view() {
	FL_LOG_DEVEL("Entering...");
	if (_full_2d_display_ != nullptr) {
	  // Remove full 2D view
	  _tabs_->RemoveTab(FULL_2D_DISPLAY);
	  delete _full_2d_display_;
	  _full_2d_display_ = nullptr;
	} else {
	  // Create a new frame
	  TGCompositeFrame* full_2d_frame = _tabs_->AddTab("Full 2D View");
	  _tab_is_uptodate_[FULL_2D_DISPLAY] = false;

	  // Create additional 2D plot objects
	  _full_2d_display_ = new event_display;
	  _full_2d_display_->set_event_server(_event_server_);
	  _full_2d_display_->set_full_2d_display(true);
	  _full_2d_display_->initialize(full_2d_frame);
	}
	_tabs_->MapSubwindows();
	_tabs_->Layout();
	_tabs_->SetTab(FULL_2D_DISPLAY);
	FL_LOG_DEVEL("Exiting...");
      }

      void event_browser::update_browser(const bool reset_view_) {
	FL_LOG_DEVEL("Entering...");
	_tab_is_uptodate_[EVENT_DISPLAY] = false;
	_tab_is_uptodate_[FULL_2D_DISPLAY] = false;
	this->update_tab((tab_id_index_type)_tabs_->GetCurrent(), reset_view_);
	FL_LOG_DEVEL("Exiting...");
      }

      void event_browser::update_tab(const tab_id_index_type index_, const bool reset_view_) {
	FL_LOG_DEVEL("Entering...");
	if (not _tab_is_uptodate_[index_]) {
	  _menu_->set_default_option(*_event_server_);
	  switch (index_) {
	  case EVENT_DISPLAY:
	    FL_LOG_DEVEL("EVENT_DISPLAY");
	    _display_->update(reset_view_);
	    break;
	  case FULL_2D_DISPLAY:
	    FL_LOG_DEVEL("FULL_2D_DISPLAY");
	    _full_2d_display_->update(reset_view_);
	    break;
	  default:
	    break;
	  }
	  _tab_is_uptodate_[index_] = true;
	}
	FL_LOG_DEVEL("Exiting...");
      }

      void event_browser::update_menu(const button_signals_type signal_) {
	FL_LOG_DEVEL("Entering...");
	switch (signal_) {
	case FULL_2D_VIEW:
	  this->add_full_2d_view();
	  _menu_->update(signal_);
	  break;
	case VIEW_X3D:
	case VIEW_OGL:
	case PRINT_3D:
	case PRINT_3D_AS_EPS:
	case PRINT_2D:
	case PRINT_2D_AS_EPS:
	case SHOW_ALL_VERTICES:
	case SHOW_ALL_MC_TRACKS:
	case SHOW_ALL_MC_HITS:
	case SHOW_ALL_MC_TRACKS_AND_HITS:
	case SHOW_ALL_CALIBRATED_HITS:
	  _display_->handle_button_signals(signal_);
	  break;
	case DUMP_EVENT:
	  _event_server_->dump_event(std::clog);
	  break;
	case DUMP_INTO_TOOLTIP:
	case DUMP_INTO_TERMINAL:
	case DUMP_INTO_WINDOW:
	  _menu_->update(signal_);
	  _display_->update(false, true);
	  break;
	default:
	  _menu_->update(signal_);
	  break;
	}
	FL_LOG_DEVEL("Exiting...");
      }

      void event_browser::change_event(const button_signals_type signal_, const int event_selected_) {
	std::cerr << "\n\n";
	FL_LOG_DEVEL("Entering...");
	FL_LOG_DEVEL("signal=" << signal_ << "  event_selected=" << event_selected_);
	// Change status bar icon following the reading mode
	_status_->update_buttons(signal_);

	// Sequential and external mode only allow NEXT_EVENT signal
	if (_event_server_->has_sequential_data() || _event_server_->has_external_data()) {
	  if (signal_ != NEXT_EVENT) {
	    FL_LOG_DEVEL("Exiting...");
	    return;
	  }
	}

	// Event selection
	_display_->handle_button_signals(signal_, event_selected_);

	// Update browser
	this->update_browser();

	// Auto event reading logic
	const options_manager& options_mgr = options_manager::get_instance();
	if (options_mgr.is_automatic_event_reading_mode()) {
	  const size_t time_in_second = (size_t)options_mgr.get_automatic_event_reading_delay() * 1000;
	  TTimer::SingleShot(time_in_second, "snemo::visualization::view::event_browser", this,
			     "change_event()");
	}

	// Release thread
	if (has_thread_ctrl()) {
	  this->unlock_thread();
	}
	FL_LOG_DEVEL("Exiting...");
      }

      void event_browser::change_tab(const button_signals_type signal_) {
	FL_LOG_DEVEL("Entering...");
	int ntabs = _tabs_->GetNumberOfTabs();
	int current_tab = _tabs_->GetCurrent();

	if (signal_ == NEXT_TAB) {
	  for (int itab = current_tab + 1; itab < ntabs; itab++) {
	    if (_tabs_->IsEnabled(itab)) {
	      _tabs_->SetTab(itab);
	      FL_LOG_DEVEL("Exiting...");
	      return;
	    }
	  }
	  for (int itab = 0; itab < current_tab; itab++) {
	    if (_tabs_->IsEnabled(itab)) {
	      _tabs_->SetTab(itab);
	      FL_LOG_DEVEL("Exiting...");
	      return;
	    }
	  }
	}

	if (signal_ == PREVIOUS_TAB) {
	  for (int itab = current_tab - 1; itab > -1; itab--) {
	    if (_tabs_->IsEnabled(itab)) {
	      _tabs_->SetTab(itab);
	      FL_LOG_DEVEL("Exiting...");
	      return;
	    }
	  }
	  for (int itab = ntabs - 1; itab > current_tab; itab--) {
	    if (_tabs_->IsEnabled(itab)) {
	      _tabs_->SetTab(itab);
	      FL_LOG_DEVEL("Exiting...");
	      return;
	    }
	  }
	}
	FL_LOG_DEVEL("Exiting...");
      }

      void event_browser::close_window() {
	if (has_thread_ctrl()) {
	  event_browser_ctrl& thread_ctrl = grab_thread_ctrl();
	  thread_ctrl.event_availability_status = event_browser_ctrl::ABORT;
	  stop_threading();
	  unlock_thread();
	} else {
	  gApplication->Terminate();
	}
      }

      bool event_browser::handle_key(Event_t* event_) { return _handlers_->handle_key(event_); }

      bool event_browser::process_message(long msg_, long parm1_, long parm2_) {
	return _handlers_->process_message(msg_, parm1_, parm2_);
      }

      /*************************************************************/

      bool event_browser::has_thread_ctrl() const { return _thread_ctrl_ != nullptr; }

      void event_browser::set_thread_ctrl(event_browser_ctrl& thread_ctrl_) {
	DT_THROW_IF(has_thread_ctrl(), std::logic_error,
		    "Operation prohibited ! Event browser already got a 'thread ctrl' object !");
	_thread_ctrl_ = &thread_ctrl_;
      }

      const event_browser_ctrl& event_browser::get_thread_ctrl() const {
	DT_THROW_IF(!has_thread_ctrl(), std::logic_error, "Manager has no 'thread ctrl' object !");
	return *_thread_ctrl_;
      }

      event_browser_ctrl& event_browser::grab_thread_ctrl() {
	DT_THROW_IF(!has_thread_ctrl(), std::logic_error, "Manager has no 'thread ctrl' object !");
	return *_thread_ctrl_;
      }

      void event_browser::unlock_thread() {
	FL_LOG_DEVEL("Entering...");
	// Debugging...
	bool must_abort_run = false;

	// External threaded run control :
	if (has_thread_ctrl()) {
	  DT_LOG_TRACE(options_manager::get_instance().get_logging_priority(), "Using event control...");
	  event_browser_ctrl& thread_ctrl = grab_thread_ctrl();
	  {
	    DT_LOG_TRACE(options_manager::get_instance().get_logging_priority(),
			 "Acquire the event control lock...");
	    boost::mutex::scoped_lock lock(*thread_ctrl.event_mutex);
	    DT_LOG_TRACE(options_manager::get_instance().get_logging_priority(),
			 "Wait for event control to be available again...");
	    while (thread_ctrl.event_availability_status != event_browser_ctrl::AVAILABLE_FOR_ROOT) {
	      DT_LOG_TRACE(options_manager::get_instance().get_logging_priority(), "Not yet...");
	      thread_ctrl.event_available_condition->wait(*thread_ctrl.event_mutex);
	    }
	    DT_LOG_TRACE(options_manager::get_instance().get_logging_priority(), "Ok let's go on...");
	    if (thread_ctrl.event_availability_status == event_browser_ctrl::ABORT) {
	      must_abort_run = true;
	    }
	    thread_ctrl.event_availability_status = event_browser_ctrl::NOT_AVAILABLE_FOR_ROOT;
	    thread_ctrl.counts++;
	    DT_LOG_TRACE(options_manager::get_instance().get_logging_priority(),
			 "Notify the event browser...");
	    thread_ctrl.event_available_condition->notify_one();
	  }

	  // Wait for the release of the event control by the external process :
	  {
	    DT_LOG_TRACE(options_manager::get_instance().get_logging_priority(),
			 "Wait for the release of the event control by the external process...");
	    boost::mutex::scoped_lock lock(*thread_ctrl.event_mutex);
	    while (thread_ctrl.event_availability_status == event_browser_ctrl::NOT_AVAILABLE_FOR_ROOT) {
	      DT_LOG_TRACE(options_manager::get_instance().get_logging_priority(), "Not yet...");
	      thread_ctrl.event_available_condition->wait(*thread_ctrl.event_mutex);
	    }
	    DT_LOG_TRACE(options_manager::get_instance().get_logging_priority(),
			 "Ok ! The event control is released by the external process...");
	    if (thread_ctrl.event_availability_status == event_browser_ctrl::ABORT) {
	      DT_LOG_WARNING(options_manager::get_instance().get_logging_priority(),
			     "Detected an 'Abort' request from the external process...");
	      must_abort_run = true;
	    }
	  }

	  if (thread_ctrl.is_stop_requested()) {
	    must_abort_run = true;
	  }
	  if (thread_ctrl.max_counts > 0 && thread_ctrl.counts > thread_ctrl.max_counts) {
	    must_abort_run = true;
	  }
	}

	// Abort run condition :
	if (must_abort_run) {
	  stop_threading();
	}
	FL_LOG_DEVEL("Exiting...");
      }

      void event_browser::lock_thread() {
	FL_LOG_DEVEL("Entering...");
	if (!has_thread_ctrl()) {
	  DT_LOG_TRACE(options_manager::get_instance().get_logging_priority(), "No thread ctrl !");
	  return;
	}
	DT_LOG_TRACE(options_manager::get_instance().get_logging_priority(), "Check point.");
	event_browser_ctrl& thread_ctrl = grab_thread_ctrl();
	{
	  DT_LOG_TRACE(options_manager::get_instance().get_logging_priority(), "Acquire the lock...");
	  boost::mutex::scoped_lock lock(*thread_ctrl.event_mutex);
	  DT_LOG_TRACE(options_manager::get_instance().get_logging_priority(),
		       "Wait for the event control to be available again for ROOT "
		       << "and for event loop start ...");
	  while (thread_ctrl.event_availability_status == event_browser_ctrl::NOT_AVAILABLE_FOR_ROOT) {
	    DT_LOG_TRACE(options_manager::get_instance().get_logging_priority(), "Not yet...");
	    thread_ctrl.event_available_condition->wait(*thread_ctrl.event_mutex);
	  }
	  if (thread_ctrl.event_availability_status == event_browser_ctrl::ABORT) {
	    DT_LOG_WARNING(options_manager::get_instance().get_logging_priority(),
			   "Detect a 'Abort' request from the external process !");
	    stop_threading();
	  }
	  DT_LOG_TRACE(options_manager::get_instance().get_logging_priority(),
		       "Starting the ROOT browsing...");
	}
	FL_LOG_DEVEL("Exiting...");
      }

      void event_browser::wait() {
	FL_LOG_DEVEL("Entering...");
	if (has_thread_ctrl()) {
	  const event_browser_ctrl& thread_ctrl = get_thread_ctrl();
	  while (thread_ctrl.event_availability_status != event_browser_ctrl::ABORT) {
	    gSystem->Sleep(100);
	    gSystem->ProcessEvents();
	  }
	}
	FL_LOG_DEVEL("Exiting...");
      }

      void event_browser::start_threading() {
	FL_LOG_DEVEL("Entering...");
	DT_THROW_IF(!has_thread_ctrl(), std::logic_error, "Event browser has no connected thread !");
	lock_thread();
	std::ostringstream window_name;
	window_name << "SuperNEMO Event Browser Module - automatic event processing";
	this->SetWindowName(window_name.str().c_str());
	_menu_->set_default_option(*_event_server_);
	change_event(NEXT_EVENT);
	wait();
	FL_LOG_DEVEL("Exiting...");
      }

      void event_browser::stop_threading() {
	FL_LOG_DEVEL("Entering...");
	DT_THROW_IF(!has_thread_ctrl(), std::logic_error, "Event browser has no connected thread !");
	DT_LOG_NOTICE(options_manager::get_instance().get_logging_priority(), "Visualization stops !");
	//        close_window();
	FL_LOG_DEVEL("Exiting...");
      }

    } // end of namespace view

  } // end of namespace visualization

} // end of namespace snemo
