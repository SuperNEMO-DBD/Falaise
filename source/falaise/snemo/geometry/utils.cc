// falaise/snemo/geometry/utils.cc
/* Copyright 2007-2021 F. Mauger
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
    const std::string& utils::side_back_label() {
      static const std::string _label("back");
      return _label;
    }

    // static
    const std::string& utils::side_front_label() {
      static const std::string _label("front");
      return _label;
    }

    // static
    bool utils::is_side_label_valid(const std::string& label_) {
      if (label_ == side_back_label()) {
        return true;
      }
      if (label_ == side_front_label()) {
        return true;
      }
      return false;
    }

    // static
    int utils::get_side_from_label(const std::string& label_) {
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

    void vertex_info::print(std::ostream & out_, const std::string & indent_) const
    {
      out_ << indent_ << "|-- Vertex type = '" <<  snemo::datamodel::particle_track::vertex_type_to_label(type) << "'\n";
      out_ << indent_ << "|-- From = " << from_to_label(from) << "'\n";
      out_ << indent_ << "|-- GID = " << gid << '\n';
      out_ << indent_ << "|-- Face intercept : " << '\n';
      face_intercept.print(out_, indent_ + "|   ");
      out_ << indent_ << "|-- Distance  : " << distance / CLHEP::cm << " cm" << '\n';
      out_ << indent_ << "`-- Tolerance : " << tolerance / CLHEP::mm << " mm" << '\n';
      return;
    }

  }  // end of namespace geometry

}  // end of namespace snemo
