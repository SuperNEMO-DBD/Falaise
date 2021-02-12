// falaise/snemo/geometry/calo_locator.cc
/*
 * Copyright (C) 2011-2014 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
#include <falaise/snemo/geometry/calo_locator.h>

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

namespace snemo {

namespace geometry {
// Constructor:
calo_locator::calo_locator() { set_defaults_(); }

// Constructor:
calo_locator::calo_locator(uint32_t moduleID, const geomtools::manager &geoMgr,
                           const falaise::property_set &ps, uint32_t blockPart) {
  set_defaults_();

  set_geo_manager(geoMgr);
  setModuleNumber(moduleID);
  setBlockPart(blockPart);
  initialize(ps);
}

bool calo_locator::is_initialized() const { return isInitialized_; }

void calo_locator::initialize(const datatools::properties &ps) {
  base_locator::_basic_initialize(ps);
  DT_THROW_IF(moduleNumber_ == geomtools::geom_id::INVALID_ADDRESS, std::logic_error,
              "Missing module number ! Use the 'setModuleNumber' method before !");
  construct_();
  isInitialized_ = true;
}

void calo_locator::reset() { set_defaults_(); }

bool calo_locator::hasSubmodule(uint32_t side) const {
  DT_THROW_IF(side >= utils::NSIDES, std::logic_error,
              "Submodule number " << side << " makes no sense !");
  return submodules_[side];
}

uint32_t calo_locator::getModuleNumber() const { return moduleNumber_; }

void calo_locator::setModuleNumber(uint32_t number) { moduleNumber_ = number; }

uint32_t calo_locator::getBlockPart() const { return blockPart_; }

void calo_locator::setBlockPart(uint32_t part) { blockPart_ = part; }

bool calo_locator::isBlockPartitioned() const { return blocksArePartitioned_; }

double calo_locator::blockWidth() const { return caloBlockBox_->get_x(); }

double calo_locator::blockHeight() const { return caloBlockBox_->get_y(); }

double calo_locator::blockThickness() const { return caloBlockBox_->get_z(); }

size_t calo_locator::numberOfSides() const { return utils::NSIDES; }

size_t calo_locator::numberOfColumns(uint32_t side) const {
  if (side == 0) {
    return backCaloBlock_Y_.size();
  }

  if (side == 1) {
    return frontCaloBlock_Y_.size();
  }

  DT_THROW(std::logic_error, "Invalid side number " << side << " !");
}

size_t calo_locator::numberOfRows(uint32_t side) const {
  if (side == 0) {
    return backCaloBlock_Z_.size();
  }

  if (side == 1) {
    return frontCaloBlock_Z_.size();
  }

  DT_THROW(std::logic_error, "Invalid side number " << side << " !");
}

uint32_t calo_locator::getModuleAddress(const geomtools::geom_id &gid) const {
  return gid.get(moduleAddressIndex_);
}

uint32_t calo_locator::getSideAddress(const geomtools::geom_id &gid) const {
  return gid.get(sideAddressIndex_);
}

uint32_t calo_locator::getColumnAddress(const geomtools::geom_id &gid) const {
  return gid.get(columnAddressIndex_);
}

uint32_t calo_locator::getRowAddress(const geomtools::geom_id &gid) const {
  return gid.get(rowAddressIndex_);
}

uint32_t calo_locator::getPartAddress(const geomtools::geom_id &gid) const {
  if (isBlockPartitioned()) {
    return gid.get(partAddressIndex_);
  }
  return geomtools::geom_id::INVALID_ADDRESS;
}

bool calo_locator::isCaloBlock(const geomtools::geom_id &gid) const {
  return gid.get_type() == caloBlockGIDType_;
}

bool calo_locator::isCaloBlockInThisModule(const geomtools::geom_id &gid) const {
  return isCaloBlock(gid) && (getModuleAddress(gid) == moduleNumber_);
}

bool calo_locator::isValidAddress(uint32_t side, uint32_t column, uint32_t row) const {
  if (side >= utils::NSIDES) {
    return false;
  }
  if (column >= numberOfColumns(side)) {
    return false;
  }
  if (row >= numberOfRows(side)) {
    return false;
  }
  return true;
}

size_t calo_locator::countNeighbours(const geomtools::geom_id &gid, uint8_t mask) const {
  DT_THROW_IF(
      gid.get(moduleAddressIndex_) != moduleNumber_, std::logic_error,
      "Invalid module number (" << gid.get(moduleAddressIndex_) << "!=" << moduleNumber_ << ")!");

  return countNeighbours(gid.get(sideAddressIndex_), gid.get(columnAddressIndex_),
                         gid.get(rowAddressIndex_), mask);
}

size_t calo_locator::countNeighbours(uint32_t side_, uint32_t column_, uint32_t row_,
                                     uint8_t mask_) const {
  bool corner = false;
  bool side = false;
  const bool sides = (mask_ & grid_mask_t::SIDE) != 0;
  const bool diagonal = (mask_ & grid_mask_t::DIAG) != 0;
  if (side_ == (uint32_t)side_t::BACK) {
    if ((column_ == 0) || (column_ == backCaloBlock_Y_.size() - 1)) {
      if ((row_ == 0) || (row_ == backCaloBlock_Z_.size() - 1)) {
        corner = true;
      } else {
        side = true;
      }
    }
  }
  if (side_ == (uint32_t)side_t::FRONT) {
    if ((column_ == 0) || (column_ == frontCaloBlock_Y_.size() - 1)) {
      if ((row_ == 0) || (row_ == frontCaloBlock_Z_.size() - 1)) {
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

  const bool second = (mask_ & grid_mask_t::SECOND) != 0;
  if (second) {
    if (side_ == (uint32_t)side_t::BACK) {
      for (int ir = -2; ir <= +2; ir++) {
        if (row_ + ir > (backCaloBlock_Z_.size() - 1)) {
          continue;
        }
        for (int ic = -2; ic <= +2; ic++) {
          if (column_ + ic > (backCaloBlock_Y_.size() - 1)) {
            continue;
          }
          if (std::abs(ir) != 2 && std::abs(ic) != 2) {
            continue;
          }
          number++;
        }
      }
    }
    if (side_ == (uint32_t)side_t::FRONT) {
      for (int ir = -2; ir <= +2; ir++) {
        if (row_ + ir > (frontCaloBlock_Z_.size() - 1)) {
          continue;
        }
        for (int ic = -2; ic <= +2; ic++) {
          if (column_ + ic > (frontCaloBlock_Y_.size() - 1)) {
            continue;
          }
          if (std::abs(ir) != 2 && std::abs(ic) != 2) {
            continue;
          }
          number++;
        }
      }
    }
  }
  return number;
}

std::vector<geomtools::geom_id> calo_locator::getNeighbourGIDs(const geomtools::geom_id &gid,
                                                               uint8_t mask) const {
  DT_THROW_IF(
      gid.get(0) != moduleNumber_, std::logic_error,
      "Invalid module number (" << gid.get(moduleAddressIndex_) << "!=" << moduleNumber_ << ")!");
  return getNeighbourGIDs(gid.get(sideAddressIndex_), gid.get(columnAddressIndex_),
                          gid.get(rowAddressIndex_), mask);
}

std::vector<geomtools::geom_id> calo_locator::getNeighbourGIDs(uint32_t side, uint32_t column,
                                                               uint32_t row, uint8_t mask) const {
  DT_THROW_IF(side > 1, std::logic_error, "Invalid side number (" << side << "> 1)!");

  std::vector<geomtools::geom_id> ids_;
  ids_.reserve(8);

  const bool sides = (mask & grid_mask_t::SIDE) != 0;
  const bool diagonal = (mask & grid_mask_t::DIAG) != 0;
  const bool second = (mask & grid_mask_t::SECOND) != 0;

  // prepare neighbour GID :
  geomtools::geom_id gid;
  gid.set_type(caloBlockGIDType_);
  gid.set(moduleAddressIndex_, moduleNumber_);
  gid.set(sideAddressIndex_, side);
  gid.set(columnAddressIndex_, geomtools::geom_id::INVALID_ADDRESS);
  gid.set(rowAddressIndex_, geomtools::geom_id::INVALID_ADDRESS);
  if (isBlockPartitioned()) {
    gid.set_any(partAddressIndex_);
    // 2015-01-30 XG: use wildcard address for partitioned
    // block otherwise comparison between these geom ids
    // with one coming from calibrated blocks differs due to
    // non matching 'part' address
    // gid.set(partAddressIndex_, blockPart_);
  }
  // back
  if (side == (uint32_t)side_t::BACK) {
    DT_THROW_IF(column >= backCaloBlock_Y_.size(), std::logic_error,
                "Invalid column number (" << column << ">" << backCaloBlock_Y_.size() - 1 << ")!");
    DT_THROW_IF(row >= backCaloBlock_Z_.size(), std::logic_error,
                "Invalid row number (" << row << ">" << backCaloBlock_Z_.size() - 1 << ")!");
    if (sides && (column < (backCaloBlock_Y_.size() - 1))) {
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
    if (sides && (row < (backCaloBlock_Z_.size() - 1))) {
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

    if (diagonal && (column < (backCaloBlock_Y_.size() - 1)) &&
        (row < (backCaloBlock_Z_.size() - 1))) {
      /*  C-1 C C+1
       *  [ ][ ][x] R+1
       *  [ ][.][ ] R
       *  [ ][ ][ ] R-1
       */
      gid.set(columnAddressIndex_, column + 1);
      gid.set(rowAddressIndex_, row + 1);
      ids_.push_back(gid);
    }

    if (diagonal && (column > 0) && (row < (backCaloBlock_Z_.size() - 1))) {
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

    if (diagonal && (column < (backCaloBlock_Y_.size() - 1)) && (row > 0)) {
      /*  C-1 C C+1
       *  [ ][ ][ ] R+1
       *  [ ][.][ ] R
       *  [ ][ ][x] R-1
       */
      gid.set(columnAddressIndex_, column + 1);
      gid.set(rowAddressIndex_, row - 1);
      ids_.push_back(gid);
    }

    /*  C-2C-1 C C+1C+2
     *  [x][x][x][x][x] R+2
     *  [x][ ][ ][ ][x] R+1
     *  [x][ ][.][ ][x] R
     *  [x][ ][ ][ ][x] R-1
     *  [x][x][x][x][x] R-2
     */
    if (second) {
      for (int ir = -2; ir <= +2; ir++) {
        if (row + ir > (backCaloBlock_Z_.size() - 1)) {
          continue;
        }
        for (int ic = -2; ic <= +2; ic++) {
          if (column + ic > (backCaloBlock_Y_.size() - 1)) {
            continue;
          }
          if (std::abs(ir) != 2 && std::abs(ic) != 2) {
            continue;
          }
          gid.set(columnAddressIndex_, column + ic);
          gid.set(rowAddressIndex_, row + ir);
          ids_.push_back(gid);
        }
      }
    }
  }

  // front:
  if (side == (uint32_t)side_t::FRONT) {
    DT_THROW_IF(column >= frontCaloBlock_Y_.size(), std::logic_error,
                "Invalid column number (" << column << ">" << frontCaloBlock_Y_.size() - 1 << ")!");
    DT_THROW_IF(row >= frontCaloBlock_Z_.size(), std::logic_error,
                "Invalid row number (" << row << ">" << frontCaloBlock_Z_.size() - 1 << ")!");
    if (sides && (column < (frontCaloBlock_Y_.size() - 1))) {
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
    if (sides && (row < (frontCaloBlock_Z_.size() - 1))) {
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

    if (diagonal && (column < (frontCaloBlock_Y_.size() - 1)) &&
        (row < (frontCaloBlock_Z_.size() - 1))) {
      /*  C-1 C C+1
       *  [ ][ ][x] R+1
       *  [ ][.][ ] R
       *  [ ][ ][ ] R-1
       */
      gid.set(columnAddressIndex_, column + 1);
      gid.set(rowAddressIndex_, row + 1);
      ids_.push_back(gid);
    }

    if (diagonal && (column > 0) && (row < (frontCaloBlock_Z_.size() - 1))) {
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

    if (diagonal && (column < (frontCaloBlock_Y_.size() - 1)) && (row > 0)) {
      /*  C-1 C C+1
       *  [ ][ ][ ] R+1
       *  [ ][.][ ] R
       *  [ ][ ][x] R-1
       */
      gid.set(columnAddressIndex_, column + 1);
      gid.set(rowAddressIndex_, row - 1);
      ids_.push_back(gid);
    }

    /*  C-2C-1 C C+1C+2
     *  [x][x][x][x][x] R+2
     *  [x][ ][ ][ ][x] R+1
     *  [x][ ][.][ ][x] R
     *  [x][ ][ ][ ][x] R-1
     *  [x][x][x][x][x] R-2
     */
    if (second) {
      for (int ir = -2; ir <= +2; ir++) {
        if (row + ir > (backCaloBlock_Z_.size() - 1)) {
          continue;
        }
        for (int ic = -2; ic <= +2; ic++) {
          if (column + ic > (backCaloBlock_Y_.size() - 1)) {
            continue;
          }
          if (std::abs(ir) != 2 && std::abs(ic) != 2) {
            continue;
          }
          gid.set(columnAddressIndex_, column + ic);
          gid.set(rowAddressIndex_, row + ir);
          ids_.push_back(gid);
        }
      }
    }
  }

  return ids_;
}

double calo_locator::getXCoordOfWall(uint32_t side) const {
  if (side == 0) {
    return blockWall_X_[0];
  }
  return blockWall_X_[1];
}

double calo_locator::getXCoordOfWallWindow(uint32_t side) const {
  if (side == 0) {
    return blockWallWindow_X_[0];
  }
  return blockWallWindow_X_[1];
}

double calo_locator::getYCoordOfColumn(uint32_t side, uint32_t column) const {
  if (side == 0) {
    DT_THROW_IF(column >= backCaloBlock_Y_.size(), std::logic_error,
                "Invalid column number (" << column << ">" << backCaloBlock_Y_.size() - 1 << ")!");
    return backCaloBlock_Y_[column];
  }
  DT_THROW_IF(column >= frontCaloBlock_Y_.size(), std::logic_error,
              "Invalid column number (" << column << ">" << frontCaloBlock_Y_.size() - 1 << ")!");
  return frontCaloBlock_Y_[column];
}

double calo_locator::getZCoordOfRow(uint32_t side, uint32_t row) const {
  DT_THROW_IF(side > 1, std::logic_error, "Invalid side number (" << side << "> 1)!");
  if (side == 0) {
    DT_THROW_IF(row >= backCaloBlock_Z_.size(), std::logic_error,
                "Invalid row number (" << row << ">" << backCaloBlock_Z_.size() - 1 << ")!");
    return backCaloBlock_Z_[row];
  }
  DT_THROW_IF(row >= frontCaloBlock_Z_.size(), std::logic_error,
              "Invalid row number (" << row << ">" << frontCaloBlock_Z_.size() - 1 << ")!");
  return frontCaloBlock_Z_[row];
}

geomtools::vector_3d calo_locator::getBlockPosition(uint32_t side, uint32_t column,
                                                    uint32_t row) const {
  return geomtools::vector_3d{getXCoordOfWall(side), getYCoordOfColumn(side, column),
                              getZCoordOfRow(side, row)};
}

geomtools::vector_3d calo_locator::getBlockWindowPosition(uint32_t side, uint32_t column,
                                                          uint32_t row) const {
  return geomtools::vector_3d{getXCoordOfWallWindow(side), getYCoordOfColumn(side, column),
                              getZCoordOfRow(side, row)};
}

geomtools::vector_3d calo_locator::getBlockPosition(const geomtools::geom_id &gid) const {
  DT_THROW_IF(
      gid.get(moduleAddressIndex_) != moduleNumber_, std::logic_error,
      "Invalid module number (" << gid.get(moduleAddressIndex_) << "!=" << moduleNumber_ << ")!");
  return getBlockPosition(gid.get(sideAddressIndex_), gid.get(columnAddressIndex_),
                          gid.get(rowAddressIndex_));
}

geomtools::vector_3d calo_locator::transformWorldToModule(
    const geomtools::vector_3d &worldPoint) const {
  return moduleWorldPlacement_->mother_to_child(worldPoint);
}

geomtools::vector_3d calo_locator::transformModuleToWorld(
    const geomtools::vector_3d &modulePoint) const {
  return moduleWorldPlacement_->child_to_mother(modulePoint);
}

bool calo_locator::isPointInModule(const geomtools::vector_3d &modulePoint,
                                   double tolerance) const {
  return moduleBox_->is_inside(modulePoint, tolerance);
}

bool calo_locator::isPointInBlock(const geomtools::vector_3d &modulePoint, uint32_t side,
                                  uint32_t column, uint32_t row, double tolerance) const {
  geomtools::vector_3d to_block_pos = modulePoint;
  to_block_pos -= getBlockPosition(side, column, row);
  // here one misses one transformation step (rotation) but
  // it is ok :
  return caloBlockBox_->is_inside(to_block_pos, tolerance);
}

bool calo_locator::isWorldPointInModule(const geomtools::vector_3d &worldPoint,
                                        double tolerance) const {
  geomtools::vector_3d in_module_position = this->transformWorldToModule(worldPoint);
  return isPointInModule(in_module_position, tolerance);
}

bool calo_locator::isWorldPointInBlock(const geomtools::vector_3d &worldPoint, uint32_t side,
                                       uint32_t column, uint32_t row, double tolerance) const {
  geomtools::vector_3d in_module_position = this->transformWorldToModule(worldPoint);
  return isPointInBlock(in_module_position, side, column, row, tolerance);
}

bool calo_locator::find_geom_id(const geomtools::vector_3d &worldPoint, int type,
                                geomtools::geom_id &gid, double tolerance) const {
  DT_THROW_IF(type != (int)caloBlockGIDType_, std::logic_error,
              "Only works with type " << caloBlockGIDType_ << " for now !");

  gid.invalidate();

  // Compute the position in the module coordinate system :
  geomtools::vector_3d inModule = this->transformWorldToModule(worldPoint);

  // First check if it is inside the module :
  if (!moduleBox_->is_inside(inModule, tolerance)) {
    // Not in this module :
    return false;
  }
  return findBlockGID_(inModule, gid);
}

bool calo_locator::findBlockGID(const geomtools::vector_3d &worldPoint, geomtools::geom_id &gid,
                                double tolerance) const {
  return find_geom_id(worldPoint, caloBlockGIDType_, gid, tolerance);
}

bool calo_locator::findBlockGID_(const geomtools::vector_3d &in_module_position_,
                                 geomtools::geom_id &gid, double tolerance) const {
  if (tolerance == GEOMTOOLS_PROPER_TOLERANCE) {
    tolerance = caloBlockBox_->get_tolerance();
  }

  gid.reset();
  uint32_t side_number(geomtools::geom_id::INVALID_ADDRESS);
  uint32_t column_number(geomtools::geom_id::INVALID_ADDRESS);
  uint32_t row_number(geomtools::geom_id::INVALID_ADDRESS);
  gid.set(moduleAddressIndex_, moduleNumber_);
  const double z = in_module_position_.z();
  const double zlim = 1000 * CLHEP::m;
  if (std::abs(z) < zlim) {
    gid.set_type(caloBlockGIDType_);
    if (isBlockPartitioned()) {
      gid.set(partAddressIndex_, blockPart_);
    }
    const double x = in_module_position_.x();
    const double y = in_module_position_.y();
    double first_block_y = 0.0;
    double block_delta_y = 0.0;
    double first_block_z = 0.0;
    double block_delta_z = 0.0;
    size_t ncolumns = 0;
    size_t nrows = 0;

    // Find side:
    if (side_number == geomtools::geom_id::INVALID_ADDRESS && hasSubmodule(side_t::BACK)) {
      side_number = side_t::BACK;
      const double delta_x = std::abs(x - blockWall_X_[side_t::BACK]) - 0.5 * blockThickness();
      if (delta_x < tolerance) {
        side_number = side_t::BACK;
        ncolumns = backCaloBlock_Y_.size();
        nrows = backCaloBlock_Z_.size();
        first_block_y = backCaloBlock_Y_.front();
        block_delta_y =
            (backCaloBlock_Y_.back() - backCaloBlock_Y_.front()) / (backCaloBlock_Y_.size() - 1);
        first_block_z = backCaloBlock_Z_.front();
        block_delta_z =
            (backCaloBlock_Z_.back() - backCaloBlock_Z_.front()) / (backCaloBlock_Z_.size() - 1);
      }
    }
    if (side_number == geomtools::geom_id::INVALID_ADDRESS && hasSubmodule(side_t::FRONT)) {
      const double delta_x = std::abs(x - blockWall_X_[side_t::FRONT]) - 0.5 * blockThickness();
      if (delta_x < tolerance) {
        side_number = side_t::FRONT;
        ncolumns = frontCaloBlock_Y_.size();
        nrows = frontCaloBlock_Z_.size();
        first_block_y = frontCaloBlock_Y_.front();
        block_delta_y =
            (frontCaloBlock_Y_.back() - frontCaloBlock_Y_.front()) / (frontCaloBlock_Y_.size() - 1);
        first_block_z = frontCaloBlock_Z_.front();
        block_delta_z =
            (frontCaloBlock_Z_.back() - frontCaloBlock_Z_.front()) / (frontCaloBlock_Z_.size() - 1);
      }
    }
    if (side_number == geomtools::geom_id::INVALID_ADDRESS) {
      gid.invalidate();
      return false;
    }

    gid.set(sideAddressIndex_, side_number);
    const int iy = (int)(((y - first_block_y) / block_delta_y) + 0.5);
    if ((iy >= 0) && (iy < (int)ncolumns)) {
      column_number = iy;
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
      // 2012-05-31 FM : we check if the 'world' position is
      // in the volume:
      geomtools::vector_3d world_position = transformModuleToWorld(in_module_position_);
      if (geomtools::mapping::check_inside(*ginfo_ptr, world_position, tolerance)) {
        return true;
      }
    }
    gid.invalidate();
  }
  return false;
}

void calo_locator::tree_dump(std::ostream &out, const std::string &title, const std::string &indent,
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
  out << indent << itag << "Module number              = " << moduleNumber_ << std::endl;
  out << indent << itag << "Initialized                = " << isInitialized_ << std::endl;
  out << indent << itag
      << "Logging priority threshold = " << datatools::logger::get_priority_label(_logging_priority)
      << std::endl;
  out << indent << itag << "Manager @                  = " << &get_geo_manager() << std::endl;
  out << indent << itag << "Mapping @                  = " << geomMapping_ << std::endl;
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
  out << indent << itag << "Back  block X-pos = " << blockWall_X_[side_t::BACK] / CLHEP::mm
      << " (mm) \n";
  out << indent << itag << "Front block X-pos = " << blockWall_X_[side_t::FRONT] / CLHEP::mm
      << " (mm) \n";
  out << indent << itag
      << "Back  block window X-pos = " << blockWallWindow_X_[side_t::BACK] / CLHEP::mm
      << " (mm) \n";
  out << indent << itag
      << "Front block window X-pos = " << blockWallWindow_X_[side_t::FRONT] / CLHEP::mm
      << " (mm) \n";
  out << indent << itag << "Back  block Y-pos [" << backCaloBlock_Y_.size() << "] = ";
  for (double i : backCaloBlock_Y_) {
    out << i / CLHEP::mm << " ";
  }
  out << " (mm)" << std::endl;
  out << indent << itag << "Back  block Z-pos [" << backCaloBlock_Z_.size() << "] = ";
  for (size_t i = 0; i < backCaloBlock_Z_.size(); i++) {
    if ((i < 4) || (i > backCaloBlock_Z_.size() - 4)) {
      out << backCaloBlock_Z_[i] / CLHEP::mm << " ";
    } else if (i == 4) {
      out << " ... ";
    }
  }
  out << " (mm)" << std::endl;
  out << indent << itag << "Front block Y-pos [" << frontCaloBlock_Y_.size() << "] = ";
  for (double i : frontCaloBlock_Y_) {
    out << i / CLHEP::mm << " ";
  }
  out << " (mm)" << std::endl;
  out << indent << itag << "Front block Z-pos [" << frontCaloBlock_Z_.size() << "] = ";
  for (size_t i = 0; i < frontCaloBlock_Z_.size(); i++) {
    if ((i < 4) || (i > frontCaloBlock_Z_.size() - 4)) {
      out << frontCaloBlock_Z_[i] / CLHEP::mm << " ";
    } else if (i == 4) {
      out << " ... ";
    }
  }
  out << " (mm)" << std::endl;

  out << indent << itag << "Block width     = " << blockWidth() / CLHEP::mm << " (mm)" << std::endl;
  out << indent << itag << "Block height    = " << blockHeight() / CLHEP::mm << " (mm)"
      << std::endl;
  out << indent << itag << "Block thickness = " << blockThickness() / CLHEP::mm << " (mm)"
      << std::endl;
  out << indent << itag << "Module address GID index = " << moduleAddressIndex_ << std::endl;
  out << indent << itag << "Side   address GID index = " << sideAddressIndex_ << std::endl;
  out << indent << itag << "Column address GID index = " << columnAddressIndex_ << std::endl;
  out << indent << itag << "Row    address GID index = " << rowAddressIndex_ << std::endl;
  if (isBlockPartitioned()) {
    out << indent << datatools::i_tree_dumpable::inherit_tag(inherit)
        << "Part address GID index   = " << partAddressIndex_ << std::endl;
  }
}

void calo_locator::set_defaults_() {
  moduleNumber_ = geomtools::geom_id::INVALID_ADDRESS;
  blockPart_ = geomtools::geom_id::INVALID_ADDRESS;
  blocksArePartitioned_ = false;

  caloBlockGIDType_ = geomtools::geom_id::INVALID_TYPE;
  moduleAddressIndex_ = geomtools::geom_id::INVALID_ADDRESS;
  sideAddressIndex_ = geomtools::geom_id::INVALID_ADDRESS;
  columnAddressIndex_ = geomtools::geom_id::INVALID_ADDRESS;
  rowAddressIndex_ = geomtools::geom_id::INVALID_ADDRESS;
  partAddressIndex_ = geomtools::geom_id::INVALID_ADDRESS;

  geomMapping_ = nullptr;
  moduleWorldPlacement_ = nullptr;
  moduleBox_ = nullptr;
  caloBlockBox_ = nullptr;

  for (size_t i = 0; i < utils::NSIDES; i++) {
    blockWall_X_[i] = datatools::invalid_real();
    blockWallWindow_X_[i] = datatools::invalid_real();
    submodules_[i] = false;
  }

  backCaloBlock_Z_.clear();
  backCaloBlock_Y_.clear();
  frontCaloBlock_Z_.clear();
  frontCaloBlock_Y_.clear();

  isInitialized_ = false;
}

void calo_locator::construct_() {
  geomMapping_ = &get_geo_manager().get_mapping();
  const geomtools::id_mgr &idManager = get_geo_manager().get_id_mgr();

  uint32_t moduleGIDType = idManager.get_category_type(detail::kModuleGIDCategory);
  uint32_t caloSubmoduleGIDType = idManager.get_category_type(detail::kCaloSubmoduleGIDCategory);
  uint32_t caloWrapperGIDType = idManager.get_category_type(detail::kCaloWrapperGIDCategory);

  // Analyse the layout of the calo block's geometry category :
  caloBlockGIDType_ = idManager.get_category_type(detail::kCaloBlockGIDCategory);
  const geomtools::id_mgr::category_info &block_ci =
      idManager.get_category_info(detail::kCaloBlockGIDCategory);

  // The get_subaddress_index member function returns an invalid index
  // rather than throwing an exception. We therefore check the subaddress
  // categories we need upfront...
  for (const std::string &subaddress : {"module", "side", "column", "row"}) {
    DT_THROW_IF(!block_ci.has_subaddress(subaddress), std::logic_error,
                "Category '" << detail::kCaloBlockGIDCategory << "' has no subaddress '"
                             << subaddress << "'");
  }

  moduleAddressIndex_ = block_ci.get_subaddress_index("module");
  sideAddressIndex_ = block_ci.get_subaddress_index("side");
  columnAddressIndex_ = block_ci.get_subaddress_index("column");
  rowAddressIndex_ = block_ci.get_subaddress_index("row");

  partAddressIndex_ = geomtools::geom_id::INVALID_ADDRESS;
  if (block_ci.has_subaddress("part")) {
    blocksArePartitioned_ = true;
    partAddressIndex_ = block_ci.get_subaddress_index("part");
  } else {
    blocksArePartitioned_ = false;
  }

  // Fetch the GID of the requested module :
  const geomtools::geom_id moduleGID(moduleGIDType, moduleNumber_);
  const geomtools::geom_info *moduleGeomInfo = &geomMapping_->get_geom_info(moduleGID);
  const geomtools::i_shape_3d *moduleShape = &moduleGeomInfo->get_logical().get_shape();
  DT_THROW_IF(moduleShape->get_shape_name() != "box", std::logic_error,
              "Cannot extract the box shape from module with ID = " << moduleGID << " !");
  moduleBox_ = dynamic_cast<const geomtools::box *>(moduleShape);
  moduleWorldPlacement_ = &moduleGeomInfo->get_world_placement();

  // Check the requested block part number:
  if (blocksArePartitioned_) {
    // analyse the geometry versioning :
    datatools::version_id geomVersion;
    get_geo_manager().fetch_setup_version_id(geomVersion);

    if (geomVersion.has_major() && geomVersion.get_major() >= 2) {
      if (blockPart_ == geomtools::geom_id::INVALID_ADDRESS) {
        blockPart_ = DEFAULT_BLOCK_PART;
      }
      DT_THROW_IF(blockPart_ > 1, std::logic_error,
                  "Cannot use block part number '" << blockPart_ << "' !");
    }
  }

  // Search for calorimeter submodules :
  geomtools::geom_id sideGID;
  sideGID.set_type(caloSubmoduleGIDType);
  uint32_t side = geomtools::geom_id::INVALID_ADDRESS;
  for (size_t iside = 0; iside < utils::NSIDES; iside++) {
    sideGID.set_address(moduleNumber_, iside);
    if (geomMapping_->validate_id(sideGID)) {
      submodules_[iside] = true;
      side = iside;
    }
  }
  DT_THROW_IF(side == geomtools::geom_id::INVALID_ADDRESS, std::logic_error,
              "Cannot extract information about any calorimeter submodules !");

  // Pick up the first available block in a submodule at given side :
  geomtools::geom_id caloBlockBoxGID;
  caloBlockBoxGID.set_type(caloBlockGIDType_);
  if (blocksArePartitioned_) {
    caloBlockBoxGID.set_address(moduleNumber_, side, 0, 0, blockPart_);
  } else {
    caloBlockBoxGID.set_address(moduleNumber_, side, 0, 0);
  }

  const geomtools::geom_info &caloBlockGeomInfo = geomMapping_->get_geom_info(caloBlockBoxGID);
  caloBlockBox_ = detail::getBlockBox(caloBlockGeomInfo);
  DT_THROW_IF(caloBlockBox_ == nullptr, std::logic_error,
              "Cannot extract a geomtools::box from block with ID '" << caloBlockBoxGID << "'");

  std::vector<double> *vcy[utils::NSIDES];
  vcy[side_t::BACK] = &backCaloBlock_Y_;
  vcy[side_t::FRONT] = &frontCaloBlock_Y_;
  for (size_t iside = 0; iside < utils::NSIDES; iside++) {
    if (!submodules_[iside]) {
      continue;
    }
    size_t i_column = 0;
    vcy[iside]->reserve(20);
    while (true) {
      geomtools::geom_id caloBlockGID;
      caloBlockGID.set_type(caloBlockGIDType_);
      caloBlockGID.set(moduleAddressIndex_, moduleNumber_);
      caloBlockGID.set(sideAddressIndex_, iside);
      caloBlockGID.set(columnAddressIndex_, i_column);
      caloBlockGID.set(rowAddressIndex_, 0);
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
      const geomtools::vector_3d translation = blockModulePlacement.get_translation();

      vcy[iside]->push_back(translation.y());

      geomtools::geom_id blockWindowGID(caloWrapperGIDType, moduleNumber_, iside, i_column, 0);
      if (i_column == 0) {
        blockWall_X_[iside] = translation.x();

        const geomtools::geom_info &blockWindowGeomInfo =
            geomMapping_->get_geom_info(blockWindowGID);
        const geomtools::placement &blockWindowWorldPlacement =
            blockWindowGeomInfo.get_world_placement();
        geomtools::placement blockWindowModulePlacement;
        moduleWorldPlacement_->relocate(blockWindowWorldPlacement, blockWindowModulePlacement);
        blockWallWindow_X_[iside] = blockWindowModulePlacement.get_translation().x();
      }
      i_column++;
    }
  }

  std::vector<double> *vrz[utils::NSIDES];
  vrz[side_t::BACK] = &backCaloBlock_Z_;
  vrz[side_t::FRONT] = &frontCaloBlock_Z_;
  for (size_t iside = 0; iside < utils::NSIDES; iside++) {
    if (!submodules_[iside]) {
      continue;
    }
    size_t i_row = 0;
    vrz[iside]->reserve(13);
    while (true) {
      geomtools::geom_id caloBlockGID;
      caloBlockGID.set_type(caloBlockGIDType_);
      caloBlockGID.set(moduleAddressIndex_, moduleNumber_);
      caloBlockGID.set(sideAddressIndex_, iside);
      caloBlockGID.set(columnAddressIndex_, 0);
      caloBlockGID.set(rowAddressIndex_, i_row);
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
      vrz[iside]->push_back(blockModulePlacement.get_translation().z());
      i_row++;
    }
  }
}

}  // namespace geometry

}  // end of namespace snemo

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
