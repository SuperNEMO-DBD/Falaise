/* root_utilities.cc
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

// Standard libraries
#include <algorithm>
#include <iomanip>

// This project
#include <EventBrowser/utils/root_utilities.h>
#include <EventBrowser/view/options_manager.h>
#include <EventBrowser/view/style_manager.h>

// Third party:
// - ROOT
#include <TCanvas.h>
#include <TColor.h>
#include <TGFileDialog.h>
#include <TGeoBBox.h>
#include <TGeoCompositeShape.h>
#include <TGeoMatrix.h>
#include <TGeoPcon.h>
#include <TGeoSphere.h>
#include <TGeoTube.h>
// Only with ROOT > 6.16
// #include <TGeoTessellated.h>
// #include <TGeoVector3.h>
#include <TObjArray.h>
#include <TPolyLine3D.h>
#include <TROOT.h>
#include <TSystem.h>
// - Bayeux/datatools
#include <datatools/logger.h>
#include <datatools/units.h>
// - Bayeux/geomtools
#include <geomtools/box.h>
#include <geomtools/cylinder.h>
#include <geomtools/gnuplot_draw.h>
#include <geomtools/i_composite_shape_3d.h>
#include <geomtools/polycone.h>
#include <geomtools/sphere.h>
// #include <geomtools/tessellation.h>

namespace snemo {

namespace visualization {

namespace utils {

root_utilities::TLatex3D::TLatex3D() {
  _x_ = 0.0;
  _y_ = 0.0;
  _z_ = 0.0;
}

root_utilities::TLatex3D::~TLatex3D() = default;

void root_utilities::TLatex3D::SetText(const std::string &text_) {
  _text_ = text_;
  TLatex::SetTitle(text_.c_str());
}

void root_utilities::TLatex3D::SetPadCoordinates(const double x_, const double y_) {
  TLatex::SetText(x_, y_, TLatex::GetTitle());
}

bool root_utilities::TLatex3D::IsEqual(const TObject *object_) const {
  const TLatex3D *latex = (TLatex3D *)object_;

  if (this == object_) {
    return false;
  }
  if (this->GetX() != latex->GetX()) {
    return false;
  }
  if (this->GetY() != latex->GetY()) {
    return false;
  }
  if (this->GetZ() != latex->GetZ()) {
    return false;
  }

  return true;
}

bool root_utilities::g_initialized = false;
size_t root_utilities::g_geo_id = 0;

unsigned int root_utilities::get_line_style(const std::string &key_,
                                            const datatools::properties &property_) {
  unsigned int line = 0;

  if (property_.is_string(key_)) {
    // ROOT name convention (from Rtypes.h header)
    const std::string &name_line = property_.fetch_string(key_);

    if (name_line == "kSolid") {
      line = 1;
    } else if (name_line == "kDashed") {
      line = 2;
    } else if (name_line == "kDotted") {
      line = 3;
    } else if (name_line == "kDashDotted") {
      line = 4;
    } else
      DT_LOG_ERROR(view::options_manager::get_instance().get_logging_priority(),
                   "'" << name_line << "' not supported");
  } else if (property_.is_integer(key_)) {
    line = property_.fetch_integer(key_);
  } else {
    DT_LOG_ERROR(view::options_manager::get_instance().get_logging_priority(),
                 "Format of line not supported");
  }
  return line;
}

unsigned int root_utilities::get_color_value(const std::string &key_,
                                             const datatools::properties &property_) {
  unsigned int color = 0;
  if (property_.is_vector(key_)) {
    // RGB case
    if (property_.size(key_) != 3) {
      DT_LOG_ERROR(view::options_manager::get_instance().get_logging_priority(),
                   "'" << key_ << "' must have 3 elements for each RGB values");
    } else {
      const int red = property_.fetch_integer_vector(key_, 0);
      const int green = property_.fetch_integer_vector(key_, 1);
      const int blue = property_.fetch_integer_vector(key_, 2);

      color = TColor::GetColor(red, green, blue);
    }
  } else if (property_.is_string(key_)) {
    const std::string &name_color = property_.fetch_string(key_);
    color = get_color_value(name_color);
  } else if (property_.is_integer(key_)) {
    color = property_.fetch_integer(key_);
  } else {
    DT_LOG_ERROR(view::options_manager::get_instance().get_logging_priority(),
                 "Format of color not supported");
  }
  return color;
}

unsigned int root_utilities::get_color_value(const std::string &color_) {
  unsigned int color = 0;
  if (color_[0] == '#') {
    if (color_.size() != 7) {
      DT_LOG_ERROR(view::options_manager::get_instance().get_logging_priority(),
                   "Color '" << color_ << "' not supported");
      return color;
    }
    color = TColor::GetColor(color_.c_str());
  } else {
    // ROOT name convention (from Rtypes.h header)
    std::istringstream iss(color_);
    std::string token;
    while (iss >> token) {
      if (token == "kWhite") {
        color += 0;
      } else if (token == "kBlack") {
        color += 1;
      } else if (token == "kGray") {
        color += 920;
      } else if (token == "kRed") {
        color += 632;
      } else if (token == "kGreen") {
        color += 416;
      } else if (token == "kBlue") {
        color += 600;
      } else if (token == "kYellow") {
        color += 400;
      } else if (token == "kMagenta") {
        color += 616;
      } else if (token == "kCyan") {
        color += 432;
      } else if (token == "kOrange") {
        color += 800;
      } else if (token == "kSpring") {
        color += 820;
      } else if (token == "kTeal") {
        color += 840;
      } else if (token == "kAzure") {
        color += 860;
      } else if (token == "kViolet") {
        color += 880;
      } else if (token == "kPink") {
        color += 900;
      } else if (token == "+") {
        unsigned int increment;
        iss >> increment;
        color += increment;
      } else if (token == "-") {
        unsigned int decrement;
        iss >> decrement;
        color -= decrement;
      } else {
        DT_LOG_ERROR(view::options_manager::get_instance().get_logging_priority(),
                     "'" << token << "' not supported");
      }
    }
  }
  return color;
}

unsigned int root_utilities::get_random_color() {
  if (!g_initialized) {
    srand(time(nullptr));
    g_initialized = true;
  }

  const int r = rand() % 255;
  const int g = rand() % 255;
  const int b = rand() % 255;

  return TColor::GetColor(r, g, b);
}

unsigned int root_utilities::get_fade_color_from(const unsigned int color_, const double ratio_) {
  DT_THROW_IF(ratio_ < 0 || ratio_ > 1.0, std::domain_error,
              "Fading ratio must be greater than 0 and lower than 1");

  TColor *root_color = gROOT->GetColor(color_);
  float rr, gg, bb;
  root_color->GetRGB(rr, gg, bb);
  rr *= ratio_;
  gg *= ratio_;
  bb *= ratio_;
  return TColor::GetColor(rr, gg, bb);
}

std::string root_utilities::get_hex_color(const unsigned int color_) {
  const long pixel = TColor::Number2Pixel(color_);
  return TColor::PixelAsHexString(pixel);
}

void root_utilities::write_rgb_color(std::ostream &out_, const unsigned int color_) {
  TColor *root_color = gROOT->GetColor(color_);

  float rr, gg, bb;
  root_color->GetRGB(rr, gg, bb);

  const int r = static_cast<int>(rr * 255);
  const int g = static_cast<int>(gg * 255);
  const int b = static_cast<int>(bb * 255);

  out_ << std::setw(3) << r << " " << std::setw(3) << g << " " << std::setw(3) << b;
}

void root_utilities::get_prettified_time(std::ostream &out_, const double time_,
                                         const double sigma_, const bool latex_) {
  std::string unit_id = "ps";
  if (time_ > 1e12 * CLHEP::picosecond) {
    unit_id = "s";
  } else if (time_ > 1e9 * CLHEP::picosecond) {
    unit_id = "ms";
  } else if (time_ > 1e6 * CLHEP::picosecond) {
    unit_id = "us";
  } else if (time_ > 1e3 * CLHEP::picosecond) {
    unit_id = "ns";
  }
  const double unit = datatools::units::get_time_unit_from(unit_id);
  if (datatools::is_valid(sigma_)) {
    out_ << time_ / unit;
    if (latex_) {
      out_ << " #pm ";
    } else {
      out_ << " +/- ";
    }
    out_ << sigma_ / unit << " " << unit_id;
  } else {
    out_ << time_ / unit << " " << unit_id;
  }
}

void root_utilities::get_prettified_energy(std::ostream &out_, const double energy_,
                                           const double sigma_, const bool latex_) {
  std::string unit_id = "meV";
  if (energy_ > 1e9 * CLHEP::eV) {
    unit_id = "GeV";
  } else if (energy_ > 1e4 * CLHEP::eV) {
    unit_id = "MeV";
  } else if (energy_ > 1e3 * CLHEP::eV) {
    unit_id = "keV";
  } else if (energy_ > 1e0 * CLHEP::eV) {
    unit_id = "eV";
  }
  const double unit = datatools::units::get_energy_unit_from(unit_id);
  if (datatools::is_valid(sigma_)) {
    out_ << energy_ / unit;
    if (latex_) {
      out_ << " #pm ";
    } else {
      out_ << " +/- ";
    }
    out_ << sigma_ / unit << " " << unit_id;
  } else {
    out_ << energy_ / unit << " " << unit_id;
  }
}

bool root_utilities::save_view_as(TCanvas *canvas_, const std::string &filename_) {
  if (canvas_ == nullptr) {
    DT_LOG_ERROR(view::options_manager::get_instance().get_logging_priority(),
                 "No canvas given as argument");
    return false;
  }

  if (!filename_.empty()) {
    canvas_->SaveAs(filename_.c_str());

    DT_LOG_NOTICE(view::options_manager::get_instance().get_logging_priority(),
                  filename_ << " saved in " << gSystem->Getenv("PWD"));
    return true;
  }

  view::style_manager &style_mgr = view::style_manager::get_instance();

  const size_t nbr_file_format = 26;
  const char *save_as_types[nbr_file_format] = {"PostScript",
                                                "*.ps",
                                                "Encapsulated PostScript",
                                                "*.eps",
                                                "Portable Document Format",
                                                "*.pdf",
                                                "Scalable Vector Graphics",
                                                "*.svg",
                                                "Graphics Interchange Format",
                                                "*.gif",
                                                "Compressed Graphics",
                                                "*.jpg",
                                                "Portable Network Graphics",
                                                "*.png",
                                                "Tagged Image File Format",
                                                "*.tiff",
                                                "ROOT files",
                                                "*.root",
                                                "Extensible Markup Language",
                                                "*.xml",
                                                "X PixMap",
                                                "*.xpm",
                                                "eXperimental Computing Facility",
                                                "*.xcf",
                                                nullptr,
                                                nullptr};

  std::list<std::string> file_format;
  for (size_t i = 0; i < nbr_file_format / 2 - 1; ++i) {
    std::string dummy_one;
    dummy_one.assign(save_as_types[i * 2 + 1]);
    file_format.push_back(dummy_one);
  }

  auto it = std::find(file_format.begin(), file_format.end(),
                      std::string("*" + style_mgr.get_save_extension()));

  int typeidx = std::distance(file_format.begin(), it) * 2;
  bool overwr = false;

  TGFileInfo file_info;
  file_info.fFileTypes = save_as_types;
  file_info.fIniDir = StrDup(style_mgr.get_save_directory().c_str());
  file_info.fFileTypeIdx = typeidx;
  file_info.fOverwrite = overwr;

  new TGFileDialog(gClient->GetRoot(), nullptr, kFDSave, &file_info);
  if (file_info.fFilename == nullptr) {
    return false;
  }

  std::string file_name = file_info.fFilename;
  std::string file_type = file_info.fFileTypes[file_info.fFileTypeIdx + 1];
  std::string directory = file_info.fIniDir;

  // remove asterisk
  file_type.erase(0, 1);

  // if extension is missing to filename
  if (!TString(file_name).EndsWith(file_type.c_str())) {
    file_name += file_type;
  }

  // save previous value
  style_mgr.set_save_directory(directory);
  style_mgr.set_save_extension(file_type);

  canvas_->SaveAs(file_name.c_str());

  DT_LOG_NOTICE(view::options_manager::get_instance().get_logging_priority(),
                file_name << " saved");
  return true;
}

TGeoShape *root_utilities::get_geo_shape(const geomtools::i_shape_3d &shape_3d_) {
  datatools::logger::priority local_priority = datatools::logger::PRIO_WARNING;

  TGeoShape *shape = nullptr;

  const std::string &shape_name = shape_3d_.get_shape_name();

  DT_LOG_TRACE(local_priority, "Shape_name = " << shape_name);

  if (shape_3d_.is_composite()) {
    const auto &sub_shape = dynamic_cast<const geomtools::i_composite_shape_3d &>(shape_3d_);

    const geomtools::i_composite_shape_3d::shape_type &s1 = sub_shape.get_shape1();
    const geomtools::i_composite_shape_3d::shape_type &s2 = sub_shape.get_shape2();
    const geomtools::i_shape_3d &sh1 = s1.get_shape();
    const geomtools::i_shape_3d &sh2 = s2.get_shape();

    DT_LOG_TRACE(local_priority, "shape_name1 = " << sh1.get_shape_name() << " "
                                                  << "shape_name2 = " << sh2.get_shape_name());

    TGeoShape *geo_shape1 = get_geo_shape(sh1);
    geo_shape1->SetName(get_unique_geo_name().c_str());

    TGeoCombiTrans *ct1 = new TGeoCombiTrans(get_unique_geo_name().c_str());
    ct1->RegisterYourself();
    {
      const geomtools::placement &g_placement = s1.get_placement();
      const geomtools::vector_3d &g_translation = g_placement.get_translation();
      ct1->SetTranslation(g_translation.x(), g_translation.y(), g_translation.z());

      DT_LOG_TRACE(local_priority, "geo_shape_name = " << geo_shape1->GetName());
      DT_LOG_TRACE(local_priority, "(x, y, z) = (" << g_translation.x() << ", " << g_translation.y()
                                                   << ", " << g_translation.z() << ")");
      DT_LOG_TRACE(local_priority, "(phi, theta, psi) = (" << g_placement.get_phi() << ", "
                                                           << g_placement.get_theta() << ", "
                                                           << g_placement.get_delta() << ")");
    }

    TGeoShape *geo_shape2 = get_geo_shape(sh2);
    geo_shape2->SetName(get_unique_geo_name().c_str());

    TGeoCombiTrans *ct2 = new TGeoCombiTrans(get_unique_geo_name().c_str());
    ct2->RegisterYourself();
    {
      const geomtools::placement &g_placement = s2.get_placement();
      const geomtools::vector_3d &g_translation = g_placement.get_translation();
      ct2->SetTranslation(g_translation.x(), g_translation.y(), g_translation.z());

      DT_LOG_TRACE(local_priority, "geo_shape_name = " << geo_shape2->GetName());
      DT_LOG_TRACE(local_priority, "(x, y, z) = (" << g_translation.x() << ", " << g_translation.y()
                                                   << ", " << g_translation.z() << ")");
      DT_LOG_TRACE(local_priority, "(phi, theta, psi) = (" << g_placement.get_phi() << ", "
                                                           << g_placement.get_theta() << ", "
                                                           << g_placement.get_delta() << ")");
    }

    bool keep_it_simple = !view::style_manager::get_instance().use_opengl();
    ;
    if (keep_it_simple) {
      if (!sh1.is_composite()) {
        return geo_shape1;
      }
      { return geo_shape2; }
    }

    std::ostringstream oss1, oss2;
    oss1 << "(" << geo_shape1->GetName() << ":" << ct1->GetName() << ")";
    oss2 << "(" << geo_shape2->GetName() << ":" << ct2->GetName() << ")";
    std::string node_expr;

    if (shape_name == "subtraction_3d") {
      node_expr = oss1.str() + "-" + oss2.str();
    } else if (shape_name == "intersection_3d") {
      node_expr = oss1.str() + "*" + oss2.str();
    } else if (shape_name == "union_3d") {
      node_expr = oss1.str() + "+" + oss2.str();
    } else {
      DT_THROW_IF(true, std::logic_error,
                  "Composite shape '" << shape_name << "' not yet implemented !");
    }

    shape = new TGeoCompositeShape("composite_shape", node_expr.c_str());
  } else if (shape_name == "box") {
    const auto &mbox = dynamic_cast<const geomtools::box &>(shape_3d_);

    shape = new TGeoBBox(mbox.get_x() / 2., mbox.get_y() / 2., mbox.get_z() / 2.);
  } else if (shape_name == "cylinder") {
    const auto &mcylinder = dynamic_cast<const geomtools::cylinder &>(shape_3d_);

    shape = new TGeoTube(0.0, mcylinder.get_radius(), mcylinder.get_z());
  } else if (shape_name == "sphere") {
    const auto &msphere = dynamic_cast<const geomtools::sphere &>(shape_3d_);

    shape = new TGeoSphere(0.0, msphere.get_radius());
  } else if (shape_name == "polycone") {
    const auto &mpolycone = dynamic_cast<const geomtools::polycone &>(shape_3d_);

    auto *polycone = new TGeoPcon(0, 360, mpolycone.points().size());

    size_t i_section = 0;
    for (auto it_section = mpolycone.points().begin(); it_section != mpolycone.points().end();
         ++it_section, ++i_section) {
      polycone->DefineSection(i_section,
                              it_section->first,        // z value
                              it_section->second.rmin,  // rmin value
                              it_section->second.rmax   // rmax value
      );
    }
    shape = dynamic_cast<TGeoShape *>(polycone);
    /*
  } else if (shape_name == "tessellated") {
    const auto &mtessella = dynamic_cast<const geomtools::tessellated_solid &>(shape_3d_);

    auto *tessella = new TGeoTessellated;
    const geomtools:tessellated_solid::facets_col_type & theFacets = mtessella.facets();
    for (const auto & facetIter : theFacets) {
      unsigned int f34Index = facetIter.first;
      geomtools::tessellated_solid::facet34 f34 = facetIter.second;
      
    }
    shape = dynamic_cast<TGeoShape *>(tessella);
    */
  } else {
    DT_LOG_ERROR(view::options_manager::get_instance().get_logging_priority(),
                 shape_name << "' not yet implemented !");
    return nullptr;
  }

  return shape;
}

std::string root_utilities::get_unique_geo_name() {
  std::ostringstream oss;
  oss << "unique_name_" << g_geo_id++;
  return oss.str();
}
  

TObjArray *root_utilities::wires_to_root_draw(const geomtools::vector_3d &position_,
                                              const geomtools::rotation_3d &rotation_,
                                              const geomtools::i_shape_3d &shape_) {
  TObjArray *obj_array = nullptr;

  
  if (shape_.has_wires_drawer()) {
    // std::cerr << "*** devel *** root_utilities::wires_to_root_draw: '" << shape_.get_shape_name() << "'\n";
    obj_array = new TObjArray(100);

    // Retrieve wires drawer
    const geomtools::i_wires_3d_rendering &a_drawer = shape_.get_wires_drawer();
    geomtools::wires_type wires;
    a_drawer.generate_wires(wires, position_, rotation_);

    for (geomtools::wires_type::const_iterator i = wires.begin(); i != wires.end(); ++i) {
      auto *pl3D = new TPolyLine3D;
      obj_array->Add(pl3D);
      const geomtools::polyline_type &pl = *i;
      size_t k = 0;
      for (const auto &v3d : pl) {
        pl3D->SetPoint(k++, v3d.x(), v3d.y(), v3d.z());
      }
    }
  } else {
    DT_LOG_WARNING(view::options_manager::get_instance().get_logging_priority(),
                   "No 'i_wires_3d_rendering' has been defined in shape named '" << shape_.get_shape_name() << "'!");
    // std::cerr << "*** devel *** root_utilities::wires_to_root_draw: no wire rendering for shape '" << shape_.get_shape_name() << "'\n";
  }

  return obj_array;
}

}  // end of namespace utils

}  // end of namespace visualization

}  // end of namespace snemo

// end of root_utilities.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
