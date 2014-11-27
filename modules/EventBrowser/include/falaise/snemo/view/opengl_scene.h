// -*- mode: c++ ; -*-
/* opengl_scene.h
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
 *  Basic implementation of OpenGL scene (T)Objects are
 *  added to TPad and then given to TGLScenePad
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_VISUALIZATION_VIEW_OPENGL_SCENE_H
#define FALAISE_SNEMO_VISUALIZATION_VIEW_OPENGL_SCENE_H 1

class TGLScenePad;
class TObject;

namespace snemo {

  namespace visualization {

    namespace view {

      class opengl_pad;

      class opengl_scene
      {

      public:

        opengl_scene ();
        virtual ~opengl_scene ();

        void add (TObject* object_);
        void clean ();

        TGLScenePad * get_scene_pad ();
        // TPad * get_pad ();

      private:

        TGLScenePad * _scene_pad_;
        opengl_pad  * _opengl_pad_;

      };

    } // end of namespace view

  } // end of namespace visualization

} // end of namespace snemo

#endif // FALAISE_SNEMO_VISUALIZATION_VIEW_OPENGL_SCENE_H

// end of opengl_scene.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
