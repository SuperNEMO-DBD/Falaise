// falaise/snemo/geometry/utils.cc
/* Copyright 2007-2022 F. Mauger
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
 * You should have received a copy of the GNU General Public  License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

// Ourselves:
#include <falaise/snemo/geometry/utils.h>

namespace snemo {

  namespace geometry {

    // static
    const unsigned int utils::NSIDES;

    // static
    const std::string & utils::side_back_label()
    {
      static const std::string _label("back");
      return _label;
    }

    // static
    const std::string & utils::side_front_label()
    {
      static const std::string _label("front");
      return _label;
    }

    // static
    bool utils::is_side_label_valid(const std::string & label_)
    {
      if (label_ == side_back_label()) {
        return true;
      }
      if (label_ == side_front_label()) {
        return true;
      }
      return false;  
    }

    // static
    int utils::get_side_from_label(const std::string & label_)
    {
      if (label_ == side_back_label()) {
        return side_t::BACK;
      }
      if (label_ == side_front_label()) {
        return side_t::FRONT;
      }
      return side_t::INVALID;
    }

    // static
    std::string vertex_info::from_to_label(const int from_)
    {
      if (from_ == FROM_FIRST) {
        return std::string("first");
      } else if (from_ == FROM_LAST) {
        return std::string("last");
      } else if (from_ == FROM_BEND) {
        return std::string("bend");
      }
      return std::string("");
    }

    // static
    std::string vertex_info::to_label(const extrapolation_mode_type exmod_)
    {
      if (exmod_ == EXTRAPOLATION_LINE) {
        return std::string("line");
      } else if (exmod_ == EXTRAPOLATION_HELIX) {
        return std::string("helix");
      } 
      return std::string("undefined");
    }

    // static
    std::string vertex_info::to_label(const category_type category_)
    {
      switch(category_) {
      case CATEGORY_ON_SOURCE_FOIL: return std::string("on_source_foil");
      case CATEGORY_ON_MAIN_CALORIMETER: return std::string("on_main_calorimeter");
      case CATEGORY_ON_X_CALORIMETER: return std::string("on_x_calorimeter");
      case CATEGORY_ON_GAMMA_VETO: return std::string("on_gamma_veto");
      case CATEGORY_ON_WIRE: return std::string("on_wire");
      case CATEGORY_ON_CALIBRATION_SOURCE: return std::string("on_calibration_source");
      case CATEGORY_ON_SOURCE_GAP: return std::string("on_source_gap");
      default: break;
      }
      return std::string("undefined");
    }
 
    void vertex_info::print(std::ostream & out_, const std::string & indent_) const
    {
      out_ << indent_ << "|-- Vertex category = '" <<  to_label(category) << "'\n";
      out_ << indent_ << "|-- From = '" << from_to_label(from) << "'\n";
      out_ << indent_ << "|-- Extrapolation mode = '" << to_label(extrapolation_mode) << "'\n";
      out_ << indent_ << "|-- GID = " << gid << '\n';
      out_ << indent_ << "|-- Face intercept : " << '\n';
      face_intercept.print(out_, indent_ + "|   ");
      out_ << indent_ << "|-- Distance    : " << distance / CLHEP::cm << " cm" << '\n';
      out_ << indent_ << "|-- Distance_xy : " << distance_xy / CLHEP::cm << " cm" << '\n';
      out_ << indent_ << "|-- Tolerance   : " << tolerance / CLHEP::mm << " mm" << '\n';
      out_ << indent_ << "|-- Best        : " << std::boolalpha << best << '\n';
      out_ << indent_ << "`-- Edge        : " << std::boolalpha << edge << '\n';
      return;
    }

  } // end of namespace geometry

} // end of namespace snemo
