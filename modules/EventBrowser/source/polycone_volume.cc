/* polycone_volume.cc
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

#include <falaise/snemo/detector/polycone_volume.h>

#include <falaise/snemo/utils/root_utilities.h>

#include <geomtools/polycone.h>
#include <geomtools/geom_info.h>

#include <TGeoManager.h>
#include <TGeoPcon.h>

namespace snemo {

  namespace visualization {

    namespace detector {

      // ctor:
      polycone_volume::polycone_volume(const std::string & name_,
                                       const std::string & category_) :
        i_root_volume(name_, category_)
      {
        _type = "polycone";
        _composite = false;

        _nbr_z_section_ = 0;
        return;
      }

      // dtor:
      polycone_volume::~polycone_volume()
      {
        return;
      }

      void polycone_volume::_construct(const geomtools::i_shape_3d & shape_3d_)
      {
        const geomtools::polycone & mpolycone =
          dynamic_cast<const geomtools::polycone &>(shape_3d_);

        _nbr_z_section_ = mpolycone.points().size();

        TGeoShape* geo_shape = utils::root_utilities::get_geo_shape(mpolycone);
        geo_shape->SetName(_name.c_str());

        TGeoMaterial * material = new TGeoMaterial("Dummy");
        TGeoMedium   * medium   = new TGeoMedium("Dummy", 1, material);

        _geo_volume = new TGeoVolume(_name.c_str(), geo_shape, medium);
        return;
      }

      void polycone_volume::tree_dump(std::ostream      & out_,
                                      const std::string & title_,
                                      const std::string & indent_,
                                      bool inherit_) const
      {
        std::string indent;
        if (! indent_.empty()) indent = indent_;
        i_root_volume::tree_dump(out_, title_, indent_, true);

        const TGeoPcon * pgcon
          = dynamic_cast<const TGeoPcon*>(_geo_volume->GetShape());

        out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
             << "number of z sections : " << _nbr_z_section_ << std::endl;

        for (size_t iz = 0; iz < _nbr_z_section_; ++iz)
          {
            std::ostringstream indent_oss;
            indent_oss << indent;
            indent_oss << datatools::i_tree_dumpable::inherit_skip_tag(inherit_);
            if (iz == _nbr_z_section_-1)
              {
                indent_oss << datatools::i_tree_dumpable::last_tag;
              }
            else
              {
                indent_oss << datatools::i_tree_dumpable::tag;
              }
            out_ << indent_oss.str()
                 << "(z, rmin, rmax) = ("
                 << pgcon->GetZ(iz) << ", "
                 << pgcon->GetRmin(iz) << ", "
                 << pgcon->GetRmax(iz) << ")"
                 << std::endl;
          }
        return;
      }

      void polycone_volume::dump() const
      {
        this->tree_dump(std::clog, "snemo::visualization::detector::polycone_volume");
        return;
      }

    } // end of namespace detector

  } // end of namespace visualization

} // end of namespace snemo

// end of polycone_volume.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
