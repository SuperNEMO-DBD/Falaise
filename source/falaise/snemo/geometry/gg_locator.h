/// \file falaise/snemo/geometry/gg_locator.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-04-12
 * Last modified : 2014-01-28
 *
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
 * Description:
 *
 *   Geiger cell locator for one module of the SuperNEMO detector
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_GEOMETRY_GG_LOCATOR_H
#define FALAISE_SNEMO_GEOMETRY_GG_LOCATOR_H 1

// Standard library:
#include <string>

// Third party
// - Boost :
#include <boost/cstdint.hpp>
// - Bayeux/datatools
#include <datatools/logger.h>
// - Bayeux/geomtools
#include <geomtools/i_locator.h>

// This project:
#include <falaise/property_set.h>
#include <falaise/snemo/geometry/utils.h>

/** forward declaration */
namespace geomtools {
class mapping;
class id_mgr;
class manager;
class placement;
class box;
}  // namespace geomtools

namespace snemo {

namespace geometry {

/// \brief Fast locator class for SuperNEMO drift chamber volumes
class gg_locator : public geomtools::base_locator, public datatools::i_tree_dumpable {
 public:
  /// Strictly speaking, this constructs an invalid object, but cannot for now workaround
  /// heavy use elsewhere.
  gg_locator();

  /// Constructor
  gg_locator(uint32_t moduleID, const geomtools::manager& geoMgr, const falaise::property_set& ps);

  /// Destructor
  virtual ~gg_locator() = default;

  /// Reset
  virtual void reset();

  /// Initialize
  virtual void initialize(const datatools::properties& config_);

  /// Check intialization flag
  virtual bool is_initialized() const;

  // ATTRIBUTES

  /**! @return the number of the module (>=0).
   */
  uint32_t getModuleNumber() const;

  /**! @return set the number of the module for this locator.
   */
  void setModuleNumber(uint32_t number);

  /**! @return true if the submodule at given side is present
   */
  bool hasSubmodules(uint32_t side) const;

  /**! @return the number of sides on the tracking chamber.
   */
  size_t numberOfSides() const;

  /**! @return the number of drift cells layers in one side of the tracking chamber.
   */
  size_t numberOfLayers(uint32_t side) const;

  /**! @return the number of drift cells rows in one side of the tracking chamber.
   */
  size_t numberOfRows(uint32_t side) const;

  /**! @return the diameter of a cell.
   */
  double cellDiameter() const;

  /**! @return the length of a cell (including the base and cathode ring).
   */
  double cellLength() const;

  /**! @return the length of anode wires.
   */
  double anodeWireLength() const;

  /**! @return the diameter of anode wires.
   */
  double anodeWireDiameter() const;

  /**! @return the length of field wires.
   */
  double fieldWireLength() const;

  /**! @return the field wire diameter.
   */
  double fieldWireDiameter() const;

  // CALCULATORS

  /**! @return the module address component of the gid
   */
  uint32_t getModuleAddress(const geomtools::geom_id& gid) const;

  /**! @return the side address component of the gid
   */
  uint32_t getSideAddress(const geomtools::geom_id& gid) const;

  /**! @return the layer address component of the gid
   */
  uint32_t getLayerAddress(const geomtools::geom_id& gid) const;

  /**! @return the row address component of the gid
   */
  uint32_t getRowAddress(const geomtools::geom_id& gid) const;

  /** @arg a_gid the GID to be checked.
   *  @return true if the GID corresponds to a Geiger cell's drift volume.
   */
  bool isGeigerCell(const geomtools::geom_id& gid) const;

  /** @arg a_gid the GID to be checked
   *  @return true if the GID corresponds to a Geiger cell's drift volume in the
   *  module number associated to the locator.
   */
  bool isGeigerCellInThisModule(const geomtools::geom_id& gid) const;

  /** Given a cell at specific side, layer and row, returns the number of neighbouring cells.
   * If asked, cells on the layer==0 on any side of the chamber are associated to cells on the other
   * side of the source plane.
   */
  size_t countNeighbours(uint32_t side, uint32_t layer, uint32_t row,
                         bool acrossFoil = false) const;

  /** Given a cell with a specific geometry ID, returns the number of neighbouring cells.
   */
  size_t countNeighbours(const geomtools::geom_id& gid, bool acrossFoil = false) const;

  /** Given a cell at specific side, layer and row, compute the array of geometry IDs for associated
   * neighbouring cells.
   */
  std::vector<geomtools::geom_id> getNeighbourGIDs(uint32_t side, uint32_t layer, uint32_t row,
                                                   bool acrossFoil = false) const;

  /** Given a cell with a specific geometry IDs, compute the array of geometry IDs for associated
   * neighbouring cells.
   */
  std::vector<geomtools::geom_id> getNeighbourGIDs(const geomtools::geom_id& gid,
                                                   bool acrossFoil = false) const;

  /**! @return the X-position of a cell for specific side and layer (in module coordinate system).
   */
  double getXCoordOfLayer(uint32_t side, uint32_t layer) const;

  /**! @return the Y-position of a cell for specific side and row (in module coordinate system.
   */
  double getYCoordOfRow(uint32_t side, uint32_t row) const;

  /**! Return the position of a cell for specific side, layer and row (in module coordinate
   * system).
   */
  geomtools::vector_3d getCellPosition(uint32_t side, uint32_t layer, uint32_t row) const;

  /** Given a cell with a specific geometry ID, return its position in the module coordinate
   * system.
   */
  geomtools::vector_3d getCellPosition(const geomtools::geom_id& gid) const;

  /** Tranform a world coordinate system position to the corresponding module coordinate system
   * position.
   */
  geomtools::vector_3d transformWorldToModule(const geomtools::vector_3d& worldPoint) const;

  /** Tranform a module coordinate system position to the corresponding world coordinate system
   * position.
   */
  geomtools::vector_3d transformModuleToWorld(const geomtools::vector_3d& modulePoint) const;

  /** Check if a world coordinate system position is in the module virtual volume (its bounding
   * envelope).
   */
  bool isWorldPointInModule(const geomtools::vector_3d& worldPoint,
                            double tolerance = GEOMTOOLS_PROPER_TOLERANCE) const;

  /** Check if a world coordinate system position is in a specific cell virtual volume (its bounding
   * envelope).
   */
  bool isWorldPointInCell(const geomtools::vector_3d& worldPoint, uint32_t side, uint32_t layer,
                          uint32_t row, double tolerance = GEOMTOOLS_PROPER_TOLERANCE) const;

  /** Check if a module coordinate system position is in the module virtual volume (its bounding
   * envelope).
   */
  bool isPointInModule(const geomtools::vector_3d& modulePoint,
                       double tolerance = GEOMTOOLS_PROPER_TOLERANCE) const;

  /** Check if a module coordinate system position is in a specific cell virtual volume (its
   * bounding envelope).
   */
  bool isPointInCell(const geomtools::vector_3d& modulePoint, uint32_t side, uint32_t layer,
                     uint32_t row, double tolerance = GEOMTOOLS_PROPER_TOLERANCE) const;

  bool findCellGID(const geomtools::vector_3d& worldPoint, geomtools::geom_id& gid,
                   double tolerance = GEOMTOOLS_PROPER_TOLERANCE) const;

  // Interfaces from geomtools::i_locator :
  // Not clear that we ever use this class through the i_locator/base_locator
  // interface, so utility vague at the moment
  virtual bool find_geom_id(const geomtools::vector_3d& worldPoint, int type,
                            geomtools::geom_id& gid,
                            double tolerance = GEOMTOOLS_PROPER_TOLERANCE) const;

  /// Smart print
  virtual void tree_dump(std::ostream& out = std::clog, const std::string& title = "",
                         const std::string& indent = "", bool inherit = false) const;

 protected:
  /// Set default values
  void set_defaults_();

  /** Protected construction method. */
  void construct_();

 private:
  bool isInitialized_;

  uint32_t moduleNumber_;

  uint32_t cellGIDType_;
  int moduleAddressIndex_;
  int sideAddressIndex_;
  int layerAddressIndex_;
  int rowAddressIndex_;

  const geomtools::mapping* geomMapping_;
  const geomtools::placement* moduleWorldPlacement_;
  const geomtools::box* moduleBoxShape_;
  const geomtools::box* cellBoxShape_;

  std::vector<double> backCellX_;
  std::vector<double> backCellY_;
  std::vector<double> frontCellX_;
  std::vector<double> frontCellY_;
  double anodeWireLength_;
  double anodeWireDiameter_;
  double fieldWireLength_;
  double fieldWireDiameter_;

  // Submodules are present :
  bool submodules_[2];
};

}  // end of namespace geometry

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_GEOMETRY_GG_LOCATOR_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
