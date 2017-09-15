// -*- mode: c++ ; -*-
/* i_embedded_viewer.h
 * Author (s) :     Xavier Garrido <<garrido@lal.in2p3.fr>>
 * Creation date: 2010-12-12
 * Last modified: 2010-12-12
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
 *   Interface (abstract) class to manage ROOT viewer
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_VISUALIZATION_VIEW_I_EMBEDDED_VIEWER_H
#define FALAISE_SNEMO_VISUALIZATION_VIEW_I_EMBEDDED_VIEWER_H 1

#include <falaise/snemo/view/view_models.h>

class TCanvas;
class TGFrame;

namespace snemo {

  namespace visualization {

    namespace view {

      class i_draw_manager;

      class i_embedded_viewer
      {

      public:

        enum view_dim_type
          {
            VIEW_2D = 0,
            VIEW_3D = 1
          };

      protected:

        view_dim_type _view_dim_type;

      public:

        i_embedded_viewer (const view_dim_type view_dim_ = VIEW_3D);
        virtual ~i_embedded_viewer ();

        virtual void clear () = 0;
        virtual void reset () = 0;

        virtual void set_view_type (const view_type view_type_) = 0;
        virtual view_type get_view_type () const = 0;

        virtual void update_detector () = 0;
        virtual void update_scene    (i_draw_manager * drawer_) = 0;

        virtual TCanvas * get_canvas () = 0;
        virtual TGFrame * get_frame  () = 0;

      private:

        i_embedded_viewer            (const i_embedded_viewer &);   // not implemented
        i_embedded_viewer & operator=(const i_embedded_viewer &);   // not implemented
      };

    } // end of namespace view

  } // end of namespace visualization

} // end of namespace snemo

#endif // FALAISE_SNEMO_VISUALIZATION_VIEW_I_EMBEDDED_VIEWER_H

// end of i_embedded_viewer.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
