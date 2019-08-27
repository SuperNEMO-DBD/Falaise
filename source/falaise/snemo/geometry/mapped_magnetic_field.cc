// falaise/snemo/geometry/mapped_magnetic_field.cc
/*
 * Copyright (C) 2016 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *                    Steve Snow <S.W.Snow@warwick.ac.uk>
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

// Ourselves:
#include <falaise/snemo/geometry/mapped_magnetic_field.h>

// Standard library:
#include <vector>

// Third party:
// - Boost:
#if defined(__GNUC__)
#define BOOST_SYSTEM_NO_DEPRECATED 1
#endif
#include <boost/filesystem.hpp>
#if defined(__GNUC__)
#undef BOOST_SYSTEM_NO_DEPRECATED
#endif
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
// - Bayeux/datatools:
#include <datatools/clhep_units.h>
#include <datatools/properties.h>
#include <datatools/service_manager.h>
#include <datatools/units.h>
#include <datatools/utils.h>
// - Bayeux/geomtools:
#include <geomtools/utils.h>

#include <falaise/property_set.h>

namespace {
// http://stackoverflow.com/questions/6089231/getting-std-ifstream-to-handle-lf-cr-and-crlf
std::istream& safe_getline(std::istream& in_, std::string& out_) {
  out_.clear();
  // The characters in the stream are read one-by-one using a std::streambuf.
  // That is faster than reading them one-by-one using the std::istream.
  // Code that uses streambuf this way must be guarded by a sentry object.
  // The sentry object performs various tasks,
  // such as thread synchronization and updating the stream state.
  std::istream::sentry se(in_, true);
  std::streambuf* sb = in_.rdbuf();
  for (;;) {
    int c = sb->sbumpc();
    switch (c) {
      case '\n':
        return in_;
      case '\r':
        if (sb->sgetc() == '\n') {
          sb->sbumpc();
        }
        return in_;
      case EOF:
        // Also handle the case when the last line has no line ending
        if (out_.empty()) {
          in_.setstate(std::ios::eofbit);
        }
        return in_;
      default:
        out_ += (char)c;
    }
  }
}

/// \brief Private working data for MM_IMPORT_CSV_MAP_0 mode
struct csv_map_0_t {
 public:
  csv_map_0_t() = default;
  csv_map_0_t(std::string mapfile) : map_filename{std::move(mapfile)} { load(map_filename); }
  void load(const std::string& mapfile);
  void reset();
  int interpolate(const geomtools::vector_3d& position, geomtools::vector_3d& magnetic_field) const;
  int compute(const geomtools::vector_3d& position, geomtools::vector_3d& magnetic_field) const;

 public:
  // Configuration:
  std::string map_filename;
  double length_unit = CLHEP::meter;
  double mag_field_unit = datatools::units::milli() * CLHEP::gauss;
  // Header:
  unsigned int nx = 0;
  unsigned int ny = 0;
  unsigned int nz = 0;
  geomtools::vector_3d origin = geomtools::invalid_vector_3d();
  double dx = datatools::invalid_real();
  double dy = datatools::invalid_real();
  double dz = datatools::invalid_real();
  // Mapped B-field:
  using vd = std::vector<int>;
  using vvd = std::vector<vd>;
  using vvvd = std::vector<vvd>;
  using vvvvd = std::vector<vvvd>;
  vvvvd bmap;
};

void csv_map_0_t::reset() {
  bmap.clear();
  nx = 0;
  ny = 0;
  nz = 0;
  origin = geomtools::invalid_vector_3d();
  dx = datatools::invalid_real();
  dy = datatools::invalid_real();
  dz = datatools::invalid_real();
}

void csv_map_0_t::load(const std::string& mapfile) {
  std::string mfn = mapfile;
  datatools::fetch_path_with_env(mfn);
  DT_THROW_IF(!boost::filesystem::exists(mfn), std::runtime_error,
              "File '" << mfn << "' does not exist!");
  std::ifstream fin(mfn.c_str());
  DT_THROW_IF(!fin, std::runtime_error, "Cannot open file '" << map_filename << "'!");

  map_filename = mapfile;
  this->reset();
  {
    // Read header line:
    std::string header_line;
    safe_getline(fin, header_line);
    DT_THROW_IF(!fin, std::runtime_error, "Cannot read map file header!");

    std::vector<std::string> htokens;
    boost::split(htokens, header_line, boost::is_any_of(","));
    DT_THROW_IF(htokens.size() != 9, std::logic_error, "Invalid header line format!");
    nx = boost::lexical_cast<unsigned int>(htokens[0]);
    ny = boost::lexical_cast<unsigned int>(htokens[1]);
    nz = boost::lexical_cast<unsigned int>(htokens[2]);

    double x0(0.0), y0(0.0), z0(0.0);
    x0 = boost::lexical_cast<double>(htokens[3]);
    y0 = boost::lexical_cast<double>(htokens[4]);
    z0 = boost::lexical_cast<double>(htokens[5]);
    origin.set(x0 * length_unit, y0 * length_unit, z0 * length_unit);
    dx = boost::lexical_cast<double>(htokens[6]) * length_unit;
    dy = boost::lexical_cast<double>(htokens[7]) * length_unit;
    dz = boost::lexical_cast<double>(htokens[8]) * length_unit;
  }

  {
    // Read map:
    csv_map_0_t::vvvvd& bb = bmap;
    bb.reserve(3);
    for (size_t ax = 0; ax < 3; ax++) {
      {
        csv_map_0_t::vvvd dummy;
        bb.push_back(dummy);
      }
      csv_map_0_t::vvvd& bxyz = bb.back();
      bxyz.reserve(nz);
      for (size_t iz = 0; iz < nz; iz++) {
        {
          csv_map_0_t::vvd dummy;
          bxyz.push_back(dummy);
        }
        csv_map_0_t::vvd& bxy = bxyz.back();
        bxy.reserve(ny);
        for (size_t iy = 0; iy < ny; iy++) {
          std::string bmap_line;
          safe_getline(fin, bmap_line);
          std::vector<std::string> btokens;
          boost::split(btokens, bmap_line, boost::is_any_of(","));
          DT_THROW_IF(btokens.size() != nx + 3, std::logic_error, "Invalid B-line format!");
          auto axi = boost::lexical_cast<unsigned int>(btokens[0]);
          auto iyi = boost::lexical_cast<unsigned int>(btokens[1]);
          auto izi = boost::lexical_cast<unsigned int>(btokens[2]);
          DT_THROW_IF(axi != ax || iyi != iy || izi != iz, std::logic_error,
                      "Invalid B map line format!");
          {
            csv_map_0_t::vd dummy;
            bxy.push_back(dummy);
          }
          csv_map_0_t::vd& xdata = bxy.back();
          xdata.reserve(btokens.size() - 3);
          for (size_t ix = 3; ix < btokens.size(); ix++) {
            xdata.push_back(boost::lexical_cast<int>(btokens[ix]));
          }
        }
      }
    }
  }
}

int csv_map_0_t::interpolate(const geomtools::vector_3d& position_,
                             geomtools::vector_3d& magnetic_field) const {
  double xu = (position_.x() - origin.x()) / dx;
  double yu = (position_.y() - origin.y()) / dy;
  double zu = (position_.z() - origin.z()) / dz;
  int ixl = (int)xu;
  int iyl = (int)yu;
  int izl = (int)zu;
  double fx = xu - ixl;
  double fy = yu - iyl;
  double fz = zu - izl;
  double gx = 1.0 - fx;
  double gy = 1.0 - fy;
  double gz = 1.0 - fz;
  const vvvvd& bb = bmap;
  if (ixl >= 0 && ixl < (int)(nx - 1) && iyl >= 0 && iyl < (int)(ny - 1) && izl >= 0 &&
      izl < (int)(nz - 1)) {
    for (int ax = 0; ax < 3; ax++) {
      const vvvd& b3d = bb[ax];
      int b000 = b3d[izl][iyl][ixl];
      int b100 = b3d[izl][iyl][ixl + 1];
      int b010 = b3d[izl][iyl + 1][ixl];
      int b001 = b3d[izl + 1][iyl][ixl];
      int b110 = b3d[izl][iyl + 1][ixl + 1];
      int b011 = b3d[izl + 1][iyl + 1][ixl];
      int b101 = b3d[izl + 1][iyl][ixl + 1];
      int b111 = b3d[izl + 1][iyl + 1][ixl + 1];
      double bf00 = gx * b000 + fx * b100;
      double bf10 = gx * b010 + fx * b110;
      double bff0 = gy * bf00 + fy * bf10;
      double bf01 = gx * b001 + fx * b101;
      double bf11 = gx * b011 + fx * b111;
      double bff1 = gy * bf01 + fy * bf11;
      double bfff = gz * bff0 + fz * bff1;
      magnetic_field[ax] = bfff;
    }
  } else {
    geomtools::invalidate(magnetic_field);
    return snemo::geometry::mapped_magnetic_field::STATUS_ERROR;
  }

  return snemo::geometry::mapped_magnetic_field::STATUS_SUCCESS;
}

int csv_map_0_t::compute(const ::geomtools::vector_3d& position,
                         ::geomtools::vector_3d& magnetic_field) const {
  geomtools::invalidate(magnetic_field);
  // the coordinate system has its origin in the centre of the source foil.
  // X is in the horizontal direction within the foil.
  // Y is in the vertical direction within the foil.
  // Z is perpendicular to X and Y to make a right-handed cartesian system.
  double x = position.y();
  double y = position.z();
  double z = position.x();
  int mx[3];
  mx[0] = 1;
  mx[1] = 1;
  mx[2] = 1;
  if (x < 0.0) {
    mx[0] *= -1;
  }
  int my[3];
  my[0] = 1;
  my[1] = 1;
  my[2] = 1;
  if (y < 0.0) {
    my[0] *= -1;
    my[2] *= -1;
  }
  int mz[3];
  mz[0] = 1;
  mz[1] = 1;
  mz[2] = 1;
  if (z < 0.0) {
    mz[2] *= -1;
  }
  geomtools::vector_3d bb;
  geomtools::vector_3d pos_ppp(std::abs(x), std::abs(y), std::abs(z));
  int status = interpolate(pos_ppp, bb);
  if (status == snemo::geometry::mapped_magnetic_field::STATUS_SUCCESS) {
    double b[3];
    for (int ax = 0; ax < 3; ax++) {
      b[ax] = bb[ax] * mx[ax] * my[ax] * mz[ax];
    }
    magnetic_field[0] = b[2] * mag_field_unit;
    magnetic_field[1] = b[0] * mag_field_unit;
    magnetic_field[2] = b[1] * mag_field_unit;
  }
  return status;
}

}  // namespace

namespace snemo {

namespace geometry {

/// \brief Private working data
struct mapped_magnetic_field::MapImpl {
  MapImpl(const std::string& mapfile) : map{mapfile} {};
  ~MapImpl() = default;
  csv_map_0_t map;
};

// Registration instantiation macro :
EMFIELD_REGISTRATION_IMPLEMENT(mapped_magnetic_field, "snemo::geometry::mapped_magnetic_field")

mapped_magnetic_field::mapped_magnetic_field(uint32_t flags)
    : emfield::base_electromagnetic_field(flags) {
  _set_defaults();
}

mapped_magnetic_field::~mapped_magnetic_field() {
  // Because Bayeux base classes are insane...
  reset();
}

void mapped_magnetic_field::_set_defaults() {
  mapMode_ = map_mode_t::INVALID;
  zeroFieldOutsideMap_ = true;
  invertFieldAlongZ_ = false;
}

void mapped_magnetic_field::reset() {
  _set_initialized(false);
  fieldMap_.reset();
  mapFile_.clear();
  _set_defaults();
  this->base_electromagnetic_field::_set_defaults();
}

void mapped_magnetic_field::initialize(const datatools::properties& config_,
                                       datatools::service_manager& service_manager_,
                                       base_electromagnetic_field::field_dict_type& fields_) {
  DT_THROW_IF(is_initialized(), std::logic_error, "Field is already initialized !");

  // Fetch configuration parameters:
  base_electromagnetic_field::_parse_basic_parameters(config_, service_manager_, fields_);
  _set_electric_field(false);
  _set_magnetic_field(true);
  _set_magnetic_field_is_time_dependent(false);

  falaise::property_set ps{config_};

  auto modeStr = ps.get<std::string>("mapping_mode", "import_csv_map_0");
  if (modeStr == "import_csv_map_0") {
    mapMode_ = map_mode_t::IMPORT_CSV_MAP_0;
  } else {
    DT_THROW(std::logic_error, "Invalid mapping mode '" << modeStr << "'!");
  }

  mapFile_ = ps.get<std::string>("map_file", mapFile_);
  // if (mapMode_ == map_mode_t::IMPORT_CSV_MAP_0) { // Useless as this is the only mode
  fieldMap_.reset(new MapImpl{mapFile_});
  //}

  zeroFieldOutsideMap_ = ps.get<bool>("zero_field_outside_map", zeroFieldOutsideMap_);
  invertFieldAlongZ_ = ps.get<bool>("z_inverted", invertFieldAlongZ_);

  _set_initialized(true);
}

void mapped_magnetic_field::setMapFilename(const std::string& mfn) {
  DT_THROW_IF(is_initialized(), std::logic_error, "Cannot change the map source filename !");
  mapFile_ = mfn;
}

void mapped_magnetic_field::setMapMode(map_mode_t mm) {
  DT_THROW_IF(is_initialized(), std::logic_error, "Cannot change the mapping mode!");
  mapMode_ = mm;
}

void mapped_magnetic_field::setZeroFieldOutsideMap(bool flag) { zeroFieldOutsideMap_ = flag; }

void mapped_magnetic_field::setInvertedZ(bool flag) { invertFieldAlongZ_ = flag; }

int mapped_magnetic_field::compute_electric_field(const geomtools::vector_3d& /* position_ */,
                                                  double /* time_ */,
                                                  geomtools::vector_3d& efield) const {
  geomtools::invalidate(efield);
  return STATUS_ERROR;
}

int mapped_magnetic_field::compute_magnetic_field(const ::geomtools::vector_3d& position_,
                                                  double /* time_ */,
                                                  ::geomtools::vector_3d& magnetic_field) const {
  int status = STATUS_ERROR;
  if (mapMode_ == map_mode_t::IMPORT_CSV_MAP_0) {
    status = fieldMap_->map.compute(position_, magnetic_field);
    if (invertFieldAlongZ_) {
      double Bz = -magnetic_field.z();
      magnetic_field.setZ(Bz);
    }
    if (status != STATUS_SUCCESS) {
      if (zeroFieldOutsideMap_) {
        magnetic_field.set(0., 0., 0.);
        status = STATUS_SUCCESS;
      }
    }
  }
  return status;
}

void mapped_magnetic_field::tree_dump(std::ostream& out, const std::string& title,
                                      const std::string& indent, bool inherit) const {
  this->base_electromagnetic_field::tree_dump(out, title, indent, true);

  out << indent << datatools::i_tree_dumpable::tag
      << "Mapping mode : " << static_cast<std::underlying_type<map_mode_t>::type>(mapMode_)
      << std::endl;

  out << indent << datatools::i_tree_dumpable::inherit_tag(inherit) << "Map file : '" << mapFile_
      << std::endl;
}

}  // end of namespace geometry

}  // end of namespace snemo

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
