// -*- mode: c++ ; -*-
/* display_3d.h
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
 *   3D display view
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_VISUALIZATION_VIEW_DISPLAY_3D_H
#define FALAISE_SNEMO_VISUALIZATION_VIEW_DISPLAY_3D_H 1

#include <falaise/snemo/view/event_browser_signals.h>

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

      class display_3d
      {

      public:
        display_3d(TGCompositeFrame * main_, io::event_server * server_);
        virtual ~display_3d();

        void set_drawer(i_draw_manager * draw_manager_ = 0);

        void clear  ();
        void reset  ();

        void update_scene    ();
        void update_detector ();

        void handle_button_signals (const button_signals_type signal_);

      private:

        io::event_server  * _server_;

        TGCompositeFrame  * _main_;

        i_draw_manager    * _3d_drawer_;
        i_embedded_viewer * _3d_viewer_;

        display_3d             ();                   // disabled
        display_3d             (const display_3d &); // disabled
        display_3d & operator= (const display_3d &); // disabled

        ClassDef (display_3d, 0);
      };

    } // end of namespace view

  } // end of namespace visualization

} // end of namespace snemo

#endif // FALAISE_SNEMO_VISUALIZATION_VIEW_DISPLAY_3D_H

// end of display_3d.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
