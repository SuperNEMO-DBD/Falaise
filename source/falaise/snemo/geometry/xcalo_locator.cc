// falaise/snemo/geometry/xcalo_locator.cc
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
#include <falaise/snemo/geometry/xcalo_locator.h>

#include "private/categories.h"

// Standard library:
#include <stdexcept>

// Third party:
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/version_id.h>
// - Bayeux/geomtools:
#include <geomtools/box.h>
#include <geomtools/i_composite_shape_3d.h>
#include <geomtools/intersection_3d.h>
#include <geomtools/manager.h>
#include <geomtools/subtraction_3d.h>

// This project:
#include <falaise/snemo/geometry/utils.h>

namespace snemo {

namespace geometry {

// Constructor:
xcalo_locator::xcalo_locator() { set_defaults_(); }

// Constructor:
xcalo_locator::xcalo_locator(uint32_t moduleID, const geomtools::manager &geoMgr,
                             const falaise::property_set &ps) {
  set_defaults_();
  set_geo_manager(geoMgr);
  setModuleNumber(moduleID);
  initialize(ps);
}

// dtor:
xcalo_locator::~xcalo_locator() { reset(); }

void xcalo_locator::reset() { set_defaults_(); }

void xcalo_locator::initialize(const datatools::properties &config_) {
  base_locator::_basic_initialize(config_);
  DT_THROW_IF(moduleNumber_ == geomtools::geom_id::INVALID_ADDRESS, std::logic_error,
              "Missing module number ! Use the 'setModuleNumber' method before !");
  construct_();
  isInitialized_ = true;
}

bool xcalo_locator::is_initialized() const { return isInitialized_; }

bool xcalo_locator::hasSubmodule(uint32_t side) const {
  DT_THROW_IF(side >= utils::NSIDES, std::logic_error,
              "Submodule number " << side << " makes no sense !");
  return submodules_[side];
}

uint32_t xcalo_locator::getModuleNumber() const { return moduleNumber_; }

void xcalo_locator::setModuleNumber(uint32_t id) { moduleNumber_ = id; }

double xcalo_locator::blockWidth() const { return caloBlockBox_->get_x(); }

double xcalo_locator::blockHeight() const { return caloBlockBox_->get_y(); }

double xcalo_locator::blockThickness() const { return caloBlockBox_->get_z(); }

size_t xcalo_locator::numberOfSides() const { return utils::NSIDES; }

size_t xcalo_locator::numberOfWalls() const { return NWALLS_PER_SIDE; }

size_t xcalo_locator::numberOfColumns(uint32_t side, uint32_t wall) const {
  DT_THROW_IF(wall >= NWALLS_PER_SIDE, std::logic_error,
              "Invalid wall number (" << wall << ">" << NWALLS_PER_SIDE << ")!");
  if (side == (uint32_t)side_t::BACK) {
    return backCaloBlock_X_[wall].size();
  }

  if (side == (uint32_t)side_t::FRONT) {
    return frontCaloBlock_X_[wall].size();
  }

  DT_THROW(std::logic_error, "Invalid side number (" << side << ">= " << utils::NSIDES << ")!");
}

size_t xcalo_locator::numberOfRows(uint32_t side, uint32_t wall) const {
  DT_THROW_IF(wall >= NWALLS_PER_SIDE, std::logic_error,
              "Invalid wall number (" << wall << ">" << NWALLS_PER_SIDE << ")!");
  if (side == (uint32_t)side_t::BACK) {
    return backCaloBlock_Z_[wall].size();
  }

  if (side == (uint32_t)side_t::FRONT) {
    return frontCaloBlock_Z_[wall].size();
  }

  DT_THROW(std::logic_error, "Invalid side number (" << side << ">= " << utils::NSIDES << ")!");
}

uint32_t xcalo_locator::getModuleAddress(const geomtools::geom_id &gid) const {
  return gid.get(moduleAddressIndex_);
}

uint32_t xcalo_locator::getSideAddress(const geomtools::geom_id &gid) const {
  return gid.get(sideAddressIndex_);
}

uint32_t xcalo_locator::getWallAddress(const geomtools::geom_id &gid) const {
  return gid.get(wallAddressIndex_);
}

uint32_t xcalo_locator::getColumnAddress(const geomtools::geom_id &gid) const {
  return gid.get(columnAddressIndex_);
}

uint32_t xcalo_locator::getRowAddress(const geomtools::geom_id &gid) const {
  return gid.get(rowAddressIndex_);
}

uint32_t xcalo_locator::getPartAddress(const geomtools::geom_id &gid) const {
  if (isBlockPartitioned()) {
    return gid.get(partAddressIndex_);
  }
  return geomtools::geom_id::INVALID_ADDRESS;
}

bool xcalo_locator::isCaloOM(const geomtools::geom_id &gid) const {
  return gid.get_type() == caloOMGIDType_;
}

bool xcalo_locator::isCaloBlock(const geomtools::geom_id &gid) const {
  return gid.get_type() == caloBlockGIDType_;
}

bool xcalo_locator::isCaloBlockInThisModule(const geomtools::geom_id &gid) const {
  return isCaloBlock(gid) && (getModuleAddress(gid) == moduleNumber_);
}

bool xcalo_locator::isValidAddress(uint32_t side, uint32_t wall, uint32_t column,
                                   uint32_t row) const {
  if (side >= utils::NSIDES) {
    return false;
  }
  if (wall >= NWALLS_PER_SIDE) {
    return false;
  }
  if (column >= numberOfColumns(side, wall)) {
    return false;
  }
  if (row >= numberOfRows(side, wall)) {
    return false;
  }
  return true;
}

size_t xcalo_locator::countNeighbours(uint32_t side_, uint32_t wall_, uint32_t column_,
                                      uint32_t row_, uint8_t mask_) const {
  DT_THROW_IF(side_ >= utils::NSIDES, std::logic_error,
              "Invalid side number (" << side_ << ">= " << utils::NSIDES << ")!");
  DT_THROW_IF(wall_ >= NWALLS_PER_SIDE, std::logic_error,
              "Invalid wall number (" << wall_ << ">" << NWALLS_PER_SIDE << ")!");

  bool corner = false;
  bool side = false;
  const bool sides = (mask_ & grid_mask_t::SIDE) != 0;
  const bool diagonal = (mask_ & grid_mask_t::DIAG) != 0;
  const bool second = (mask_ & grid_mask_t::SECOND) != 0;
  if (second) {
    DT_LOG_NOTICE(get_logging_priority(),
                  "Looking for second order neighbour of 'xcalo' locator is not implemented !");
  }
  if (side_ == (uint32_t)side_t::BACK) {
    if ((column_ == 0) || (column_ == backCaloBlock_X_[wall_].size() - 1)) {
      if ((row_ == 0) || (row_ == backCaloBlock_Z_[wall_].size() - 1)) {
        corner = true;
      } else {
        side = true;
      }
    }
  }
  if (side_ == (uint32_t)side_t::FRONT) {
    if ((column_ == 0) || (column_ == frontCaloBlock_X_[wall_].size() - 1)) {
      if ((row_ == 0) || (row_ == frontCaloBlock_Z_[wall_].size() - 1)) {
        corner = true;
      } else {
        side = true;
      }
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

size_t xcalo_locator::countNeighbours(const geomtools::geom_id &gid, uint8_t mask) const {
  DT_THROW_IF(gid.get_depth() != 5, std::logic_error,
              "Invalid depth (" << gid.get_depth() << " != 5)!");
  DT_THROW_IF(
      gid.get(moduleAddressIndex_) != moduleNumber_, std::logic_error,
      "Invalid module number (" << gid.get(moduleAddressIndex_) << "!=" << moduleNumber_ << ")!");
  return countNeighbours(gid.get(sideAddressIndex_), gid.get(wallAddressIndex_),
                         gid.get(columnAddressIndex_), gid.get(rowAddressIndex_), mask);
}

std::vector<geomtools::geom_id> xcalo_locator::getNeighbourGIDs(uint32_t side, uint32_t wall,
                                                                uint32_t column, uint32_t row,
                                                                uint8_t mask) const {
  DT_THROW_IF(side >= utils::NSIDES, std::logic_error,
              "Invalid side number (" << side << ">= " << utils::NSIDES << ")!");
  DT_THROW_IF(wall >= NWALLS_PER_SIDE, std::logic_error,
              "Invalid wall number (" << wall << ">" << NWALLS_PER_SIDE << ")!");

  std::vector<geomtools::geom_id> ids_;
  ids_.reserve(8);

  const bool sides = (mask & grid_mask_t::SIDE) != 0;
  const bool diagonal = (mask & grid_mask_t::DIAG) != 0;
  const bool second = (mask & grid_mask_t::SECOND) != 0;
  if (second) {
    DT_LOG_NOTICE(get_logging_priority(),
                  "Looking for second order neighbour of 'xcalo' locator is not implemented !");
  }

  // prepare neighbour GID :
  geomtools::geom_id gid;
  gid.set_type(caloBlockGIDType_);
  gid.set(moduleAddressIndex_, moduleNumber_);
  gid.set(wallAddressIndex_, wall);
  gid.set(sideAddressIndex_, side);
  gid.set(columnAddressIndex_, geomtools::geom_id::INVALID_ADDRESS);
  gid.set(rowAddressIndex_, geomtools::geom_id::INVALID_ADDRESS);
  if (isBlockPartitioned()) {
    gid.set(partAddressIndex_, blockPart_);
  }
  // back
  if (side == (uint32_t)side_t::BACK) {
    DT_THROW_IF(
        column >= backCaloBlock_X_[wall].size(), std::logic_error,
        "Invalid column number (" << column << ">" << backCaloBlock_X_[wall].size() - 1 << ")!");
    DT_THROW_IF(row >= backCaloBlock_Z_[wall].size(), std::logic_error,
                "Invalid row number (" << row << ">" << backCaloBlock_Z_[wall].size() - 1 << ")!");
    if (sides && (column < (backCaloBlock_X_[wall].size() - 1))) {
      /*  C-1 C C+1
       *  [ ][ ][ ]
       *  [ ][.][x]
       *  [ ][ ][ ]
       */
      gid.set(columnAddressIndex_, column + 1);
      gid.set(rowAddressIndex_, row);
      ids_.push_back(gid);
    }
    if (sides && (column > 0)) {
      /*  C-1 C C+1
       *  [ ][ ][ ] R+1
       *  [x][.][ ] R
       *  [ ][ ][ ] R-1
       */
      gid.set(columnAddressIndex_, column - 1);
      gid.set(rowAddressIndex_, row);
      ids_.push_back(gid);
    }
    if (sides && (row < (backCaloBlock_Z_[wall].size() - 1))) {
      /*  C-1 C C+1
       *  [ ][x][ ] R+1
       *  [ ][.][ ] R
       *  [ ][ ][ ] R-1
       */
      gid.set(columnAddressIndex_, column);
      gid.set(rowAddressIndex_, row + 1);
      ids_.push_back(gid);
    }
    if (sides && (row > 0)) {
      /*  C-1 C C+1
       *  [ ][ ][ ] R+1
       *  [ ][.][ ] R
       *  [ ][x][ ] R-1
       */
      gid.set(columnAddressIndex_, column);
      gid.set(rowAddressIndex_, row - 1);
      ids_.push_back(gid);
    }

    if (diagonal && (column < (backCaloBlock_X_[wall].size() - 1)) &&
        (row < (backCaloBlock_Z_[wall].size() - 1))) {
      /*  C-1 C C+1
       *  [ ][ ][x] R+1
       *  [ ][.][ ] R
       *  [ ][ ][ ] R-1
       */
      gid.set(columnAddressIndex_, column + 1);
      gid.set(rowAddressIndex_, row + 1);
      ids_.push_back(gid);
    }

    if (diagonal && (column > 0) && (row < (backCaloBlock_Z_[wall].size() - 1))) {
      /*  C-1 C C+1
       *  [x][ ][ ] R+1
       *  [ ][.][ ] R
       *  [ ][ ][ ] R-1
       */
      gid.set(columnAddressIndex_, column - 1);
      gid.set(rowAddressIndex_, row + 1);
      ids_.push_back(gid);
    }

    if (diagonal && (column > 0) && (row > 0)) {
      /*  C-1 C C+1
       *  [ ][ ][ ] R+1
       *  [ ][.][ ] R
       *  [x][ ][ ] R-1
       */
      gid.set(columnAddressIndex_, column - 1);
      gid.set(rowAddressIndex_, row - 1);
      ids_.push_back(gid);
    }

    if (diagonal && (column < (backCaloBlock_X_[wall].size() - 1)) && (row > 0)) {
      /*  C-1 C C+1
       *  [ ][ ][ ] R+1
       *  [ ][.][ ] R
       *  [ ][ ][x] R-1
       */
      gid.set(columnAddressIndex_, column + 1);
      gid.set(rowAddressIndex_, row - 1);
      ids_.push_back(gid);
    }
  }

  // front:
  if (side == (uint32_t)side_t::FRONT) {
    DT_THROW_IF(
        column >= frontCaloBlock_X_[wall].size(), std::logic_error,
        "Invalid column number (" << column << ">" << frontCaloBlock_X_[wall].size() - 1 << ")!");
    DT_THROW_IF(row >= frontCaloBlock_Z_[wall].size(), std::logic_error,
                "Invalid row number (" << row << ">" << frontCaloBlock_Z_[wall].size() - 1 << ")!");
    if (sides && (column < (frontCaloBlock_X_[wall].size() - 1))) {
      /*  C-1 C C+1
       *  [ ][ ][ ]
       *  [ ][.][x]
       *  [ ][ ][ ]
       */
      gid.set(columnAddressIndex_, column + 1);
      gid.set(rowAddressIndex_, row);
      ids_.push_back(gid);
    }
    if (sides && (column > 0)) {
      /*  C-1 C C+1
       *  [ ][ ][ ] R+1
       *  [x][.][ ] R
       *  [ ][ ][ ] R-1
       */
      gid.set(columnAddressIndex_, column - 1);
      gid.set(rowAddressIndex_, row);
      ids_.push_back(gid);
    }
    if (sides && (row < (frontCaloBlock_Z_[wall].size() - 1))) {
      /*  C-1 C C+1
       *  [ ][x][ ] R+1
       *  [ ][.][ ] R
       *  [ ][ ][ ] R-1
       */
      gid.set(columnAddressIndex_, column);
      gid.set(rowAddressIndex_, row + 1);
      ids_.push_back(gid);
    }
    if (sides && (row > 0)) {
      /*  C-1 C C+1
       *  [ ][ ][ ] R+1
       *  [ ][.][ ] R
       *  [ ][x][ ] R-1
       */
      gid.set(columnAddressIndex_, column);
      gid.set(rowAddressIndex_, row - 1);
      ids_.push_back(gid);
    }

    if (diagonal && (column < (frontCaloBlock_X_[wall].size() - 1)) &&
        (row < (frontCaloBlock_Z_[wall].size() - 1))) {
      /*  C-1 C C+1
       *  [ ][ ][x] R+1
       *  [ ][.][ ] R
       *  [ ][ ][ ] R-1
       */
      gid.set(columnAddressIndex_, column + 1);
      gid.set(rowAddressIndex_, row + 1);
      ids_.push_back(gid);
    }

    if (diagonal && (column > 0) && (row < (frontCaloBlock_Z_[wall].size() - 1))) {
      /*  C-1 C C+1
       *  [x][ ][ ] R+1
       *  [ ][.][ ] R
       *  [ ][ ][ ] R-1
       */
      gid.set(columnAddressIndex_, column - 1);
      gid.set(rowAddressIndex_, row + 1);
      ids_.push_back(gid);
    }

    if (diagonal && (column > 0) && (row > 0)) {
      /*  C-1 C C+1
       *  [ ][ ][ ] R+1
       *  [ ][.][ ] R
       *  [x][ ][ ] R-1
       */
      gid.set(columnAddressIndex_, column - 1);
      gid.set(rowAddressIndex_, row - 1);
      ids_.push_back(gid);
    }

    if (diagonal && (column < (frontCaloBlock_X_[wall].size() - 1)) && (row > 0)) {
      /*  C-1 C C+1
       *  [ ][ ][ ] R+1
       *  [ ][.][ ] R
       *  [ ][ ][x] R-1
       */
      gid.set(columnAddressIndex_, column + 1);
      gid.set(rowAddressIndex_, row - 1);
      ids_.push_back(gid);
    }
  }
  return ids_;
}

std::vector<geomtools::geom_id> xcalo_locator::getNeighbourGIDs(const geomtools::geom_id &gid,
                                                                uint8_t mask) const {
  DT_THROW_IF(gid.get_depth() != 5, std::logic_error,
              "Invalid depth (" << gid.get_depth() << " != 5)!");
  DT_THROW_IF(
      gid.get(moduleAddressIndex_) != moduleNumber_, std::logic_error,
      "Invalid module number (" << gid.get(moduleAddressIndex_) << "!=" << moduleNumber_ << ")!");
  return getNeighbourGIDs(gid.get(sideAddressIndex_), gid.get(wallAddressIndex_),
                          gid.get(columnAddressIndex_), gid.get(rowAddressIndex_), mask);
}

double xcalo_locator::getYCoordOfWall(uint32_t side, uint32_t wall) const {
  DT_THROW_IF(side >= utils::NSIDES, std::logic_error,
              "Invalid side number (" << side << ">" << utils::NSIDES << ")!");
  DT_THROW_IF(wall >= NWALLS_PER_SIDE, std::logic_error,
              "Invalid wall number (" << wall << ">" << NWALLS_PER_SIDE << ")!");
  return blockWall_Y_[side][wall];
}

double xcalo_locator::getYCoordOfWallWindow(uint32_t side, uint32_t wall) const {
  DT_THROW_IF(side >= utils::NSIDES, std::logic_error,
              "Invalid side number (" << side << ">" << utils::NSIDES << ")!");
  DT_THROW_IF(wall >= NWALLS_PER_SIDE, std::logic_error,
              "Invalid wall number (" << wall << ">" << NWALLS_PER_SIDE << ")!");
  return blockWallWindow_Y_[side][wall];
}

double xcalo_locator::getXCoordOfColumn(uint32_t side, uint32_t wall, uint32_t column) const {
  DT_THROW_IF(side >= utils::NSIDES, std::logic_error,
              "Invalid side number (" << side << ">" << utils::NSIDES << ")!");
  DT_THROW_IF(wall >= NWALLS_PER_SIDE, std::logic_error,
              "Invalid wall number (" << wall << ">" << NWALLS_PER_SIDE << ")!");

  if (side == (uint32_t)side_t::BACK) {
    DT_THROW_IF(
        column >= backCaloBlock_X_[wall].size(), std::logic_error,
        "Invalid column number (" << column << ">" << backCaloBlock_X_[wall].size() - 1 << ")!");
    return backCaloBlock_X_[wall][column];
  }
  DT_THROW_IF(
      column >= frontCaloBlock_X_[wall].size(), std::logic_error,
      "Invalid column number (" << column << ">" << frontCaloBlock_X_[wall].size() - 1 << ")!");
  return frontCaloBlock_X_[wall][column];
}

double xcalo_locator::getZCoordOfRow(uint32_t side, uint32_t wall, uint32_t row) const {
  DT_THROW_IF(side >= utils::NSIDES, std::logic_error,
              "Invalid side number (" << side << ">" << utils::NSIDES << ")!");
  DT_THROW_IF(wall >= NWALLS_PER_SIDE, std::logic_error,
              "Invalid wall number (" << wall << ">" << NWALLS_PER_SIDE << ")!");

  if (side == (uint32_t)side_t::BACK) {
    DT_THROW_IF(row >= backCaloBlock_Z_[wall].size(), std::logic_error,
                "Invalid row number (" << row << ">" << backCaloBlock_Z_[wall].size() - 1 << ")!");
    return backCaloBlock_Z_[wall][row];
  }
  DT_THROW_IF(row >= frontCaloBlock_Z_[wall].size(), std::logic_error,
              "Invalid row number (" << row << ">" << frontCaloBlock_Z_[wall].size() - 1 << ")!");
  return frontCaloBlock_Z_[wall][row];
}

geomtools::vector_3d xcalo_locator::getBlockPosition(uint32_t side, uint32_t wall, uint32_t column,
                                                     uint32_t row) const {
  return geomtools::vector_3d(getXCoordOfColumn(side, wall, column), getYCoordOfWall(side, wall),
                              getZCoordOfRow(side, wall, row));
}

geomtools::vector_3d xcalo_locator::getBlockPosition(const geomtools::geom_id &gid) const {
  DT_THROW_IF(gid.get_depth() != 5, std::logic_error,
              "Invalid depth (" << gid.get_depth() << " != 5)!");
  DT_THROW_IF(
      gid.get(moduleAddressIndex_) != moduleNumber_, std::logic_error,
      "Invalid module number (" << gid.get(moduleAddressIndex_) << "!=" << moduleNumber_ << ")!");
  return getBlockPosition(gid.get(sideAddressIndex_), gid.get(wallAddressIndex_),
                          gid.get(columnAddressIndex_), gid.get(rowAddressIndex_));
}

geomtools::vector_3d xcalo_locator::getBlockWindowPosition(uint32_t side, uint32_t wall,
                                                           uint32_t column, uint32_t row) const {
  return geomtools::vector_3d(getXCoordOfColumn(side, wall, column),
                              getYCoordOfWallWindow(side, wall), getZCoordOfRow(side, wall, row));
}

geomtools::vector_3d xcalo_locator::transformWorldToModule(
    const geomtools::vector_3d &worldPoint) const {
  return moduleWorldPlacement_->mother_to_child(worldPoint);
}

geomtools::vector_3d xcalo_locator::transformModuleToWorld(
    const geomtools::vector_3d &modulePoint) const {
  return moduleWorldPlacement_->child_to_mother(modulePoint);
}

bool xcalo_locator::isPointInModule(const geomtools::vector_3d &modulePoint,
                                    double tolerance) const {
  return moduleBox_->is_inside(modulePoint, tolerance);
}

bool xcalo_locator::isPointInBlock(const geomtools::vector_3d &modulePoint, uint32_t side,
                                   uint32_t wall, uint32_t column, uint32_t row,
                                   double tolerance) const {
  geomtools::vector_3d to_block_pos = modulePoint;
  to_block_pos -= getBlockPosition(side, wall, column, row);
  // here one misses one transformation step (rotation) but it is ok :
  return caloBlockBox_->is_inside(to_block_pos, tolerance);
}

bool xcalo_locator::isWorldPointInModule(const geomtools::vector_3d &worldPoint,
                                         double tolerance) const {
  geomtools::vector_3d in_module_position = this->transformWorldToModule(worldPoint);
  return isPointInModule(in_module_position, tolerance);
}

bool xcalo_locator::isWorldPointInBlock(const geomtools::vector_3d &worldPoint, uint32_t side,
                                        uint32_t wall, uint32_t column, uint32_t row,
                                        double tolerance) const {
  geomtools::vector_3d in_module_position = this->transformWorldToModule(worldPoint);
  return isPointInBlock(in_module_position, side, wall, column, row, tolerance);
}

bool xcalo_locator::findBlockGID(const geomtools::vector_3d &worldPoint, geomtools::geom_id &gid,
                                 double tolerance) const {
  return find_geom_id(worldPoint, caloBlockGIDType_, gid, tolerance);
}

bool xcalo_locator::find_geom_id(const geomtools::vector_3d &world_position_, int type_,
                                 geomtools::geom_id &gid_, double tolerance_) const {
  DT_THROW_IF(type_ != (int)caloBlockGIDType_, std::logic_error,
              "Only works with type " << caloBlockGIDType_ << " for now !");

  gid_.invalidate();

  // Compute the position in the module coordinate system :
  geomtools::vector_3d in_module_position_ = this->transformWorldToModule(world_position_);

  // First check if it is inside the module :
  if (!moduleBox_->is_inside(in_module_position_, tolerance_)) {
    // Not in this module :
    return false;
  }

  double the_tolerance = tolerance_;
  if (the_tolerance == GEOMTOOLS_PROPER_TOLERANCE) {
    the_tolerance = caloBlockBox_->get_tolerance();
  }

  geomtools::geom_id &gid = gid_;
  gid.reset();
  uint32_t side_number(geomtools::geom_id::INVALID_ADDRESS);
  uint32_t wall_number(geomtools::geom_id::INVALID_ADDRESS);
  uint32_t column_number(geomtools::geom_id::INVALID_ADDRESS);
  uint32_t row_number(geomtools::geom_id::INVALID_ADDRESS);
  const double z = in_module_position_.z();
  const double zlim = 1000 * CLHEP::m;
  if (std::abs(z) < zlim) {
    gid.set_type(caloBlockGIDType_);
    gid.set(moduleAddressIndex_, moduleNumber_);
    if (isBlockPartitioned()) {
      gid.set(partAddressIndex_, blockPart_);
    }
    const double x = in_module_position_.x();
    const double y = in_module_position_.y();
    double first_block_x;
    double block_delta_x;
    double first_block_z;
    double block_delta_z;
    size_t ncolumns = 0;
    size_t nrows = 0;

    // Find the side:
    if (side_number == geomtools::geom_id::INVALID_ADDRESS && submodules_[side_t::BACK]) {
      double xmax0 =
          backCaloBlock_X_[xcalo_wall_t::LEFT].front() + 0.5 * blockWidth() + the_tolerance;
      double xmax1 =
          backCaloBlock_X_[xcalo_wall_t::RIGHT].front() + 0.5 * blockWidth() + the_tolerance;
      double xmax = std::max(xmax0, xmax1);
      if (x <= xmax) {
        side_number = side_t::BACK;
      }
    }
    if (side_number == geomtools::geom_id::INVALID_ADDRESS && submodules_[side_t::FRONT]) {
      double xmin0 =
          frontCaloBlock_X_[xcalo_wall_t::LEFT].front() - 0.5 * blockWidth() - the_tolerance;
      double xmin1 =
          frontCaloBlock_X_[xcalo_wall_t::RIGHT].front() - 0.5 * blockWidth() - the_tolerance;
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
    if (wall_number == geomtools::geom_id::INVALID_ADDRESS && y < 0.0) {
      const double delta_y =
          std::abs(y - blockWall_Y_[side_number][xcalo_wall_t::LEFT]) - 0.5 * blockThickness();
      if (delta_y <= the_tolerance) {
        wall_number = xcalo_wall_t::LEFT;
        const std::vector<double> *block_x_ptr = nullptr;
        const std::vector<double> *block_z_ptr = nullptr;
        if (submodules_[side_t::BACK] && side_number == side_t::BACK) {
          block_x_ptr = &backCaloBlock_X_[wall_number];
          block_z_ptr = &backCaloBlock_Z_[wall_number];
        }
        if (submodules_[side_t::FRONT] && side_number == side_t::FRONT) {
          block_x_ptr = &frontCaloBlock_X_[wall_number];
          block_z_ptr = &frontCaloBlock_Z_[wall_number];
        }
        ncolumns = block_x_ptr->size();
        nrows = block_z_ptr->size();
        first_block_x = block_x_ptr->front();
        block_delta_x = (block_x_ptr->back() - block_x_ptr->front()) / (block_x_ptr->size() - 1);
        first_block_z = block_z_ptr->front();
        block_delta_z = (block_z_ptr->back() - block_z_ptr->front()) / (block_z_ptr->size() - 1);
      }
    }
    if (wall_number == geomtools::geom_id::INVALID_ADDRESS && y > 0.0) {
      const double delta_y =
          std::abs(y - blockWall_Y_[side_number][xcalo_wall_t::RIGHT]) - 0.5 * blockThickness();
      if (delta_y <= the_tolerance) {
        wall_number = xcalo_wall_t::RIGHT;
        const std::vector<double> *block_x_ptr = nullptr;
        const std::vector<double> *block_z_ptr = nullptr;
        if (submodules_[side_t::BACK] && side_number == side_t::BACK) {
          block_x_ptr = &backCaloBlock_X_[wall_number];
          block_z_ptr = &backCaloBlock_Z_[wall_number];
        }
        if (submodules_[side_t::FRONT] && side_number == side_t::FRONT) {
          block_x_ptr = &frontCaloBlock_X_[wall_number];
          block_z_ptr = &frontCaloBlock_Z_[wall_number];
        }
        ncolumns = block_x_ptr->size();
        nrows = block_z_ptr->size();
        first_block_x = block_x_ptr->front();
        block_delta_x = (block_x_ptr->back() - block_x_ptr->front()) / (block_x_ptr->size() - 1);
        first_block_z = block_z_ptr->front();
        block_delta_z = (block_z_ptr->back() - block_z_ptr->front()) / (block_z_ptr->size() - 1);
      }
    }
    if (wall_number == geomtools::geom_id::INVALID_ADDRESS) {
      gid.invalidate();
      return false;
    }

    gid.set(sideAddressIndex_, side_number);
    gid.set(wallAddressIndex_, wall_number);
    const int ix = (int)(((x - first_block_x) / block_delta_x) + 0.5);
    if ((ix >= 0) && (ix < (int)ncolumns)) {
      column_number = ix;
    }
    gid.set(columnAddressIndex_, column_number);
    const int iz = (int)(((z - first_block_z) / block_delta_z) + 0.5);
    if ((iz >= 0) && (iz < (int)nrows)) {
      row_number = iz;
    }
    gid.set(rowAddressIndex_, row_number);
    if (gid.is_valid()) {
      // 2012-05-31 FM : use ginfo from mapping (see below)
      const geomtools::geom_info *ginfo_ptr = geomMapping_->get_geom_info_ptr(gid);
      if (ginfo_ptr == nullptr) {
        gid.invalidate();
        return false;
      }
      // 2012-05-31 FM : we check if the 'world' position is in the volume:
      geomtools::vector_3d world_position = transformModuleToWorld(in_module_position_);
      double the_tolerance2 = 1.e-7 * CLHEP::mm;
      if (geomtools::mapping::check_inside(*ginfo_ptr, world_position, the_tolerance2)) {
        return true;
      }
    }
    gid.invalidate();
  }
  return false;
}

void xcalo_locator::tree_dump(std::ostream &out, const std::string &title,
                              const std::string &indent, bool inherit) const {
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
  out << indent << itag << "Calorimeter block type     = " << caloBlockGIDType_ << std::endl;
  out << indent << itag << "Block partitioned          = " << blocksArePartitioned_ << std::endl;
  if (isBlockPartitioned()) {
    out << indent << itag << "Block part                 = " << blockPart_ << std::endl;
  }
  out << indent << itag << "Module placement : " << std::endl;
  if (moduleWorldPlacement_ != nullptr) {
    moduleWorldPlacement_->tree_dump(out, "", indent + stag);
  }
  out << indent << itag << "Module box : " << std::endl;
  if (moduleBox_ != nullptr) {
    moduleBox_->tree_dump(out, "", indent + stag);
  }
  out << indent << itag << "Back  submodule : " << submodules_[side_t::BACK] << std::endl;
  out << indent << itag << "Front submodule : " << submodules_[side_t::FRONT] << std::endl;
  out << indent << itag << "Block box : " << std::endl;
  if (caloBlockBox_ != nullptr) {
    caloBlockBox_->tree_dump(out, "", indent + stag);
  }
  for (size_t i = 0; i < NWALLS_PER_SIDE; ++i) {
    const std::string wall_name = (i == (uint32_t)xcalo_wall_t::LEFT) ? "left wall" : "right wall";
    out << indent << itag << "Back  block X-pos on " << wall_name << " ["
        << backCaloBlock_X_[i].size() << "] = ";
    for (double j : backCaloBlock_X_[i]) {
      out << j / CLHEP::mm << " ";
    }
    out << " (mm)" << std::endl;
    out << indent << itag << "Front block X-pos on " << wall_name << " ["
        << frontCaloBlock_X_[i].size() << "] = ";
    for (double j : frontCaloBlock_X_[i]) {
      out << j / CLHEP::mm << " ";
    }
    out << " (mm)" << std::endl;
    out << indent << itag << "Back  block Y-pos on " << wall_name << "  = "
        << blockWall_Y_[side_t::BACK][i] / CLHEP::mm << " (mm) \n";
    out << indent << itag << "Front block Y-pos on " << wall_name << " = "
        << blockWall_Y_[side_t::FRONT][i] / CLHEP::mm << " (mm) \n";
    out << indent << itag << "Back  block window Y-pos on " << wall_name << " = "
        << blockWallWindow_Y_[side_t::BACK][i] / CLHEP::mm << " (mm) \n";
    out << indent << itag << "Front block window Y-pos on " << wall_name << " = "
        << blockWallWindow_Y_[side_t::FRONT][i] / CLHEP::mm << " (mm) \n";
    out << indent << itag << "Back  block Z-pos on " << wall_name << " ["
        << backCaloBlock_Z_[i].size() << "] = ";
    for (size_t j = 0; j < backCaloBlock_Z_[i].size(); j++) {
      if ((j < 4) || (j > backCaloBlock_Z_[i].size() - 4)) {
        out << backCaloBlock_Z_[i][j] / CLHEP::mm << " ";
      } else if (j == 4) {
        out << " ... ";
      }
    }
    out << " (mm)" << std::endl;
    out << indent << itag << "Front  block Z-pos on " << wall_name << " ["
        << frontCaloBlock_Z_[i].size() << "] = ";
    for (size_t j = 0; j < frontCaloBlock_Z_[i].size(); j++) {
      if ((j < 4) || (j > frontCaloBlock_Z_[i].size() - 4)) {
        out << frontCaloBlock_Z_[i][j] / CLHEP::mm << " ";
      } else if (j == 4) {
        out << " ... ";
      }
    }
    out << " (mm)" << std::endl;
  }

  out << indent << itag << "Block width              = " << blockWidth() / CLHEP::mm << " (mm)"
      << std::endl;
  out << indent << itag << "Block height             = " << blockHeight() / CLHEP::mm << " (mm)"
      << std::endl;
  out << indent << itag << "Block thickness          = " << blockThickness() / CLHEP::mm << " (mm)"
      << std::endl;
  out << indent << itag << "Module address GID index = " << moduleAddressIndex_ << std::endl;
  out << indent << itag << "Side   address GID index = " << sideAddressIndex_ << std::endl;
  out << indent << itag << "Wall   address GID index = " << wallAddressIndex_ << std::endl;
  out << indent << itag << "Column address GID index = " << columnAddressIndex_ << std::endl;
  out << indent;
  if (isBlockPartitioned()) {
    out << itag;
  } else {
    out << datatools::i_tree_dumpable::inherit_tag(inherit);
  }
  out << "Row    address GID index = " << rowAddressIndex_ << std::endl;
  if (isBlockPartitioned()) {
    out << indent << datatools::i_tree_dumpable::inherit_tag(inherit)
        << "Part   address GID index = " << partAddressIndex_ << std::endl;
  }
}

void xcalo_locator::set_defaults_() {
  moduleNumber_ = geomtools::geom_id::INVALID_ADDRESS;
  blockPart_ = geomtools::geom_id::INVALID_ADDRESS;

  blocksArePartitioned_ = false;
  caloBlockGIDType_ = geomtools::geom_id::INVALID_TYPE;
  moduleAddressIndex_ = geomtools::geom_id::INVALID_ADDRESS;
  wallAddressIndex_ = geomtools::geom_id::INVALID_ADDRESS;
  sideAddressIndex_ = geomtools::geom_id::INVALID_ADDRESS;
  columnAddressIndex_ = geomtools::geom_id::INVALID_ADDRESS;
  rowAddressIndex_ = geomtools::geom_id::INVALID_ADDRESS;
  partAddressIndex_ = geomtools::geom_id::INVALID_ADDRESS;

  geomMapping_ = nullptr;
  moduleWorldPlacement_ = nullptr;
  moduleBox_ = nullptr;
  caloBlockBox_ = nullptr;

  for (size_t i = 0; i < utils::NSIDES; i++) {
    for (size_t j = 0; j < NWALLS_PER_SIDE; j++) {
      datatools::invalidate(blockWall_Y_[i][j]);
      datatools::invalidate(blockWallWindow_Y_[i][j]);
    }
    submodules_[i] = false;
  }

  for (size_t i = 0; i < NWALLS_PER_SIDE; i++) {
    backCaloBlock_Z_[i].clear();
    backCaloBlock_X_[i].clear();
    frontCaloBlock_Z_[i].clear();
    frontCaloBlock_X_[i].clear();
  }

  isInitialized_ = false;
}

void xcalo_locator::construct_() {
  geomMapping_ = &get_geo_manager().get_mapping();
  const geomtools::id_mgr &idManager = get_geo_manager().get_id_mgr();

  uint32_t moduleGIDType = idManager.get_category_type(detail::kModuleGIDCategory);
  uint32_t trackerSubmoduleGIDType = idManager.get_category_type(detail::kTrackerSubmoduleGIDCategory);
  uint32_t caloBlockWrapperGIDType = idManager.get_category_type(detail::kXCaloWrapperGIDCategory);

  // Analyse the layout of the calo block's geometry category :
  caloBlockGIDType_ = idManager.get_category_type(detail::kXCaloBlockGIDCategory);
  const geomtools::id_mgr::category_info &block_ci =
      idManager.get_category_info(detail::kXCaloBlockGIDCategory);

  // The get_subaddress_index member function returns an invalid index
  // rather than throwing an exception. We therefore check the subaddress
  // categories we need upfront...
  for (const std::string &subaddress : {"module", "side", "wall", "column", "row"}) {
    DT_THROW_IF(
        !block_ci.has_subaddress(subaddress), std::logic_error,
        "Category '" << detail::kXCaloBlockGIDCategory << "' has no subaddress '" << subaddress << "'");
  }
  moduleAddressIndex_ = block_ci.get_subaddress_index("module");
  sideAddressIndex_ = block_ci.get_subaddress_index("side");
  wallAddressIndex_ = block_ci.get_subaddress_index("wall");
  columnAddressIndex_ = block_ci.get_subaddress_index("column");
  rowAddressIndex_ = block_ci.get_subaddress_index("row");

  partAddressIndex_ = geomtools::geom_id::INVALID_ADDRESS;
  if (block_ci.has_subaddress("part")) {
    blocksArePartitioned_ = true;
    partAddressIndex_ = block_ci.get_subaddress_index("part");
  } else {
    blocksArePartitioned_ = false;
  }

  // Get the box and placement for the detector module
  {
    geomtools::geom_id module_gid(moduleGIDType, moduleNumber_);
    DT_THROW_IF(!geomMapping_->validate_id(module_gid), std::logic_error,
                "No module with ID = " << module_gid << " !");
    const geomtools::geom_info *moduleGeomInfo = &geomMapping_->get_geom_info(module_gid);

    const geomtools::i_shape_3d *a_shape = &moduleGeomInfo->get_logical().get_shape();
    DT_THROW_IF(a_shape->get_shape_name() != "box", std::logic_error,
                "Cannot extract the box shape from module with ID = " << module_gid << " !");
    moduleBox_ = dynamic_cast<const geomtools::box *>(a_shape);
    moduleWorldPlacement_ = &moduleGeomInfo->get_world_placement();
  }

  // Fill the array indicating if a side is active.
  // side variable is used later...
  geomtools::geom_id side_gid;
  side_gid.set_type(trackerSubmoduleGIDType);
  uint32_t side = geomtools::geom_id::INVALID_ADDRESS;
  for (size_t iside = 0; iside < utils::NSIDES; iside++) {
    side_gid.set_address(moduleNumber_, iside);
    if (geomMapping_->validate_id(side_gid)) {
      submodules_[iside] = true;
      side = iside;
    }
  }
  DT_THROW_IF(side == geomtools::geom_id::INVALID_ADDRESS, std::logic_error,
              "Cannot extract information about any tracker submodules !");

  // Get the box shape for the calorimeter block(s)
  {
    geomtools::geom_id block_gid;
    block_gid.set_type(caloBlockGIDType_);
    if (isBlockPartitioned()) {
      block_gid.set_address(moduleNumber_, side, 0, 0, blockPart_);
    } else {
      block_gid.set_address(moduleNumber_, side, 0, 0, 0);
    }

    const geomtools::geom_info &block_ginfo = geomMapping_->get_geom_info(block_gid);
    caloBlockBox_ = detail::getBlockBox(block_ginfo);
    DT_THROW_IF(caloBlockBox_ == nullptr, std::logic_error,
                "Cannot extract the shape from block with ID = " << block_gid << " !");
  }

  std::vector<double> *vcx[utils::NSIDES][NWALLS_PER_SIDE];
  vcx[side_t::BACK][xcalo_wall_t::LEFT] = &backCaloBlock_X_[xcalo_wall_t::LEFT];
  vcx[side_t::BACK][xcalo_wall_t::RIGHT] = &backCaloBlock_X_[xcalo_wall_t::RIGHT];
  vcx[side_t::FRONT][xcalo_wall_t::LEFT] = &frontCaloBlock_X_[xcalo_wall_t::LEFT];
  vcx[side_t::FRONT][xcalo_wall_t::RIGHT] = &frontCaloBlock_X_[xcalo_wall_t::RIGHT];
  for (size_t local_side = 0; local_side < utils::NSIDES; ++local_side) {
    if (!submodules_[local_side]) {
      continue;
    }
    for (size_t wall = 0; wall < NWALLS_PER_SIDE; wall++) {
      size_t i_column = 0;
      vcx[local_side][wall]->reserve(2);
      while (true) {
        geomtools::geom_id blockGID;
        blockGID.set_type(caloBlockGIDType_);
        blockGID.set(moduleAddressIndex_, moduleNumber_);
        blockGID.set(sideAddressIndex_, local_side);
        blockGID.set(wallAddressIndex_, wall);
        blockGID.set(columnAddressIndex_, i_column);
        blockGID.set(rowAddressIndex_, 0);
        if (isBlockPartitioned()) {
          blockGID.set(partAddressIndex_, blockPart_);
        }
        if (!geomMapping_->validate_id(blockGID)) {
          break;
        }
        const geomtools::geom_info &blockGeomInfo = geomMapping_->get_geom_info(blockGID);
        const geomtools::placement &blockWorldPlacement = blockGeomInfo.get_world_placement();
        geomtools::placement blockModulePlacement;
        moduleWorldPlacement_->relocate(blockWorldPlacement, blockModulePlacement);
        const geomtools::vector_3d translation = blockModulePlacement.get_translation();

        vcx[local_side][wall]->push_back(translation.x());

        if (i_column == 0) {
          blockWall_Y_[local_side][wall] = translation.y();

          geomtools::geom_id blockWindowGID(caloBlockWrapperGIDType, moduleNumber_, local_side,
                                            wall, i_column, 0);
          const geomtools::geom_info &blockWindowGeomInfo =
              geomMapping_->get_geom_info(blockWindowGID);
          const geomtools::placement &blockWindowWorldPlacement =
              blockWindowGeomInfo.get_world_placement();
          geomtools::placement blockWindowModulePlacement;
          moduleWorldPlacement_->relocate(blockWindowWorldPlacement, blockWindowModulePlacement);
          blockWallWindow_Y_[local_side][wall] = blockWindowModulePlacement.get_translation().y();
        }
        i_column++;
      }
    }
  }

  std::vector<double> *vrz[utils::NSIDES][NWALLS_PER_SIDE];
  vrz[side_t::BACK][xcalo_wall_t::LEFT] = &backCaloBlock_Z_[xcalo_wall_t::LEFT];
  vrz[side_t::BACK][xcalo_wall_t::RIGHT] = &backCaloBlock_Z_[xcalo_wall_t::RIGHT];
  vrz[side_t::FRONT][xcalo_wall_t::LEFT] = &frontCaloBlock_Z_[xcalo_wall_t::LEFT];
  vrz[side_t::FRONT][xcalo_wall_t::RIGHT] = &frontCaloBlock_Z_[xcalo_wall_t::RIGHT];
  for (size_t iside = 0; iside < utils::NSIDES; iside++) {
    if (!submodules_[iside]) {
      continue;
    }
    for (size_t wall = 0; wall < NWALLS_PER_SIDE; wall++) {
      size_t i_row = 0;
      vrz[iside][wall]->reserve(16);
      while (true) {
        geomtools::geom_id blockGID;
        blockGID.set_type(caloBlockGIDType_);
        blockGID.set(moduleAddressIndex_, moduleNumber_);
        blockGID.set(sideAddressIndex_, iside);
        blockGID.set(wallAddressIndex_, wall);
        blockGID.set(columnAddressIndex_, 0);
        blockGID.set(rowAddressIndex_, i_row);
        if (isBlockPartitioned()) {
          blockGID.set(partAddressIndex_, blockPart_);
        }
        if (!geomMapping_->validate_id(blockGID)) {
          break;
        }
        const geomtools::geom_info &blockGeomInfo = geomMapping_->get_geom_info(blockGID);
        const geomtools::placement &blockWorldPlacement = blockGeomInfo.get_world_placement();
        geomtools::placement blockModulePlacement;
        moduleWorldPlacement_->relocate(blockWorldPlacement, blockModulePlacement);
        vrz[iside][wall]->push_back(blockModulePlacement.get_translation().z());
        i_row++;
      }
    }
  }
}

bool xcalo_locator::isBlockPartitioned() const { return blocksArePartitioned_; }
}  // end of namespace geometry

}  // end of namespace snemo

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
