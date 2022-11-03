// falaise/snemo/geometry/gg_locator.cc
/* Copyright (C) 2011-2014 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
#include <falaise/snemo/geometry/gg_locator.h>

#include "private/categories.h"

// Standard library:
#include <stdexcept>

// Third party
// - Bayeux/datatools :
#include <datatools/units.h>
#include <datatools/utils.h>
#include <datatools/version_id.h>
// - Bayeux/geomtools :
#include <geomtools/box.h>
#include <geomtools/cylinder.h>
#include <geomtools/manager.h>

namespace snemo {

namespace geometry {
gg_locator::gg_locator() { set_defaults_(); }

// Constructor:
gg_locator::gg_locator(uint32_t moduleID, const geomtools::manager &geoMgr,
                       const falaise::property_set &ps) {
  set_defaults_();
  set_geo_manager(geoMgr);
  setModuleNumber(moduleID);
  initialize(ps);
}

void gg_locator::reset() { set_defaults_(); }

bool gg_locator::is_initialized() const { return isInitialized_; }

void gg_locator::initialize(const datatools::properties &ps) {
  base_locator::_basic_initialize(ps);
  DT_THROW_IF(moduleNumber_ == geomtools::geom_id::INVALID_ADDRESS, std::logic_error,
              "Missing module number ! Use the 'setModuleNumber' method before !");
  construct_();
  isInitialized_ = true;
}

// ----- ACCESSORS AND CALCULATIONS -----

uint32_t gg_locator::cellGIDType() const
{
  return cellGIDType_;
}
  
void gg_locator::setModuleNumber(uint32_t number) { moduleNumber_ = number; }

uint32_t gg_locator::getModuleNumber() const { return moduleNumber_; }

double gg_locator::cellDiameter() const { return cellBoxShape_->get_x(); }

double gg_locator::cellRadius() const { return 0.5 * cellBoxShape_->get_x(); }

double gg_locator::cellLength() const { return cellBoxShape_->get_z(); }

double gg_locator::anodeWireLength() const { return anodeWireLength_; }

double gg_locator::anodeWireDiameter() const { return anodeWireDiameter_; }

double gg_locator::fieldWireLength() const { return fieldWireLength_; }

double gg_locator::fieldWireDiameter() const { return fieldWireDiameter_; }

bool gg_locator::hasSubmodules(uint32_t side) const {
  DT_THROW_IF(side >= utils::NSIDES, std::logic_error, "Side number is invalid !");
  return submodules_[side];
}

size_t gg_locator::numberOfSides() const { return utils::NSIDES; }

size_t gg_locator::numberOfLayers(uint32_t side) const {
  if (side == side_t::BACK) {
    return backCellX_.size();
  }

  if (side == side_t::FRONT) {
    return frontCellX_.size();
  }

  DT_THROW(std::logic_error, "Invalid side number " << side << " !");
}

size_t gg_locator::numberOfRows(uint32_t side) const {
  if (side == side_t::BACK) {
    return backCellY_.size();
  }

  if (side == side_t::FRONT) {
    return frontCellY_.size();
  }

  DT_THROW(std::logic_error, "Invalid side number " << side << " !");
}

double gg_locator::getXCoordOfLayer(uint32_t side, uint32_t layer) const {
  DT_THROW_IF(side > 1, std::logic_error, "Invalid side number (" << side << "> 1)!");
  if (side == 0) {
    DT_THROW_IF(layer >= backCellX_.size(), std::logic_error,
                "Invalid layer number (" << layer << ">" << backCellX_.size() - 1 << ")!");
    return backCellX_[layer];
  }
  DT_THROW_IF(layer >= frontCellX_.size(), std::logic_error,
              "Invalid layer number (" << layer << ">" << frontCellX_.size() - 1 << ")!");
  return frontCellX_[layer];
}

double gg_locator::getYCoordOfRow(uint32_t side, uint32_t row) const {
  DT_THROW_IF(side > 1, std::logic_error, "Invalid side number (" << side << "> 1)!");
  if (side == 0) {
    DT_THROW_IF(row >= backCellY_.size(), std::logic_error,
                "Invalid row number (" << row << ">" << backCellY_.size() - 1 << ")!");
    return backCellY_[row];
  }
  DT_THROW_IF(row >= frontCellY_.size(), std::logic_error,
              "Invalid row number (" << row << ">" << frontCellY_.size() - 1 << ")!");
  return frontCellY_[row];
}

geomtools::vector_3d gg_locator::getCellPosition(uint32_t side, uint32_t layer,
                                                 uint32_t row) const {
  return geomtools::vector_3d{getXCoordOfLayer(side, layer), getYCoordOfRow(side, row), 0.0};
}

geomtools::vector_3d gg_locator::getCellPosition(const geomtools::geom_id &gid) const {
  DT_THROW_IF(
      gid.get(moduleAddressIndex_) != moduleNumber_, std::logic_error,
      "Invalid module number (" << gid.get(moduleAddressIndex_) << "!=" << moduleNumber_ << ")!");
  return getCellPosition(gid.get(sideAddressIndex_), gid.get(layerAddressIndex_),
                         gid.get(rowAddressIndex_));
}

size_t gg_locator::countNeighbours(const geomtools::geom_id &gid, bool acrossFoil) const {
  DT_THROW_IF(
      gid.get(moduleAddressIndex_) != moduleNumber_, std::logic_error,
      "Invalid module number (" << gid.get(moduleAddressIndex_) << "!=" << moduleNumber_ << ")!");
  return countNeighbours(gid.get(sideAddressIndex_), gid.get(layerAddressIndex_),
                         gid.get(rowAddressIndex_), acrossFoil);
}

size_t gg_locator::countNeighbours(uint32_t side_, uint32_t layer_, uint32_t row_,
                                   bool acrossFoil) const {
  bool corner = false;
  bool side = false;
  size_t plus = 0;
  if (side_ == side_t::BACK) {
    if ((layer_ == 0) || (layer_ == backCellX_.size() - 1)) {
      if ((row_ == 0) || (row_ == backCellY_.size() - 1)) {
        corner = true;
        if (acrossFoil && (layer_ == 0)) {
          plus = 2;
        }
      } else {
        if (acrossFoil && (layer_ == 0)) {
          plus = 3;
        }
        side = true;
      }
    }
  }
  if (side_ == side_t::FRONT) {
    if ((layer_ == 0) || (layer_ == frontCellX_.size() - 1)) {
      if ((row_ == 0) || (row_ == frontCellY_.size() - 1)) {
        corner = true;
        if (acrossFoil && (layer_ == 0)) {
          plus = 2;
        }
      } else {
        if (acrossFoil && (layer_ == 0)) {
          plus = 3;
        }
        side = true;
      }
    }
  }
  if (corner) {
    return 3 + plus;
  }
  if (side) {
    return 5 + plus;
  }
  return 8;
}

std::vector<geomtools::geom_id> gg_locator::getNeighbourGIDs(const geomtools::geom_id &gid,
                                                             bool acrossFoil) const {
  DT_THROW_IF(
      gid.get(moduleAddressIndex_) != moduleNumber_, std::logic_error,
      "Invalid module number (" << gid.get(moduleAddressIndex_) << "!=" << moduleNumber_ << ")!");
  return getNeighbourGIDs(gid.get(sideAddressIndex_), gid.get(layerAddressIndex_),
                          gid.get(rowAddressIndex_), acrossFoil);
}

std::vector<geomtools::geom_id> gg_locator::getNeighbourGIDs(uint32_t side_, uint32_t layer_,
                                                             uint32_t row_, bool acrossFoil) const {
  DT_THROW_IF(side_ != side_t::BACK && side_ != side_t::FRONT, std::logic_error,
              "Invalid side number (" << side_ << "> 1)!");

  std::vector<geomtools::geom_id> ids_;
  ids_.reserve(8);

  // back
  if (side_ == side_t::BACK) {
    DT_THROW_IF(layer_ >= backCellX_.size(), std::logic_error,
                "Invalid layer number (" << layer_ << ">" << backCellX_.size() - 1 << ")!");
    DT_THROW_IF(row_ >= backCellY_.size(), std::logic_error,
                "Invalid row number (" << row_ << ">" << backCellY_.size() - 1 << ")!");
    if (layer_ > 0) {
      /*  L+1 L L-1
       *  [ ][ ][ ]
       *  [ ][.][x]
       *  [ ][ ][ ]
       */
      ids_.emplace_back(cellGIDType_, moduleNumber_, side_, layer_ - 1, row_);
    }
    if (layer_ < (backCellX_.size() - 1)) {
      /*  L+1 L L-1
       *  [ ][ ][ ] R+1
       *  [x][.][ ] R
       *  [ ][ ][ ] R-1
       */
      ids_.emplace_back(cellGIDType_, moduleNumber_, side_, layer_ + 1, row_);
    }
    if (row_ > 0) {
      /*  L+1 L L-1
       *  [ ][ ][ ] R+1
       *  [ ][.][ ] R
       *  [ ][x][ ] R-1
       */
      ids_.emplace_back(cellGIDType_, moduleNumber_, side_, layer_, row_ - 1);
    }
    if (row_ < (backCellY_.size() - 1)) {
      /*  L+1 L L-1
       *  [ ][x][ ] R+1
       *  [ ][.][ ] R
       *  [ ][ ][ ] R-1
       */
      ids_.emplace_back(cellGIDType_, moduleNumber_, side_, layer_, row_ + 1);
    }

    if ((layer_ < (backCellX_.size() - 1)) && (row_ > 0)) {
      /*  L+1 L L-1
       *  [ ][ ][ ] R+1
       *  [ ][.][ ] R
       *  [x][ ][ ] R-1
       */
      ids_.emplace_back(cellGIDType_, moduleNumber_, side_, layer_ + 1, row_ - 1);
    }
    if ((layer_ < (backCellX_.size() - 1)) && (row_ < (backCellY_.size() - 1))) {
      /*  L+1 L L-1
       *  [x][ ][ ] R+1
       *  [ ][.][ ] R
       *  [ ][ ][ ] R-1
       */
      ids_.emplace_back(cellGIDType_, moduleNumber_, side_, layer_ + 1, row_ + 1);
    }
    if ((layer_ > 0) && (row_ > 0)) {
      /*  L+1 L L-1
       *  [ ][ ][ ] R+1
       *  [ ][.][ ] R
       *  [ ][ ][x] R-1
       */
      ids_.emplace_back(cellGIDType_, moduleNumber_, side_, layer_ - 1, row_ - 1);
    }
    if ((layer_ > 0) && (row_ < (backCellY_.size() - 1))) {
      /*  L+1 L L-1
       *  [ ][ ][x] R+1
       *  [ ][.][ ] R
       *  [ ][ ][ ] R-1
       */
      ids_.emplace_back(cellGIDType_, moduleNumber_, side_, layer_ - 1, row_ + 1);
    }
    if ((layer_ == 0) && (row_ > 0) && acrossFoil) {
      /*   1  0     0
       *  [ ][ ] | [ ] R+1
       *  [ ][.] | [ ] R
       *  [ ][ ] | [x] R-1
       */
      ids_.emplace_back(cellGIDType_, moduleNumber_, side_ + 1, 0, row_ - 1);
    }
    if ((layer_ == 0) && acrossFoil) {
      /*   1  0     0
       *  [ ][ ] | [ ] R+1
       *  [ ][.] | [x] R
       *  [ ][ ] | [ ] R-1
       */
      ids_.emplace_back(cellGIDType_, moduleNumber_, side_ + 1, 0, row_);
    }
    if ((layer_ == 0) && (row_ < (backCellY_.size() - 1)) && acrossFoil) {
      /*   1  0     0
       *  [ ][ ] | [x] R+1
       *  [ ][.] | [ ] R
       *  [ ][ ] | [ ] R-1
       */
      ids_.emplace_back(cellGIDType_, moduleNumber_, side_ + 1, 0, row_ + 1);
    }
  }
  // front:
  if (side_ == side_t::FRONT) {
    DT_THROW_IF(layer_ >= frontCellX_.size(), std::logic_error,
                "Invalid layer number (" << layer_ << ">" << frontCellX_.size() - 1 << ")!");
    DT_THROW_IF(row_ >= frontCellY_.size(), std::logic_error,
                "Invalid row number (" << row_ << ">" << frontCellY_.size() - 1 << ")!");
    if (layer_ > 0) {
      /*  L-1 L L+1
       *  [ ][ ][ ]
       *  [x][.][ ]
       *  [ ][ ][ ]
       */
      ids_.emplace_back(cellGIDType_, moduleNumber_, side_, layer_ - 1, row_);
    }
    if (layer_ < (frontCellX_.size() - 1)) {
      /*  L-1 L L+1
       *  [ ][ ][ ] R+1
       *  [ ][.][x] R
       *  [ ][ ][ ] R-1
       */
      ids_.emplace_back(cellGIDType_, moduleNumber_, side_, layer_ + 1, row_);
    }
    if (row_ > 0) {
      /*  L-1 L L+1
       *  [ ][ ][ ] R+1
       *  [ ][.][ ] R
       *  [ ][x][ ] R-1
       */
      ids_.emplace_back(cellGIDType_, moduleNumber_, side_, layer_, row_ - 1);
    }
    if (row_ < (frontCellY_.size() - 1)) {
      /*  L-1 L L+1
       *  [ ][x][ ] R+1
       *  [ ][.][ ] R
       *  [ ][ ][ ] R-1
       */
      ids_.emplace_back(cellGIDType_, moduleNumber_, side_, layer_, row_ + 1);
    }

    if ((layer_ < (frontCellX_.size() - 1)) && (row_ > 0)) {
      /*  L-1 L L+1
       *  [ ][ ][ ] R+1
       *  [ ][.][ ] R
       *  [ ][ ][x] R-1
       */
      ids_.emplace_back(cellGIDType_, moduleNumber_, side_, layer_ + 1, row_ - 1);
    }
    if ((layer_ < (frontCellX_.size() - 1)) && (row_ < (frontCellY_.size() - 1))) {
      /*  L-1 L L+1
       *  [ ][ ][x] R+1
       *  [ ][.][ ] R
       *  [ ][ ][ ] R-1
       */
      ids_.emplace_back(cellGIDType_, moduleNumber_, side_, layer_ + 1, row_ + 1);
    }
    if ((layer_ > 0) && (row_ > 0)) {
      /*  L-1 L L+1
       *  [ ][ ][ ] R+1
       *  [ ][.][ ] R
       *  [x][ ][ ] R-1
       */
      ids_.emplace_back(cellGIDType_, moduleNumber_, side_, layer_ - 1, row_ - 1);
    }
    if ((layer_ > 0) && (row_ < (frontCellY_.size() - 1))) {
      /*  L-1 L L+1
       *  [x][ ][ ] R+1
       *  [ ][.][ ] R
       *  [ ][ ][ ] R-1
       */
      ids_.emplace_back(cellGIDType_, moduleNumber_, side_, layer_ - 1, row_ + 1);
    }
    if ((layer_ == 0) && (row_ > 0) && acrossFoil) {
      /*   0     0  1
       *  [ ] | [ ][ ] R+1
       *  [ ] | [.][ ] R
       *  [x] | [ ]| [x] R-1
       */
      ids_.emplace_back(cellGIDType_, moduleNumber_, side_ - 1, 0, row_ - 1);
    }
    if ((layer_ == 0) && acrossFoil) {
      /*   0     0  1
       *  [ ] | [ ][ ] R+1
       *  [x] | [.][ ] R
       *  [ ] | [ ]| [x] R-1
       */
      ids_.emplace_back(cellGIDType_, moduleNumber_, side_ - 1, 0, row_);
    }
    if ((layer_ == 0) && (row_ < (frontCellY_.size() - 1)) && acrossFoil) {
      /*   0     0  1
       *  [x] | [ ][ ] R+1
       *  [ ] | [.][ ] R
       *  [ ] | [ ]| [x] R-1
       */
      ids_.emplace_back(cellGIDType_, moduleNumber_, side_ - 1, 0, row_ + 1);
    }
  }

  return ids_;
}

uint32_t gg_locator::getModuleAddress(const geomtools::geom_id &gid) const {
  return gid.get(moduleAddressIndex_);
}

uint32_t gg_locator::getSideAddress(const geomtools::geom_id &gid) const {
  return gid.get(sideAddressIndex_);
}

uint32_t gg_locator::getLayerAddress(const geomtools::geom_id &gid) const {
  return gid.get(layerAddressIndex_);
}

uint32_t gg_locator::getRowAddress(const geomtools::geom_id &gid) const {
  return gid.get(rowAddressIndex_);
}

bool gg_locator::isGeigerCell(const geomtools::geom_id &gid) const {
  if (gid.get_type() != cellGIDType_) return false;
  unsigned int moduleId = gid.get(0); 
  if (moduleId != moduleNumber_) return false;
  unsigned int sideId = gid.get(1); 
  if (sideId >= numberOfSides()) return false;
  if (gid.get(2) >= numberOfLayers(sideId)) return false;
  if (gid.get(3) >= numberOfRows(sideId)) return false;
  return true;
}

bool gg_locator::isGeigerCellInThisModule(const geomtools::geom_id &gid) const {
  return isGeigerCell(gid) && (getModuleAddress(gid) == moduleNumber_);
}

geomtools::vector_3d gg_locator::transformWorldToModule(
    const geomtools::vector_3d &worldPoint) const {
  return moduleWorldPlacement_->mother_to_child(worldPoint);
}

geomtools::vector_3d gg_locator::transformModuleToWorld(
    const geomtools::vector_3d &modulePoint) const {
  return moduleWorldPlacement_->child_to_mother(modulePoint);
}

bool gg_locator::isPointInModule(const geomtools::vector_3d &modulePoint, double tolerance) const {
  return !moduleBoxShape_->is_outside(modulePoint, tolerance);
}

bool gg_locator::isPointInCell(const geomtools::vector_3d &modulePoint, uint32_t side,
                               uint32_t layer, uint32_t row, double tolerance) const {
  if (!submodules_[side]) {
    return false;
  }
  geomtools::vector_3d cellPoint = modulePoint;
  cellPoint -= getCellPosition(side, layer, row);
  // here one misses one transformation step (rotation) but it is ok :
  // return cellBoxShape_->is_inside(to_cell_pos, tolerance_);
  return !cellBoxShape_->is_outside(cellPoint, tolerance);
}

bool gg_locator::isWorldPointInCell(const geomtools::vector_3d &worldPoint, uint32_t side,
                                    uint32_t layer, uint32_t row, double tolerance) const {
  if (!submodules_[side]) {
    return false;
  }
  geomtools::vector_3d modulePoint = transformWorldToModule(worldPoint);
  return isPointInCell(modulePoint, side, layer, row, tolerance);
}

bool gg_locator::isWorldPointInModule(const geomtools::vector_3d &worldPoint,
                                      double tolerance) const {
  geomtools::vector_3d modulePoint = transformWorldToModule(worldPoint);
  return isPointInModule(modulePoint, tolerance);
}

bool gg_locator::find_geom_id(const geomtools::vector_3d &worldPoint, int type,
                              geomtools::geom_id &gid, double tolerance) const {
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  logging = datatools::logger::PRIO_DEBUG;
  DT_THROW_IF(type != (int)cellGIDType_, std::logic_error, "Only works with type " << cellGIDType_);
  gid.invalidate();
  DT_LOG_DEBUG(logging, "worldPoint = " << worldPoint);
  geomtools::vector_3d modulePoint = transformWorldToModule(worldPoint);
  DT_LOG_DEBUG(logging, "modulePoint = " << modulePoint);

  if (!moduleBoxShape_->is_inside(modulePoint, tolerance)) {
    DT_LOG_DEBUG(logging, "not in module bix shape!");
    return false;
  }

  // If inside the module, check in detail
  if (tolerance == GEOMTOOLS_PROPER_TOLERANCE) {
    tolerance = cellBoxShape_->get_tolerance();
    DT_LOG_DEBUG(logging, "using cell box shape proper tolerance=" << tolerance / CLHEP::mm << " mm");
   }

  gid.invalidate();
  gid.set_type(cellGIDType_);
  gid.set(moduleAddressIndex_, geomtools::geom_id::INVALID_ADDRESS);
  gid.set(sideAddressIndex_, geomtools::geom_id::INVALID_ADDRESS);
  gid.set(layerAddressIndex_, geomtools::geom_id::INVALID_ADDRESS);
  gid.set(rowAddressIndex_, geomtools::geom_id::INVALID_ADDRESS);

  uint32_t side_number(geomtools::geom_id::INVALID_ADDRESS);
  uint32_t layer_number(geomtools::geom_id::INVALID_ADDRESS);
  uint32_t cell_number(geomtools::geom_id::INVALID_ADDRESS);
  const double z = modulePoint.z();

  if (std::abs(z) < (cellBoxShape_->get_half_z() + 0.5 * tolerance)) {
    gid.set(moduleAddressIndex_, moduleNumber_);
    DT_LOG_DEBUG(logging, " z ok : gid=" << gid);
    const double y = modulePoint.y();
    const double x = modulePoint.x();
    double first_cell_x;
    double cell_delta_x;
    double first_cell_y;
    double cell_delta_y;
    size_t nlayers = 0;
    size_t nrows = 0;

    // Find the side:
    if (side_number == geomtools::geom_id::INVALID_ADDRESS && submodules_[side_t::BACK]) {
      nlayers = backCellX_.size();
      nrows = backCellY_.size();
      if (nlayers > 0 && nrows > 0) {
        double xmax = backCellX_.front() + 0.5 * cellDiameter() + tolerance;
        if (x <= xmax) {
          side_number = 0;
          first_cell_x = backCellX_.front();
          cell_delta_x = (backCellX_.back() - backCellX_.front()) / (backCellX_.size() - 1);
          first_cell_y = backCellY_.front();
          cell_delta_y = (backCellY_.back() - backCellY_.front()) / (backCellY_.size() - 1);
        }
      }
    }
    if (side_number == geomtools::geom_id::INVALID_ADDRESS && submodules_[side_t::FRONT]) {
      nlayers = frontCellX_.size();
      nrows = frontCellY_.size();
      if (nlayers > 0 && nrows > 0) {
        double xmin = frontCellX_.front() - 0.5 * cellDiameter() - tolerance;
        if (x >= xmin) {
          side_number = 1;
          first_cell_x = frontCellX_.front();
          cell_delta_x = (frontCellX_.back() - frontCellX_.front()) / (frontCellX_.size() - 1);
          first_cell_y = frontCellY_.front();
          cell_delta_y = (frontCellY_.back() - frontCellY_.front()) / (frontCellY_.size() - 1);
        }
      }
    }
    if (side_number == geomtools::geom_id::INVALID_ADDRESS) {
      DT_LOG_DEBUG(logging, "invalid  side");
      gid.invalidate();
      return false;
    }
    gid.set(sideAddressIndex_, side_number);
    DT_LOG_DEBUG(logging, " side ok : gid=" << gid);
    const int ix = (int)(((x - first_cell_x) / cell_delta_x) + 0.5);
    if ((ix >= 0) && (ix < (int)nlayers)) {
      layer_number = ix;
    }
    gid.set(layerAddressIndex_, layer_number);
    DT_LOG_DEBUG(logging, " layer ok : gid=" << gid);
    const int iy = (int)(((y - first_cell_y) / cell_delta_y) + 0.5);
    if ((iy >= 0) && (iy < (int)nrows)) {
      cell_number = iy;
    }
    gid.set(rowAddressIndex_, cell_number);
    DT_LOG_DEBUG(logging, " row ok : gid=" << gid);
    if (gid.is_valid()) {
      DT_LOG_DEBUG(logging, " gid ok : gid=" << gid);
      const geomtools::geom_info *ginfo_ptr = geomMapping_->get_geom_info_ptr(gid);
      if (ginfo_ptr == nullptr) {
        DT_LOG_DEBUG(logging, " gid not mapped");
        gid.invalidate();
        return false;
      }
      geomtools::vector_3d inWorldPoint = transformModuleToWorld(modulePoint);
      if (geomtools::mapping::check_inside(*ginfo_ptr, inWorldPoint, tolerance, true)) {
        DT_LOG_DEBUG(logging, " gid post-validation");
        return true;
      }
    }
    gid.invalidate();
  }
  // 2012-06-05 FM: add missing invalidate call
  gid.invalidate();
  return false;
}

bool gg_locator::findCellGID(const geomtools::vector_3d &worldPoint, geomtools::geom_id &gid,
                             double tolerance) const {
  return find_geom_id(worldPoint, cellGIDType_, gid, tolerance);
}

// ----- OUTPUT

void gg_locator::tree_dump(std::ostream &out, const std::string &title, const std::string &indent,
                           bool inherit) const {
  const std::string itag = datatools::i_tree_dumpable::tags::item();
  const std::string stag = datatools::i_tree_dumpable::tags::skip_item();
  if (!title.empty()) {
    out << indent << title << std::endl;
  }
  if (!is_initialized()) {
    out << indent << datatools::i_tree_dumpable::inherit_tag(inherit)
        << "Initialized        = " << isInitialized_ << std::endl;
    return;
  }
  out << indent << itag << "Initialized                = " << isInitialized_ << std::endl;
  out << indent << itag << "Logging priority threshold = "
      << datatools::logger::get_priority_label(get_logging_priority()) << std::endl;
  out << indent << itag << "Module number              = " << moduleNumber_ << std::endl;
  out << indent << itag << "Manager @                  = " << &get_geo_manager() << std::endl;
  out << indent << itag << "Mapping @                  = " << geomMapping_ << std::endl;
  out << indent << itag << "Cell type                  = " << cellGIDType_ << std::endl;
  out << indent << itag << "Module placement    : " << std::endl;
  if (moduleWorldPlacement_ != nullptr) {
    moduleWorldPlacement_->tree_dump(out, "", indent + stag);
  }
  out << indent << itag << "Module box : " << std::endl;
  if (moduleBoxShape_ != nullptr) {
    moduleBoxShape_->tree_dump(out, "", indent + stag);
  }
  out << indent << itag << "Back  submodule : " << submodules_[side_t::BACK] << std::endl;
  out << indent << itag << "Front submodule : " << submodules_[side_t::FRONT] << std::endl;
  out << indent << itag << "Cell box : " << std::endl;
  if (cellBoxShape_ != nullptr) {
    cellBoxShape_->tree_dump(out, "", indent + stag);
  }
  out << indent << itag << "Back layer X-pos [" << backCellX_.size() << "] = ";
  for (double i : backCellX_) {
    out << i / CLHEP::mm << " ";
  }
  out << " (mm)" << std::endl;
  out << indent << itag << "Back cell Y-pos [" << backCellY_.size() << "] = ";
  for (size_t i = 0; i < backCellY_.size(); i++) {
    if ((i < 4) || (i > backCellY_.size() - 4)) {
      out << backCellY_[i] / CLHEP::mm << " ";
    } else if (i == 4) {
      out << " ... ";
    }
  }
  out << " (mm)" << std::endl;
  out << indent << itag << "Front layer X-pos [" << frontCellX_.size() << "] =  ";
  for (double i : frontCellX_) {
    out << i / CLHEP::mm << " ";
  }
  out << " (mm)" << std::endl;
  out << indent << itag << "Front cell Y-pos [" << frontCellY_.size() << "] = ";
  for (size_t i = 0; i < frontCellY_.size(); i++) {
    if ((i < 4) || (i > backCellY_.size() - 4)) {
      out << frontCellY_[i] / CLHEP::mm << " ";
    } else if (i == 4) {
      out << " ... ";
    }
  }
  out << " (mm)" << std::endl;

  out << indent << itag << "Anode wire length   = " << anodeWireLength_ / CLHEP::mm << " (mm)"
      << std::endl;
  out << indent << itag << "Anode wire diameter = " << anodeWireDiameter_ / CLHEP::mm << " (mm)"
      << std::endl;
  out << indent << itag << "Field wire length   = " << fieldWireLength_ / CLHEP::mm << " (mm)"
      << std::endl;
  out << indent << itag << "Field wire diameter = " << fieldWireDiameter_ / CLHEP::mm << " (mm)"
      << std::endl;
  out << indent << itag << "Module address GID index = " << moduleAddressIndex_ << std::endl;
  out << indent << itag << "Side address GID index   = " << sideAddressIndex_ << std::endl;
  out << indent << itag << "Layer address GID index  = " << layerAddressIndex_ << std::endl;
  out << indent << datatools::i_tree_dumpable::inherit_tag(inherit)
      << "Row address GID index    = " << rowAddressIndex_ << std::endl;
}

void gg_locator::set_defaults_() {
  moduleNumber_ = geomtools::geom_id::INVALID_ADDRESS;

  // Cell GID type+subaddress indices :
  cellGIDType_ = geomtools::geom_id::INVALID_TYPE;
  moduleAddressIndex_ = -1;
  sideAddressIndex_ = -1;
  layerAddressIndex_ = -1;
  rowAddressIndex_ = -1;

  geomMapping_ = nullptr;
  moduleWorldPlacement_ = nullptr;
  moduleBoxShape_ = nullptr;
  cellBoxShape_ = nullptr;

  for (bool &_submodule : submodules_) {
    _submodule = false;
  }

  anodeWireLength_ = datatools::invalid_real();
  anodeWireDiameter_ = datatools::invalid_real();
  fieldWireLength_ = datatools::invalid_real();
  fieldWireDiameter_ = datatools::invalid_real();

  backCellX_.clear();
  backCellY_.clear();
  frontCellX_.clear();
  frontCellY_.clear();

  isInitialized_ = false;
}

void gg_locator::construct_() {
  const geomtools::id_mgr &idManager = get_geo_manager().get_id_mgr();

  const geomtools::id_mgr::category_info &cellCatInfo =
      idManager.get_category_info(detail::kDriftCellGIDCategory);
  cellGIDType_ = cellCatInfo.get_type();

  // The get_subaddress_index member function returns an invalid index
  // rather than throwing an exception. We therefore check the subaddress
  // categories we need upfront...
  for (const std::string subaddress : {"module", "side", "layer", "row"}) {
    DT_THROW_IF(!cellCatInfo.has_subaddress(subaddress), std::logic_error,
                "Category '" << detail::kDriftCellGIDCategory << "' has no subaddress '"
                             << subaddress << "'");
  }

  moduleAddressIndex_ = cellCatInfo.get_subaddress_index("module");
  sideAddressIndex_ = cellCatInfo.get_subaddress_index("side");
  layerAddressIndex_ = cellCatInfo.get_subaddress_index("layer");
  rowAddressIndex_ = cellCatInfo.get_subaddress_index("row");

  geomMapping_ = &get_geo_manager().get_mapping();

  uint32_t moduleGIDType = idManager.get_category_type(detail::kModuleGIDCategory);
  const geomtools::geom_id moduleGID(moduleGIDType, moduleNumber_);
  const geomtools::geom_info &moduleGeomInfo = geomMapping_->get_geom_info(moduleGID);
  const geomtools::i_shape_3d *moduleShape = &moduleGeomInfo.get_logical().get_shape();
  DT_THROW_IF(moduleShape->get_shape_name() != "box", std::logic_error,
              "Cannot extract a 'box' shape from module with ID = " << moduleGID << " !");
  moduleBoxShape_ = dynamic_cast<const geomtools::box *>(moduleShape);
  moduleWorldPlacement_ = &moduleGeomInfo.get_world_placement();

  // Search for tracker submodules :
  uint32_t trackerVolumeGIDType = idManager.get_category_type(detail::kTrackerVolumeGIDCategory);
  geomtools::geom_id side_gid;
  side_gid.set_type(trackerVolumeGIDType);
  uint32_t ref_side = geomtools::geom_id::INVALID_ADDRESS;
  for (size_t iside = 0; iside < utils::NSIDES; iside++) {
    side_gid.set_address(moduleNumber_, iside);
    if (geomMapping_->validate_id(side_gid)) {
      submodules_[iside] = true;
      ref_side = iside;
    }
  }
  DT_THROW_IF(ref_side == geomtools::geom_id::INVALID_ADDRESS, std::logic_error,
              "Cannot extract information about any tracker submodules !");

  // Pick up the first available cell :
  {
    const geomtools::geom_id cellGID(cellGIDType_, moduleNumber_, ref_side, 0, 0);
    const geomtools::geom_info &cellGeomInfo = geomMapping_->get_geom_info(cellGID);
    const geomtools::i_shape_3d &cellShape = cellGeomInfo.get_logical().get_shape();
    DT_THROW_IF(cellShape.get_shape_name() != "box", std::logic_error,
                "Cannot extract the box shape from cell with ID = " << cellGID << " !");
    cellBoxShape_ = dynamic_cast<const geomtools::box *>(&cellShape);
  }

  std::vector<double> *vlx[utils::NSIDES];
  vlx[side_t::BACK] = &backCellX_;
  vlx[side_t::FRONT] = &frontCellX_;
  // Loop on tracker sides:
  for (size_t side = 0; side < utils::NSIDES; side++) {
    if (!submodules_[side]) {
      continue;
    }
    size_t i_layer = 0;
    vlx[side]->reserve(10);
    while (true) {
      geomtools::geom_id cellGID(cellGIDType_, moduleNumber_, side, i_layer, 0);
      if (!geomMapping_->validate_id(cellGID)) {
        break;
      }
      const geomtools::geom_info &cellGeomInfo = geomMapping_->get_geom_info(cellGID);
      const geomtools::placement &cellWorldPlacement = cellGeomInfo.get_world_placement();
      geomtools::placement cellModulePlacement;
      moduleWorldPlacement_->relocate(cellWorldPlacement, cellModulePlacement);
      vlx[side]->push_back(cellModulePlacement.get_translation().x());
      i_layer++;
    }
  }

  std::vector<double> *vcy[utils::NSIDES];
  vcy[0] = &backCellY_;
  vcy[1] = &frontCellY_;
  // Loop on tracker sides:
  for (size_t side = 0; side < utils::NSIDES; side++) {
    if (!submodules_[side]) {
      continue;
    }
    size_t i_cell = 0;
    vlx[side]->reserve(130);
    while (true) {
      geomtools::geom_id cellGID(cellGIDType_, moduleNumber_, side, 0, i_cell);
      if (!geomMapping_->validate_id(cellGID)) {
        break;
      }
      const geomtools::geom_info &cellGeomInfo = geomMapping_->get_geom_info(cellGID);
      const geomtools::placement &cellWorldPlacement = cellGeomInfo.get_world_placement();
      geomtools::placement cellModulePlacement;
      moduleWorldPlacement_->relocate(cellWorldPlacement, cellModulePlacement);
      vcy[side]->push_back(cellModulePlacement.get_translation().y());
      i_cell++;
    }
  }

  // analyse the geometry versioning :
  datatools::version_id geom_mgr_setup_vid;
  get_geo_manager().fetch_setup_version_id(geom_mgr_setup_vid);

  // extract the geometry model associated to the "anode_wire" :
  {
    std::string model_name = "anode_wire.model";  // default model name
    if (geom_mgr_setup_vid.has_major()) {
      // trick for an old version of the geometry
      if (get_geo_manager().get_setup_label() == "snemo::demonstrator" &&
          geom_mgr_setup_vid.get_major() < 2) {
        model_name = "anode_wire";
      }
    }

    auto found = get_geo_manager().get_factory().get_models().find(model_name);
    DT_THROW_IF(found == get_geo_manager().get_factory().get_models().end(), std::logic_error,
                "Model factory has no model '" << model_name << "'");
    const geomtools::i_model *anode_wire_model = found->second;
    const geomtools::logical_volume &anode_wire_log = anode_wire_model->get_logical();
    const geomtools::i_shape_3d &anode_wire_shape = anode_wire_log.get_shape();
    const auto &anode_wire_cylinder = dynamic_cast<const geomtools::cylinder &>(anode_wire_shape);

    anodeWireLength_ = anode_wire_cylinder.get_z();
    anodeWireDiameter_ = anode_wire_cylinder.get_diameter();
  }

  // extract the geometry model associated to the "field_wire" :
  {
    std::string model_name = "field_wire.model";  // default model name
    if (geom_mgr_setup_vid.has_major()) {
      // trick for an old version of the geometry
      if (get_geo_manager().get_setup_label() == "snemo::demonstrator" &&
          geom_mgr_setup_vid.get_major() < 2) {
        model_name = "field_wire";
      }
    }

    auto found = get_geo_manager().get_factory().get_models().find(model_name);
    DT_THROW_IF(found == get_geo_manager().get_factory().get_models().end(), std::logic_error,
                "Model factory has no model '" << model_name << "'");
    const geomtools::i_model *field_wire_model = found->second;
    const geomtools::logical_volume &field_wire_log = field_wire_model->get_logical();
    const geomtools::i_shape_3d &field_wire_shape = field_wire_log.get_shape();
    const auto &field_wire_cylinder = dynamic_cast<const geomtools::cylinder &>(field_wire_shape);

    fieldWireLength_ = field_wire_cylinder.get_z();
    fieldWireDiameter_ = field_wire_cylinder.get_diameter();
  }
}

}  // namespace geometry
}  // namespace snemo

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
