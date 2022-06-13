/// \file falaise/snemo/geometry/calo_locator.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-05-05
 * Last modified : 2015-09-19
 *
 * Copyright (C) 2011-2015 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 * Description:
 *
 *   Calorimeter scintillator block locator for one module of the SuperNEMO detector
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_GEOMETRY_CALO_LOCATOR_H
#define FALAISE_SNEMO_GEOMETRY_CALO_LOCATOR_H 1

// Standard library:
#include <string>

// Third party:
// - Boost :
#include <boost/cstdint.hpp>
// - Bayeux/datatools:
#include <datatools/i_tree_dump.h>
// - Bayeux/geomtools:
#include <geomtools/i_locator.h>

// This project:
#include <falaise/property_set.h>
#include <falaise/snemo/geometry/utils.h>

// Forward declaration:
namespace geomtools {
class mapping;
class id_mgr;
class manager;
class placement;
class i_shape_3d;
class box;
}  // namespace geomtools

namespace snemo {

namespace geometry {

/// \brief Fast locator class for SuperNEMO main calorimeter scintillator block volumes
class calo_locator : public geomtools::base_locator, public datatools::i_tree_dumpable {
 public:
  /// Block part identifier (for geometry config snemo::demonstrator >=2.0)
  /*
   *            Front
   *            +---+    Back
   *            |   +------------+
   *            |   :           .|..
   *            |   :          . |  .....
   *    part=1->|   : part=0   : | PMT  :
   *            |   :          . |  .....
   *            |   :           .|..
   *            |   :------------+
   *            +---+
   *
   */
  enum block_part_type {
    BLOCK_BACK_PART = 0,   /// Back volume of scintillator (possibly extruded)
    BLOCK_FRONT_PART = 1,  /// Front volume of scintillator (entrance window)
    DEFAULT_BLOCK_PART = BLOCK_FRONT_PART
  };

  /// Default constructor
  /// Strictly speaking, this constructs an invalid object, but cannot for now workaround
  /// heavy use elsewhere.
  calo_locator();

  /// Constructor
  calo_locator(uint32_t moduleID, const geomtools::manager& geoMgr, const falaise::property_set& ps,
               uint32_t blockPart = DEFAULT_BLOCK_PART);

  /// Destructor
  virtual ~calo_locator() = default;

  /// Reset
  virtual void reset();

  /// Initialization from parameters
  virtual void initialize(const datatools::properties& ps);

  virtual bool is_initialized() const;

  /// Check if the calorimeter setup includes submodule with given side number
  bool hasSubmodule(uint32_t side) const;

  /**! @return the number of the module(>=0).
   */
  uint32_t getModuleNumber() const;

  /**! set the number of the module for this locator.
   */
  void setModuleNumber(uint32_t number);

  /**! return the block part number for this locator.
   */
  uint32_t getBlockPart() const;

  /**! set the block part number for this locator.
   */
  void setBlockPart(uint32_t part);

  /**! @return the width of a calorimeter block.
   */
  double blockWidth() const;

  /**! @return the height of a calorimeter block.
   */
  double blockHeight() const;

  /**! @return the thickness of a block.
   */
  double blockThickness() const;

  /**! @return the number of sides on the calorimeter.
   */
  size_t numberOfSides() const;

  /**! @return the number of columns in one side of the calorimeter.
   */
  size_t numberOfColumns(uint32_t side) const;

  /**! @return the number of rows in one side of the calorimeter.
   */
  size_t numberOfRows(uint32_t side) const;

  // ADDRESS/ID CHECKS
  uint32_t getModuleAddress(const geomtools::geom_id& gid) const;

  uint32_t getSideAddress(const geomtools::geom_id& gid) const;

  uint32_t getColumnAddress(const geomtools::geom_id& gid) const;

  uint32_t getRowAddress(const geomtools::geom_id& gid) const;

  uint32_t getPartAddress(const geomtools::geom_id& gid) const;

  bool isCaloOM(const geomtools::geom_id& gid) const;

  bool isCaloBlock(const geomtools::geom_id& gid) const;

  bool isCaloBlockInThisModule(const geomtools::geom_id& gid) const;

  /// Check if a calorimeter block address is valid
  bool isValidAddress(uint32_t side, uint32_t column, uint32_t row) const;

  /** Given a block at specific side, column and row, returns the number of neighbouring blocks.
   */
  size_t countNeighbours(uint32_t side, uint32_t column, uint32_t row,
                         uint8_t mask = grid_mask_t::FIRST) const;

  /** Given a block with a specific geometry ID, returns the number of neighbouring blocks.
   */
  size_t countNeighbours(const geomtools::geom_id& gid,
                         uint8_t mask = grid_mask_t::FIRST) const;

  /** Given a block at specific side, column and row, compute the array of geometry IDs for
   * associated neighbouring blocks.
   */
  std::vector<geomtools::geom_id> getNeighbourGIDs(uint32_t side, uint32_t column, uint32_t row,
                                                   uint8_t mask = grid_mask_t::FIRST) const;

  /** Given a block with a specific geometry IDs, compute the array of geometry IDs for associated
   * neighbouring blocks.
   */
  std::vector<geomtools::geom_id> getNeighbourGIDs(const geomtools::geom_id& gid,
                                                   uint8_t mask = grid_mask_t::FIRST) const;

  /**! @return the X-position of a wall for specific side (in module coordinate system).
   */
  double getXCoordOfWall(uint32_t side) const;

  /**! @return the X-position of the entrance window of a wall for specific side (in module
   * coordinate system).
   */
  double getXCoordOfWallWindow(uint32_t side) const;

  /**! @return the Y-position of a block for specific side and column (in module coordinate system).
   */
  double getYCoordOfColumn(uint32_t side, uint32_t column) const;

  /**! @return the Z-position of a block for specific side and row (in module coordinate system).
   */
  double getZCoordOfRow(uint32_t side, uint32_t row) const;

  /**! @return the position of a block for specific side, column and row (in module coordinate
   * system).
   */
  geomtools::vector_3d getBlockPosition(uint32_t side, uint32_t column, uint32_t row) const;

  /** Given a block with a specific geometry IDs, compute its position in the module coordinate
   * system.
   */
  geomtools::vector_3d getBlockPosition(const geomtools::geom_id& gid) const;

  /**! @return the position of the center of the entrance window of a block
   *   for specific side, column and row (in module coordinate system).
   */
  geomtools::vector_3d getBlockWindowPosition(uint32_t side, uint32_t column, uint32_t row) const;

  /** Tranform a world coordinate system position to the corresponding module coordinate system
   * position.
   */
  geomtools::vector_3d transformWorldToModule(const geomtools::vector_3d& worldPoint) const;

  /** Tranform a module coordinate system position to the corresponding world coordinate system
   * position.
   */
  geomtools::vector_3d transformModuleToWorld(const geomtools::vector_3d& modulePoint) const;

  /** Check if a module coordinate system position is in the module virtual volume (its bounding
   * envelope).
   */
  bool isPointInModule(const geomtools::vector_3d& modulePoint,
                       double tolerance = GEOMTOOLS_PROPER_TOLERANCE) const;

  /** Check if a module coordinate system position is in a specific block virtual volume (its
   * bounding envelope).
   */
  bool isPointInBlock(const geomtools::vector_3d& modulePoint, uint32_t side, uint32_t column,
                      uint32_t row, double tolerance = GEOMTOOLS_PROPER_TOLERANCE) const;

  /** Check if a world coordinate system position is in the module virtual volume (its bounding
   * envelope).
   */
  bool isWorldPointInModule(const geomtools::vector_3d& worldPoint,
                            double tolerance = GEOMTOOLS_PROPER_TOLERANCE) const;

  /** Check if a world coordinate system position is in a specific block virtual volume (its
   * bounding envelope).
   */
  bool isWorldPointInBlock(const geomtools::vector_3d& worldPoint, uint32_t side, uint32_t column,
                           uint32_t row, double tolerance = GEOMTOOLS_PROPER_TOLERANCE) const;

  bool findBlockGID(const geomtools::vector_3d& worldPoint, geomtools::geom_id& gid_,
                    double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const;

  // Interfaces from geomtools::i_locator :
  virtual bool find_geom_id(const geomtools::vector_3d& worldPoint, int type_,
                            geomtools::geom_id& gid_,
                            double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const;
  /// Smart print
  virtual void tree_dump(std::ostream& out_ = std::clog, const std::string& title_ = "",
                         const std::string& indent_ = "", bool inherit_ = false) const;

 protected:
  void set_defaults_();
  void construct_();

  bool findBlockGID_(const geomtools::vector_3d& in_module_position_, geomtools::geom_id& gid_,
                     double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const;

  /**! check if block is partitioned in the current setup.
   */
  bool isBlockPartitioned() const;

 private:
  bool isInitialized_;

  // Configuration parameters :
  uint32_t moduleNumber_;
  uint32_t blockPart_;

  // Block GID addressing :
  bool blocksArePartitioned_;
  uint32_t caloBlockGIDType_;
  uint32_t caloOMGIDType_;
  int moduleAddressIndex_;
  int sideAddressIndex_;
  int columnAddressIndex_;
  int rowAddressIndex_;
  int partAddressIndex_;

  const geomtools::mapping* geomMapping_ = nullptr;
  const geomtools::placement* moduleWorldPlacement_ = nullptr;
  const geomtools::box* moduleBox_ = nullptr;
  const geomtools::box* caloBlockBox_ = nullptr;

  bool submodules_[2];
  double blockWall_X_[2];
  double blockWallWindow_X_[2];
  std::vector<double> backCaloBlock_Z_;
  std::vector<double> backCaloBlock_Y_;
  std::vector<double> frontCaloBlock_Z_;
  std::vector<double> frontCaloBlock_Y_;
};

}  // end of namespace geometry

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_GEOMETRY_CALO_LOCATOR_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
