// falaise/snemo/geometry/utils.cc
/* Copyright 2007-2014 F. Mauger
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
const unsigned int utils::NSUBMODULES;

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
  if (label_ == side_back_label()) return true;
  if (label_ == side_front_label()) return true;
  return false;
}

// static
int utils::get_side_from_label(const std::string& label_) {
  if (label_ == side_back_label()) return SIDE_BACK;
  if (label_ == side_front_label()) return SIDE_FRONT;
  return SIDE_INVALID;
}

}  // end of namespace geometry

}  // end of namespace snemo
