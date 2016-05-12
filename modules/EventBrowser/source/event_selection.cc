/* event_selection.cc
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
#include <falaise/snemo/view/event_selection.h>
// This project
#include <falaise/snemo/view/event_browser.h>
#include <falaise/snemo/view/event_browser_signals.h>
#include <falaise/snemo/view/options_manager.h>
#include <falaise/snemo/view/status_bar.h>
#include <falaise/snemo/io/event_server.h>

// Third party
// - ROOT
#include <TGFrame.h>
#include <TGNumberEntry.h>
#include <TGLabel.h>
#include <TGComboBox.h>
#include <TGMsgBox.h>
#include <TGButtonGroup.h>
#include <TColor.h>
#include <TGFileDialog.h>
#include <TSystem.h>
// - Bayeux/datatools
#include <datatools/utils.h>
// - Bayeux/cuts
#include <cuts/cut_manager.h>
// - Falaise
#include <falaise/resource.h>

ClassImp(snemo::visualization::view::event_selection)

namespace snemo {

  namespace visualization {

    namespace view {

      const std::string & event_selection::multi_and_cut_label()
      {
        static const std::string _label("_visu.and.cut");
        return _label;
      }

      const std::string & event_selection::multi_or_cut_label()
      {
        static const std::string _label("_visu.or.cut");
        return _label;
      }

      const std::string & event_selection::multi_xor_cut_label()
      {
        static const std::string _label("_visu.xor.cut");
        return _label;
      }

      const std::string & event_selection::eh_cut_label()
      {
        static const std::string _label("_visu.eh.cut");
        return _label;
      }

      const std::string & event_selection::sd_cut_label()
      {
        static const std::string _label("_visu.sd.cut");
        return _label;
      }

      event_selection::base_widget::base_widget(event_selection * selection_)
        : _selection(selection_)
      {
        return;
      }

      event_selection::base_widget::~base_widget()
      {
        return;
      }

      event_selection::selection_widget::selection_widget(event_selection * selection_)
        : event_selection::base_widget(selection_)
      {
        return;
      }

      void event_selection::selection_widget::initialize()
      {
        if (_or_button_) _or_button_->SetOn(false);
        if (_and_button_) _and_button_->SetOn(true);
        if (_xor_button_) _xor_button_->SetOn(false);
        if (_save_button_) _save_button_->SetState(kButtonDisabled);
        return;
      }

      void event_selection::selection_widget::set_state(const bool enable_)
      {
        const EButtonState state = enable_ ? kButtonEngaged : kButtonDisabled;
        if (_or_button_) _or_button_->SetState(state);
        if (_and_button_) _and_button_->SetState(state);
        if (_xor_button_) _xor_button_->SetState(state);
        return;
      }

      void event_selection::selection_widget::build(TGCompositeFrame * frame_)
      {
        // Add buttons to save, reset and apply selection
        TGVerticalFrame * vframe = new TGVerticalFrame(frame_);
        frame_->AddFrame(vframe, new TGLayoutHints(kLHintsBottom | kLHintsRight, 10, 10, 10, 10));

        TGHorizontalFrame * hframe = new TGHorizontalFrame(vframe);
        vframe->AddFrame(hframe);

        _load_button_ = new TGTextButton(hframe, "Load selection file", LOAD_SELECTION);
        _load_button_->SetToolTipText("save current selection into file");
        _load_button_->Connect("Clicked()", "snemo::visualization::view::event_selection",
                               _selection, "process()");
        hframe->AddFrame(_load_button_, new TGLayoutHints(kLHintsNoHints, 2, 2, 2, 2));

        _save_button_ = new TGTextButton(hframe, "Save selection as...", SAVE_SELECTION);
        _save_button_->SetToolTipText("save current selection into file");
        _save_button_->Connect("Clicked()", "snemo::visualization::view::event_selection",
                               _selection, "process()");
        hframe->AddFrame(_save_button_, new TGLayoutHints(kLHintsNoHints, 2, 2, 2, 2));

        _reset_button_ = new TGTextButton(hframe, "Reset selection", RESET_SELECTION);
        _reset_button_->SetToolTipText("reset current selection");
        _reset_button_->Connect("Clicked()", "snemo::visualization::view::event_selection",
                                _selection, "process()");
        hframe->AddFrame(_reset_button_, new TGLayoutHints(kLHintsNoHints, 2, 2, 2, 2));

        _update_button_ = new TGTextButton(hframe, "Update selection", UPDATE_SELECTION);
        _update_button_->SetToolTipText("update selection");
        _update_button_->Connect("Clicked()", "snemo::visualization::view::event_selection",
                                 _selection, "process()");
        hframe->AddFrame(_update_button_, new TGLayoutHints(kLHintsNoHints, 2, 2, 2, 2));

        TGHButtonGroup * bgroup = new TGHButtonGroup(frame_, "Event selection logic");
        bgroup->SetTitlePos(TGGroupFrame::kLeft);
        frame_->AddFrame(bgroup, new TGLayoutHints(kLHintsBottom | kLHintsLeft, 3, 3, 3, 3));

        _or_button_  = new TGRadioButton(bgroup, "or ", OR_SELECTION);
        _and_button_ = new TGRadioButton(bgroup, "and ", AND_SELECTION);
        _xor_button_ = new TGRadioButton(bgroup, "xor ", XOR_SELECTION);
        _or_button_->Connect("Clicked()", "snemo::visualization::view::event_selection",
                             _selection, "process()");
        _and_button_->Connect("Clicked()", "snemo::visualization::view::event_selection",
                              _selection, "process()");
        _xor_button_->Connect("Clicked()", "snemo::visualization::view::event_selection",
                              _selection, "process()");

        // Initialize values
        initialize();
        return;
      }

      event_selection::complex_selection_widget::complex_selection_widget(event_selection * selection_)
        : event_selection::base_widget(selection_)
      {
        return;
      }

      void event_selection::complex_selection_widget::initialize()
      {
        _combo_->RemoveAll();
        size_t j = 1;
        for (cuts::cut_handle_dict_type::const_iterator
               i = _selection->get_cut_manager().get_cuts().begin();
             i != _selection->get_cut_manager().get_cuts().end(); ++i) {
          const std::string & the_cut_name = i->first;
          if (the_cut_name[0] != '_') {
            _combo_->AddEntry(the_cut_name.c_str(), j++);
          }
        }
        if (j == 1) {
          _combo_->AddEntry(" +++ NO CUTS +++ ", 0);
          set_state(false);
        } else {
          _combo_->AddEntry(" +++ SELECT CUT NAME +++ ", 0);
          set_state(true);
        }
        if (_combo_) _combo_->Select(0);
        return;
      }

      void event_selection::complex_selection_widget::set_state(const bool enable_)
      {
        if (_combo_) _combo_->SetEnabled(enable_);
        if (enable_) {
          _enable_->SetEnabled(enable_);
        } else {
          _enable_->SetDisabledAndSelected(enable_);
        }
        return;
      }

      void event_selection::complex_selection_widget::build(TGCompositeFrame * frame_)
      {
        TGGroupFrame * gframe = new TGGroupFrame(frame_, "      Complex cuts", kVerticalFrame);
        gframe->SetTitlePos(TGGroupFrame::kLeft);
        frame_->AddFrame(gframe, new TGLayoutHints(kLHintsTop | kLHintsLeft, 3, 3, 3, 3));
        _enable_ = new TGCheckButton(gframe, "", ENABLE_COMPLEX_SELECTION);
        _enable_->Connect("Clicked()", "snemo::visualization::view::event_selection",
                          _selection, "process()");
        gframe->AddFrame(_enable_, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 0, -15, 0));

        TGCompositeFrame * cframe = new TGCompositeFrame(gframe, 200, 1,
                                                         kHorizontalFrame | kFixedWidth);
        gframe->AddFrame(cframe);

        _combo_ = new TGComboBox(cframe, COMBO_SELECTION);
        _combo_->Connect("Selected(int)", "snemo::visualization::view::event_selection",
                         _selection, "process()");
        _combo_->Resize(200, 20);
        cframe->AddFrame(_combo_, new TGLayoutHints(kLHintsTop     | kLHintsLeft |
                                                    kLHintsExpandX | kLHintsExpandY,
                                                    2, 2, 2, 2));
        initialize();
        return;
      }

      event_selection::event_header_selection_widget::event_header_selection_widget(event_selection * selection_)
        : event_selection::base_widget(selection_)
      {
        return;
      }

      void event_selection::event_header_selection_widget::initialize()
      {
        if (_run_id_min_) _run_id_min_->SetNumber(datatools::event_id::INVALID_RUN_NUMBER);
        if (_run_id_max_) _run_id_max_->SetNumber(datatools::event_id::INVALID_RUN_NUMBER);
        if (_event_id_min_) _event_id_min_->SetNumber(datatools::event_id::INVALID_EVENT_NUMBER);
        if (_event_id_max_) _event_id_max_->SetNumber(datatools::event_id::INVALID_EVENT_NUMBER);
        this->set_state(false);
      }

      void event_selection::event_header_selection_widget::set_state(const bool enable_)
      {
        if (_run_id_min_) _run_id_min_->SetState(enable_);
        if (_run_id_max_) _run_id_max_->SetState(enable_);
        if (_event_id_min_) _event_id_min_->SetState(enable_);
        if (_event_id_max_) _event_id_max_->SetState(enable_);
        return;
      }

      void event_selection::event_header_selection_widget::build(TGCompositeFrame * frame_)
      {
        // Layout for positionning frame
        TGLayoutHints * field_layout = new TGLayoutHints(kLHintsTop | kLHintsRight,
                                                         2, 2, 20, 2);
        TGLayoutHints * label_layout = new TGLayoutHints(kLHintsTop | kLHintsLeft,
                                                         2, 2, 2, 2);
        TGGroupFrame * gframe = new TGGroupFrame(frame_, "      Event header cut", kVerticalFrame);
        gframe->SetTitlePos(TGGroupFrame::kLeft);
        frame_->AddFrame(gframe,
                         new TGLayoutHints(kLHintsTop | kLHintsLeft, 3, 3, 3, 3));

        _enable_ = new TGCheckButton(gframe, "", ENABLE_EH_SELECTION);
        _enable_->SetToolTipText("enable selection on event header bank");
        _enable_->Connect("Clicked()", "snemo::visualization::view::event_selection",
                          _selection, "process()");
        gframe->AddFrame(_enable_, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 0, -15, 0));

        {
          TGCompositeFrame * cframe = new TGCompositeFrame(gframe, 200, 1,
                                                           kHorizontalFrame | kFixedWidth);
          gframe->AddFrame(cframe);
          cframe->AddFrame(new TGLabel(cframe, "Run ID (min - max):"), label_layout);
          _run_id_max_ = new TGNumberEntry(cframe, datatools::event_id::INVALID_RUN_NUMBER,
                                         10, -1, TGNumberFormat::kNESInteger);
          _run_id_max_->Connect("ValueSet(Long_t)",
                                "snemo::visualization::view::event_selection",
                              _selection, "process()");
          cframe->AddFrame(_run_id_max_, field_layout);
          _run_id_min_ = new TGNumberEntry(cframe, datatools::event_id::INVALID_RUN_NUMBER,
                                           10, -1, TGNumberFormat::kNESInteger);
          _run_id_min_->Connect("ValueSet(Long_t)",
                              "snemo::visualization::view::event_selection",
                              _selection, "process()");
          cframe->AddFrame(_run_id_min_, field_layout);
        }
        {
          TGCompositeFrame * cframe = new TGCompositeFrame(gframe, 200, 1,
                                                           kHorizontalFrame | kFixedWidth);
          gframe->AddFrame(cframe);
          cframe->AddFrame(new TGLabel(cframe, "Event ID (min - max):"), label_layout);
          _event_id_max_ = new TGNumberEntry(cframe, datatools::event_id::INVALID_EVENT_NUMBER,
                                             10, -1, TGNumberFormat::kNESInteger);
          _event_id_max_->Connect("ValueSet(Long_t)",
                                  "snemo::visualization::view::event_selection",
                                  _selection, "process()");
          cframe->AddFrame(_event_id_max_, field_layout);
          _event_id_min_ = new TGNumberEntry(cframe, datatools::event_id::INVALID_RUN_NUMBER,
                                             10, -1, TGNumberFormat::kNESInteger);
          _event_id_min_->Connect("ValueSet(Long_t)",
                                  "snemo::visualization::view::event_selection",
                                  _selection, "process()");
          cframe->AddFrame(_event_id_min_, field_layout);
        }

        // Initialize values
        initialize();
        return;
      }

      event_selection::simulated_data_selection_widget::simulated_data_selection_widget(event_selection * selection_)
        : event_selection::base_widget(selection_)
      {
        return;
      }

      void event_selection::simulated_data_selection_widget::initialize()
      {
        if (_hit_category_) _hit_category_->Clear();
        this->set_state(false);
      }

      void event_selection::simulated_data_selection_widget::set_state(const bool enable_)
      {
        if (_hit_category_) _hit_category_->SetState(enable_);
        return;
      }

      void event_selection::simulated_data_selection_widget::build(TGCompositeFrame * frame_)
      {
        // Layout for positionning frame
        // TGLayoutHints * field_layout = new TGLayoutHints(kLHintsTop | kLHintsRight,
        //                                                  2, 2, 20, 2);
        // TGLayoutHints * label_layout = new TGLayoutHints(kLHintsTop | kLHintsLeft,
        //                                                  2, 2, 2, 2);
        TGGroupFrame * gframe = new TGGroupFrame(frame_, "      Simulated data cut", kVerticalFrame);
        gframe->SetTitlePos(TGGroupFrame::kLeft);
        frame_->AddFrame(gframe, new TGLayoutHints(kLHintsTop | kLHintsLeft, 3, 3, 3, 3));
        {
          _enable_ = new TGCheckButton(gframe, "", ENABLE_SD_SELECTION);
          _enable_->SetToolTipText("enable selection on simulated data bank");
          _enable_->Connect("Clicked()", "snemo::visualization::view::event_selection",
                            _selection, "process()");
          gframe->AddFrame(_enable_, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 0, -15, 0));
        }

        {
          TGCompositeFrame * cframe = new TGCompositeFrame(gframe, 200, 1,
                                                           kHorizontalFrame | kFixedWidth);
          gframe->AddFrame(cframe);
          cframe->AddFrame(new TGLabel(cframe, "Hit category name:"),
                           new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 2, 2, 2));
          _hit_category_ = new TGTextEntry(cframe, "", -1);
          _hit_category_->Connect("ValueSet(Long_t)",
                                  "snemo::visualization::view::event_selection",
                                  _selection, "process()");
          cframe->AddFrame(_hit_category_, new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 2, 0, 2));
        }

        // Initialize values
        initialize();
        return;
      }

      //*************************************************************//

      bool event_selection::is_initialized() const
      {
        return _initialized_;
      }

      bool event_selection::is_selection_enable() const
      {
        return _selection_enable_;
      }

      // ctor:
      event_selection::event_selection()
      {
        _main_        = 0;
        _server_      = 0;
        _browser_     = 0;
        _status_      = 0;
        _cut_manager_ = 0;
        _initialized_ = false;
        return;
      }

      // dtor:
      event_selection::~event_selection()
      {
        this->reset();
        return;
      }

      void event_selection::set_event_server(io::event_server * server_)
      {
        DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized !");
        _server_ = server_;
        return;
      }

      void event_selection::set_status_bar(view::status_bar * status_)
      {
        DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized !");
        _status_ = status_;
        return;
      }

      const cuts::cut_manager & event_selection::get_cut_manager() const
      {
        DT_THROW_IF(_cut_manager_ == 0, std::logic_error, "Cut manager is not set !");
        return *_cut_manager_;
      }

      cuts::cut_manager & event_selection::grab_cut_manager()
      {
        DT_THROW_IF(_cut_manager_ == 0, std::logic_error, "Cut manager is not set !");
        return *_cut_manager_;
      }

      void event_selection::initialize(TGCompositeFrame * main_)
      {
        DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized !");

        // Keep track of main frame in order to regenerate it for
        // different detector setup
        _main_ = main_;
        _main_->SetCleanup(kDeepCleanup);

        // Get pointer to browser for button connections
        TGCompositeFrame * parent = (TGCompositeFrame *) _main_
          ->GetParent()->GetParent()
          ->GetParent()->GetParent()
          ->GetParent()->GetParent();
        _browser_ = dynamic_cast<event_browser *>(parent);
        DT_THROW_IF(!_browser_, std::logic_error, "Event_browser can't be cast from frame!");

        _cut_manager_ = new cuts::cut_manager;
        this->_build_widgets_();
        this->_install_cut_manager_();

        _initial_event_id_ = 0;
        _selection_enable_ = false;
        _initialized_      = true;
        return;
      }

      void event_selection::reset()
      {
        if (_cut_manager_) {
          delete _cut_manager_;
          _cut_manager_ = 0;
        }
        return;
      }

      void event_selection::process()
      {
        DT_LOG_WARNING(datatools::logger::PRIO_WARNING, "Entering...");
        TGButton * button = static_cast<TGButton*>(gTQSender);
        const unsigned int id = button->WidgetId();

        if (id == LOAD_SELECTION) {
          const std::string dir = falaise::get_resource_dir(true);
          TString directory(dir.c_str());
          TGFileInfo file_info;
          const char * config_file_types [] = { "Config. file" , "*.conf",
                                                "All files"    , "*",
                                                0              , 0 };
          file_info.fFileTypes = config_file_types;
          file_info.fIniDir = StrDup(directory.Data());
          new TGFileDialog(gClient->GetRoot(), _browser_, kFDOpen, &file_info);
          if (!file_info.fFilename) return;

          std::string file_name  = file_info.fFilename;
          std::string file_type  = file_info.fFileTypes[file_info.fFileTypeIdx+1];

          // remove asterisk
          file_type.erase(0,1);

          // if extension is missing to filename
          if (!TString(file_name).EndsWith(file_type.c_str())) {
            file_name += file_type;
          }

          options_manager::get_instance().set_cut_config_file(file_name);
          this->reset();
          this->_install_cut_manager_();
        } else if (id == RESET_SELECTION) {
          for (widget_collection_type::iterator i = _widgets_.begin();
               i != _widgets_.end(); ++i) {
            base_widget * a_widget = *i;
            a_widget->initialize();
          }

          _server_->clear_selection();
          _server_->fill_selection();
          _status_->update(true);

          this->_build_cuts_();

          _browser_->change_event(CURRENT_EVENT, _initial_event_id_);
          return;
        } else if (id == UPDATE_SELECTION) {
          // _selection_widgets_.tg_update->SetState(kButtonDisabled);

          this->_build_cuts_();

          if (is_selection_enable()) {
            _server_->clear_selection();
            _server_->fill_selection();
            _initial_event_id_ = _server_->get_current_event_number();
            _browser_->change_event(FIRST_EVENT);
          }
          return;
        } else if (id == ENABLE_EH_SELECTION) {
          DT_LOG_WARNING(datatools::logger::PRIO_WARNING, "Enable EH selection");
          // if (_eh_widgets_.tg_enable->IsDown())
          //   _eh_widgets_.set_state(true);
          // else _eh_widgets_.set_state(false);
        } else if (id == ENABLE_COMPLEX_SELECTION) {
          // if (_complex_widgets_.tg_enable->IsDown())
          //   _complex_widgets_.set_state(true);
          // else _complex_widgets_.set_state(false);
        }

        // // Disable update button if none of the check box is enable
        // if (!_complex_widgets_.tg_enable->IsDown() &&
        //     !_eh_widgets_.tg_enable->IsDown()) {
        //   DT_LOG_WARNING(datatools::logger::PRIO_WARNING, "coucou");
        //   _selection_widgets_.tg_update->SetState(kButtonDisabled);
        //   _server_->clear_selection();
        //   _server_->fill_selection();
        //   _status_->update(true);
        //   _selection_enable_ = false;
        // } else {
        //   DT_LOG_WARNING(datatools::logger::PRIO_WARNING, "caca");
          // Change update button
          // _selection_widgets_.tg_update->SetState(kButtonUp);
          // _selection_widgets_.tg_update->SetTextColor(TColor::Number2Pixel(kRed));
        // }
        DT_LOG_WARNING(datatools::logger::PRIO_WARNING, "Exiting.");
        return;
      }

      void event_selection::select_events(const button_signals_type signal_,
                                          const int event_selected_)
      {
        io::event_server & the_server = *_server_;

        if (the_server.has_external_data()) {
          DT_LOG_TRACE(options_manager::get_instance().get_logging_priority(),
                       "No selection since data record comes from external part !");
          return;
        }

        if (is_selection_enable()) {
          DT_LOG_DEBUG(options_manager::get_instance().get_logging_priority(),
                       "Looking for event filling selection...");
        }

        button_signals_type the_signal = signal_;

        int current_event_id = the_server.get_current_event_number();
        if (event_selected_ != -1)
          current_event_id = event_selected_;

        bool is_selected = false;
        while (!is_selected) {
          if (the_signal == NEXT_EVENT) {
            current_event_id++;
          } else if (the_signal == PREVIOUS_EVENT) {
            current_event_id--;
          } else if (the_signal == LAST_EVENT) {
            current_event_id = _server_->get_last_selected_event();
          } else if (the_signal == FIRST_EVENT) {
            current_event_id = _server_->get_first_selected_event();
          }

          // Reach the end of events
          if (current_event_id >  _server_->get_last_selected_event()) {
            current_event_id = _server_->get_last_selected_event();
          }
          // Reach the begin of events
          if (current_event_id < _server_->get_first_selected_event()) {
            current_event_id = _server_->get_first_selected_event();
          }

          if (!the_server.read_event(current_event_id)) {
            DT_LOG_ERROR(options_manager::get_instance().get_logging_priority(),
                         "Something gets wrong when reading event #" << current_event_id << " !");
            break;
          }

          if (is_selection_enable()) {
            // Get selected events
            if (this->_check_cuts_()) {
              _server_->select_event(current_event_id);
              is_selected = true;
            } else {
              _server_->remove_event(current_event_id);
              if (the_signal == FIRST_EVENT) the_signal = NEXT_EVENT;
              if (the_signal == LAST_EVENT)  the_signal = PREVIOUS_EVENT;
            }

            // No more events to look for
            if (! _server_->has_selected_event()) {
              EMsgBoxIcon icon_type     = kMBIconExclamation;
              EMsgBoxButton button_type = kMBClose;
              int retval;
              std::ostringstream info;
              info << "No record full fills the selection cuts !";
              DT_LOG_INFORMATION(options_manager::get_instance().get_logging_priority(),
                                 info.str());
              new TGMsgBox(gClient->GetRoot(), _browser_,
                           "Event selection", info.str().c_str(),
                           icon_type, button_type, &retval);

              break;
            }
          } else {
            is_selected = true;
          }
        }

        // Update buttons given avalaible banks
        this->_update_widgets_();

        // Update status bar
        _status_->update(is_selection_enable(), is_selection_enable());
        _status_->update_buttons(signal_);

        return;
      }

      void event_selection::_update_widgets_()
      {
        // if (! _server_->get_event().has(io::EH_LABEL) &&
        //     _widgets_.count(event_selection::eh_cut_label())) {
        //   _widgets_[event_selection::eh_cut_label()]->set_state(false);
        // }
        // if (! _server_->get_event().has(io::SD_LABEL) &&
        //     _widgets_.count(event_selection::sd_cut_label())) {
        //   _widgets_[event_selection::sd_cut_label()]->set_state(false);
        // }
        return;
      }

      void event_selection::_build_widgets_()
      {
        _widgets_.push_back(new selection_widget(this));
        _widgets_.push_back(new event_header_selection_widget(this));
        _widgets_.push_back(new simulated_data_selection_widget(this));
        _widgets_.push_back(new complex_selection_widget(this));
        for (widget_collection_type::iterator i = _widgets_.begin();
             i != _widgets_.end(); ++i) {
          base_widget * a_widget = *i;
          a_widget->build(_main_);
        }
        return;
      }

      bool event_selection::_check_cuts_()
      {
        int cut_status = cuts::SELECTION_REJECTED;
        try {
          cuts::i_cut * ptr_cut = 0;
          // if (_selection_widgets_.tg_and_button->IsDown()) {
          //   ptr_cut = &_cut_manager_->grab(event_selection::multi_and_cut_label());
          // } else if (_selection_widgets_.tg_or_button->IsDown()) {
          //   ptr_cut = &_cut_manager_->grab(event_selection::multi_or_cut_label());
          // } else if (_selection_widgets_.tg_xor_button->IsDown()) {
          //   ptr_cut = &_cut_manager_->grab(event_selection::multi_xor_cut_label());
          // }
          DT_THROW_IF(ptr_cut == 0, std::logic_error, "None of the cut (AND, XOR, OR) have been registered !");
          cuts::i_cut & the_cut = *ptr_cut;

          const io::event_record & a_record = _server_->get_event();
          the_cut.set_user_data(a_record);
          cut_status = the_cut.process();
          the_cut.reset_user_data();
        } catch (std::exception & x) {
          DT_LOG_ERROR(options_manager::get_instance().get_logging_priority(), x.what());
        }

        if (cut_status != cuts::SELECTION_ACCEPTED) return false;

        return true;
      }

      void event_selection::_build_cuts_()
      {
        // cuts::cut_handle_dict_type & the_cuts = _cut_manager_->get_cuts();
        cuts::cut_handle_dict_type::iterator found;
        // if (_selection_widgets_.tg_and_button->IsDown()) {
        //   found = the_cuts.find(event_selection::multi_and_cut_label());
        // } else if (_selection_widgets_.tg_or_button->IsDown()) {
        //   found = the_cuts.find(event_selection::multi_or_cut_label());
        // } else if (_selection_widgets_.tg_xor_button->IsDown()) {
        //   found = the_cuts.find(event_selection::multi_xor_cut_label());
        // } else {
        //   DT_THROW_IF(true, std::logic_error, "None of the cut (AND, XOR, OR) have been registered !");
        // }

        // Reset cut
        if (found->second.is_initialized()) {
          found->second.grab_cut().reset();
          found->second.set_uninitialized();
        }

        std::vector<std::string> cut_lists;
        // Complex Cut
        // if (_complex_widgets_.tg_enable->IsDown()) {
        //   if (_complex_widgets_.tg_combo->GetSelected() != 0) {
        //     TGTextLBEntry * tgt
        //       = (TGTextLBEntry*)_complex_widgets_.tg_combo->GetSelectedEntry();
        //     const std::string cut_label = tgt->GetText()->GetString();
        //     cut_lists.push_back(cut_label);
        //   }
        // }

        // // Event Header Cut
        // if (this->_build_event_header_data_cuts_()) {
        //   cut_lists.push_back(event_selection::eh_cut_label());
        // }
        // // Simulated Data Cut
        // if (this->_build_simulated_data_cuts_()) {
        //   cut_lists.push_back(event_selection::sd_cut_label());
        // }

        if (cut_lists.empty()) {
          DT_LOG_INFORMATION(options_manager::get_instance().get_logging_priority(),
                             "No cuts have been enabled !");
          _selection_enable_ = false;
          return;
        }

        datatools::properties & cuts_prop = found->second.grab_cut_config();
        cuts_prop.update("cuts", cut_lists);

        _selection_enable_ = true;
        return;
      }

      bool event_selection::_build_event_header_data_cuts_()
      {
        // if (! _eh_widgets_.tg_enable->IsDown()) return false;

        // cuts::cut_handle_dict_type & the_cuts = _cut_manager_->get_cuts();
        // cuts::cut_handle_dict_type::iterator found = the_cuts.find(event_selection::eh_cut_label());
        // if (found == the_cuts.end()) {
        //   DT_LOG_ERROR(options_manager::get_instance().get_logging_priority(),
        //                "Cut '" << event_selection::eh_cut_label() << "' has not been registered !");
        //   return false;
        // }

        // // Reset cut
        // if (found->second.is_initialized()) {
        //   found->second.grab_cut().reset();
        //   found->second.set_uninitialized();
        // }

        // bool has_cut_enabled = false;
        // datatools::properties & cuts_prop = found->second.grab_cut_config();
        // // Run number
        // const int run_number_selected_min = _eh_widgets_.tg_run_id_min->GetIntNumber();
        // const int run_number_selected_max = _eh_widgets_.tg_run_id_max->GetIntNumber();
        // if (run_number_selected_min != datatools::event_id::INVALID_RUN_NUMBER ||
        //     run_number_selected_max != datatools::event_id::INVALID_RUN_NUMBER) {
        //   cuts_prop.update_flag("mode.run_number");
        //   if (run_number_selected_min != datatools::event_id::INVALID_RUN_NUMBER)
        //     cuts_prop.update_integer("run_number.min", run_number_selected_min);
        //   if (run_number_selected_max != datatools::event_id::INVALID_RUN_NUMBER)
        //     cuts_prop.update_integer("run_number.max", run_number_selected_max);
        //   has_cut_enabled = true;
        // }
        // // Event number
        // const int event_number_selected_min = _eh_widgets_.tg_event_id_min->GetIntNumber();
        // const int event_number_selected_max = _eh_widgets_.tg_event_id_max->GetIntNumber();
        // if (event_number_selected_min != datatools::event_id::INVALID_EVENT_NUMBER ||
        //     event_number_selected_max != datatools::event_id::INVALID_EVENT_NUMBER) {
        //   cuts_prop.update_flag("mode.event_number");
        //   if (event_number_selected_min != datatools::event_id::INVALID_EVENT_NUMBER)
        //     cuts_prop.update_integer("event_number.min", event_number_selected_min);
        //   if (event_number_selected_max != datatools::event_id::INVALID_EVENT_NUMBER)
        //     cuts_prop.update_integer("event_number.max", event_number_selected_max);
        //   has_cut_enabled = true;
        // }

        // return has_cut_enabled;
        return true;
      }

      void event_selection::_install_cut_manager_()
      {
        const options_manager & opt_mgr = options_manager::get_instance();
        _cut_manager_->set_logging_priority(opt_mgr.get_logging_priority());

        // Load event browser cuts
        this->_install_private_cuts_();

        const std::string & config_filename = opt_mgr.get_cut_config_file();

        if (! config_filename.empty()) {
          std::string filename = config_filename;
          datatools::fetch_path_with_env(filename);
          datatools::properties cut_manager_config;
          datatools::properties::read_config(filename,
                                             cut_manager_config);
          _cut_manager_->initialize(cut_manager_config);
        } else {
          datatools::properties dummy;
          _cut_manager_->initialize(dummy);
        }
        if (opt_mgr.get_logging_priority() >= datatools::logger::PRIO_DEBUG) {
          _cut_manager_->tree_dump(std::clog);
        }

        for (widget_collection_type::iterator i = _widgets_.begin();
             i != _widgets_.end(); ++i) {
          base_widget * a_widget = *i;
          a_widget->initialize();
        }
        return;
      }

      void event_selection::_install_private_cuts_()
      {
        const std::string priority_str
          = datatools::logger::get_priority_label(options_manager::get_instance().get_logging_priority());

        // Cut logic
        {
          datatools::properties cuts_prop;
          cuts_prop.store_string("logging.priority", priority_str);

          // Install a 'multi_and_cut' and a 'multi_or_cut'
          _cut_manager_->load_cut(event_selection::multi_and_cut_label(),
                                  "cuts::multi_and_cut",
                                  cuts_prop);
          _cut_manager_->load_cut(event_selection::multi_or_cut_label(),
                                  "cuts::multi_or_cut",
                                  cuts_prop);
          _cut_manager_->load_cut(event_selection::multi_xor_cut_label(),
                                  "cuts::multi_xor_cut",
                                  cuts_prop);
        }

        // Event header cut
        {
          datatools::properties cuts_prop;
          cuts_prop.store_string("logging.priority", priority_str);
          cuts_prop.store_string("EH_label", io::EH_LABEL);
          _cut_manager_->load_cut(event_selection::eh_cut_label(),
                                  "snemo::cut::event_header_cut",
                                  cuts_prop);
        }

        // Simulated data cut
        {
          datatools::properties cuts_prop;
          cuts_prop.store_string("logging.priority", priority_str);
          cuts_prop.store_string("SD_label", io::SD_LABEL);
          _cut_manager_->load_cut(event_selection::sd_cut_label(),
                                  "snemo::cut::simulated_data_cut",
                                  cuts_prop);
        }

        return;
      }

    } // end of namespace view

  } // end of namespace visualization

} // end of namespace snemo

// end of event_selection.cc

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
