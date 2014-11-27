/* sphere_volume.cc
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

#include <falaise/snemo/detector/sphere_volume.h>

#include <geomtools/sphere.h>
#include <geomtools/geom_info.h>

#include <TGeoManager.h>

namespace snemo {

  namespace visualization {

    namespace detector {

      // ctor:
      sphere_volume::sphere_volume(const std::string & name_,
                                   const std::string & category_) :
        i_root_volume(name_, category_)
      {
        _type = "sphere";
        _composite = false;

        _inner_radius_ = 0.0;
        _outer_radius_ = 0.0;
        _theta_min_    = 0.0;
        _theta_max_    = 0.0;
        _phi_min_      = 0.0;
        _phi_max_      = 0.0;

        return;
      }

      // dtor:
      sphere_volume::~sphere_volume()
      {
        return;
      }

      void sphere_volume::_construct(const geomtools::i_shape_3d & shape_3d_)
      {
        const geomtools::sphere & msphere = dynamic_cast<const geomtools::sphere &>(shape_3d_);

        _inner_radius_ = 0.0;
        _outer_radius_ = msphere.get_radius();
        _theta_min_    = 0.0;
        _theta_max_    = 180.0;
        _phi_min_      = 0.0;
        _phi_max_      = 360;

        TGeoMaterial * material = new TGeoMaterial("Dummy");
        TGeoMedium   * medium   = new TGeoMedium("Dummy", 1, material);

        _geo_volume = gGeoManager->MakeSphere(_name.c_str(), medium,
                                              _inner_radius_,
                                              _outer_radius_,
                                              _theta_min_,
                                              _theta_max_,
                                              _phi_min_,
                                              _phi_max_);
        return;
      }

      void sphere_volume::tree_dump(std::ostream      & out_,
                                    const std::string & title_,
                                    const std::string & indent_,
                                    bool inherit_) const
      {
        std::string indent;
        if (! indent_.empty()) indent = indent_;
        i_root_volume::tree_dump(out_, title_, indent_, true);

        out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
             << "(r1, r2, th1, th2, ph1, ph2) : ("
             << _inner_radius_ << ", "
             << _outer_radius_ << ", "
             << _theta_min_    << ", "
             << _theta_max_    << ", "
             << _phi_min_      << ", "
             << _phi_max_      << ")" << std::endl;
        return;
      }

      void sphere_volume::dump() const
      {
        this->tree_dump(std::clog, "snemo::visualization::detector::sphere_volume");
        return;
      }

    } // end of namespace detector

  } // end of namespace visualization

} // end of namespace snemo

// end of sphere_volume.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
