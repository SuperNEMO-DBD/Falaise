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
 * 2012-12-19 : Refactoring event selection on the basis of what I've
 *              been done for bipovisualization
 */

#ifndef FALAISE_SNEMO_VISUALIZATION_VIEW_EVENT_SELECTION_H
#define FALAISE_SNEMO_VISUALIZATION_VIEW_EVENT_SELECTION_H 1

#include <Rtypes.h>

#include <falaise/snemo/view/event_browser_signals.h>

class TGCompositeFrame;
class TGNumberEntry;
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

      class event_selection
      {

      public:

        static const std::string MULTI_AND_CUT_LABEL;
        static const std::string MULTI_OR_CUT_LABEL;
        static const std::string MULTI_XOR_CUT_LABEL;
        static const std::string EH_CUT_LABEL;

      public:

        struct selection_widgets
        {
          TGRadioButton * tg_or_button;
          TGRadioButton * tg_and_button;
          TGRadioButton * tg_xor_button;
          TGTextButton  * tg_load;
          TGTextButton  * tg_save;
          TGTextButton  * tg_reset;
          TGTextButton  * tg_update;
          void initialize ();
          void set_state (const bool enable_ = true);
        };

        struct complex_selection_widgets
        {
          TGCheckButton * tg_enable;
          TGComboBox    * tg_combo;
          void initialize ();
          void set_state (const bool enable_ = true);
        };

        struct event_header_selection_widgets
        {
          TGCheckButton * tg_enable;
          TGNumberEntry * tg_run_id_min;
          TGNumberEntry * tg_run_id_max;
          TGNumberEntry * tg_event_id_min;
          TGNumberEntry * tg_event_id_max;
          void initialize ();
          void set_state (const bool enable_ = true);
        };

      public:

        bool is_initialized () const;

        bool is_selection_enable () const;

        event_selection ();

        virtual ~event_selection ();

        void initialize (TGCompositeFrame * main_);

        void reset ();

        void set_event_server (io::event_server * server_);

        void set_status_bar (view::status_bar * status_);

        void process_selection ();

        void select_events (const button_signals_type signal_ = UNDEFINED,
                            const int event_selected_ = -1);

      private:

        void _at_init_ (TGCompositeFrame * main_);

        void _at_construct_ ();

        void _install_cut_manager_ ();

        void _install_private_cuts_ ();

        void _build_cuts_ ();

        bool _build_event_header_data_cuts_ ();

        bool _check_cuts_ ();

        void _build_buttons_ ();

        void _update_buttons_ ();

        void _build_event_header_data_buttons_ ();

        void _build_complex_selection_buttons_ ();

      private:

        bool _initialized_;

        bool _selection_enable_;

        int _initial_event_id_;

        TGCompositeFrame * _main_;
        io::event_server * _server_;
        event_browser    * _browser_;
        status_bar       * _status_;

        // Selection widgets
        selection_widgets              _selection_widgets_;
        complex_selection_widgets      _complex_widgets_;
        event_header_selection_widgets _eh_widgets_;

        cuts::cut_manager * _cut_manager_;

        // No I/O so ClassDefVersionID = 0
        ClassDef (event_selection, 0);

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
