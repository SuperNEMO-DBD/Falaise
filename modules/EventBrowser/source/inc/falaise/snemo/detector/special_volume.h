// -*- mode: c++ ; -*-
/* special_volume.h
 * Author (s) :   Xavier Garrido <<garrido@lal.in2p3.fr>>
 * Creation date: 2012-01-24
 * Last modified: 2014-07-11
 *
 * Copyright (C) 2011-2014 Xavier Garrido <garrido@lal.in2p3.fr>
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
 *  Construction of a ROOT "pseudo-volume" for few shape such as BiPo3
 *  light guide : Volume is only shown as wired line using gnuplot
 *  rendering from geomtools/sngeometry
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_VISUALIZATION_DETECTOR_SPECIAL_VOLUME_H
#define FALAISE_SNEMO_VISUALIZATION_DETECTOR_SPECIAL_VOLUME_H 1

#include <falaise/snemo/detector/i_root_volume.h>

class TObjArray;

namespace snemo {

  namespace visualization {

    namespace detector {

      /// \brief A special volume class to handle complex volume geometry
      //
      //  This class can represent a complex geometrical form using an array of
      //  polyline. It is used for example to visualize BiPo3 light guide shape.
      class special_volume : public i_root_volume
      {

      public:

        /// Default constructor
        special_volume(const std::string & name_     = "",
                       const std::string & category_ = "");

        /// Destructor
        virtual ~special_volume();

        /// Overload the clear method to set the TPolyLine style
        virtual void clear();

        /// Overload the reset method to clean the TObjArray
        virtual void reset();

        /// Smart print
        virtual void tree_dump(std::ostream & out_         = std::clog,
                               const std::string & title_  = "",
                               const std::string & indent_ = "",
                               bool inherit_               = false) const;

        /// Default print
        virtual void dump() const;

        /// Check objects presence
        bool has_objects() const;

        void draw() const;

      protected:

        /// Highlight all polylines
        virtual void _highlight();

        /// Construct the special volume
        virtual void _construct(const geomtools::i_shape_3d & shape_3d_);

      private:

        TObjArray * _objects_; //<! ROOT object array

      };

    } // end of namespace detector

  } // end of namespace visualization

} // end of namespace snemo

#endif // FALAISE_SNEMO_VISUALIZATION_DETECTOR_SPECIAL_VOLUME_H

// end of special_volume.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
