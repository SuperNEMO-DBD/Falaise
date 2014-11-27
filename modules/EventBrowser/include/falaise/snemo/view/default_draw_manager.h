// -*- mode: c++ ; -*-
/* default_draw_manager.h
 * Author(s): Xavier Garrido <<garrido@lal.in2p3.fr>>
 * Creation date: 2010-11-20
 * Last modified: 2014-11-05
 *
 * Copyright (C) 2011-2014 Xavier Garrido <garrido@lal.in2p3.fr>
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
 *  Default draw manager
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_VISUALIZATION_VIEW_DEFAULT_DRAW_MANAGER_H
#define FALAISE_SNEMO_VISUALIZATION_VIEW_DEFAULT_DRAW_MANAGER_H 1

// This project
#include <falaise/snemo/view/i_draw_manager.h>
#include <falaise/snemo/view/calorimeter_hit_renderer.h>
#include <falaise/snemo/view/visual_track_renderer.h>

class TObjArray;

namespace snemo {

  namespace visualization {

    namespace io {
      class event_server;
    }

    namespace view {

      /// \brief A default draw manager
      class default_draw_manager : public i_draw_manager
      {

      public:
        /// Default constructor
        default_draw_manager(const io::event_server * server_);

        /// Destructor
        virtual ~default_draw_manager();

        /// Getting geometrical ROOT::TObject array
        virtual TObjArray * get_objects();

        /// Getting text ROOT::TObject array
        virtual TObjArray * get_text_objects();

        /// Update event visualization objects
        virtual void update();

        /// Drawing all geometrical scene objects
        virtual void draw();

        /// Drawing all text scene objects
        virtual void draw_text();

        /// Clearing objects
        virtual void clear();

        /// Reset
        virtual void reset();

      protected:

        /// Add 'simulated_data' bank objects
        void _add_simulated_data();

      private:

        /// Add simulated vertex position
        void _add_simulated_vertex_();

        /// Add simulated sensitive hits
        void _add_simulated_hits_();

      private:

        const io::event_server  * _server_;                  //!< Pointer to event server
        TObjArray               * _objects_;                 //!< ROOT array for geometrical TObject
        TObjArray               * _text_objects_;            //!< ROOT array for textual TObject

        calorimeter_hit_renderer _calorimeter_hit_renderer_; //<! Calorimeter hit renderer
        visual_track_renderer    _visual_track_renderer_;    //<! Tracker hit renderer
      };

    } // end of namespace view

  } // end of namespace visualization

} // end of namespace test_bench

#endif // FALAISE_SNEMO_VISUALIZATION_VIEW_DEFAULT_DRAW_MANAGER_H

// end of default_draw_manager.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
