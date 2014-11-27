/* opengl_scene.cc
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

#include <falaise/snemo/view/opengl_scene.h>
#include <falaise/snemo/view/opengl_pad.h>

#include <TGLScenePad.h>
#include <TObject.h>
#include <THashList.h>

#include <iostream>

namespace snemo {

  namespace visualization {

    namespace view {

      TGLScenePad * opengl_scene::get_scene_pad ()
      {
        return _scene_pad_;
      }

      // TPad * opengl_scene::get_pad ()
      // {
      //   return __pad;
      // }

      opengl_scene::opengl_scene () :
        _scene_pad_  (0),
        _opengl_pad_ (0)
      {
        _opengl_pad_ = new opengl_pad ();
        _scene_pad_  = new TGLScenePad (_opengl_pad_);
        _scene_pad_->SetAutoDestruct (false);
        _scene_pad_->SetSmartRefresh (true);

        return;
      }

      opengl_scene::~opengl_scene ()
      {
        this->clean ();

        delete _scene_pad_;
        delete _opengl_pad_;
        return;
      }

      void opengl_scene::add (TObject* object_)
      {
        _opengl_pad_->GetListOfPrimitives ()->Add (object_);
        // std::cout << "number of pad entries = "
        //           << __opengl_pad->GetListOfPrimitives ()->GetEntries ()
        //           << std::endl;
        return;
      }

      void opengl_scene::clean ()
      {
        _opengl_pad_->Clear ();
        return;
      }

    } // end of namespace view

  } // end of namespace visualization

} // end of namespace snemo

// end of opengl_scene.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
