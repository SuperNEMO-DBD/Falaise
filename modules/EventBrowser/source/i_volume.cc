/* i_volume.cc
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

#include <falaise/snemo/detector/i_volume.h>

namespace snemo {

  namespace visualization {

    namespace detector {

      volume_model::volume_model () :
        _composite (false),
        _color (0),
        _highlight_color (0),
        _transparency (0),
        _visibility (false)
      {
        return;
      }

      volume_model::~volume_model ()
      {
        return;
      }

      const std::string & volume_model::get_name () const
      {
        return _name;
      }

      const std::string & volume_model::get_category () const
      {
        return _category;
      }

      const std::string & volume_model::get_type () const
      {
        return _type;
      }

      const geomtools::placement & volume_model::get_placement () const
      {
        return _placement;
      }

      bool volume_model::is_composite () const
      {
        return _composite;
      }

      // ctor:
      i_volume::i_volume ()
      {
        return;
      }

      // dtor:
      i_volume::~i_volume ()
      {
        return;
      }

    } // end of namespace detector

  } // end of namespace visualization

} // end of namespace snemo

// end of i_volume.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
