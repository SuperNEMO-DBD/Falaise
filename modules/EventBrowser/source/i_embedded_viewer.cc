/* i_embedded_viewer.cc
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

#include <falaise/snemo/view/i_embedded_viewer.h>

namespace snemo {

namespace visualization {

namespace view {

// ctor:
i_embedded_viewer::i_embedded_viewer(const view_dim_type view_dim_) : _view_dim_type(view_dim_) {
  return;
}

// dtor:
i_embedded_viewer::~i_embedded_viewer() { return; }

}  // end of namespace view

}  // end of namespace visualization

}  // end of namespace snemo

// end of i_embedded_viewer.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
