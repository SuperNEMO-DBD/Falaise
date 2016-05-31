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
 *
 *
 */

// Ourselves:
#include <falaise/snemo/geometry/mapped_magnetic_field.h>

// Standard library:
#include <vector>

// Third party:
// - Boost:
#if defined (__GNUC__)
#define BOOST_SYSTEM_NO_DEPRECATED 1
#endif
#include <boost/filesystem.hpp>
#if defined (__GNUC__)
#undef BOOST_SYSTEM_NO_DEPRECATED
#endif
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/units.h>
#include <datatools/clhep_units.h>
#include <datatools/utils.h>
#include <datatools/service_manager.h>
// - Bayeux/geomtools:
#include <geomtools/utils.h>

// http://stackoverflow.com/questions/6089231/getting-std-ifstream-to-handle-lf-cr-and-crlf
std::istream & safe_getline(std::istream & in_, std::string & out_)
{
    out_.clear();
    // The characters in the stream are read one-by-one using a std::streambuf.
    // That is faster than reading them one-by-one using the std::istream.
    // Code that uses streambuf this way must be guarded by a sentry object.
    // The sentry object performs various tasks,
    // such as thread synchronization and updating the stream state.
    std::istream::sentry se(in_, true);
    std::streambuf* sb = in_.rdbuf();
    for(;;) {
        int c = sb->sbumpc();
        switch (c) {
        case '\n':
            return in_;
        case '\r':
            if(sb->sgetc() == '\n') sb->sbumpc();
            return in_;
        case EOF:
            // Also handle the case when the last line has no line ending
            if (out_.empty()) in_.setstate(std::ios::eofbit);
            return in_;
        default:
            out_ += (char) c;
        }
    }
}

namespace snemo {

  namespace geometry {

    // Registration instantiation macro :
    EMFIELD_REGISTRATION_IMPLEMENT(mapped_magnetic_field, "snemo::geometry::mapped_magnetic_field")

    /// \brief Private working data for MM_IMPORT_CSV_MAP_0 mode
    struct csv_map_0_type {
    public:
      csv_map_0_type();
      void init();
      void load();
      void dump(std::ostream & = std::clog) const;
      void reset();
      int interpolate(const ::geomtools::vector_3d & position_,
                      ::geomtools::vector_3d & magnetic_field_) const;
      int compute(const ::geomtools::vector_3d & position_,
                  ::geomtools::vector_3d & magnetic_field_) const;
    public:
      // Configuration:
      std::string map_filename;
      datatools::logger::priority logging;
      double length_unit;
      double mag_field_unit;
      // Header:
      unsigned int nx;
      unsigned int ny;
      unsigned int nz;
      geomtools::vector_3d origin;
      double dx;
      double dy;
      double dz;
      // Mapped B-field:
      typedef std::vector<int>  vd;
      typedef std::vector<vd>   vvd;
      typedef std::vector<vvd>  vvvd;
      typedef std::vector<vvvd> vvvvd;
      vvvvd bmap;
    };

    csv_map_0_type::csv_map_0_type()
    {
      length_unit = CLHEP::meter;
      mag_field_unit = datatools::units::milli() * CLHEP::gauss;
      nx = 0;
      ny = 0;
      nz = 0;
      geomtools::invalidate(origin);
      datatools::invalidate(dx);
      datatools::invalidate(dy);
      datatools::invalidate(dz);
      return;
    }

    void csv_map_0_type::reset()
    {
      bmap.clear();
      nx = 0;
      ny = 0;
      nz = 0;
      geomtools::invalidate(origin);
      datatools::invalidate(dx);
      datatools::invalidate(dy);
      datatools::invalidate(dz);
      return;
    }

    void csv_map_0_type::dump(std::ostream & out_) const
    {
      out_ << "mapped_magnetic_field: mode 'MM_IMPORT_CSV_MAP_0' \n";
      out_ << "  nx = " << nx << '\n';
      out_ << "  ny = " << ny << '\n';
      out_ << "  nz = " << nz << '\n';
      out_ << "  Origin = " << origin / CLHEP::mm << " mm" << '\n';
      out_ << "  dx = " << dx / CLHEP::mm << " mm"  << '\n';
      out_ << "  dy = " << dy / CLHEP::mm << " mm"  << '\n';
      out_ << "  dz = " << dz / CLHEP::mm << " mm"  << '\n';
      return;
    }

    /// \brief Private working data
    struct mapped_magnetic_field::_work_type {
      _work_type();
      ~_work_type();
      void reset();
      csv_map_0_type csv_map_0_data;
    };

    mapped_magnetic_field::_work_type::_work_type()
    {
      return;
    }

    mapped_magnetic_field::_work_type::~_work_type()
    {
      reset();
      return;
    }

    void mapped_magnetic_field::_work_type::reset()
    {
      csv_map_0_data.reset();
      return;
    }

    void mapped_magnetic_field::_set_defaults()
    {
      _mapping_mode_ = MM_INVALID;
      _zero_field_outside_map_ = true;
      return;
    }

    mapped_magnetic_field::mapped_magnetic_field(uint32_t flags_)
      : ::emfield::base_electromagnetic_field(flags_)
    {
      _set_defaults();
      return;
    }

    mapped_magnetic_field::~mapped_magnetic_field()
    {
      if (is_initialized()) {
        reset();
      }
      return;
    }

    void mapped_magnetic_field::set_map_filename(const std::string & mfn_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Cannot change the map source filename !");
      _map_filename_ = mfn_;
      return;
    }

    void mapped_magnetic_field::set_mapping_mode(mapping_mode_type mm_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Cannot change the magnetic field value !");
      _mapping_mode_ = mm_;
      return;
    }

    void mapped_magnetic_field::initialize(const ::datatools::properties & config_,
                                           ::datatools::service_manager & service_manager_,
                                           base_electromagnetic_field::field_dict_type & fields_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Field is already initialized !");

      // Fetch configuration parameters:
      base_electromagnetic_field::_parse_basic_parameters(config_, service_manager_, fields_);
      _set_electric_field(false);
      _set_magnetic_field(true);
      _set_magnetic_field_is_time_dependent(false);

      if (_mapping_mode_ == MM_INVALID) {
        DT_THROW_IF(!config_.has_key("mapping_mode"), std::logic_error,
                    "Missing 'mapping_mode' configuration property!");
        const std::string & mm_str = config_.fetch_string("mapping_mode");
        if (mm_str == "import_csv_map_0") {
          set_mapping_mode(MM_IMPORT_CSV_MAP_0);
        } else {
          DT_THROW(std::logic_error,
                   "Invalid mapping mode '" << mm_str << "'!");
        }
      }

      if (_map_filename_.empty()) {
        DT_THROW_IF(!config_.has_key("map_file"), std::logic_error,
                    "Missing 'map_file' configuration property!");
        const std::string & mf_str = config_.fetch_string("map_file");
        DT_THROW_IF(mf_str.empty(), std::logic_error,
                    "Empty map file name!");
        set_map_filename(mf_str);
      }

      if (config_.has_key("zero_field_outside_map")) {
        bool zfom = config_.fetch_boolean("zero_field_outside_map");
        if (zfom) {
          _zero_field_outside_map_ = true;
        } else {
          _zero_field_outside_map_ = false;
        }
      }

      // Private initialization:
      _work_.reset(new _work_type);

      if (_mapping_mode_ == MM_IMPORT_CSV_MAP_0) {
        _work_->csv_map_0_data.logging = get_logging_priority();
        _work_->csv_map_0_data.map_filename = _map_filename_;
        _work_->csv_map_0_data.init();
      }

      _set_initialized(true);
      return;
    }

    void mapped_magnetic_field::reset()
    {
      DT_THROW_IF(! is_initialized(), std::logic_error, "Cannot reset the mapped magnetic field !");
      _set_initialized(false);
      // Private reset:
      if (_mapping_mode_ == MM_IMPORT_CSV_MAP_0) {
        _work_->csv_map_0_data.reset();
      }
      _work_.reset();
      _map_filename_.clear();
      _set_defaults();
      this->base_electromagnetic_field::_set_defaults();
      return;
    }

    void mapped_magnetic_field::tree_dump(std::ostream & out_,
                                          const std::string & title_,
                                          const std::string & indent_,
                                          bool inherit_) const
    {
      this->base_electromagnetic_field::tree_dump(out_, title_, indent_, true);

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Mapping mode : " << _mapping_mode_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "Map file : '" << _map_filename_ << std::endl;

      return;
    }

    int mapped_magnetic_field::compute_electric_field(const geomtools::vector_3d & /* position_ */,
                                                      double /* time_ */,
                                                      geomtools::vector_3d & electric_field_) const
    {
      geomtools::invalidate(electric_field_);
      return STATUS_ERROR;
    }

    int mapped_magnetic_field::compute_magnetic_field(const ::geomtools::vector_3d &  position_,
                                                      double  /* time_ */,
                                                      ::geomtools::vector_3d & magnetic_field_) const
    {
      int status = STATUS_ERROR;
      if (_mapping_mode_ == MM_IMPORT_CSV_MAP_0) {
        status = _work_->csv_map_0_data.compute(position_, magnetic_field_);
        if (status != STATUS_SUCCESS) {
          if (_zero_field_outside_map_) {
            magnetic_field_.set(0.,0.,0.);
            status = STATUS_SUCCESS;
          }
        }
      }
      DT_LOG_DEBUG(get_logging_priority(),
                   "Magnetic field values = "
                   << magnetic_field_ / CLHEP::gauss << " gauss");
      return status;
    }

    void csv_map_0_type::init()
    {
      load();
      return;
    }

    void csv_map_0_type::load()
    {
      DT_LOG_TRACE_ENTERING(logging);
      std::string mfn = map_filename;
      datatools::fetch_path_with_env(mfn);
      DT_THROW_IF(!boost::filesystem::exists(mfn), std::runtime_error,
                  "File '" << mfn << "' does not exist!");
      std::ifstream fin(mfn.c_str());
      DT_THROW_IF(!fin, std::runtime_error,
                  "Cannot open file '" << map_filename << "'!");

      DT_LOG_TRACE(logging, "Loading B map header...");
      {
        // Read header line:
        std::string header_line;
        safe_getline(fin, header_line);
        DT_LOG_TRACE(logging, "header_line = '" << header_line << "'");
        DT_THROW_IF(!fin, std::runtime_error, "Cannot read map file header!");
        // std::cerr << "DEVEL: TEST 1" << std::endl;

        std::vector<std::string> htokens;
        boost::split(htokens, header_line, boost::is_any_of(","));
        // std::cerr << "DEVEL: TEST 2: htokens.size() = " << htokens.size() << std::endl;

        DT_THROW_IF(htokens.size() != 9, std::logic_error, "Invalid header line format!");
        // std::cerr << "DEVEL: TEST 3" << std::endl;
        nx = boost::lexical_cast<unsigned int>(htokens[0]);
        DT_LOG_TRACE(logging, " nx = [" << nx << "]");
        ny = boost::lexical_cast<unsigned int>(htokens[1]);
        DT_LOG_TRACE(logging, " ny = [" << ny << "]");
        nz = boost::lexical_cast<unsigned int>(htokens[2]);
        DT_LOG_TRACE(logging, " nz = [" << nz << "]");

        double x0(0.0), y0(0.0), z0(0.0);
        x0 = boost::lexical_cast<double>(htokens[3]);
        y0 = boost::lexical_cast<double>(htokens[4]);
        z0 = boost::lexical_cast<double>(htokens[5]);
        origin.set(x0 * length_unit,
                   y0 * length_unit,
                   z0 * length_unit);
        dx = boost::lexical_cast<double>(htokens[6]) * length_unit;
        dy = boost::lexical_cast<double>(htokens[7]) * length_unit;
        dz = boost::lexical_cast<double>(htokens[8]) * length_unit;
        if (logging >= datatools::logger::PRIO_DEBUG) {
          dump(std::clog);
        }
      }

      DT_LOG_TRACE(logging, "Loading B  map...");
      {
        // Read map:
        csv_map_0_type::vvvvd & bb = bmap;
        bb.reserve(3);
        for (size_t ax = 0; ax < 3; ax++) {
          if (ax == 0) {
            DT_LOG_TRACE(logging, "Loading Bx table...");
          } else if (ax == 1) {
            DT_LOG_TRACE(logging, "Loading By table...");
          } else if (ax == 2) {
            DT_LOG_TRACE(logging, "Loading Bz table...");
          }
          {
            csv_map_0_type::vvvd dummy;
            bb.push_back(dummy);
          }
          csv_map_0_type::vvvd & bxyz = bb.back();
          bxyz.reserve(nz);
          for (size_t iz = 0; iz < nz; iz++) {
            DT_LOG_TRACE(logging, "  Scanning iz=" << iz);
            {
              csv_map_0_type::vvd dummy;
              bxyz.push_back(dummy);
            }
            csv_map_0_type::vvd & bxy = bxyz.back();
            bxy.reserve(ny);
            for (size_t iy = 0; iy < ny; iy++) {
              DT_LOG_TRACE(logging, "    Scanning iy=" << iy);
              std::string bmap_line;
              safe_getline(fin, bmap_line);
              DT_LOG_TRACE(logging, "    iz=[" << iz << "] iy=[" << iy << "] : B-line = '" << bmap_line << "'");
              std::vector<std::string> btokens;
              boost::split(btokens, bmap_line, boost::is_any_of(","));
              DT_LOG_TRACE(logging, "    btokens.size = [" << btokens.size() << "]");
              DT_THROW_IF(btokens.size() != nx + 3, std::logic_error,
                          "Invalid B-line format!");
              unsigned int axi = boost::lexical_cast<unsigned int>(btokens[0]);
              unsigned int iyi = boost::lexical_cast<unsigned int>(btokens[1]);
              unsigned int izi = boost::lexical_cast<unsigned int>(btokens[2]);
              DT_LOG_TRACE(logging, "    axi=[" << axi << "] iyi=[" << iyi << "] izi=[" << izi << "] ");
              DT_THROW_IF(axi != ax || iyi != iy || izi != iz,
                          std::logic_error,
                          "Invalid B map line format!");
              {
                csv_map_0_type::vd dummy;
                bxy.push_back(dummy);
              }
              csv_map_0_type::vd & xdata = bxy.back();
              xdata.reserve(btokens.size() - 3);
              for (size_t ix = 3; ix < btokens.size(); ix++) {
                xdata.push_back(boost::lexical_cast<int>(btokens[ix]));
              }
            }
          }
        }
      }

      DT_LOG_TRACE_EXITING(logging);
      return;
    }

    int csv_map_0_type::interpolate(const ::geomtools::vector_3d & position_,
                                    ::geomtools::vector_3d & magnetic_field_) const
    {
      double xu = (position_.x() - origin.x()) / dx;
      double yu = (position_.y() - origin.y()) / dy;
      double zu = (position_.z() - origin.z()) / dz;
      DT_LOG_DEBUG(logging, "xu=[" << xu << "] yu=[" << yu << "] zu=[" << zu << "]");
      int ixl = (int) xu;
      int iyl = (int) yu;
      int izl = (int) zu;
      DT_LOG_DEBUG(logging, "ixl=[" << ixl << "] iyl=[" << iyl << "] izl=[" << izl << "]");
      double fx = xu - ixl;
      double fy = yu - iyl;
      double fz = zu - izl;
      double gx = 1.0 - fx;
      double gy = 1.0 - fy;
      double gz = 1.0 - fz;
      DT_LOG_DEBUG(logging, "  fx=[" << fx << "] fy=[" << fy << "] fz=[" << fz << "]");
      DT_LOG_DEBUG(logging, "  gx=[" << gx << "] gy=[" << gy << "] gz=[" << gz << "]");
      const vvvvd & bb = bmap;
      if (ixl >= 0 && ixl < (int)(nx - 1)
          && iyl >= 0 && iyl < (int)(ny - 1)
          && izl >= 0 && izl < (int)(nz - 1) ) {
        DT_LOG_DEBUG(logging, "TEST...");
        for (int ax = 0; ax < 3; ax++) {
          const vvvd & b3d = bb[ax];
          int b000 = b3d[izl][iyl][ixl];
          int b100 = b3d[izl][iyl][ixl+1];
          int b010 = b3d[izl][iyl+1][ixl];
          int b001 = b3d[izl+1][iyl][ixl];
          int b110 = b3d[izl][iyl+1][ixl+1];
          int b011 = b3d[izl+1][iyl+1][ixl];
          int b101 = b3d[izl+1][iyl][ixl+1];
          int b111 = b3d[izl+1][iyl+1][ixl+1];
          DT_LOG_DEBUG(logging, "  b000=[" << b000 << "] b100=[" << b100 << "] b010=[" << b010 << "] b001=[" << b001 << "]");
          DT_LOG_DEBUG(logging, "  b110=[" << b110 << "] b011=[" << b011 << "] b101=[" << b101 << "] b111=[" << b111 << "]");
          double bf00 = gx * b000 + fx * b100;
          double bf10 = gx * b010 + fx * b110;
          double bff0 = gy * bf00 + fy * bf10;
          double bf01 = gx * b001 + fx * b101;
          double bf11 = gx * b011 + fx * b111;
          double bff1 = gy * bf01 + fy * bf11;
          double bfff = gz * bff0 + fz * bff1;
          magnetic_field_[ax] = bfff;
        }
      } else {
        geomtools::invalidate(magnetic_field_);
        return mapped_magnetic_field::STATUS_ERROR;
      }

      return mapped_magnetic_field::STATUS_SUCCESS;
    }

    int csv_map_0_type::compute(const ::geomtools::vector_3d & position_,
                                ::geomtools::vector_3d & magnetic_field_) const
    {
      DT_LOG_DEBUG(logging, "Position = " << position_ / CLHEP::cm << " cm");
      geomtools::invalidate(magnetic_field_);
      // the coordinate system has its origin in the centre of the source foil.
      // X is in the horizontal direction within the foil.
      // Y is in the vertical direction within the foil.
      // Z is perpendicular to X and Y to make a right-handed cartesian system.
      double x = position_.y();
      double y = position_.z();
      double z = position_.x();
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
      ::geomtools::vector_3d bb;
      ::geomtools::vector_3d pos_ppp(std::abs(x), std::abs(y), std::abs(z));
      DT_LOG_DEBUG(logging, "Position = " << pos_ppp / CLHEP::cm << " cm");
      int status = interpolate(pos_ppp, bb);
      if (status == mapped_magnetic_field::STATUS_SUCCESS) {
        DT_LOG_DEBUG(logging, "B-field interpolation success!");
        double b[3];
        for (int ax = 0; ax < 3; ax++) {
          b[ax] = bb[ax] * mx[ax] * my[ax] * mz[ax];
        }
        magnetic_field_[0] = b[2] * mag_field_unit;
        magnetic_field_[1] = b[0] * mag_field_unit;
        magnetic_field_[2] = b[1] * mag_field_unit;
      } else {
        // Position is out of interpolation domain:
        DT_LOG_DEBUG(logging, "B-field interpolation failed!");
      }
      return status;
    }

  }  // end of namespace geometry

}  // end of namespace snemo

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
