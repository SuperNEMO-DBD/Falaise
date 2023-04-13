// falaise/snemo/geometry/gveto_locator.cc
/* Copyright (C) 2011-2014 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or(at
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
#include <falaise/snemo/geometry/gveto_locator.h>

#include "private/categories.h"

// Standard library:
#include <stdexcept>

// Third party
// - Bayeux/datatools :
#include <datatools/utils.h>
#include <datatools/version_id.h>
// - Bayeux/geomtools :
#include <geomtools/box.h>
#include <geomtools/i_composite_shape_3d.h>
#include <geomtools/intersection_3d.h>
#include <geomtools/manager.h>
#include <geomtools/subtraction_3d.h>

namespace snemo {

namespace geometry {

// Constructor:
gveto_locator::gveto_locator() { set_defaults_(); }

// Constructor:
gveto_locator::gveto_locator(uint32_t moduleID, const geomtools::manager &geoMgr,
                             const falaise::property_set &ps) {
  set_defaults_();
  set_geo_manager(geoMgr);
  setModuleNumber(moduleID);
  initialize(ps);
}

// dtor:
gveto_locator::~gveto_locator() = default;

void gveto_locator::reset() { set_defaults_(); }

void gveto_locator::initialize(const datatools::properties &ps) {
  base_locator::_basic_initialize(ps);
  DT_THROW_IF(moduleNumber_ == geomtools::geom_id::INVALID_ADDRESS, std::logic_error,
              "Missing module number ! Use the 'setModuleNumber' method before !");
  construct_();
  isInitialized_ = true;
}

bool gveto_locator::is_initialized() const { return isInitialized_; }

bool gveto_locator::hasSubmodule(uint32_t side) const {
  DT_THROW_IF(side >= utils::NSIDES, std::out_of_range, "Invalid submodule number " << side);
  return submodules_[side];
}

uint32_t gveto_locator::getModuleNumber() const { return moduleNumber_; }

void gveto_locator::setModuleNumber(uint32_t id) { moduleNumber_ = id; }

double gveto_locator::blockWidth() const { return caloBlockBox_->get_x(); }

double gveto_locator::blockHeight() const { return caloBlockBox_->get_y(); }

double gveto_locator::blockThickness() const { return caloBlockBox_->get_z(); }

size_t gveto_locator::numberOfSides() const { return utils::NSIDES; }

size_t gveto_locator::numberOfWalls() const { return NWALLS_PER_SIDE; }

size_t gveto_locator::numberOfColumns(uint32_t side, uint32_t wall) const {
  DT_THROW_IF(wall >= NWALLS_PER_SIDE, std::out_of_range,
              "Invalid wall number(" << wall << ">" << NWALLS_PER_SIDE << ")!");
  if (side == (uint32_t)side_t::BACK) {
    return backCaloBlock_Y_[wall].size();
  }

  if (side == (uint32_t)side_t::FRONT) {
    return frontCaloBlock_Y_[wall].size();
  }

  DT_THROW(std::logic_error, "Invalid side number(" << side << ">= " << utils::NSIDES << ")!");
}

uint32_t gveto_locator::getModuleAddress(const geomtools::geom_id &gid) const {
  return gid.get(moduleAddressIndex_);
}

uint32_t gveto_locator::getSideAddress(const geomtools::geom_id &gid) const {
  return gid.get(sideAddressIndex_);
}

uint32_t gveto_locator::getWallAddress(const geomtools::geom_id &gid) const {
  return gid.get(wallAddressIndex_);
}

uint32_t gveto_locator::getColumnAddress(const geomtools::geom_id &gid) const {
  return gid.get(columnAddressIndex_);
}

uint32_t gveto_locator::getPartAddress(const geomtools::geom_id &gid) const {
  if (isBlockPartitioned()) {
    return gid.get(partAddressIndex_);
  }
  return geomtools::geom_id::INVALID_ADDRESS;
}

bool gveto_locator::isCaloOM(const geomtools::geom_id &gid) const {
  return gid.get_type() == caloOMGIDType_;
}

bool gveto_locator::isCaloBlock(const geomtools::geom_id &gid) const {
  return gid.get_type() == caloBlockGIDType_;
}

bool gveto_locator::isCaloBlockInThisModule(const geomtools::geom_id &gid) const {
  return isCaloBlock(gid) && (getModuleAddress(gid) == moduleNumber_);
}

bool gveto_locator::isValidAddress(uint32_t side, uint32_t wall, uint32_t column) const {
  if (side >= utils::NSIDES) {
    return false;
  }
  if (wall >= NWALLS_PER_SIDE) {
    return false;
  }
  if (column >= numberOfColumns(side, wall)) {
    return false;
  }
  return true;
}

size_t gveto_locator::countNeighbours(uint32_t side_, uint32_t wall_, uint32_t column_,
                                      uint8_t mask_) const {
  DT_THROW_IF(side_ >= utils::NSIDES, std::logic_error,
              "Invalid side number(" << side_ << ">= " << utils::NSIDES << ")!");
  DT_THROW_IF(wall_ >= NWALLS_PER_SIDE, std::logic_error,
              "Invalid wall number(" << wall_ << ">" << NWALLS_PER_SIDE << ")!");

  bool corner = false;
  bool side = false;
  const bool sides = (mask_ & grid_mask_t::SIDE) != 0;
  const bool diagonal = (mask_ & grid_mask_t::DIAG) != 0;
  const bool second = (mask_ & grid_mask_t::SECOND) != 0;
  if (second) {
    DT_LOG_NOTICE(get_logging_priority(),
                  "Looking for second order neighbour of 'gveto' locator is not implemented !");
  }

  if (side_ == (uint32_t)side_t::BACK) {
    if ((column_ == 0) || (column_ == backCaloBlock_Y_[wall_].size() - 1)) {
      side = true;
    }
  }
  if (side_ == (uint32_t)side_t::FRONT) {
    if ((column_ == 0) || (column_ == frontCaloBlock_Y_[wall_].size() - 1)) {
      side = true;
    }
  }
  size_t number = 0;
  if (corner) {
    if (sides) {
      number += 2;
    }
    if (diagonal) {
      number += 1;
    }
  } else if (side) {
    if (sides) {
      number += 3;
    }
    if (diagonal) {
      number += 2;
    }
  } else {
    if (sides) {
      number += 4;
    }
    if (diagonal) {
      number += 4;
    }
  }
  return number;
}

size_t gveto_locator::countNeighbours(const geomtools::geom_id &gid, uint8_t mask) const {
  DT_THROW_IF(gid.get_depth() != 4, std::out_of_range,
              "Invalid depth(" << gid.get_depth() << " != 4)!");
  DT_THROW_IF(
      gid.get(moduleAddressIndex_) != moduleNumber_, std::out_of_range,
      "Invalid module number(" << gid.get(moduleAddressIndex_) << "!=" << moduleNumber_ << ")!");
  return countNeighbours(gid.get(sideAddressIndex_), gid.get(wallAddressIndex_),
                         gid.get(columnAddressIndex_), mask);
}

std::vector<geomtools::geom_id> gveto_locator::getNeighbourGIDs(const geomtools::geom_id &gid,
                                                                uint8_t mask) const {
  DT_THROW_IF(gid.get_depth() != 4, std::out_of_range,
              "Invalid depth(" << gid.get_depth() << " != 4)!");
  DT_THROW_IF(
      gid.get(moduleAddressIndex_) != moduleNumber_, std::out_of_range,
      "Invalid module number(" << gid.get(moduleAddressIndex_) << "!=" << moduleNumber_ << ")!");
  return getNeighbourGIDs(gid.get(sideAddressIndex_), gid.get(wallAddressIndex_),
                          gid.get(columnAddressIndex_), mask);
}

std::vector<geomtools::geom_id> gveto_locator::getNeighbourGIDs(uint32_t side_, uint32_t wall_,
                                                                uint32_t column_,
                                                                uint8_t mask_) const {
  DT_THROW_IF(side_ >= utils::NSIDES, std::out_of_range,
              "Invalid side number(" << side_ << ">= " << utils::NSIDES << ")!");
  DT_THROW_IF(wall_ >= NWALLS_PER_SIDE, std::out_of_range,
              "Invalid wall number(" << wall_ << ">" << NWALLS_PER_SIDE << ")!");

  std::vector<geomtools::geom_id> ids_;
  ids_.reserve(8);

  const bool sides = (mask_ & grid_mask_t::SIDE) != 0;
  const bool diagonal = (mask_ & grid_mask_t::DIAG) != 0;
  const bool second = (mask_ & grid_mask_t::SECOND) != 0;
  if (second) {
    DT_LOG_NOTICE(get_logging_priority(),
                  "Looking for second order neighbour of 'gveto' locator is not implemented !");
  }

  // prepare neighbour GID :
  geomtools::geom_id gid;
  gid.set_type(caloBlockGIDType_);
  gid.set(moduleAddressIndex_, moduleNumber_);
  gid.set(wallAddressIndex_, wall_);
  gid.set(sideAddressIndex_, side_);
  gid.set(columnAddressIndex_, geomtools::geom_id::INVALID_ADDRESS);
  if (isBlockPartitioned()) {
    gid.set(partAddressIndex_, blockPart_);
  }
  // back
  if (side_ == (uint32_t)side_t::BACK) {
    DT_THROW_IF(
        column_ >= backCaloBlock_Y_[wall_].size(), std::out_of_range,
        "Invalid column number(" << column_ << ">" << backCaloBlock_Y_[wall_].size() - 1 << ")!");
    if (sides && (column_ < (backCaloBlock_Y_[wall_].size() - 1))) {
      /*  C-1 C C+1
       *  [ ][ ][ ]
       *  [ ][.][x]
       *  [ ][ ][ ]
       */
      gid.set(columnAddressIndex_, column_ + 1);
      ids_.push_back(gid);
    }
    if (sides && (column_ > 0)) {
      /*  C-1 C C+1
       *  [ ][ ][ ] R+1
       *  [x][.][ ] R
       *  [ ][ ][ ] R-1
       */
      gid.set(columnAddressIndex_, column_ - 1);
      ids_.push_back(gid);
    }

    if (diagonal && (column_ < (backCaloBlock_Y_[wall_].size() - 1))) {
      /*  C-1 C C+1
       *  [ ][ ][x] R+1
       *  [ ][.][ ] R
       *  [ ][ ][ ] R-1
       */
      gid.set(columnAddressIndex_, column_ + 1);
      ids_.push_back(gid);
    }

    if (diagonal && (column_ > 0)) {
      /*  C-1 C C+1
       *  [x][ ][ ] R+1
       *  [ ][.][ ] R
       *  [ ][ ][ ] R-1
       */
      gid.set(columnAddressIndex_, column_ - 1);
      ids_.push_back(gid);
    }

    if (diagonal && (column_ > 0)) {
      /*  C-1 C C+1
       *  [ ][ ][ ] R+1
       *  [ ][.][ ] R
       *  [x][ ][ ] R-1
       */
      gid.set(columnAddressIndex_, column_ - 1);
      ids_.push_back(gid);
    }

    if (diagonal && (column_ < (backCaloBlock_Y_[wall_].size() - 1))) {
      /*  C-1 C C+1
       *  [ ][ ][ ] R+1
       *  [ ][.][ ] R
       *  [ ][ ][x] R-1
       */
      gid.set(columnAddressIndex_, column_ + 1);
      ids_.push_back(gid);
    }
  }

  // front:
  if (side_ == (uint32_t)side_t::FRONT) {
    DT_THROW_IF(
        column_ >= frontCaloBlock_Y_[wall_].size(), std::out_of_range,
        "Invalid column number(" << column_ << ">" << frontCaloBlock_Y_[wall_].size() - 1 << ")!");
    if (sides && (column_ < (frontCaloBlock_Y_[wall_].size() - 1))) {
      /*  C-1 C C+1
       *  [ ][ ][ ]
       *  [ ][.][x]
       *  [ ][ ][ ]
       */
      gid.set(columnAddressIndex_, column_ + 1);
      ids_.push_back(gid);
    }
    if (sides && (column_ > 0)) {
      /*  C-1 C C+1
       *  [ ][ ][ ] R+1
       *  [x][.][ ] R
       *  [ ][ ][ ] R-1
       */
      gid.set(columnAddressIndex_, column_ - 1);
      ids_.push_back(gid);
    }

    if (diagonal && (column_ < (frontCaloBlock_Y_[wall_].size() - 1))) {
      /*  C-1 C C+1
       *  [ ][ ][x] R+1
       *  [ ][.][ ] R
       *  [ ][ ][ ] R-1
       */
      gid.set(columnAddressIndex_, column_ + 1);
      ids_.push_back(gid);
    }

    if (diagonal && (column_ > 0)) {
      /*  C-1 C C+1
       *  [x][ ][ ] R+1
       *  [ ][.][ ] R
       *  [ ][ ][ ] R-1
       */
      gid.set(columnAddressIndex_, column_ - 1);
      ids_.push_back(gid);
    }

    if (diagonal && (column_ > 0)) {
      /*  C-1 C C+1
       *  [ ][ ][ ] R+1
       *  [ ][.][ ] R
       *  [x][ ][ ] R-1
       */
      gid.set(columnAddressIndex_, column_ - 1);
      ids_.push_back(gid);
    }

    if (diagonal && (column_ < (frontCaloBlock_Y_[wall_].size() - 1))) {
      /*  C-1 C C+1
       *  [ ][ ][ ] R+1
       *  [ ][.][ ] R
       *  [ ][ ][x] R-1
       */
      gid.set(columnAddressIndex_, column_ + 1);
      ids_.push_back(gid);
    }
  }

  return ids_;
}

double gveto_locator::getZCoordOfWall(uint32_t side, uint32_t wall) const {
  DT_THROW_IF(side >= utils::NSIDES, std::out_of_range,
              "Invalid side number(" << side << ">" << utils::NSIDES << ")!");
  DT_THROW_IF(wall >= NWALLS_PER_SIDE, std::out_of_range,
              "Invalid wall number(" << wall << ">" << NWALLS_PER_SIDE << ")!");
  return blockWall_Z_[side][wall];
}

double gveto_locator::getZCoordOfWallWindow(uint32_t side, uint32_t wall) const {
  DT_THROW_IF(side >= utils::NSIDES, std::out_of_range,
              "Invalid side number(" << side << ">" << utils::NSIDES << ")!");
  DT_THROW_IF(wall >= NWALLS_PER_SIDE, std::out_of_range,
              "Invalid wall number(" << wall << ">" << NWALLS_PER_SIDE << ")!");
  return blockWallWindow_Z_[side][wall];
}

double gveto_locator::getXCoordOfColumn(uint32_t side, uint32_t wall, uint32_t column) const {
  DT_THROW_IF(side >= utils::NSIDES, std::out_of_range,
              "Invalid side number(" << side << ">" << utils::NSIDES << ")!");
  DT_THROW_IF(wall >= NWALLS_PER_SIDE, std::out_of_range,
              "Invalid wall number(" << wall << ">" << NWALLS_PER_SIDE << ")!");

  if (side == (uint32_t)side_t::BACK) {
    DT_THROW_IF(
        column >= backCaloBlock_Y_[wall].size(), std::out_of_range,
        "Invalid column number(" << column << ">" << backCaloBlock_Y_[wall].size() - 1 << ")!");
    return backCaloBlock_X_[wall][column];
  }
  DT_THROW_IF(
      column >= frontCaloBlock_Y_[wall].size(), std::out_of_range,
      "Invalid column number(" << column << ">" << frontCaloBlock_Y_[wall].size() - 1 << ")!");
  return frontCaloBlock_X_[wall][column];
}

double gveto_locator::getYCoordOfColumn(uint32_t side, uint32_t wall, uint32_t column) const {
  DT_THROW_IF(side >= utils::NSIDES, std::out_of_range,
              "Invalid side number(" << side << ">" << utils::NSIDES << ")!");
  DT_THROW_IF(wall >= NWALLS_PER_SIDE, std::out_of_range,
              "Invalid wall number(" << wall << ">" << NWALLS_PER_SIDE << ")!");

  if (side == (uint32_t)side_t::BACK) {
    DT_THROW_IF(
        column >= backCaloBlock_Y_[wall].size(), std::out_of_range,
        "Invalid column number(" << column << ">" << backCaloBlock_Y_[wall].size() - 1 << ")!");
    return backCaloBlock_Y_[wall][column];
  }
  DT_THROW_IF(
      column >= frontCaloBlock_Y_[wall].size(), std::out_of_range,
      "Invalid column number(" << column << ">" << frontCaloBlock_Y_[wall].size() - 1 << ")!");
  return frontCaloBlock_Y_[wall][column];
}

geomtools::vector_3d gveto_locator::getBlockPosition(uint32_t side, uint32_t wall,
                                                     uint32_t column) const {
  return geomtools::vector_3d(getXCoordOfColumn(side, wall, column),
                              getYCoordOfColumn(side, wall, column), getZCoordOfWall(side, wall));
}

geomtools::vector_3d gveto_locator::getBlockPosition(const geomtools::geom_id &gid) const {
  DT_THROW_IF(gid.get_depth() != 4, std::logic_error,
              "Invalid depth(" << gid.get_depth() << " != 4)!");
  DT_THROW_IF(
      gid.get(moduleAddressIndex_) != moduleNumber_, std::logic_error,
      "Invalid module number(" << gid.get(moduleAddressIndex_) << "!=" << moduleNumber_ << ")!");
  return getBlockPosition(gid.get(sideAddressIndex_), gid.get(wallAddressIndex_),
                          gid.get(columnAddressIndex_));
}

geomtools::vector_3d gveto_locator::getBlockWindowPosition(uint32_t side, uint32_t wall,
                                                           uint32_t column) const {
  return geomtools::vector_3d(getXCoordOfColumn(side, wall, column),
                              getYCoordOfColumn(side, wall, column), getZCoordOfWall(side, wall));
}

geomtools::vector_3d gveto_locator::transformWorldToModule(
    const geomtools::vector_3d &worldPoint) const {
  return moduleWorldPlacement_->mother_to_child(worldPoint);
}

geomtools::vector_3d gveto_locator::transformModuleToWorld(
    const geomtools::vector_3d &modulePoint) const {
  return moduleWorldPlacement_->child_to_mother(modulePoint);
}

bool gveto_locator::isPointInModule(const geomtools::vector_3d &modulePoint,
                                    double tolerance) const {
  return moduleBox_->is_inside(modulePoint, tolerance);
}

bool gveto_locator::isPointInBlock(const geomtools::vector_3d &modulePoint, uint32_t side,
                                   uint32_t wall, uint32_t column, double tolerance) const {
  geomtools::vector_3d to_block_pos = modulePoint;
  to_block_pos -= getBlockPosition(side, wall, column);
  // here one misses one transformation step(rotation) but it is ok :
  return caloBlockBox_->is_inside(to_block_pos, tolerance);
}

bool gveto_locator::isWorldPointInModule(const geomtools::vector_3d &worldPoint,
                                         double tolerance) const {
  geomtools::vector_3d in_module_position = this->transformWorldToModule(worldPoint);
  return isPointInModule(in_module_position, tolerance);
}

bool gveto_locator::isWorldPointInBlock(const geomtools::vector_3d &worldPoint, uint32_t side,
                                        uint32_t wall, uint32_t column, double tolerance) const {
  geomtools::vector_3d in_module_position = this->transformWorldToModule(worldPoint);
  return isPointInBlock(in_module_position, side, wall, column, tolerance);
}

bool gveto_locator::findBlockGID(const geomtools::vector_3d &worldPoint, geomtools::geom_id &gid,
                                 double tolerance) const {
  return find_geom_id(worldPoint, caloBlockGIDType_, gid, tolerance);
}

bool gveto_locator::find_geom_id(const geomtools::vector_3d &world_position_, int type,
                                 geomtools::geom_id &gid, double tolerance) const {
  DT_THROW_IF(type != (int)caloBlockGIDType_, std::logic_error,
              "Only works with type " << caloBlockGIDType_ << " for now !");

  gid.invalidate();
  geomtools::vector_3d in_module_position_ = this->transformWorldToModule(world_position_);
  if (!moduleBox_->is_inside(in_module_position_, tolerance)) {
    // Not in this module :
    return false;
  }

  if (tolerance == GEOMTOOLS_PROPER_TOLERANCE) {
    tolerance = caloBlockBox_->get_tolerance();
  }

  gid.reset();
  uint32_t side_number(geomtools::geom_id::INVALID_ADDRESS);
  uint32_t wall_number(geomtools::geom_id::INVALID_ADDRESS);
  uint32_t column_number(geomtools::geom_id::INVALID_ADDRESS);
  const double x = in_module_position_.x();
  const double xlim = 1000 * CLHEP::m;
  if (std::abs(x) < xlim) {
    gid.set_type(caloBlockGIDType_);
    gid.set(moduleAddressIndex_, moduleNumber_);
    if (isBlockPartitioned()) {
      gid.set(partAddressIndex_, blockPart_);
    }
    const double y = in_module_position_.y();
    const double z = in_module_position_.z();
    double first_block_y;
    double block_delta_y;
    size_t ncolumns = 0;

    // Find the side:
    if (side_number == geomtools::geom_id::INVALID_ADDRESS && submodules_[side_t::BACK]) {
      double xmax0 =
          backCaloBlock_X_[gveto_wall_t::BOTTOM].front() + 0.5 * blockWidth() + tolerance;
      double xmax1 = backCaloBlock_X_[gveto_wall_t::TOP].front() + 0.5 * blockWidth() + tolerance;
      double xmax = std::max(xmax0, xmax1);
      if (x <= xmax) {
        side_number = side_t::BACK;
      }
    }
    if (side_number == geomtools::geom_id::INVALID_ADDRESS && submodules_[side_t::FRONT]) {
      double xmin0 =
          frontCaloBlock_X_[gveto_wall_t::BOTTOM].front() - 0.5 * blockWidth() - tolerance;
      double xmin1 = frontCaloBlock_X_[gveto_wall_t::TOP].front() - 0.5 * blockWidth() - tolerance;
      double xmin = std::min(xmin0, xmin1);
      if (x >= xmin) {
        side_number = side_t::FRONT;
      }
    }
    if (side_number == geomtools::geom_id::INVALID_ADDRESS) {
      gid.invalidate();
      return false;
    }

    // Find the wall:
    // 2012-06-13 XG: For config 2.0 the 16 gveto block
    // are separated into two series of 8 blocks : between
    // these series there is a gap arround y=0. To
    // determine the column number then we have to take
    // care of this gap by splitting the column range into
    // two separate vectors. This of course does not
    // change anything to config 1.0
    if (wall_number == geomtools::geom_id::INVALID_ADDRESS && z < 0.0) {
      const double delta_z =
          std::abs(z - blockWall_Z_[side_number][gveto_wall_t::BOTTOM]) - 0.5 * blockThickness();
      if (delta_z < tolerance) {
        wall_number = gveto_wall_t::gveto_wall_t::BOTTOM;
        const std::vector<double> *block_y_ptr = nullptr;
        if (submodules_[side_t::BACK] && side_number == side_t::BACK) {
          block_y_ptr = &backCaloBlock_Y_[wall_number];
        }
        if (submodules_[side_t::FRONT] && side_number == side_t::FRONT) {
          block_y_ptr = &frontCaloBlock_Y_[wall_number];
        }
        ncolumns = block_y_ptr->size() / 2;
        if (y < 0.0) {
          first_block_y = block_y_ptr->front();
          block_delta_y = (block_y_ptr->at(ncolumns - 1) - block_y_ptr->front()) / (ncolumns - 1);
        } else {
          first_block_y = block_y_ptr->at(ncolumns);
          block_delta_y = (block_y_ptr->back() - block_y_ptr->at(ncolumns)) / (ncolumns - 1);
        }
      }
    }
    if (wall_number == geomtools::geom_id::INVALID_ADDRESS && z > 0.0) {
      const double delta_z =
          std::abs(z - blockWall_Z_[side_number][gveto_wall_t::TOP]) - 0.5 * blockThickness();
      if (delta_z < tolerance) {
        wall_number = gveto_wall_t::TOP;
        const std::vector<double> *block_y_ptr = nullptr;
        if (submodules_[side_t::BACK] && side_number == side_t::BACK) {
          block_y_ptr = &backCaloBlock_Y_[wall_number];
        }
        if (submodules_[side_t::FRONT] && side_number == side_t::FRONT) {
          block_y_ptr = &frontCaloBlock_Y_[wall_number];
        }
        ncolumns = block_y_ptr->size() / 2;
        if (y < 0.0) {
          first_block_y = frontCaloBlock_Y_[wall_number].front();
          block_delta_y = (frontCaloBlock_Y_[wall_number].at(ncolumns - 1) -
                           frontCaloBlock_Y_[wall_number].front()) /
                          (ncolumns - 1);
        } else {
          first_block_y = frontCaloBlock_Y_[wall_number].at(ncolumns);
          block_delta_y = (frontCaloBlock_Y_[wall_number].back() -
                           frontCaloBlock_Y_[wall_number].at(ncolumns)) /
                          (ncolumns - 1);
        }
      }
    }
    if (wall_number == geomtools::geom_id::INVALID_ADDRESS) {
      gid.invalidate();
      return false;
    }

    gid.set(sideAddressIndex_, side_number);
    gid.set(wallAddressIndex_, wall_number);
    const int iy = (int)(((y - first_block_y) / block_delta_y) + 0.5);
    if ((iy >= 0) && (iy < (int)ncolumns)) {
      column_number = iy;
      if (y > 0.0) {
        column_number += ncolumns;
      }
    }
    gid.set(columnAddressIndex_, column_number);
    if (gid.is_valid()) {
      // 2012-05-31 FM : use ginfo from mapping(see below)
      const geomtools::geom_info *ginfo_ptr = geomMapping_->get_geom_info_ptr(gid);
      if (ginfo_ptr == nullptr) {
        gid.invalidate();
        return false;
      }
      // 2012-05-31 FM : we check if the 'world' position is in the volume:
      geomtools::vector_3d world_position = transformModuleToWorld(in_module_position_);
      double tolerance_2 = 1.e-7 * CLHEP::mm;
      if (geomtools::mapping::check_inside(*ginfo_ptr, world_position, tolerance_2)) {
        return true;
      }
    }
    gid.invalidate();
  }
  return false;
}

void gveto_locator::tree_dump(std::ostream &out_, const std::string &title_,
                              const std::string &indent_, bool inherit_) const {
  const std::string itag = datatools::i_tree_dumpable::tags::item();
  const std::string stag = datatools::i_tree_dumpable::tags::skip_item();
  std::string indent;
  if (!indent_.empty()) {
    indent = indent_;
  }
  if (!title_.empty()) {
    out_ << indent << title_ << std::endl;
  }
  if (!is_initialized()) {
    out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Initialized        = " << isInitialized_ << std::endl;
    return;
  }
  out_ << indent << itag << "Initialized                = " << isInitialized_ << std::endl;
  out_ << indent << itag << "Logging priority threshold = "
       << datatools::logger::get_priority_label(get_logging_priority()) << std::endl;
  out_ << indent << itag << "Module number              = " << moduleNumber_ << std::endl;
  out_ << indent << itag << "Manager @                  = " << &get_geo_manager() << std::endl;
  out_ << indent << itag << "Mapping @                  = " << geomMapping_ << std::endl;
  out_ << indent << itag << "Calorimeter block type     = " << caloBlockGIDType_ << std::endl;
  out_ << indent << itag << "Block partitioned          = " << blocksArePartitioned_ << std::endl;
  if (isBlockPartitioned()) {
    out_ << indent << itag << "Block part                 = " << blockPart_ << std::endl;
  }
  out_ << indent << itag << "Module placement : " << std::endl;
  if (moduleWorldPlacement_ != nullptr) {
    moduleWorldPlacement_->tree_dump(out_, "", indent + stag);
  }
  out_ << indent << itag << "Module box : " << std::endl;
  if (moduleBox_ != nullptr) {
    moduleBox_->tree_dump(out_, "", indent + stag);
  }
  out_ << indent << itag << "Back  submodule : " << submodules_[side_t::BACK] << std::endl;
  out_ << indent << itag << "Front submodule : " << submodules_[side_t::FRONT] << std::endl;
  out_ << indent << itag << "Block box : " << std::endl;
  if (caloBlockBox_ != nullptr) {
    caloBlockBox_->tree_dump(out_, "", indent + stag);
  }
  for (size_t i = 0; i < NWALLS_PER_SIDE; ++i) {
    const std::string wall_name =
        (i == (unsigned int)gveto_wall_t::TOP) ? "top wall" : "bottom wall";
    out_ << indent << itag << "Back block X-pos on " << wall_name << " ["
         << backCaloBlock_X_[i].size() << "] = ";
    for (double j : backCaloBlock_X_[i]) {
      out_ << j / CLHEP::mm << " ";
    }
    out_ << "(mm)" << std::endl;
    out_ << indent << itag << "Front block X-pos on " << wall_name << " ["
         << frontCaloBlock_X_[i].size() << "] = ";
    for (double j : frontCaloBlock_X_[i]) {
      out_ << j / CLHEP::mm << " ";
    }
    out_ << "(mm)" << std::endl;
    out_ << indent << itag << "Back block Y-pos on " << wall_name << " ["
         << backCaloBlock_Y_[i].size() << "] = ";
    for (size_t j = 0; j < backCaloBlock_Y_[i].size(); j++) {
      if ((j < 4) || (j > backCaloBlock_Y_[i].size() - 4)) {
        out_ << backCaloBlock_Y_[i][j] / CLHEP::mm << " ";
      } else if (j == 4) {
        out_ << " ... ";
      }
    }
    out_ << "(mm)" << std::endl;
    out_ << indent << itag << "Front block Y-pos on " << wall_name << " ["
         << frontCaloBlock_Y_[i].size() << "] = ";
    for (size_t j = 0; j < frontCaloBlock_Y_[i].size(); j++) {
      if ((j < 4) || (j > frontCaloBlock_Y_[i].size() - 4)) {
        out_ << frontCaloBlock_Y_[i][j] / CLHEP::mm << " ";
      } else if (j == 4) {
        out_ << " ... ";
      }
    }
    out_ << "(mm)" << std::endl;
    out_ << indent << itag << "Back block Z-pos on " << wall_name << "  = "
         << blockWall_Z_[side_t::BACK][i] / CLHEP::mm << "(mm) \n";
    out_ << indent << itag << "Front block Z-pos on " << wall_name << " = "
         << blockWall_Z_[side_t::FRONT][i] / CLHEP::mm << "(mm) \n";
    out_ << indent << itag << "Back block window Z-pos on " << wall_name << " = "
         << blockWallWindow_Z_[side_t::BACK][i] / CLHEP::mm << "(mm) \n";
    out_ << indent << itag << "Front block window Z-pos on " << wall_name << " = "
         << blockWallWindow_Z_[side_t::FRONT][i] / CLHEP::mm << "(mm) \n";
  }

  out_ << indent << itag << "Block width              = " << blockWidth() / CLHEP::mm << "(mm)"
       << std::endl;
  out_ << indent << itag << "Block height             = " << blockHeight() / CLHEP::mm << "(mm)"
       << std::endl;
  out_ << indent << itag << "Block thickness          = " << blockThickness() / CLHEP::mm << "(mm)"
       << std::endl;
  out_ << indent << itag << "Module address GID index = " << moduleAddressIndex_ << std::endl;
  out_ << indent << itag << "Side address GID index   = " << sideAddressIndex_ << std::endl;
  out_ << indent << itag << "Wall address GID index   = " << wallAddressIndex_ << std::endl;
  out_ << indent << itag << "Column address GID index = " << columnAddressIndex_ << std::endl;
  if (isBlockPartitioned()) {
    out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Part address GID index   = " << partAddressIndex_ << std::endl;
  }
}

void gveto_locator::set_defaults_() {
  moduleNumber_ = geomtools::geom_id::INVALID_ADDRESS;
  blockPart_ = geomtools::geom_id::INVALID_ADDRESS;

  blocksArePartitioned_ = false;
  caloBlockGIDType_ = geomtools::geom_id::INVALID_TYPE;
  moduleAddressIndex_ = geomtools::geom_id::INVALID_ADDRESS;
  wallAddressIndex_ = geomtools::geom_id::INVALID_ADDRESS;
  sideAddressIndex_ = geomtools::geom_id::INVALID_ADDRESS;
  columnAddressIndex_ = geomtools::geom_id::INVALID_ADDRESS;
  partAddressIndex_ = geomtools::geom_id::INVALID_ADDRESS;

  geomMapping_ = nullptr;
  moduleWorldPlacement_ = nullptr;
  moduleBox_ = nullptr;
  caloBlockBox_ = nullptr;

  for (unsigned int i = 0; i < utils::NSIDES; i++) {
    for (unsigned int j = 0; j < NWALLS_PER_SIDE; j++) {
      datatools::invalidate(blockWall_Z_[i][j]);
      datatools::invalidate(blockWallWindow_Z_[i][j]);
    }

    backCaloBlock_X_[i].clear();
    frontCaloBlock_X_[i].clear();
    backCaloBlock_Y_[i].clear();
    frontCaloBlock_Y_[i].clear();
    submodules_[i] = false;
  }

  isInitialized_ = false;
}

void gveto_locator::construct_() {
  geomMapping_ = &get_geo_manager().get_mapping();
  const geomtools::id_mgr &idManager = get_geo_manager().get_id_mgr();

  const uint32_t moduleGIDType = idManager.get_category_type(detail::kModuleGIDCategory);
  const uint32_t trackerSubmoduleGIDType =
      idManager.get_category_type(detail::kTrackerSubmoduleGIDCategory);
  const uint32_t gvetoWrapperGIDType =
      idManager.get_category_type(detail::kGammaVetoWrapperGIDCategory);

  caloBlockGIDType_ = idManager.get_category_type(detail::kGammaVetoBlockGIDCategory);
  const geomtools::id_mgr::category_info &block_ci =
      idManager.get_category_info(detail::kGammaVetoBlockGIDCategory);

  // The get_subaddress_index member function returns an invalid index
  // rather than throwing an exception. We therefore check the subaddress
  // categories we need upfront...
  for (const std::string &subaddress :
       {std::string("module"), std::string("side"), std::string("wall"), std::string("column")}) {
    DT_THROW_IF(!block_ci.has_subaddress(subaddress), std::logic_error,
                "Category '" << detail::kGammaVetoBlockGIDCategory << "' has no subaddress '"
                             << subaddress << "'");
  }

  moduleAddressIndex_ = block_ci.get_subaddress_index("module");
  sideAddressIndex_ = block_ci.get_subaddress_index("side");
  wallAddressIndex_ = block_ci.get_subaddress_index("wall");
  columnAddressIndex_ = block_ci.get_subaddress_index("column");

  partAddressIndex_ = geomtools::geom_id::INVALID_ADDRESS;
  if (block_ci.has_subaddress("part")) {
    blocksArePartitioned_ = true;
    partAddressIndex_ = block_ci.get_subaddress_index("part");
  } else {
    blocksArePartitioned_ = false;
  }

  // Get pointers to the box and placement of the detector module
  {
    geomtools::geom_id moduleGID(moduleGIDType, moduleNumber_);
    DT_THROW_IF(!geomMapping_->validate_id(moduleGID), std::logic_error,
                "No module with ID = " << moduleGID << " !");
    const geomtools::geom_info *moduleGeomInfo = &geomMapping_->get_geom_info(moduleGID);
    const geomtools::i_shape_3d *moduleShape = &moduleGeomInfo->get_logical().get_shape();
    DT_THROW_IF(moduleShape->get_shape_name() != "box", std::logic_error,
                "Cannot extract the box shape from module with ID = " << moduleGID << " !");
    moduleBox_ = dynamic_cast<const geomtools::box *>(moduleShape);
    moduleWorldPlacement_ = &moduleGeomInfo->get_world_placement();
  }

  // Fill array indicating if the side(s) are active in this geometry
  // side variable is used later....
  geomtools::geom_id sideGID;
  sideGID.set_type(trackerSubmoduleGIDType);
  uint32_t side = geomtools::geom_id::INVALID_ADDRESS;
  for (uint32_t iside = 0; iside < utils::NSIDES; iside++) {
    sideGID.set_address(moduleNumber_, iside);
    if (geomMapping_->validate_id(sideGID)) {
      submodules_[iside] = true;
      side = iside;
    }
  }
  DT_THROW_IF(side == geomtools::geom_id::INVALID_ADDRESS, std::logic_error,
              "Cannot extract information about any tracker submodules !");

  // Get a pointer to the box shape of the calorimeter block(s)
  {
    geomtools::geom_id caloBlockBoxGID;
    caloBlockBoxGID.set_type(caloBlockGIDType_);
    // gid is module.side.wall.column:
    if (blocksArePartitioned_) {
      caloBlockBoxGID.set_address(moduleNumber_, side, 0, 0, blockPart_);
    } else {
      caloBlockBoxGID.set_address(moduleNumber_, side, 0, 0);
    }

    const geomtools::geom_info &blockBoxGeomInfo = geomMapping_->get_geom_info(caloBlockBoxGID);
    caloBlockBox_ = detail::getBlockBox(blockBoxGeomInfo);
    DT_THROW_IF(caloBlockBox_ == nullptr, std::logic_error,
                "Cannot extract the shape from block with ID = " << caloBlockBoxGID << " !");
  }

  std::vector<double> *vcx[utils::NSIDES][NWALLS_PER_SIDE];
  vcx[side_t::BACK][gveto_wall_t::TOP] = &backCaloBlock_X_[gveto_wall_t::TOP];
  vcx[side_t::BACK][gveto_wall_t::BOTTOM] = &backCaloBlock_X_[gveto_wall_t::BOTTOM];
  vcx[side_t::FRONT][gveto_wall_t::TOP] = &frontCaloBlock_X_[gveto_wall_t::TOP];
  vcx[side_t::FRONT][gveto_wall_t::BOTTOM] = &frontCaloBlock_X_[gveto_wall_t::BOTTOM];
  std::vector<double> *vcy[utils::NSIDES][NWALLS_PER_SIDE];
  vcy[side_t::BACK][gveto_wall_t::TOP] = &backCaloBlock_Y_[gveto_wall_t::TOP];
  vcy[side_t::BACK][gveto_wall_t::BOTTOM] = &backCaloBlock_Y_[gveto_wall_t::BOTTOM];
  vcy[side_t::FRONT][gveto_wall_t::TOP] = &frontCaloBlock_Y_[gveto_wall_t::TOP];
  vcy[side_t::FRONT][gveto_wall_t::BOTTOM] = &frontCaloBlock_Y_[gveto_wall_t::BOTTOM];
  for (unsigned int iside = 0; iside < utils::NSIDES; iside++) {
    if (!submodules_[iside]) {
      continue;
    }
    for (unsigned int wall = 0; wall < NWALLS_PER_SIDE; wall++) {
      size_t i_column = 0;
      vcx[iside][wall]->reserve(1);
      vcy[iside][wall]->reserve(16);
      while (true) {
        geomtools::geom_id caloBlockGID;
        caloBlockGID.set_type(caloBlockGIDType_);
        caloBlockGID.set(moduleAddressIndex_, moduleNumber_);
        caloBlockGID.set(sideAddressIndex_, iside);
        caloBlockGID.set(wallAddressIndex_, wall);
        caloBlockGID.set(columnAddressIndex_, i_column);
        if (isBlockPartitioned()) {
          caloBlockGID.set(partAddressIndex_, blockPart_);
        }
        if (!geomMapping_->validate_id(caloBlockGID)) {
          break;
        }
        const geomtools::geom_info &blockGeomInfo = geomMapping_->get_geom_info(caloBlockGID);
        const geomtools::placement &blockWorldPlacement = blockGeomInfo.get_world_placement();
        geomtools::placement blockModulePlacement;
        moduleWorldPlacement_->relocate(blockWorldPlacement, blockModulePlacement);
        const geomtools::vector_3d &translation = blockModulePlacement.get_translation();

        vcy[iside][wall]->push_back(translation.y());

        if (i_column == 0) {
          vcx[iside][wall]->push_back(translation.x());
          blockWall_Z_[iside][wall] = translation.z();

          geomtools::geom_id blockWindowGID(gvetoWrapperGIDType, moduleNumber_, iside, wall,
                                            i_column);
          const geomtools::geom_info &blockWindowGeomInfo =
              geomMapping_->get_geom_info(blockWindowGID);
          const geomtools::placement &blockWindowWorldPlacement =
              blockWindowGeomInfo.get_world_placement();
          geomtools::placement blockWindowModulePlacement;
          moduleWorldPlacement_->relocate(blockWindowWorldPlacement, blockWindowModulePlacement);
          blockWallWindow_Z_[iside][wall] = blockWindowModulePlacement.get_translation().z();
        }
        i_column++;
      }
    }
  }
}

bool gveto_locator::isBlockPartitioned() const { return blocksArePartitioned_; }

}  // end of namespace geometry

}  // end of namespace snemo

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
