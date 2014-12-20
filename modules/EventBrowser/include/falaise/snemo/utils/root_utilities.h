// -*- mode: c++ ; -*-
/* root_utilities.h
 * Author(s): Xavier Garrido <<garrido@lal.in2p3.fr>>
 * Creation date: 2010-11-12
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
 *
 * Few useful function to ease the conversion between
 * human format and ROOT conventions
 *
 * History:
 *
 */

#ifndef __root_utilities_h
#define __root_utilities_h 1

#include <string>

#include <TLatex.h>

namespace datatools {
  class properties;
}

#include <geomtools/clhep.h>

namespace geomtools {
  class i_shape_3d;
}

class TCanvas;
class TGeoShape;

namespace snemo {

  namespace visualization {

    namespace utils {

      class root_utilities
      {
      public:

        class TLatex3D : public TLatex
        {
        public:
          TLatex3D();
          virtual ~TLatex3D();

          virtual bool IsEqual(const TObject* object_) const;

          void SetX(const double x_) { _x_ = x_; }
          void SetY(const double y_) { _y_ = y_; }
          void SetZ(const double z_) { _z_ = z_; }

          void SetText(const std::string & text_);
          std::string GetText() const { return _text_; }

          void SetPadCoordinates(const double x_, const double y_);

          double GetX() const { return _x_; }
          double GetY() const { return _y_; }
          double GetZ() const { return _z_; }

        private:
          double _x_;
          double _y_;
          double _z_;
          std::string _text_;
        };

      public:
        static bool   g_initialized;
        static size_t g_geo_id;

        /// Convert property into ROOT line style
        static unsigned int get_line_style(const std::string & key_,
                                           const datatools::properties & property_);

        /// Convert property into ROOT color value
        static unsigned int get_color_value(const std::string & key_,
                                            const datatools::properties & property_);

        /// Get ROOT color id from a string
        static unsigned int get_color_value(const std::string & color_);

        /// Get a random color registered into ROOT color system
        static unsigned int get_random_color();

        /// Get ROOT color id for a fading color
        static unsigned int get_fade_color_from(const unsigned int color_, const double ratio);

        /// Get color as hexadecimal string
        static std::string get_hex_color(const unsigned int color_);

        /// Write RGB color into stream
        static void write_rgb_color(std::ostream & out_, const unsigned int color_);

        /// Utility to emulate 'Save as' panel
        static bool save_view_as(TCanvas* canvas_ = 0,
                                 const std::string & filename_ = "");

        /// Deconvolve geomtools shape into ROOT geo shape
        static TGeoShape * get_geo_shape(const geomtools::i_shape_3d & shape_3d_);

        /// Generate unique geometry model name
        static std::string get_unique_geo_name();

        /// Convert 'gnuplot_user_draw' function into a set of ROOT TPolyLine
        static TObjArray * gnuplot_to_root_draw(const geomtools::vector_3d   & position_,
                                                const geomtools::rotation_3d & rotation_,
                                                const geomtools::i_shape_3d  & shape_);
      };

    } // end of namespace utils

  } // end of namespace visualization

} // end of namespace snemo

#endif // __root_utilities_h

// end of root_utilities.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
