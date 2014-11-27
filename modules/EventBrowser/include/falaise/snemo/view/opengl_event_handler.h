// -*- mode: c++ ; -*-
/* opengl_event_handler.h
 * Author (s) :     Xavier Garrido <<garrido@lal.in2p3.fr>>
 * Creation date: 2010-03-23
 * Last modified: 2010-03-23
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
 *   OpenGL event handler: redefine some default behavior
 *   of TGLEventHandler
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_VISUALIZATION_VIEW_OPENGL_EVENT_HANDLER_H
#define FALAISE_SNEMO_VISUALIZATION_VIEW_OPENGL_EVENT_HANDLER_H 1

#include <TGLEventHandler.h>

class TGLViewer;

namespace snemo {

  namespace visualization {

    namespace view {

      class opengl_embedded_viewer;

      class opengl_event_handler : public TGLEventHandler
      {

      public:

        opengl_event_handler (opengl_embedded_viewer * viewer_);
        virtual ~opengl_event_handler ();

        virtual bool handle_button (Event_t * event_);

        // ROOT Interface
        virtual bool HandleButton (Event_t * event_) { return handle_button (event_); }

      private:

        opengl_event_handler             (const opengl_event_handler &);   // not implemented
        opengl_event_handler & operator= (const opengl_event_handler &);   // not implemented

      };

    } // end of namespace view

  } // end of namespace visualization

} // end of namespace snemo

#endif // FALAISE_SNEMO_VISUALIZATION_VIEW_OPENGL_EVENT_HANDLER_H

// end of opengl_event_handler.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
