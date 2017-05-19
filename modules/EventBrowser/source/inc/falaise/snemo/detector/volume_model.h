// -*- mode: c++ ; -*-
/* volume_model.h
 * Author(s): Xavier Garrido <<garrido@lal.in2p3.fr>>
 * Creation date: 2011-02-24
 * Last modified: 2011-02-24
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
 *  Simple class setting volume properties
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_VISUALIZATION_DETECTOR_VOLUME_MODEL_H
#define FALAISE_SNEMO_VISUALIZATION_DETECTOR_VOLUME_MODEL_H 1

// Standard library
#include <string>

// - Bayeux/geomtools
#include <geomtools/placement.h>

namespace snemo {

  namespace visualization {

    namespace detector {

       /// Enumeration related to visibility status of volume part
      enum visibility_type
        {
          VISIBLE   = 0,
          INVISIBLE = 1,
          DISABLE   = 2
        };

      // \brief A volume model structure holding volume properties
      class volume_model
      {
      public:
        /// Default constructor
        volume_model();

        /// Destructor
        ~volume_model();

        /// Return volume name
        const std::string & get_name() const;

        /// Return volume category
        const std::string & get_category() const;

        /// Return volume type
        const std::string & get_type() const;

        /// Return volume position
        const geomtools::placement & get_placement() const;

        /// Check if object is a composite volume
        bool is_composite() const;

      protected:

        std::string  _name;             //<! Volume name
        std::string  _category;         //<! Volume category
        std::string  _type;             //<! Volume type

        bool         _composite;        //<! Composite volume flag

        geomtools::placement _placement; //<! Volume position

        size_t _color;                  //<! Volume color
        size_t _highlight_color;        //<! Volume highlight color
        size_t _transparency;           //<! Volume transparency level (OpenGL only)
        bool   _visibility;             //<! Volume visibility flag
      };

    } // end of namespace detector

  } // end of namespace visualization

} // end of namespace snemo

#endif // FALAISE_SNEMO_VISUALIZATION_DETECTOR_VOLUME_MODEL_H

// end of volume_model.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
