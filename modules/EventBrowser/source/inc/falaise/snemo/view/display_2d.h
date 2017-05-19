// -*- mode: c++ ; -*-
/* display_2d.h
 * Author (s) :     Xavier Garrido <<garrido@lal.in2p3.fr>>
 * Creation date: 2010-06-13
 * Last modified: 2011-03-16
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
 *
 *   2D display view
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_VISUALIZATION_VIEW_DISPLAY_2D_H
#define FALAISE_SNEMO_VISUALIZATION_VIEW_DISPLAY_2D_H 1

#include <falaise/snemo/view/event_browser_signals.h>
#include <falaise/snemo/view/view_models.h>

#include <Rtypes.h>

class TGCompositeFrame;

namespace snemo {

  namespace visualization {

    namespace io {
      class event_server;
    }

    namespace view {

      class i_draw_manager;
      class i_embedded_viewer;

      class display_2d
      {
      public:
        void set_view_type (const view_type vtype_ = TOP_VIEW);

        display_2d (TGCompositeFrame * main_,
                    io::event_server * server_,
                    const bool switch_mode = true);
        virtual ~display_2d ();

        void set_drawer (i_draw_manager * draw_manager_ = 0);

        void clear  ();
        void reset  ();

        void update_detector ();
        void update_scene    ();

        void process_option_buttons ();
        void handle_button_signals (const button_signals_type signal_);

      private:

        io::event_server  *  _server_;

        i_draw_manager    *  _2d_drawer_;
        i_embedded_viewer *  _2d_viewer_;

        display_2d             ();                   // disabled
        display_2d             (const display_2d &); // disabled
        display_2d & operator= (const display_2d &); // disabled

        ClassDef (display_2d, 0);
      };

    } // end of namespace view

  } // end of namespace visualization

} // end of namespace snemo

#endif // FALAISE_SNEMO_VISUALIZATION_VIEW_DISPLAY_2D_H

// end of display_2d.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
