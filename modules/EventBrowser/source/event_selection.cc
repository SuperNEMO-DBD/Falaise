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

      const std::string & multi_and_cut_label()
      {
        static const std::string _label("_visu.and.cut");
        return _label;
      }

      const std::string & multi_or_cut_label()
      {
        static const std::string _label("_visu.or.cut");
        return _label;
      }

      const std::string & multi_xor_cut_label()
      {
        static const std::string _label("_visu.xor.cut");
        return _label;
      }

      const std::string & eh_cut_label()
      {
        static const std::string _label("_visu.eh.cut");
        return _label;
      }

      const std::string & sd_cut_label()
      {
        static const std::string _label("_visu.sd.cut");
        return _label;
      }

      // Private data member not to expose
      class base_widget
      {
      public:
        base_widget(event_selection * selection_, int id_ = 0);
        virtual ~base_widget();
        virtual void initialize() = 0;
        virtual void set_state() = 0;
        virtual bool get_state() const = 0;
        virtual void build(TGCompositeFrame * frame_) = 0;
        virtual std::string get_cut_name() = 0;
        virtual int id();
      protected:
        event_selection * _selection;
        int _id;
      };

      /// Structure hosting GUI widgets
      class selection_widget : public base_widget
      {
      public:
        selection_widget(event_selection * selection_);
        virtual void initialize();
        virtual void set_state();
        virtual bool get_state() const;
        virtual void build(TGCompositeFrame * frame_);
        virtual std::string get_cut_name();
      private:
        TGRadioButton * _or_button_;
        TGRadioButton * _and_button_;
        TGRadioButton * _xor_button_;
      };

      /// Structure hosting complex selection widgets
      class complex_selection_widget : public base_widget
      {
      public:
        complex_selection_widget(event_selection * selection_);
        virtual void initialize();
        virtual void set_state();
        virtual bool get_state() const;
        virtual void build(TGCompositeFrame * frame_);
        virtual std::string get_cut_name();
      private:
        TGCheckButton * _enable_;
        TGComboBox * _combo_;
      };

      /// Structure hosting event header selection widgets
      class event_header_selection_widget : public base_widget
      {
      public:
        event_header_selection_widget(event_selection * selection_);
        virtual void initialize();
        virtual void set_state();
        virtual bool get_state() const;
        virtual void build(TGCompositeFrame * frame_);
        virtual std::string get_cut_name();
      private:
        TGCheckButton * _enable_;
        TGNumberEntry * _run_id_min_;
        TGNumberEntry * _run_id_max_;
        TGNumberEntry * _event_id_min_;
        TGNumberEntry * _event_id_max_;
      };

      // struct simulated_data_selection_widget : public base_widget
      // {
      // public:
      //   simulated_data_selection_widget(event_selection * selection_);
      //   virtual void initialize();
      //   virtual void set_state(const bool enable_ = true);
      //   virtual void build(TGCompositeFrame * frame_);
      // private:
      //   TGCheckButton * _enable_;
      //   TGTextEntry * _hit_category_;
      // };

      base_widget::base_widget(event_selection * selection_, int id_)
        : _selection(selection_), _id(id_)
      {
        return;
      }

      base_widget::~base_widget()
      {
        return;
      }

      int base_widget::id()
      {
        return _id;
      }

      selection_widget::selection_widget(event_selection * selection_)
        : base_widget(selection_, ENABLE_LOGIC_SELECTION)
      {
        return;
      }

      void selection_widget::initialize()
      {
        _or_button_->SetOn(false);
        _and_button_->SetOn(true);
        _xor_button_->SetOn(false);
        return;
      }

      void selection_widget::set_state()
      {
        return;
      }

      bool selection_widget::get_state() const
      {
        // Always return false since there is no activation check box associated
        return false;
      }

      void selection_widget::build(TGCompositeFrame * frame_)
      {
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

      std::string selection_widget::get_cut_name()
      {
        std::string cut_name;
        if (_and_button_->IsDown()) {
          cut_name = multi_and_cut_label();
        } else if (_or_button_->IsDown()) {
          cut_name = multi_or_cut_label();
        } else if (_xor_button_->IsDown()) {
          cut_name = multi_xor_cut_label();
        } else {
          DT_THROW_IF(true, std::logic_error, "None of the cut (AND, XOR, OR) have been registered !");
        }

        // Instantiate cut if needed
        cuts::cut_manager & a_cut_mgr = _selection->grab_cut_manager();
        cuts::cut_handle_dict_type & the_cuts = a_cut_mgr.get_cuts();
        cuts::cut_handle_dict_type::iterator found = the_cuts.find(cut_name);
        DT_THROW_IF(found == the_cuts.end(), std::logic_error,
                    "Cut '" << cut_name << "' has not been registered !");
        found = the_cuts.find(cut_name);

        // Reset cut
        cuts::cut_entry_type & a_cet = found->second;
        if (a_cet.is_initialized()) {
          a_cet.grab_cut().reset();
          a_cet.set_uninitialized();
        }
        return cut_name;
      }

      complex_selection_widget::complex_selection_widget(event_selection * selection_)
        : base_widget(selection_, ENABLE_COMPLEX_SELECTION)
      {
        return;
      }

      void complex_selection_widget::initialize()
      {
        _enable_->SetState(kButtonUp);
        _combo_->RemoveAll();
        size_t j = 1;
        for (const auto & i : _selection->get_cut_manager().get_cuts()) {
          const std::string & the_cut_name = i.first;
          if (the_cut_name[0] != '_') {
            _combo_->AddEntry(the_cut_name.c_str(), j++);
          }
        }
        if (j == 1) {
          _combo_->AddEntry(" +++ NO CUTS +++ ", 0);
          _combo_->SetEnabled(false);
          _enable_->SetEnabled(false);
        } else {
          _combo_->AddEntry(" +++ SELECT CUT NAME +++ ", 0);
          _combo_->SetEnabled(true);
        }
        _combo_->Select(0);
        return;
      }

      void complex_selection_widget::set_state()
      {
        bool enable = _enable_->IsDown();
        _combo_->SetEnabled(enable);
        return;
      }

      bool complex_selection_widget::get_state() const
      {
        if (_enable_->IsEnabled()) {
          return _enable_->IsDown();
        }
        return false;
      }

      void complex_selection_widget::build(TGCompositeFrame * frame_)
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

      std::string complex_selection_widget::get_cut_name()
      {
        if (! _enable_->IsDown()) return {}; // empty string

        std::string cut_label;
        if (_combo_->GetSelected() != 0) {
          TGTextLBEntry * tgt
            = (TGTextLBEntry*)_combo_->GetSelectedEntry();
          cut_label = tgt->GetText()->GetString();
        }

        return cut_label;
      }

      event_header_selection_widget::event_header_selection_widget(event_selection * selection_)
        : base_widget(selection_, ENABLE_EH_SELECTION)
      {
        return;
      }

      void event_header_selection_widget::initialize()
      {
        _enable_->SetState(kButtonUp);
        _run_id_min_->SetNumber(datatools::event_id::INVALID_RUN_NUMBER);
        _run_id_max_->SetNumber(datatools::event_id::INVALID_RUN_NUMBER);
        _event_id_min_->SetNumber(datatools::event_id::INVALID_EVENT_NUMBER);
        _event_id_max_->SetNumber(datatools::event_id::INVALID_EVENT_NUMBER);
      }

      void event_header_selection_widget::set_state()
      {
        bool enable = _enable_->IsDown();
        const io::event_server & server = _selection->get_event_server();
        if (! server.get_event().has(io::EH_LABEL)) {
          _enable_->SetState(kButtonDisabled);
          enable = false;
        } else {
          if (! _enable_->IsEnabled()) {
            _enable_->SetState(kButtonEngaged);
          }
        }
        _run_id_min_->SetState(enable);
        _run_id_max_->SetState(enable);
        _event_id_min_->SetState(enable);
        _event_id_max_->SetState(enable);
        return;
      }

      bool event_header_selection_widget::get_state() const
      {
        if (_enable_->IsEnabled()) {
          return _enable_->IsDown();
        }
        return false;
      }

      void event_header_selection_widget::build(TGCompositeFrame * frame_)
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

      std::string event_header_selection_widget::get_cut_name()
      {
        if (! _enable_->IsDown()) return {}; // empty string

        cuts::cut_manager & a_cut_mgr = _selection->grab_cut_manager();
        cuts::cut_handle_dict_type & the_cuts = a_cut_mgr.get_cuts();
        cuts::cut_handle_dict_type::iterator found = the_cuts.find(eh_cut_label());
        DT_THROW_IF(found == the_cuts.end(), std::logic_error,
                    "Cut '" << eh_cut_label() << "' has not been registered !");

        // Reset cut
        cuts::cut_entry_type & a_cet = found->second;
        if (a_cet.is_initialized()) {
          a_cet.grab_cut().reset();
          a_cet.set_uninitialized();
        }

        datatools::properties & cuts_prop = a_cet.grab_cut_config();
        bool cut_updated = false;
        // Run number
        const int run_number_selected_min = _run_id_min_->GetIntNumber();
        const int run_number_selected_max = _run_id_max_->GetIntNumber();
        if (run_number_selected_min != datatools::event_id::INVALID_RUN_NUMBER ||
            run_number_selected_max != datatools::event_id::INVALID_RUN_NUMBER) {
          cuts_prop.update_flag("mode.run_number");
          if (run_number_selected_min != datatools::event_id::INVALID_RUN_NUMBER)
            cuts_prop.update_integer("run_number.min", run_number_selected_min);
          if (run_number_selected_max != datatools::event_id::INVALID_RUN_NUMBER)
            cuts_prop.update_integer("run_number.max", run_number_selected_max);
          cut_updated = true;
        }
        // Event number
        const int event_number_selected_min = _event_id_min_->GetIntNumber();
        const int event_number_selected_max = _event_id_max_->GetIntNumber();
        if (event_number_selected_min != datatools::event_id::INVALID_EVENT_NUMBER ||
            event_number_selected_max != datatools::event_id::INVALID_EVENT_NUMBER) {
          cuts_prop.update_flag("mode.event_number");
          if (event_number_selected_min != datatools::event_id::INVALID_EVENT_NUMBER)
            cuts_prop.update_integer("event_number.min", event_number_selected_min);
          if (event_number_selected_max != datatools::event_id::INVALID_EVENT_NUMBER)
            cuts_prop.update_integer("event_number.max", event_number_selected_max);
          cut_updated = true;
        }

        if (! cut_updated) {
          DT_LOG_WARNING(options_manager::get_instance().get_logging_priority(),
                         "Missing run id or event id !");
          return {};
        }

        return eh_cut_label();
      }

      // simulated_data_selection_widget::simulated_data_selection_widget(event_selection * selection_)
      //   : base_widget(selection_)
      // {
      //   return;
      // }

      // void simulated_data_selection_widget::initialize()
      // {
      //   if (_hit_category_) _hit_category_->Clear();
      //   this->set_state(false);
      // }

      // void simulated_data_selection_widget::set_state(const bool enable_)
      // {
      //   if (_hit_category_) _hit_category_->SetState(enable_);
      //   return;
      // }

      // void simulated_data_selection_widget::build(TGCompositeFrame * frame_)
      // {
      //   // Layout for positionning frame
      //   // TGLayoutHints * field_layout = new TGLayoutHints(kLHintsTop | kLHintsRight,
      //   //                                                  2, 2, 20, 2);
      //   // TGLayoutHints * label_layout = new TGLayoutHints(kLHintsTop | kLHintsLeft,
      //   //                                                  2, 2, 2, 2);
      //   TGGroupFrame * gframe = new TGGroupFrame(frame_, "      Simulated data cut", kVerticalFrame);
      //   gframe->SetTitlePos(TGGroupFrame::kLeft);
      //   frame_->AddFrame(gframe, new TGLayoutHints(kLHintsTop | kLHintsLeft, 3, 3, 3, 3));
      //   {
      //     _enable_ = new TGCheckButton(gframe, "", ENABLE_SD_SELECTION);
      //     _enable_->SetToolTipText("enable selection on simulated data bank");
      //     _enable_->Connect("Clicked()", "snemo::visualization::view::event_selection",
      //                       _selection, "process()");
      //     gframe->AddFrame(_enable_, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 0, -15, 0));
      //   }

      //   {
      //     TGCompositeFrame * cframe = new TGCompositeFrame(gframe, 200, 1,
      //                                                      kHorizontalFrame | kFixedWidth);
      //     gframe->AddFrame(cframe);
      //     cframe->AddFrame(new TGLabel(cframe, "Hit category name:"),
      //                      new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 2, 2, 2));
      //     _hit_category_ = new TGTextEntry(cframe, "", -1);
      //     // _hit_category_->Connect("ValueSet(Long_t)",
      //     //                         "snemo::visualization::view::event_selection",
      //     //                         _selection, "process()");
      //     cframe->AddFrame(_hit_category_, new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 2, 0, 2));
      //   }

      //   // Initialize values
      //   initialize();
      //   return;
      // }

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
        _selection_enable_ = false;
        _initial_event_id_ = 0;
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

      io::event_server & event_selection::get_event_server() const
      {
        DT_THROW_IF(_server_ == 0, std::logic_error, "Event server is not set !");
        return *_server_;
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
        this->_build_();
        this->_install_cut_manager_();

        _initialized_ = true;
        return;
      }

      void event_selection::reset()
      {
        if (_cut_manager_) {
          delete _cut_manager_;
        }
        _cut_manager_ = 0;
        return;
      }

      void event_selection::process()
      {
        TGButton * button = static_cast<TGButton*>(gTQSender);
        const unsigned int id = button->WidgetId();

        if (id == LOAD_SELECTION) {
          const std::string dir = falaise::get_resource_dir();
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
            i->second->initialize();
          }

          _server_->clear_selection();
          _server_->fill_selection();
          _status_->update(true);

          this->_build_cuts_();

          _browser_->change_event(CURRENT_EVENT);//, _initial_event_id_);
          return;
        } else if (id == UPDATE_SELECTION) {
          this->_build_cuts_();
          if (is_selection_enable()) {
            DT_LOG_DEBUG(options_manager::get_instance().get_logging_priority(),
                         "Selection is enable !");
            _server_->clear_selection();
            _server_->fill_selection();
            _initial_event_id_ = _server_->get_current_event_number();
            _browser_->change_event(FIRST_EVENT);
          }
          return;
        }

        widget_collection_type::iterator found = _widgets_.find(id);
        if (found != _widgets_.end()) {
          found->second->set_state();
        }

        // Change update button state
        for (const auto & i : _widgets_) {
          if (i.second->get_state()) {
            _update_button_->SetState(kButtonUp);
            return;
          }
        }
        _update_button_->SetState(kButtonDisabled);

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
            DT_LOG_TRACE(options_manager::get_instance().get_logging_priority(),
                         "Selection enable");
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
        for (auto & i : _widgets_) i.second->set_state();

        // Update status bar
        _status_->update(is_selection_enable(), is_selection_enable());
        _status_->update_buttons(signal_);

        return;
      }

      void event_selection::_build_()
      {
        // Add buttons to save, reset and apply selection
        TGVerticalFrame * vframe = new TGVerticalFrame(_main_);
        _main_->AddFrame(vframe, new TGLayoutHints(kLHintsBottom | kLHintsRight, 10, 10, 10, 10));

        TGHorizontalFrame * hframe = new TGHorizontalFrame(vframe);
        vframe->AddFrame(hframe);

        _load_button_ = new TGTextButton(hframe, "Load selection file", LOAD_SELECTION);
        _load_button_->SetToolTipText("save current selection into file");
        _load_button_->Connect("Clicked()", "snemo::visualization::view::event_selection",
                               this, "process()");
        hframe->AddFrame(_load_button_, new TGLayoutHints(kLHintsNoHints, 2, 2, 2, 2));

        _save_button_ = new TGTextButton(hframe, "Save selection as...", SAVE_SELECTION);
        _save_button_->SetToolTipText("save current selection into file");
        _save_button_->Connect("Clicked()", "snemo::visualization::view::event_selection",
                               this, "process()");
        _save_button_->SetState(kButtonDisabled);
        hframe->AddFrame(_save_button_, new TGLayoutHints(kLHintsNoHints, 2, 2, 2, 2));

        _reset_button_ = new TGTextButton(hframe, "Reset selection", RESET_SELECTION);
        _reset_button_->SetToolTipText("reset current selection");
        _reset_button_->Connect("Clicked()", "snemo::visualization::view::event_selection",
                                this, "process()");
        hframe->AddFrame(_reset_button_, new TGLayoutHints(kLHintsNoHints, 2, 2, 2, 2));

        _update_button_ = new TGTextButton(hframe, "Update selection", UPDATE_SELECTION);
        _update_button_->SetToolTipText("update selection");
        _update_button_->Connect("Clicked()", "snemo::visualization::view::event_selection",
                                 this, "process()");
        _update_button_->SetState(kButtonDisabled);
        hframe->AddFrame(_update_button_, new TGLayoutHints(kLHintsNoHints, 2, 2, 2, 2));

        // Build selection widget
        _widgets_[ENABLE_LOGIC_SELECTION] = new selection_widget(this);
        auto * ehsw = new event_header_selection_widget(this);
        _widgets_[ehsw->id()] = ehsw;
        auto * csw = new complex_selection_widget(this);
        _widgets_[csw->id()] = csw;
        // _widgets_.push_back(new simulated_data_selection_widget(this));
        for (auto & i : _widgets_) {
          i.second->build(_main_);
        }
        return;
      }

      bool event_selection::_check_cuts_()
      {
        int cut_status = cuts::SELECTION_REJECTED;
        try {
          DT_THROW_IF(! _cut_manager_->has(_current_cut_name_), std::logic_error,
                      "None of the cut (AND, XOR, OR) have been registered !");
          cuts::i_cut & the_cut = _cut_manager_->grab(_current_cut_name_);
          const io::event_record & a_record = _server_->get_event();
          the_cut.set_user_data(a_record);
          cut_status = the_cut.process();
          the_cut.reset_user_data();
        } catch (std::exception & x) {
          DT_LOG_WARNING(options_manager::get_instance().get_logging_priority(), x.what());
        }

        if (cut_status != cuts::SELECTION_ACCEPTED) {
          DT_LOG_TRACE(options_manager::get_instance().get_logging_priority(), "Event rejected");
          return false;
        }

        DT_LOG_TRACE(options_manager::get_instance().get_logging_priority(), "Event accepted");
        return true;
      }

      void event_selection::_build_cuts_()
      {
        std::vector<std::string> cut_lists;
        for (auto & a_widget : _widgets_) {
          if (a_widget.first == ENABLE_LOGIC_SELECTION) continue;
          const std::string a_cut_name = a_widget.second->get_cut_name();
          if (! a_cut_name.empty()) cut_lists.push_back(a_cut_name);
        }

        if (cut_lists.empty()) {
          DT_LOG_WARNING(options_manager::get_instance().get_logging_priority(),
                         "No cuts have been enabled !");
          _selection_enable_ = false;
          return;
        }

        _current_cut_name_ = _widgets_[ENABLE_LOGIC_SELECTION]->get_cut_name();
        cuts::cut_handle_dict_type & the_cuts = _cut_manager_->get_cuts();
        cuts::cut_handle_dict_type::iterator found = the_cuts.find(_current_cut_name_);
        datatools::properties & cuts_prop = found->second.grab_cut_config();
        cuts_prop.update("cuts", cut_lists);

        _selection_enable_ = true;
        return;
      }

      void event_selection::_install_cut_manager_()
      {
        const options_manager & opt_mgr = options_manager::get_instance();
        _cut_manager_->set_logging_priority(opt_mgr.get_logging_priority());

        // Load event browser cuts before cut manager is initialized
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
          i->second->initialize();
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
          _cut_manager_->load_cut(multi_and_cut_label(), "cuts::multi_and_cut", cuts_prop);
          _cut_manager_->load_cut(multi_or_cut_label(), "cuts::multi_or_cut", cuts_prop);
          _cut_manager_->load_cut(multi_xor_cut_label(), "cuts::multi_xor_cut", cuts_prop);
        }

        // Event header cut
        {
          datatools::properties cuts_prop;
          cuts_prop.store_string("logging.priority", priority_str);
          cuts_prop.store_string("EH_label", io::EH_LABEL);
          _cut_manager_->load_cut(eh_cut_label(), "snemo::cut::event_header_cut", cuts_prop);
        }

        // Simulated data cut
        {
          datatools::properties cuts_prop;
          cuts_prop.store_string("logging.priority", priority_str);
          cuts_prop.store_string("SD_label", io::SD_LABEL);
          _cut_manager_->load_cut(sd_cut_label(), "snemo::cut::simulated_data_cut", cuts_prop);
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
