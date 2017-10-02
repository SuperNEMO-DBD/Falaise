/* composite_volume.cc
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

#include <falaise/snemo/detector/composite_volume.h>
#include <falaise/snemo/utils/root_utilities.h>

#include <geomtools/geom_info.h>

#include <TGeoManager.h>

namespace snemo {

namespace visualization {

namespace detector {

// ctor:
composite_volume::composite_volume(const std::string& name_, const std::string& category_)
    : i_root_volume(name_, category_) {
  _type = "composite";
  _composite = true;
  return;
}

// dtor:
composite_volume::~composite_volume() { return; }

void composite_volume::_construct(const geomtools::i_shape_3d& shape_3d_) {
  TGeoShape* geo_shape = utils::root_utilities::get_geo_shape(shape_3d_);

  TGeoMaterial* material = new TGeoMaterial("Dummy");
  TGeoMedium* medium = new TGeoMedium("Dummy", 1, material);
  _geo_volume = new TGeoVolume(_name.c_str(), geo_shape, medium);
  return;
}

void composite_volume::dump() const {
  this->tree_dump(std::clog, "snemo::visualization::detector::composite_volume");

  return;
}

}  // end of namespace detector

}  // end of namespace visualization

}  // end of namespace snemo

// end of composite_volume.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
