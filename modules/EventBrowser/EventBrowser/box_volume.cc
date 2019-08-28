/* box_volume.cc
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

#include <EventBrowser/detector/box_volume.h>

#include <geomtools/box.h>
#include <geomtools/geom_info.h>

#include <TGeoManager.h>

namespace snemo {

namespace visualization {

namespace detector {

// ctor:
box_volume::box_volume(const std::string &name_, const std::string &category_)
    : i_root_volume(name_, category_) {
  _type = "box";
  _composite = false;

  _length_ = 0.0;
  _width_ = 0.0;
  _height_ = 0.0;
}

// dtor:
box_volume::~box_volume() = default;

void box_volume::_construct(const geomtools::i_shape_3d &shape_3d_) {
  const auto &mbox = dynamic_cast<const geomtools::box &>(shape_3d_);
  _length_ = mbox.get_x();
  _width_ = mbox.get_y();
  _height_ = mbox.get_z();

  auto *material = new TGeoMaterial("Dummy");
  auto *medium = new TGeoMedium("Dummy", 1, material);

  _geo_volume =
      gGeoManager->MakeBox(_name.c_str(), medium, _length_ / 2., _width_ / 2., _height_ / 2.);
}

void box_volume::tree_dump(std::ostream &out_, const std::string &title_,
                           const std::string &indent_, bool inherit_) const {
  std::string indent;
  if (!indent_.empty()) {
    indent = indent_;
  }
  i_root_volume::tree_dump(out_, title_, indent_, true);

  out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_) << "(l, w, h) : (" << _length_
       << ", " << _width_ << ", " << _height_ << ")" << std::endl;
}

void box_volume::dump() const {
  this->tree_dump(std::clog, "snemo::visualization::detector::box_volume");
}

}  // end of namespace detector

}  // end of namespace visualization

}  // end of namespace snemo

// end of box_volume.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
