/* opengl_event_handler.cc
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

#include <iostream>
#include <falaise/snemo/view/opengl_event_handler.h>
#include <falaise/snemo/view/opengl_embedded_viewer.h>

#include <TGLViewer.h>
#include <TMath.h>

namespace snemo {

  namespace visualization {

    namespace view {

      // ctor:
      opengl_event_handler::opengl_event_handler (opengl_embedded_viewer * viewer_) :
        TGLEventHandler (viewer_->GetFrame (), viewer_)
      {
      }

      // dtor:
      opengl_event_handler::~opengl_event_handler ()
      {
      }

      bool opengl_event_handler::handle_button (Event_t * event_)
      {
        // Handle mouse button 'event':  invert mouse wheel behavior,
        // make it à la Google Maps.  Left mouse button move the camera
        // whereas middle button rotate the camera.
        switch (event_->fCode)
          {
            // left mouse button
          case kButton1:
            event_->fCode = kButton2;
            break;

            // middle mouse button
          case kButton2:
            event_->fCode = kButton1;
            break;

            // right mouse button: disabled in future to allow popup
            // notification
          case kButton3:
            //            return false;
            break;

            // mouse wheel button
          case kButton5:
            event_->fCode = kButton4;
            break;

          case kButton4:
            event_->fCode = kButton5;
            break;
          }

        return TGLEventHandler::HandleButton (event_);
      }

    } // end of namespace view

  } // end of namespace visualization

} // end of namespace snemo

// end of opengl_event_handler.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
