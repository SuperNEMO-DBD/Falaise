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

bool calo_locator::is_initialized() const { return _initialized_; }

bool calo_locator::has_submodule(uint32_t side_) const {
  DT_THROW_IF(side_ >= utils::NSIDES, std::logic_error,
              "Submodule number " << side_ << " makes no sense !");
  return _submodules_[side_];
}

double calo_locator::get_block_width() const {
  DT_THROW_IF(!is_initialized(), std::logic_error, "Locator is not initialized !");
  return _block_width_;
}

double calo_locator::get_block_height() const {
  DT_THROW_IF(!is_initialized(), std::logic_error, "Locator is not initialized !");
  return _block_height_;
}

double calo_locator::get_block_thickness() const {
  DT_THROW_IF(!is_initialized(), std::logic_error, "Locator is not initialized !");
  return _block_thickness_;
}

size_t calo_locator::get_number_of_sides() const {
  DT_THROW_IF(!is_initialized(), std::logic_error, "Locator is not initialized !");
  return utils::NSIDES;
}

double calo_locator::get_wall_x(uint32_t side_) const {
  DT_THROW_IF(!is_initialized(), std::logic_error, "Locator is not initialized !");
  if (side_ == 0) {
    return _block_x_[0];
  }
  return _block_x_[1];
}

double calo_locator::get_wall_window_x(uint32_t side_) const {
  DT_THROW_IF(!is_initialized(), std::logic_error, "Locator is not initialized !");
  if (side_ == 0) {
    return _block_window_x_[0];
  }
  return _block_window_x_[1];
}

double calo_locator::get_column_y(uint32_t side_, uint32_t column_) const {
  DT_THROW_IF(!is_initialized(), std::logic_error, "Locator is not initialized !");
  if (side_ == 0) {
    DT_THROW_IF(column_ >= _back_block_y_.size(), std::logic_error,
                "Invalid column number (" << column_ << ">" << _back_block_y_.size() - 1 << ")!");
    return _back_block_y_[column_];
  }
  DT_THROW_IF(column_ >= _front_block_y_.size(), std::logic_error,
              "Invalid column number (" << column_ << ">" << _front_block_y_.size() - 1 << ")!");
  return _front_block_y_[column_];
}

double calo_locator::get_row_z(uint32_t side_, uint32_t row_) const {
  DT_THROW_IF(!is_initialized(), std::logic_error, "Locator is not initialized !");
  DT_THROW_IF(side_ > 1, std::logic_error, "Invalid side number (" << side_ << "> 1)!");
  if (side_ == 0) {
    DT_THROW_IF(row_ >= _back_block_z_.size(), std::logic_error,
                "Invalid row number (" << row_ << ">" << _back_block_z_.size() - 1 << ")!");
    return _back_block_z_[row_];
  }
  DT_THROW_IF(row_ >= _front_block_z_.size(), std::logic_error,
              "Invalid row number (" << row_ << ">" << _front_block_z_.size() - 1 << ")!");
  return _front_block_z_[row_];
}

void calo_locator::compute_block_position(uint32_t side_, uint32_t column_, uint32_t row_,
                                          geomtools::vector_3d &module_position_) const {
  geomtools::invalidate(module_position_);
  module_position_.set(get_wall_x(side_), get_column_y(side_, column_), get_row_z(side_, row_));
}

void calo_locator::compute_block_window_position(uint32_t side_, uint32_t column_, uint32_t row_,
                                                 geomtools::vector_3d &module_position_) const {
  geomtools::invalidate(module_position_);
  module_position_.set(get_wall_window_x(side_), get_column_y(side_, column_),
                       get_row_z(side_, row_));
}

geomtools::vector_3d calo_locator::get_block_position(uint32_t side_, uint32_t column_,
                                                      uint32_t row_) const {
  geomtools::vector_3d module_block_pos;
  compute_block_position(side_, column_, row_, module_block_pos);
  return module_block_pos;
}

geomtools::vector_3d calo_locator::get_block_window_position(uint32_t side_, uint32_t column_,
                                                             uint32_t row_) const {
  geomtools::vector_3d module_block_pos;
  compute_block_window_position(side_, column_, row_, module_block_pos);
  return module_block_pos;
}

size_t calo_locator::get_number_of_neighbours(const geomtools::geom_id &gid_, uint8_t mask_) const {
  DT_THROW_IF(!is_initialized(), std::logic_error, "Locator is not initialized !");
  DT_THROW_IF(gid_.get(_module_address_index_) != _module_number_, std::logic_error,
              "Invalid module number (" << gid_.get(_module_address_index_)
                                        << "!=" << _module_number_ << ")!");
  return get_number_of_neighbours(gid_.get(_side_address_index_), gid_.get(_column_address_index_),
                                  gid_.get(_row_address_index_), mask_);
}

void calo_locator::get_block_position(const geomtools::geom_id &gid_,
                                      geomtools::vector_3d &position_) const {
  DT_THROW_IF(!is_initialized(), std::logic_error, "Locator is not initialized !");
  DT_THROW_IF(gid_.get(_module_address_index_) != _module_number_, std::logic_error,
              "Invalid module number (" << gid_.get(_module_address_index_)
                                        << "!=" << _module_number_ << ")!");
  return get_block_position(gid_.get(_side_address_index_), gid_.get(_column_address_index_),
                            gid_.get(_row_address_index_), position_);
}

void calo_locator::get_block_position(uint32_t side_, uint32_t column_, uint32_t row_,
                                      geomtools::vector_3d &position_) const {
  DT_THROW_IF(!is_initialized(), std::logic_error, "Locator is not initialized !");
  DT_THROW_IF(side_ > 1, std::logic_error, "Invalid side number (" << side_ << "> 1)!");
  geomtools::invalidate(position_);
  if (side_ == (uint32_t)utils::SIDE_BACK) {
    DT_THROW_IF(column_ >= _back_block_y_.size(), std::logic_error,
                "Invalid column number (" << column_ << ">" << _back_block_y_.size() - 1 << ")!");
    DT_THROW_IF(row_ >= _back_block_z_.size(), std::logic_error,
                "Invalid row number (" << row_ << ">" << _back_block_z_.size() - 1 << ")!");
    position_.set(_block_x_[utils::SIDE_BACK], _back_block_y_[column_], _back_block_z_[row_]);
  } else {
    DT_THROW_IF(column_ >= _front_block_y_.size(), std::logic_error,
                "Invalid column number (" << column_ << ">" << _front_block_y_.size() - 1 << ")!");
    DT_THROW_IF(row_ >= _front_block_z_.size(), std::logic_error,
                "Invalid row number (" << row_ << ">" << _front_block_z_.size() - 1 << ")!");
    position_.set(_block_x_[utils::SIDE_FRONT], _front_block_y_[column_], _front_block_z_[row_]);
  }
}

void calo_locator::get_neighbours_ids(const geomtools::geom_id &gid_,
                                      std::vector<geomtools::geom_id> &ids_, uint8_t mask_) const {
  DT_THROW_IF(!is_initialized(), std::logic_error, "Locator is not initialized !");
  DT_THROW_IF(gid_.get(0) != _module_number_, std::logic_error,
              "Invalid module number (" << gid_.get(_module_address_index_)
                                        << "!=" << _module_number_ << ")!");
  get_neighbours_ids(gid_.get(_side_address_index_), gid_.get(_column_address_index_),
                     gid_.get(_row_address_index_), ids_, mask_);
}

void calo_locator::get_neighbours_ids(uint32_t side_, uint32_t column_, uint32_t row_,
                                      std::vector<geomtools::geom_id> &ids_, uint8_t mask_) const {
  DT_THROW_IF(!is_initialized(), std::logic_error, "Locator is not initialized !");
  DT_THROW_IF(side_ > 1, std::logic_error, "Invalid side number (" << side_ << "> 1)!");

  ids_.clear();
  ids_.reserve(8);

  const bool sides = (mask_ & utils::NEIGHBOUR_SIDE) != 0;
  const bool diagonal = (mask_ & utils::NEIGHBOUR_DIAG) != 0;
  const bool second = (mask_ & utils::NEIGHBOUR_SECOND) != 0;

  // prepare neighbour GID :
  geomtools::geom_id gid;
  gid.set_type(_block_type_);
  gid.set(_module_address_index_, _module_number_);
  gid.set(_side_address_index_, side_);
  gid.set(_column_address_index_, geomtools::geom_id::INVALID_ADDRESS);
  gid.set(_row_address_index_, geomtools::geom_id::INVALID_ADDRESS);
  if (is_block_partitioned()) {
    gid.set_any(_part_address_index_);
    // 2015-01-30 XG: use wildcard address for partitioned block otherwise
    // comparison between these geom ids with one coming from calibrated
    // blocks differs due to non matching 'part' address
    // gid.set(_part_address_index_, _block_part_);
  }
  // back
  if (side_ == (uint32_t)utils::SIDE_BACK) {
    DT_THROW_IF(column_ >= _back_block_y_.size(), std::logic_error,
                "Invalid column number (" << column_ << ">" << _back_block_y_.size() - 1 << ")!");
    DT_THROW_IF(row_ >= _back_block_z_.size(), std::logic_error,
                "Invalid row number (" << row_ << ">" << _back_block_z_.size() - 1 << ")!");
    if (sides && (column_ < (_back_block_y_.size() - 1))) {
      /*  C-1 C C+1
       *  [ ][ ][ ]
       *  [ ][.][x]
       *  [ ][ ][ ]
       */
      gid.set(_column_address_index_, column_ + 1);
      gid.set(_row_address_index_, row_);
      ids_.push_back(gid);
    }
    if (sides && (column_ > 0)) {
      /*  C-1 C C+1
       *  [ ][ ][ ] R+1
       *  [x][.][ ] R
       *  [ ][ ][ ] R-1
       */
      gid.set(_column_address_index_, column_ - 1);
      gid.set(_row_address_index_, row_);
      ids_.push_back(gid);
    }
    if (sides && (row_ < (_back_block_z_.size() - 1))) {
      /*  C-1 C C+1
       *  [ ][x][ ] R+1
       *  [ ][.][ ] R
       *  [ ][ ][ ] R-1
       */
      gid.set(_column_address_index_, column_);
      gid.set(_row_address_index_, row_ + 1);
      ids_.push_back(gid);
    }
    if (sides && (row_ > 0)) {
      /*  C-1 C C+1
       *  [ ][ ][ ] R+1
       *  [ ][.][ ] R
       *  [ ][x][ ] R-1
       */
      gid.set(_column_address_index_, column_);
      gid.set(_row_address_index_, row_ - 1);
      ids_.push_back(gid);
    }

    if (diagonal && (column_ < (_back_block_y_.size() - 1)) &&
        (row_ < (_back_block_z_.size() - 1))) {
      /*  C-1 C C+1
       *  [ ][ ][x] R+1
       *  [ ][.][ ] R
       *  [ ][ ][ ] R-1
       */
      gid.set(_column_address_index_, column_ + 1);
      gid.set(_row_address_index_, row_ + 1);
      ids_.push_back(gid);
    }

    if (diagonal && (column_ > 0) && (row_ < (_back_block_z_.size() - 1))) {
      /*  C-1 C C+1
       *  [x][ ][ ] R+1
       *  [ ][.][ ] R
       *  [ ][ ][ ] R-1
       */
      gid.set(_column_address_index_, column_ - 1);
      gid.set(_row_address_index_, row_ + 1);
      ids_.push_back(gid);
    }

    if (diagonal && (column_ > 0) && (row_ > 0)) {
      /*  C-1 C C+1
       *  [ ][ ][ ] R+1
       *  [ ][.][ ] R
       *  [x][ ][ ] R-1
       */
      gid.set(_column_address_index_, column_ - 1);
      gid.set(_row_address_index_, row_ - 1);
      ids_.push_back(gid);
    }

    if (diagonal && (column_ < (_back_block_y_.size() - 1)) && (row_ > 0)) {
      /*  C-1 C C+1
       *  [ ][ ][ ] R+1
       *  [ ][.][ ] R
       *  [ ][ ][x] R-1
       */
      gid.set(_column_address_index_, column_ + 1);
      gid.set(_row_address_index_, row_ - 1);
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
        if (row_ + ir > (_back_block_z_.size() - 1)) {
          continue;
        }
        for (int ic = -2; ic <= +2; ic++) {
          if (column_ + ic > (_back_block_y_.size() - 1)) {
            continue;
          }
          if (std::abs(ir) != 2 && std::abs(ic) != 2) {
            continue;
          }
          gid.set(_column_address_index_, column_ + ic);
          gid.set(_row_address_index_, row_ + ir);
          ids_.push_back(gid);
        }
      }
    }
  }

  // front:
  if (side_ == (uint32_t)utils::SIDE_FRONT) {
    DT_THROW_IF(column_ >= _front_block_y_.size(), std::logic_error,
                "Invalid column number (" << column_ << ">" << _front_block_y_.size() - 1 << ")!");
    DT_THROW_IF(row_ >= _front_block_z_.size(), std::logic_error,
                "Invalid row number (" << row_ << ">" << _front_block_z_.size() - 1 << ")!");
    if (sides && (column_ < (_front_block_y_.size() - 1))) {
      /*  C-1 C C+1
       *  [ ][ ][ ]
       *  [ ][.][x]
       *  [ ][ ][ ]
       */
      gid.set(_column_address_index_, column_ + 1);
      gid.set(_row_address_index_, row_);
      ids_.push_back(gid);
    }
    if (sides && (column_ > 0)) {
      /*  C-1 C C+1
       *  [ ][ ][ ] R+1
       *  [x][.][ ] R
       *  [ ][ ][ ] R-1
       */
      gid.set(_column_address_index_, column_ - 1);
      gid.set(_row_address_index_, row_);
      ids_.push_back(gid);
    }
    if (sides && (row_ < (_front_block_z_.size() - 1))) {
      /*  C-1 C C+1
       *  [ ][x][ ] R+1
       *  [ ][.][ ] R
       *  [ ][ ][ ] R-1
       */
      gid.set(_column_address_index_, column_);
      gid.set(_row_address_index_, row_ + 1);
      ids_.push_back(gid);
    }
    if (sides && (row_ > 0)) {
      /*  C-1 C C+1
       *  [ ][ ][ ] R+1
       *  [ ][.][ ] R
       *  [ ][x][ ] R-1
       */
      gid.set(_column_address_index_, column_);
      gid.set(_row_address_index_, row_ - 1);
      ids_.push_back(gid);
    }

    if (diagonal && (column_ < (_front_block_y_.size() - 1)) &&
        (row_ < (_front_block_z_.size() - 1))) {
      /*  C-1 C C+1
       *  [ ][ ][x] R+1
       *  [ ][.][ ] R
       *  [ ][ ][ ] R-1
       */
      gid.set(_column_address_index_, column_ + 1);
      gid.set(_row_address_index_, row_ + 1);
      ids_.push_back(gid);
    }

    if (diagonal && (column_ > 0) && (row_ < (_front_block_z_.size() - 1))) {
      /*  C-1 C C+1
       *  [x][ ][ ] R+1
       *  [ ][.][ ] R
       *  [ ][ ][ ] R-1
       */
      gid.set(_column_address_index_, column_ - 1);
      gid.set(_row_address_index_, row_ + 1);
      ids_.push_back(gid);
    }

    if (diagonal && (column_ > 0) && (row_ > 0)) {
      /*  C-1 C C+1
       *  [ ][ ][ ] R+1
       *  [ ][.][ ] R
       *  [x][ ][ ] R-1
       */
      gid.set(_column_address_index_, column_ - 1);
      gid.set(_row_address_index_, row_ - 1);
      ids_.push_back(gid);
    }

    if (diagonal && (column_ < (_front_block_y_.size() - 1)) && (row_ > 0)) {
      /*  C-1 C C+1
       *  [ ][ ][ ] R+1
       *  [ ][.][ ] R
       *  [ ][ ][x] R-1
       */
      gid.set(_column_address_index_, column_ + 1);
      gid.set(_row_address_index_, row_ - 1);
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
        if (row_ + ir > (_back_block_z_.size() - 1)) {
          continue;
        }
        for (int ic = -2; ic <= +2; ic++) {
          if (column_ + ic > (_back_block_y_.size() - 1)) {
            continue;
          }
          if (std::abs(ir) != 2 && std::abs(ic) != 2) {
            continue;
          }
          gid.set(_column_address_index_, column_ + ic);
          gid.set(_row_address_index_, row_ + ir);
          ids_.push_back(gid);
        }
      }
    }
  }
}

size_t calo_locator::get_number_of_neighbours(uint32_t side_, uint32_t column_, uint32_t row_,
                                              uint8_t mask_) const {
  DT_THROW_IF(!is_initialized(), std::logic_error, "Locator is not initialized !");
  bool corner = false;
  bool side = false;
  const bool sides = (mask_ & utils::NEIGHBOUR_SIDE) != 0;
  const bool diagonal = (mask_ & utils::NEIGHBOUR_DIAG) != 0;
  if (side_ == (uint32_t)utils::SIDE_BACK) {
    if ((column_ == 0) || (column_ == _back_block_y_.size() - 1)) {
      if ((row_ == 0) || (row_ == _back_block_z_.size() - 1)) {
        corner = true;
      } else {
        side = true;
      }
    }
  }
  if (side_ == (uint32_t)utils::SIDE_FRONT) {
    if ((column_ == 0) || (column_ == _front_block_y_.size() - 1)) {
      if ((row_ == 0) || (row_ == _front_block_z_.size() - 1)) {
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

  const bool second = (mask_ & utils::NEIGHBOUR_SECOND) != 0;
  if (second) {
    if (side_ == (uint32_t)utils::SIDE_BACK) {
      for (int ir = -2; ir <= +2; ir++) {
        if (row_ + ir > (_back_block_z_.size() - 1)) {
          continue;
        }
        for (int ic = -2; ic <= +2; ic++) {
          if (column_ + ic > (_back_block_y_.size() - 1)) {
            continue;
          }
          if (std::abs(ir) != 2 && std::abs(ic) != 2) {
            continue;
          }
          number++;
        }
      }
    }
    if (side_ == (uint32_t)utils::SIDE_FRONT) {
      for (int ir = -2; ir <= +2; ir++) {
        if (row_ + ir > (_front_block_z_.size() - 1)) {
          continue;
        }
        for (int ic = -2; ic <= +2; ic++) {
          if (column_ + ic > (_front_block_y_.size() - 1)) {
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

size_t calo_locator::get_number_of_columns(uint32_t side_) const {
  DT_THROW_IF(!is_initialized(), std::logic_error, "Locator is not initialized !");
  DT_THROW_IF(side_ > 1, std::logic_error, "Invalid side number " << side_ << " !");
  size_t nbr_columns = 0;
  if (side_ == 0) {
    nbr_columns = _back_block_y_.size();
  } else if (side_ == 1) {
    nbr_columns = _front_block_y_.size();
  }
  return nbr_columns;
}

size_t calo_locator::get_number_of_rows(uint32_t side_) const {
  DT_THROW_IF(!is_initialized(), std::logic_error, "Locator is not initialized !");
  DT_THROW_IF(side_ > 1, std::logic_error, "Invalid side number " << side_ << " !");
  size_t nbr_rows = 0;
  if (side_ == 0) {
    nbr_rows = _back_block_z_.size();
  } else if (side_ == 1) {
    nbr_rows = _front_block_z_.size();
  }
  return nbr_rows;
}

void calo_locator::_set_defaults_() {
  _module_type_ = geomtools::geom_id::INVALID_TYPE;
  _calorimeter_submodule_type_ = geomtools::geom_id::INVALID_TYPE;
  _block_type_ = geomtools::geom_id::INVALID_TYPE;
  _wrapper_type_ = geomtools::geom_id::INVALID_TYPE;
  _module_number_ = geomtools::geom_id::INVALID_ADDRESS;
  _block_part_ = geomtools::geom_id::INVALID_ADDRESS;
  _block_partitioned_ = false;

  _mapping_ = 0;
  _id_manager_ = 0;
  _module_ginfo_ = 0;
  _module_world_placement_ = 0;
  _module_box_ = 0;
  _block_shape_ = 0;
  _composite_block_shape_ = false;
  _block_box_ = 0;

  for (size_t i = 0; i < utils::NSIDES; i++) {
    datatools::invalidate(_block_x_[i]);
    datatools::invalidate(_block_window_x_[i]);
    _submodules_[i] = false;
  }

  datatools::invalidate(_block_width_);
  datatools::invalidate(_block_height_);
  datatools::invalidate(_block_thickness_);

  _module_address_index_ = geomtools::geom_id::INVALID_ADDRESS;
  _side_address_index_ = geomtools::geom_id::INVALID_ADDRESS;
  _column_address_index_ = geomtools::geom_id::INVALID_ADDRESS;
  _row_address_index_ = geomtools::geom_id::INVALID_ADDRESS;
  _part_address_index_ = geomtools::geom_id::INVALID_ADDRESS;

  _initialized_ = false;
}

// Constructor:
calo_locator::calo_locator() { _set_defaults_(); }

// Constructor:
calo_locator::calo_locator(const ::geomtools::manager &mgr_, uint32_t module_number_,
                           uint32_t block_part_) {
  _set_defaults_();

  set_geo_manager(mgr_);
  set_module_number(module_number_);
  set_block_part(block_part_);
}

// dtor:
calo_locator::~calo_locator() {
  if (is_initialized()) {
    reset();
  }
}

bool calo_locator::is_block_partitioned() const { return _block_partitioned_; }

void calo_locator::_construct() {
  // analyse the geometry versioning :
  datatools::version_id geom_mgr_setup_vid;
  get_geo_manager().fetch_setup_version_id(geom_mgr_setup_vid);

  const std::string module_cat = "module";
  const std::string calorimeter_submodule_cat = "calorimeter_submodule";
  const std::string calorimeter_block_cat = "calorimeter_block";
  const std::string calorimeter_wrapper_cat = "calorimeter_wrapper";

  _mapping_ = &get_geo_manager().get_mapping();
  _id_manager_ = &get_geo_manager().get_id_mgr();

  const geomtools::id_mgr::categories_by_name_col_type &categories =
      _id_manager_->categories_by_name();

  DT_THROW_IF(categories.find(module_cat) == categories.end(), std::logic_error,
              "No category named '" << module_cat << "' !");
  _module_type_ = categories.find(module_cat)->second.get_type();

  DT_THROW_IF(categories.find(calorimeter_submodule_cat) == categories.end(), std::logic_error,
              "No category named '" << calorimeter_submodule_cat << "' !");
  _calorimeter_submodule_type_ = categories.find(calorimeter_submodule_cat)->second.get_type();

  DT_THROW_IF(categories.find(calorimeter_block_cat) == categories.end(), std::logic_error,
              "No category named '" << calorimeter_block_cat << "' !");
  _block_type_ = categories.find(calorimeter_block_cat)->second.get_type();

  // Analyse the layout of the calo block's geometry category :
  const geomtools::id_mgr::category_info &block_ci = categories.find(calorimeter_block_cat)->second;
  DT_THROW_IF(!block_ci.has_subaddress("module"), std::logic_error,
              "Category 'calorimeter_block' has no subaddress 'module' !");
  _module_address_index_ = block_ci.get_subaddress_index("module");

  DT_THROW_IF(!block_ci.has_subaddress("side"), std::logic_error,
              "Category 'calorimeter_block' has no subaddress 'side' !");
  _side_address_index_ = block_ci.get_subaddress_index("side");

  DT_THROW_IF(!block_ci.has_subaddress("column"), std::logic_error,
              "Category 'calorimeter_block' has no subaddress 'column' !");
  _column_address_index_ = block_ci.get_subaddress_index("column");

  DT_THROW_IF(!block_ci.has_subaddress("row"), std::logic_error,
              "Category 'calorimeter_block' has no subaddress 'row' !");
  _row_address_index_ = block_ci.get_subaddress_index("row");

  _part_address_index_ = geomtools::geom_id::INVALID_ADDRESS;
  if (block_ci.has_subaddress("part")) {
    _block_partitioned_ = true;
    _part_address_index_ = block_ci.get_subaddress_index("part");
  } else {
    _block_partitioned_ = false;
  }

  DT_THROW_IF(categories.find(calorimeter_wrapper_cat) == categories.end(), std::logic_error,
              "No category named '" << calorimeter_wrapper_cat << "' !");
  _wrapper_type_ = categories.find(calorimeter_wrapper_cat)->second.get_type();

  // Fetch the GID of the requested module :
  const geomtools::geom_id module_gid(_module_type_, _module_number_);
  DT_THROW_IF(!_mapping_->validate_id(module_gid), std::logic_error,
              "No module with ID = " << module_gid << " !");
  _module_ginfo_ = &_mapping_->get_geom_info(module_gid);

  const geomtools::i_shape_3d *a_shape = &_module_ginfo_->get_logical().get_shape();
  DT_THROW_IF(a_shape->get_shape_name() != "box", std::logic_error,
              "Cannot extract the box shape from module with ID = " << module_gid << " !");
  _module_box_ = dynamic_cast<const geomtools::box *>(a_shape);
  _module_world_placement_ = &_module_ginfo_->get_world_placement();

  // Check the requested block part number:
  if (_block_partitioned_) {
    if (geom_mgr_setup_vid.has_major() && geom_mgr_setup_vid.get_major() >= 2) {
      if (_block_part_ == geomtools::geom_id::INVALID_ADDRESS) {
        _block_part_ = DEFAULT_BLOCK_PART;
      }
      DT_THROW_IF(_block_part_ > 1, std::logic_error,
                  "Cannot use block part number '" << _block_part_ << "' !");
    }
  }

  // Search for calorimeter submodules :
  geomtools::geom_id side_gid;
  side_gid.set_type(_calorimeter_submodule_type_);
  uint32_t side = geomtools::geom_id::INVALID_ADDRESS;
  for (size_t iside = 0; iside < utils::NSIDES; iside++) {
    side_gid.set_address(_module_number_, iside);
    if (_mapping_->validate_id(side_gid)) {
      _submodules_[iside] = true;
      side = iside;
    }
  }
  DT_THROW_IF(side == geomtools::geom_id::INVALID_ADDRESS, std::logic_error,
              "Cannot extract information about any calorimeter submodules !");

  // Pick up the first available block in a submodule at given side :
  geomtools::geom_id block_gid;
  block_gid.set_type(_block_type_);
  if (_block_partitioned_) {
    block_gid.set_address(_module_number_, side, 0, 0, _block_part_);
  } else {
    block_gid.set_address(_module_number_, side, 0, 0);
  }

  DT_THROW_IF(!_mapping_->validate_id(block_gid), std::logic_error,
              "Cannot extract information about a block with ID = " << block_gid << " !");
  const geomtools::geom_info &block_ginfo = _mapping_->get_geom_info(block_gid);
  const geomtools::i_shape_3d &b_shape = block_ginfo.get_logical().get_shape();
  _block_shape_ = &b_shape;
  if (_block_shape_->get_shape_name() == "subtraction_3d") {
    const geomtools::subtraction_3d &ref_s3d =
        dynamic_cast<const geomtools::subtraction_3d &>(*_block_shape_);
    // Example : 'calo_scin_box_model' case :
    _composite_block_shape_ = true;
    const geomtools::i_composite_shape_3d::shape_type &sht1 = ref_s3d.get_shape1();
    const geomtools::i_shape_3d &sh1 = sht1.get_shape();
    DT_THROW_IF(sh1.get_shape_name() != "box", std::logic_error,
                "Do not support non-box shaped block with ID = " << block_gid << " !");
    _block_box_ = dynamic_cast<const geomtools::box *>(&sh1);
  } else if (_block_shape_->get_shape_name() == "intersection_3d") {
    // Example : 'calo_tapered_scin_box_model' case :
    _composite_block_shape_ = true;

    const geomtools::intersection_3d &ref_i3d =
        dynamic_cast<const geomtools::intersection_3d &>(*_block_shape_);
    const geomtools::i_composite_shape_3d::shape_type &sht1 = ref_i3d.get_shape1();
    const geomtools::i_shape_3d &sh1 = sht1.get_shape();
    DT_THROW_IF(sh1.get_shape_name() != "subtraction_3d", std::logic_error,
                "Do not support non-subtraction_3d shaped block with ID = " << block_gid << " !");
    const geomtools::subtraction_3d &ref_s3d = dynamic_cast<const geomtools::subtraction_3d &>(sh1);
    const geomtools::i_composite_shape_3d::shape_type &sht11 = ref_s3d.get_shape1();
    const geomtools::i_shape_3d &sh11 = sht11.get_shape();
    DT_THROW_IF(sh11.get_shape_name() != "box", std::logic_error,
                "Do not support non-box shaped block with ID = " << block_gid << " !");
    _block_box_ = dynamic_cast<const geomtools::box *>(&sh11);
  } else if (_block_shape_->get_shape_name() == "box") {
    // Simple box case :
    _composite_block_shape_ = false;
    _block_box_ = dynamic_cast<const geomtools::box *>(_block_shape_);
  } else {
    DT_THROW_IF(true, std::logic_error,
                "Cannot extract the shape from block with ID = " << block_gid << " !");
  }

  std::vector<double> *vcy[utils::NSIDES];
  vcy[utils::SIDE_BACK] = &_back_block_y_;
  vcy[utils::SIDE_FRONT] = &_front_block_y_;
  for (size_t iside = 0; iside < utils::NSIDES; iside++) {
    if (!_submodules_[iside]) {
      continue;
    }
    size_t i_column = 0;
    vcy[iside]->reserve(20);
    while (true) {
      geomtools::geom_id a_block_gid;
      a_block_gid.set_type(_block_type_);
      a_block_gid.set(_module_address_index_, _module_number_);
      a_block_gid.set(_side_address_index_, iside);
      a_block_gid.set(_column_address_index_, i_column);
      a_block_gid.set(_row_address_index_, 0);
      if (is_block_partitioned()) {
        a_block_gid.set(_part_address_index_, _block_part_);
      }
      if (!_mapping_->validate_id(a_block_gid)) {
        break;
      }
      const geomtools::geom_info &a_block_ginfo = _mapping_->get_geom_info(a_block_gid);
      const geomtools::placement &a_block_world_placement = a_block_ginfo.get_world_placement();
      geomtools::placement a_block_module_placement;
      _module_world_placement_->relocate(a_block_world_placement, a_block_module_placement);
      const double y = a_block_module_placement.get_translation().y();
      vcy[iside]->push_back(y);
      if (i_column == 0) {
        const double x = a_block_module_placement.get_translation().x();
        _block_x_[iside] = x;

        geomtools::geom_id a_block_window_gid(_wrapper_type_, _module_number_, iside, i_column, 0);
        const geomtools::geom_info &a_block_window_ginfo =
            _mapping_->get_geom_info(a_block_window_gid);
        const geomtools::placement &a_block_window_world_placement =
            a_block_window_ginfo.get_world_placement();
        geomtools::placement a_block_window_module_placement;
        _module_world_placement_->relocate(a_block_window_world_placement,
                                           a_block_window_module_placement);
        const double x2 = a_block_window_module_placement.get_translation().x();
        _block_window_x_[iside] = x2;
      }
      i_column++;
    }
  }

  std::vector<double> *vrz[utils::NSIDES];
  vrz[utils::SIDE_BACK] = &_back_block_z_;
  vrz[utils::SIDE_FRONT] = &_front_block_z_;
  for (size_t iside = 0; iside < utils::NSIDES; iside++) {
    if (!_submodules_[iside]) {
      continue;
    }
    size_t i_row = 0;
    vrz[iside]->reserve(13);
    while (true) {
      geomtools::geom_id a_block_gid;
      a_block_gid.set_type(_block_type_);
      a_block_gid.set(_module_address_index_, _module_number_);
      a_block_gid.set(_side_address_index_, iside);
      a_block_gid.set(_column_address_index_, 0);
      a_block_gid.set(_row_address_index_, i_row);
      if (is_block_partitioned()) {
        a_block_gid.set(_part_address_index_, _block_part_);
      }
      if (!_mapping_->validate_id(a_block_gid)) {
        break;
      }
      const geomtools::geom_info &a_block_ginfo = _mapping_->get_geom_info(a_block_gid);
      const geomtools::placement &a_block_world_placement = a_block_ginfo.get_world_placement();
      geomtools::placement a_block_module_placement;
      _module_world_placement_->relocate(a_block_world_placement, a_block_module_placement);
      const double z = a_block_module_placement.get_translation().z();
      vrz[iside]->push_back(z);
      i_row++;
    }
  }

  _block_width_ = _block_box_->get_x();
  _block_height_ = _block_box_->get_y();
  _block_thickness_ = _block_box_->get_z();
}

int calo_locator::get_module_address_index() const { return _module_address_index_; }

int calo_locator::get_side_address_index() const { return _side_address_index_; }

int calo_locator::get_column_address_index() const { return _column_address_index_; }

int calo_locator::get_row_address_index() const { return _row_address_index_; }

int calo_locator::get_part_address_index() const { return _part_address_index_; }

uint32_t calo_locator::extract_module(const geomtools::geom_id &gid_) const {
  return gid_.get(_module_address_index_);
}

uint32_t calo_locator::extract_side(const geomtools::geom_id &gid_) const {
  return gid_.get(_side_address_index_);
}

uint32_t calo_locator::extract_column(const geomtools::geom_id &gid_) const {
  return gid_.get(_column_address_index_);
}

uint32_t calo_locator::extract_row(const geomtools::geom_id &gid_) const {
  return gid_.get(_row_address_index_);
}

uint32_t calo_locator::extract_part(const geomtools::geom_id &gid_) const {
  if (is_block_partitioned()) {
    return gid_.get(_part_address_index_);
  }
  return geomtools::geom_id::INVALID_ADDRESS;
}

bool calo_locator::is_calo_block(const geomtools::geom_id &gid_) const {
  return gid_.get_type() == _block_type_;
}

bool calo_locator::is_calo_block_in_current_module(const geomtools::geom_id &gid_) const {
  return is_calo_block(gid_) && (extract_module(gid_) == _module_number_);
}

void calo_locator::set_module_number(uint32_t a_module_number) {
  DT_THROW_IF(is_initialized(), std::logic_error, "Locator is already initialized !");
  _module_number_ = a_module_number;
}

void calo_locator::set_block_part(uint32_t a_block_part) {
  DT_THROW_IF(is_initialized(), std::logic_error, "Locator is already initialized !");
  _block_part_ = a_block_part;
}

uint32_t calo_locator::get_module_number() const { return _module_number_; }

uint32_t calo_locator::get_block_part() const { return _block_part_; }

void calo_locator::initialize() {
  datatools::properties dummy;
  initialize(dummy);
}

void calo_locator::_hack_trace() {
  char *ev = getenv("FLGEOMLOCATOR");
  if (ev != 0) {
    std::string evstr(ev);
    if (evstr == "trace") {
      set_logging_priority(datatools::logger::PRIO_TRACE);
      DT_LOG_TRACE(get_logging_priority(),
                   "Trace logging activated through env 'FLGEOMLOCATOR'...");
    }
  }
}

void calo_locator::initialize(const datatools::properties &config_) {
  base_locator::_basic_initialize(config_);
  DT_THROW_IF(_module_number_ == geomtools::geom_id::INVALID_ADDRESS, std::logic_error,
              "Missing module number ! Use the 'set_module_number' method before !");
  _hack_trace();
  _construct();
  _initialized_ = true;
  if (datatools::logger::is_trace(get_logging_priority())) {
    tree_dump(std::cerr, "Main calo locator : ", "[trace] ");
  }
}

void calo_locator::dump(std::ostream &out_) const {
  calo_locator::tree_dump(out_, "snemo::geometry:calo_locator::dump: ");
}

void calo_locator::tree_dump(std::ostream &out_, const std::string &title_,
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
         << "Initialized        = " << _initialized_ << std::endl;
    return;
  }
  out_ << indent << itag << "Initialized                = " << _initialized_ << std::endl;
  out_ << indent << itag << "Logging priority threshold = "
       << datatools::logger::get_priority_label(_logging_priority) << std::endl;
  out_ << indent << itag << "Module number              = " << _module_number_ << std::endl;
  out_ << indent << itag << "Manager @                  = " << &get_geo_manager() << std::endl;
  out_ << indent << itag << "Mapping @                  = " << _mapping_ << std::endl;
  out_ << indent << itag << "ID manager @               = " << _id_manager_ << std::endl;
  out_ << indent << itag << "Module type                = " << _module_type_ << std::endl;
  out_ << indent << itag << "Calorimeter submodule type = " << _calorimeter_submodule_type_
       << std::endl;
  out_ << indent << itag << "Calorimeter block type     = " << _block_type_ << std::endl;
  out_ << indent << itag << "Calorimeter wrapper type   = " << _wrapper_type_ << std::endl;
  out_ << indent << itag << "Block partitioned          = " << _block_partitioned_ << std::endl;
  if (is_block_partitioned()) {
    out_ << indent << itag << "Block part                 = " << _block_part_ << std::endl;
  }
  out_ << indent << itag << "Module ginfo @             = " << _module_ginfo_ << std::endl;
  out_ << indent << itag << "Module placement : " << std::endl;
  if (_module_world_placement_ != 0) {
    _module_world_placement_->tree_dump(out_, "", indent + stag);
  }
  out_ << indent << itag << "Module box : " << std::endl;
  if (_module_box_ != 0) {
    _module_box_->tree_dump(out_, "", indent + stag);
  }
  out_ << indent << itag << "Back  submodule : " << _submodules_[utils::SIDE_BACK] << std::endl;
  out_ << indent << itag << "Front submodule : " << _submodules_[utils::SIDE_FRONT] << std::endl;
  out_ << indent << itag << "Block shape : " << _block_shape_->get_shape_name() << std::endl;
  out_ << indent << itag << "Composite block shape = " << _composite_block_shape_ << std::endl;
  out_ << indent << itag << "Block box : " << std::endl;
  if (_block_box_ != 0) {
    _block_box_->tree_dump(out_, "", indent + stag);
  }
  out_ << indent << itag << "Back  block X-pos = " << _block_x_[utils::SIDE_BACK] / CLHEP::mm
       << " (mm) \n";
  out_ << indent << itag << "Front block X-pos = " << _block_x_[utils::SIDE_FRONT] / CLHEP::mm
       << " (mm) \n";
  out_ << indent << itag
       << "Back  block window X-pos = " << _block_window_x_[utils::SIDE_BACK] / CLHEP::mm
       << " (mm) \n";
  out_ << indent << itag
       << "Front block window X-pos = " << _block_window_x_[utils::SIDE_FRONT] / CLHEP::mm
       << " (mm) \n";
  out_ << indent << itag << "Back  block Y-pos [" << _back_block_y_.size() << "] = ";
  for (size_t i = 0; i < _back_block_y_.size(); i++) {
    out_ << _back_block_y_[i] / CLHEP::mm << " ";
  }
  out_ << " (mm)" << std::endl;
  out_ << indent << itag << "Back  block Z-pos [" << _back_block_z_.size() << "] = ";
  for (size_t i = 0; i < _back_block_z_.size(); i++) {
    if ((i < 4) || (i > _back_block_z_.size() - 4)) {
      out_ << _back_block_z_[i] / CLHEP::mm << " ";
    } else if (i == 4) {
      out_ << " ... ";
    }
  }
  out_ << " (mm)" << std::endl;
  out_ << indent << itag << "Front block Y-pos [" << _front_block_y_.size() << "] = ";
  for (size_t i = 0; i < _front_block_y_.size(); i++) {
    out_ << _front_block_y_[i] / CLHEP::mm << " ";
  }
  out_ << " (mm)" << std::endl;
  out_ << indent << itag << "Front block Z-pos [" << _front_block_z_.size() << "] = ";
  for (size_t i = 0; i < _front_block_z_.size(); i++) {
    if ((i < 4) || (i > _front_block_z_.size() - 4)) {
      out_ << _front_block_z_[i] / CLHEP::mm << " ";
    } else if (i == 4) {
      out_ << " ... ";
    }
  }
  out_ << " (mm)" << std::endl;

  out_ << indent << itag << "Block width     = " << _block_width_ / CLHEP::mm << " (mm)"
       << std::endl;
  out_ << indent << itag << "Block height    = " << _block_height_ / CLHEP::mm << " (mm)"
       << std::endl;
  out_ << indent << itag << "Block thickness = " << _block_thickness_ / CLHEP::mm << " (mm)"
       << std::endl;
  out_ << indent << itag << "Module address GID index = " << _module_address_index_ << std::endl;
  out_ << indent << itag << "Side   address GID index = " << _side_address_index_ << std::endl;
  out_ << indent << itag << "Column address GID index = " << _column_address_index_ << std::endl;
  out_ << indent << itag << "Row    address GID index = " << _row_address_index_ << std::endl;
  if (is_block_partitioned()) {
    out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Part address GID index   = " << _part_address_index_ << std::endl;
  }
}

void calo_locator::reset() {
  DT_THROW_IF(!is_initialized(), std::logic_error, "Locator is not initialized !");
  _back_block_z_.clear();
  _front_block_z_.clear();
  _back_block_y_.clear();
  _front_block_y_.clear();
  _set_defaults_();
}

void calo_locator::transform_world_to_module(const geomtools::vector_3d &world_position_,
                                             geomtools::vector_3d &module_position_) const {
  DT_THROW_IF(!is_initialized(), std::logic_error, "Locator is not initialized !");
  _module_world_placement_->mother_to_child(world_position_, module_position_);
}

void calo_locator::transform_module_to_world(const geomtools::vector_3d &module_position_,
                                             geomtools::vector_3d &world_position_) const {
  DT_THROW_IF(!is_initialized(), std::logic_error, "Locator is not initialized !");
  _module_world_placement_->child_to_mother(module_position_, world_position_);
}

bool calo_locator::is_in_module(const geomtools::vector_3d &module_position_,
                                double tolerance_) const {
  DT_THROW_IF(!is_initialized(), std::logic_error, "Locator is not initialized !");
  return _module_box_->is_inside(module_position_, tolerance_);
}

bool calo_locator::is_in_block(const geomtools::vector_3d &module_position_, uint32_t side_,
                               uint32_t column_, uint32_t row_, double tolerance_) const {
  DT_THROW_IF(!is_initialized(), std::logic_error, "Locator is not initialized !");
  geomtools::vector_3d to_block_pos = module_position_;
  to_block_pos -= get_block_position(side_, column_, row_);
  // here one misses one transformation step (rotation) but it is ok :
  return _block_box_->is_inside(to_block_pos, tolerance_);
}

bool calo_locator::is_world_position_in_block(const geomtools::vector_3d &world_position_,
                                              uint32_t side_, uint32_t column_, uint32_t row_,
                                              double tolerance_) const {
  DT_THROW_IF(!is_initialized(), std::logic_error, "Locator is not initialized !");
  geomtools::vector_3d in_module_position;
  this->transform_world_to_module(world_position_, in_module_position);
  return is_in_block(in_module_position, side_, column_, row_, tolerance_);
}

bool calo_locator::is_world_position_in_module(const geomtools::vector_3d &world_position_,
                                               double tolerance_) const {
  DT_THROW_IF(!is_initialized(), std::logic_error, "Locator is not initialized !");
  geomtools::vector_3d in_module_position;
  this->transform_world_to_module(world_position_, in_module_position);
  return is_in_module(in_module_position, tolerance_);
}

bool calo_locator::find_geom_id(const geomtools::vector_3d &world_position_, int type_,
                                geomtools::geom_id &gid_, double tolerance_) const {
  DT_THROW_IF(!is_initialized(), std::logic_error, "Locator is not initialized !");
  DT_THROW_IF(type_ != (int)_block_type_, std::logic_error,
              "Only works with type " << _block_type_ << " for now !");

  gid_.invalidate();

  // Compute the position in the module coordinate system :
  geomtools::vector_3d in_module_position;
  this->transform_world_to_module(world_position_, in_module_position);

  // First check if it is inside the module :
  if (!_module_box_->is_inside(in_module_position, tolerance_)) {
    // Not in this module :
    return false;
  }
  return const_cast<calo_locator *>(this)->calo_locator::find_block_geom_id_(in_module_position,
                                                                             gid_);
}

bool calo_locator::id_is_valid(uint32_t side_, uint32_t column_, uint32_t row_) const {
  if (side_ >= utils::NSIDES) {
    return false;
  }
  if (column_ >= get_number_of_columns(side_)) {
    return false;
  }
  if (row_ >= get_number_of_rows(side_)) {
    return false;
  }
  return true;
}

bool calo_locator::find_block_geom_id(const geomtools::vector_3d &world_position_,
                                      geomtools::geom_id &gid_, double tolerance_) const {
  return find_geom_id(world_position_, _block_type_, gid_, tolerance_);
}

bool calo_locator::find_block_geom_id_(const geomtools::vector_3d &in_module_position_,
                                       geomtools::geom_id &gid_, double tolerance_) {
  DT_LOG_TRACE_ENTERING(get_logging_priority());

  double tolerance = tolerance_;
  if (tolerance == GEOMTOOLS_PROPER_TOLERANCE) {
    tolerance = _block_box_->get_tolerance();
  }

  geomtools::geom_id &gid = gid_;
  gid.reset();
  uint32_t side_number(geomtools::geom_id::INVALID_ADDRESS);
  uint32_t column_number(geomtools::geom_id::INVALID_ADDRESS);
  uint32_t row_number(geomtools::geom_id::INVALID_ADDRESS);
  const double z = in_module_position_.z();
  const double zlim = 1000 * CLHEP::m;
  if (std::abs(z) < zlim) {
    gid.set_type(_block_type_);
    gid.set(_module_address_index_, _module_number_);
    if (is_block_partitioned()) {
      DT_LOG_TRACE(get_logging_priority(), "Block part address = " << _block_part_);
      gid.set(_part_address_index_, _block_part_);
    }
    const double x = in_module_position_.x();
    const double y = in_module_position_.y();
    DT_LOG_TRACE(get_logging_priority(), "x = " << x / CLHEP::mm);
    DT_LOG_TRACE(get_logging_priority(), "y = " << y / CLHEP::mm);
    DT_LOG_TRACE(get_logging_priority(), "z = " << z / CLHEP::mm);
    double first_block_y = 0.0;
    double block_delta_y = 0.0;
    double first_block_z = 0.0;
    double block_delta_z = 0.0;
    size_t ncolumns = 0;
    size_t nrows = 0;

    // Find side:
    if (side_number == geomtools::geom_id::INVALID_ADDRESS && has_submodule(utils::SIDE_BACK)) {
      side_number = utils::SIDE_BACK;
      const double delta_x =
          std::abs(x - _block_x_[utils::SIDE_BACK]) - 0.5 * get_block_thickness();
      if (delta_x < tolerance) {
        side_number = utils::SIDE_BACK;
        ncolumns = _back_block_y_.size();
        nrows = _back_block_z_.size();
        first_block_y = _back_block_y_.front();
        block_delta_y =
            (_back_block_y_.back() - _back_block_y_.front()) / (_back_block_y_.size() - 1);
        first_block_z = _back_block_z_.front();
        block_delta_z =
            (_back_block_z_.back() - _back_block_z_.front()) / (_back_block_z_.size() - 1);
      }
    }
    if (side_number == geomtools::geom_id::INVALID_ADDRESS && has_submodule(utils::SIDE_FRONT)) {
      const double delta_x =
          std::abs(x - _block_x_[utils::SIDE_FRONT]) - 0.5 * get_block_thickness();
      if (delta_x < tolerance) {
        side_number = utils::SIDE_FRONT;
        ncolumns = _front_block_y_.size();
        nrows = _front_block_z_.size();
        first_block_y = _front_block_y_.front();
        block_delta_y =
            (_front_block_y_.back() - _front_block_y_.front()) / (_front_block_y_.size() - 1);
        first_block_z = _front_block_z_.front();
        block_delta_z =
            (_front_block_z_.back() - _front_block_z_.front()) / (_front_block_z_.size() - 1);
      }
    }
    if (side_number == geomtools::geom_id::INVALID_ADDRESS) {
      DT_LOG_TRACE(get_logging_priority(), "Not a main calo!");
      gid.invalidate();
      DT_LOG_TRACE_EXITING(get_logging_priority());
      return false;
    }

    DT_LOG_TRACE(get_logging_priority(), "side_number   = " << side_number);
    DT_LOG_TRACE(get_logging_priority(), "ncolumns      = " << ncolumns);
    DT_LOG_TRACE(get_logging_priority(), "nrows         = " << nrows);
    DT_LOG_TRACE(get_logging_priority(), "first_block_y = " << first_block_y / CLHEP::mm);
    DT_LOG_TRACE(get_logging_priority(), "block_delta_y = " << block_delta_y / CLHEP::mm);
    DT_LOG_TRACE(get_logging_priority(), "x             = " << x / CLHEP::mm);
    DT_LOG_TRACE(get_logging_priority(), "y             = " << y / CLHEP::mm);
    DT_LOG_TRACE(get_logging_priority(), "z             = " << z / CLHEP::mm);
    DT_LOG_TRACE(get_logging_priority(), "first_block_z = " << first_block_z / CLHEP::mm);
    DT_LOG_TRACE(get_logging_priority(), "block_delta_z = " << block_delta_z / CLHEP::mm);

    gid.set(_side_address_index_, side_number);
    DT_LOG_TRACE(get_logging_priority(), "gid = " << gid);
    const int iy = (int)(((y - first_block_y) / block_delta_y) + 0.5);
    if ((iy >= 0) && (iy < (int)ncolumns)) {
      column_number = iy;
    }
    gid.set(_column_address_index_, column_number);
    DT_LOG_TRACE(get_logging_priority(), "gid = " << gid);
    const int iz = (int)(((z - first_block_z) / block_delta_z) + 0.5);
    if ((iz >= 0) && (iz < (int)nrows)) {
      row_number = iz;
    }
    gid.set(_row_address_index_, row_number);
    if (gid.is_valid()) {
      // 2012-05-31 FM : use ginfo from mapping (see below)
      const geomtools::geom_info *ginfo_ptr = _mapping_->get_geom_info_ptr(gid);
      if (ginfo_ptr == 0) {
        DT_LOG_TRACE(get_logging_priority(), "Unmapped gid = " << gid);
        DT_LOG_TRACE(get_logging_priority(), "Not a main calo!");
        gid.invalidate();
        DT_LOG_TRACE_EXITING(get_logging_priority());
        return false;
      }
      DT_LOG_TRACE(get_logging_priority(), "Valid mapped gid = " << gid);
      // 2012-05-31 FM : we check if the 'world' position is in the volume:
      geomtools::vector_3d world_position;
      transform_module_to_world(in_module_position_, world_position);
      if (geomtools::mapping::check_inside(*ginfo_ptr, world_position, tolerance)) {
        DT_LOG_TRACE(get_logging_priority(), "INSIDE " << gid);
        DT_LOG_TRACE_EXITING(get_logging_priority());
        return true;
      }
    }
    DT_LOG_TRACE(get_logging_priority(), "Not a main calo!");
    gid.invalidate();
  }
  DT_LOG_TRACE_EXITING(get_logging_priority());
  return false;
}

}  // end of namespace geometry

}  // end of namespace snemo

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
