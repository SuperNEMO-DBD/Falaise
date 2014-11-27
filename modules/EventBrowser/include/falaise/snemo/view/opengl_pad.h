// -*- mode: c++ ; -*-
/* opengl_pad.h
 * Author (s) :     Xavier Garrido <<garrido@lal.in2p3.fr>>
 * Creation date: 2011-03-23
 * Last modified: 2011-03-23
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
 *  Slightly different declaration of TPad with THashList
 *  for primitives storage
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_VISUALIZATION_VIEW_OPENGL_PAD_H
#define FALAISE_SNEMO_VISUALIZATION_VIEW_OPENGL_PAD_H 1

#include <TPad.h>

namespace snemo {

  namespace visualization {

    namespace view {

      class opengl_pad : public TPad
      {
      public:

        opengl_pad ();
        virtual ~opengl_pad ();

        bool is_batch () const;
        void update ();
        TVirtualViewer3D * get_viewer_3d (Option_t * /*type*/ = "");

        // ROOT Interface
        virtual bool IsBatch () const { return is_batch (); }
        virtual void Update ()        { return update (); }
        virtual TVirtualViewer3D *GetViewer3D (Option_t * /*type*/ = "") { return fViewer3D; }
      };

    } // end of namespace view

  } // end of namespace visualization

} // end of namespace snemo

#endif // FALAISE_SNEMO_VISUALIZATION_VIEW_OPENGL_PAD_H

// end of opengl_pad.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
