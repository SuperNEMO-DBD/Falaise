// -*- mode: c++ ; -*-
/* bipo_draw_manager.h
 * Author(s): Xavier Garrido <<garrido@lal.in2p3.fr>>
 * Creation date: 2011-11-04
 * Last modified: 2011-11-04
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
 *  BiPo specific drawer
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_VISUALIZATION_VIEW_BIPO_DRAW_MANAGER_H
#define FALAISE_SNEMO_VISUALIZATION_VIEW_BIPO_DRAW_MANAGER_H 1

#include <falaise/snemo/view/i_draw_manager.h>

#include <string>

class TObjArray;

namespace snemo {

  namespace visualization {

    namespace io {
      class event_server;
    }

    namespace view {

      class calorimeter_hit_renderer;
      class visual_track_renderer;

      class bipo_draw_manager : public i_draw_manager
      {

      public:
        bipo_draw_manager (const io::event_server * server_);
        virtual ~bipo_draw_manager ();

        virtual TObjArray * get_objects ();
        virtual TObjArray * get_text_objects ();

        virtual void update      ();
        virtual void draw        ();
        virtual void draw_text   ();
        virtual void clear       ();
        virtual void reset       ();

      protected:

        void _add_simulated_data  ();
        void _add_calibrated_data ();

      private:

        void _add_simulated_vertex_ ();
        void _add_simulated_hits_   ();
        void _add_simulated_tracks_ ();

        void _add_calibrated_hits_  ();
        void _add_calibrated_info_  ();

      private:

        const io::event_server  * _server_;
        TObjArray               * _objects_;
        TObjArray               * _text_objects_;

        calorimeter_hit_renderer * _calorimeter_hit_renderer_;
        visual_track_renderer    * _visual_track_renderer_;

      };

    } // end of namespace view

  } // end of namespace visualization

} // end of namespace bipo

#endif // FALAISE_SNEMO_VISUALIZATION_VIEW_BIPO_DRAW_MANAGER_H

// end of bipo_draw_manager.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
