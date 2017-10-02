/* opengl_pad.cc
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

#include <falaise/snemo/view/opengl_pad.h>

#include <THashList.h>

namespace snemo {

namespace visualization {

namespace view {

bool opengl_pad::is_batch() const {
  // no TCanvas is instantiated
  return true;
}

void opengl_pad::update() {
  // Traverse pad hierarchy and (re)paint only modified pads
  this->PaintModified();
}

TVirtualViewer3D* opengl_pad::get_viewer_3d(Option_t* /*type*/) {
  // current viewer is returned
  return fViewer3D;
}

// ctor:
opengl_pad::opengl_pad() { fPrimitives = new THashList(); }

// dtor:
opengl_pad::~opengl_pad() {}

}  // end of namespace view

}  // end of namespace visualization

}  // end of namespace snemo

// end of opengl_pad.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
