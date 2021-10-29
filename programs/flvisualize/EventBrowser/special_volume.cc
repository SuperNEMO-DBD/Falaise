/* special_volume.cc
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

#include <EventBrowser/detector/special_volume.h>
#include <EventBrowser/utils/root_utilities.h>

#include <geomtools/geom_info.h>
#include <geomtools/tessellation.h>
#include <geomtools/i_wires_drawer.h>

#include <TObjArray.h>
#include <TPolyLine3D.h>

namespace snemo {

namespace visualization {

namespace detector {

/// \brief Special wires 3D rendering
struct tessellated_wires_drawer
  : public geomtools::i_wires_drawer<geomtools::tessellated_solid>
{
  tessellated_wires_drawer(const geomtools::tessellated_solid & tessella_)
    : geomtools::i_wires_drawer<geomtools::tessellated_solid>(tessella_)
  {
  }
  
  ~tessellated_wires_drawer() = default;

  void generate_wires_self(geomtools::wires_type & wires_, uint32_t options_ = 0) const override
  {
    get().generate_wires_self(wires_, options_);
  }
  
};

bool special_volume::has_objects() const { return _objects_ != nullptr; }

special_volume::special_volume(const std::string &name_, const std::string &category_)
    : i_root_volume(name_, category_), _objects_(nullptr) {
  // std::cerr << "*** devel *** special_volume::ctor : name = '"
  //           << name_ << "' category = '"
  //           << category_ << "'\n";
  _type = "special";
  _composite = false;
}

special_volume::~special_volume() { this->reset(); }

void special_volume::_construct(const geomtools::i_shape_3d &shape_3d_) {
  // std::cerr << "*** devel *** special_volume::_construct : for shape type = '"
  //           << shape_3d_.get_shape_name() << "'\n";

  if (shape_3d_.get_shape_name() == "tessellated") {
    // shape_3d_.tree_dump(std::cerr, "special_volume::_construct: tessellated: ", "*** devel *** ");
    // std::cerr << "*** devel *** special_volume::_construct : installing a 3D-wires renderer in shape = '"
    //           << shape_3d_.get_shape_name() << "'\n";
    
    // Remark: This is not that elegant for we would prefer to use a drawer not embedded to the shape itself (break shape's constness)
    _shape_ = &shape_3d_;
    // Installing a dedicated wire drawer associated to this shape:
    _wires_drawer_ = new tessellated_wires_drawer(dynamic_cast<const geomtools::tessellated_solid&>(shape_3d_));
    const geomtools::i_object_3d & constObj = dynamic_cast<const geomtools::i_object_3d&>(shape_3d_);
    geomtools::i_object_3d & obj = const_cast<geomtools::i_object_3d &>(constObj);
    obj.set_wires_drawer(*_wires_drawer_);
  } 
  
  _objects_ = utils::root_utilities::wires_to_root_draw(get_placement().get_translation(),
                                                        get_placement().get_rotation(),
                                                        shape_3d_);
}

void special_volume::clear() {
  if (!has_objects()) {
    return;
  }

  TObjArrayIter iter(_objects_);
  while (iter.Next() != nullptr) {
    TObject *a_object = *iter;

    if (a_object->IsA() == TPolyLine3D::Class()) {
      auto *pl3d = dynamic_cast<TPolyLine3D *>(a_object);
      pl3d->SetLineWidth(1);
      pl3d->SetLineColor(_color);
    }
  }
}

void special_volume::reset() {
  if (_wires_drawer_ != nullptr) {
    // Detach the wires drawer from the referenced shape
    if (_shape_ != nullptr) {
      const_cast<geomtools::i_shape_3d *>(_shape_)->reset_wires_drawer();
    }
    // Then destroy it
    delete _wires_drawer_;
    _wires_drawer_ = nullptr;
  }
  _objects_->Delete();
  _objects_ = nullptr;
  _shape_ = nullptr;
  _initialized = false;
}

void special_volume::_highlight() {
  _color = _highlight_color;
  this->clear();
}

void special_volume::draw() const {
  // std::cerr << "*** devel *** special_volume::draw() : name='" << get_name() << "'\n";
  if (has_objects()) {
    // std::cerr << "*** devel *** special_volume::draw() : " << "drawing objects..." << '\n';
    _objects_->Draw();
  } else {
    // std::cerr << "*** devel *** special_volume::draw() : " << "no object to draw." << '\n';
  }
}

void special_volume::tree_dump(std::ostream &out_, const std::string &title_,
                               const std::string &indent_, bool inherit_) const {
  std::string indent;
  if (!indent_.empty()) {
    indent = indent_;
  }
  i_root_volume::tree_dump(out_, title_, indent_, true);

  out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_);
  if (has_objects()) {
    out_ << "Attached polylines : " << _objects_->GetEntriesFast() << std::endl;
  } else {
    out_ << "No attached polylines" << std::endl;
  }
}

void special_volume::dump() const {
  this->tree_dump(std::clog, "snemo::visualization::detector::special_volume");
}

}  // end of namespace detector

}  // end of namespace visualization

}  // end of namespace snemo

// end of special_volume.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
