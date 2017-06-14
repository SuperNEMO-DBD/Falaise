// -*- mode: c++ ; -*-
/* event_selection.h
 * Author (s) :   Xavier Garrido <<garrido@lal.in2p3.fr>>
 * Creation date: 2010-06-12
 * Last modified: 2012-12-19
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
 *   definition of cut selection
 *
 * History:
 *
 * 2016-04-02 : Adding base_widget class to create widget
 * 2012-12-19 : Refactoring event selection
 */

#ifndef FALAISE_SNEMO_VISUALIZATION_VIEW_EVENT_SELECTION_H
#define FALAISE_SNEMO_VISUALIZATION_VIEW_EVENT_SELECTION_H 1

// Standard library:
#include <map>

// - ROOT:
#include <Rtypes.h>

// - This project:
#include <falaise/snemo/view/event_browser_signals.h>

// Forward declaration
class TGCompositeFrame;
class TGNumberEntry;
class TGTextEntry;
class TGCheckButton;
class TGComboBox;
class TGRadioButton;
class TGTextButton;

namespace cuts {
  class cut_manager;
}

namespace snemo {

  namespace visualization {

    namespace io {
      class event_server;
    }

    namespace view {

      class event_browser;
      class status_bar;

      /// \brief A class hosting interactive event selection cut
      class event_selection
      {
      public:

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

        // /// Structure hosting complex selection widgets
        // class complex_selection_widget : public base_widget
        // {
        // public:
        //   complex_selection_widget(event_selection * selection_);
        //   virtual void initialize();
        //   virtual void set_state(const bool enable_ = true);
        //   virtual void build(TGCompositeFrame * frame_);
        // private:
        //   TGCheckButton * _enable_;
        //   TGComboBox * _combo_;
        // };

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

      public:

        /// Collection of widgets
        typedef std::map<int, base_widget*> widget_collection_type;

        /// Check initialization status
        bool is_initialized() const;

        /// Check if selection is enabled
        bool is_selection_enable() const;

        /// Constructor
        event_selection();

        /// Destructor
        virtual ~event_selection();

        /// Initialization
        void initialize(TGCompositeFrame * main_);

        /// Reset
        void reset();

        /// Assign event server pointer
        void set_event_server(io::event_server * server_);

        /// Return a non-mutable reference to event server
        io::event_server & get_event_server() const;

        /// Assign status bar pointer
        void set_status_bar(view::status_bar * status_);

        /// Return a non-mutable reference to cut manager
        const cuts::cut_manager & get_cut_manager() const;

        /// Return a mutable reference to cut manager
        cuts::cut_manager & grab_cut_manager();

        /// Main process method
        void process();

        /// Event selection
        void select_events(const button_signals_type signal_ = UNDEFINED,
                           const int event_selected_ = -1);

      private:

        /// Install cut manager
        void _install_cut_manager_();

        /// Install private cuts
        void _install_private_cuts_();

        /// Build cuts
        void _build_cuts_();

        /// Check cuts
        bool _check_cuts_();

        /// Build GUI widgets
        void _build_();

      private:

        bool _initialized_;
        bool _selection_enable_;

        int _initial_event_id_;

        TGCompositeFrame * _main_;
        io::event_server * _server_;
        event_browser    * _browser_;
        status_bar       * _status_;

        cuts::cut_manager * _cut_manager_;//!< Cut manager pointer
        std::string _current_cut_name_;

        TGTextButton  * _load_button_;
        TGTextButton  * _save_button_;
        TGTextButton  * _reset_button_;
        TGTextButton  * _update_button_;
        widget_collection_type _widgets_;

        // No I/O so ClassDefVersionID = 0
        ClassDef(event_selection, 0);

      };

    } // end of namespace view

  } // end of namespace visualization

} // end of namespace snemo

#endif // FALAISE_SNEMO_VISUALIZATION_VIEW_EVENT_SELECTION_H

// end of event_selection.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
