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
// - Bayeux/geomtools
#include <geomtools/i_locator.h>

// This project:
#include <falaise/snemo/geometry/utils.h>

/** forward declaration */
namespace geomtools {
  class mapping;
  class id_mgr;
  class manager;
  class placement;
  class box;
}

namespace snemo {

  namespace geometry {

    /// \brief Fast locator class for SuperNEMO drift chamber volumes
    class gg_locator : public geomtools::base_locator,
                       public datatools::i_tree_dumpable
    {
    public:

      /// Check intialization flag
      virtual bool is_initialized() const;

      /// Constructor
      gg_locator();

      /// Constructor
      gg_locator(const ::geomtools::manager & mgr_, uint32_t module_number_);

      /// Destructor
      virtual ~gg_locator();

      /// Reset
      virtual void reset();

      /// Initialize
      virtual void initialize(const datatools::properties & config_);

      /// Initialize
      void initialize();

      // Interfaces from geomtools::i_locator :
      virtual bool find_geom_id(const geomtools::vector_3d & world_position_,
                                int type_,
                                geomtools::geom_id & gid_,
                                double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const;

      bool find_cell_geom_id(const geomtools::vector_3d & world_position_,
                             geomtools::geom_id & gid_,
                             double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const;
    protected:

      /**
       */
      bool _find_cell_geom_id(const geomtools::vector_3d & in_module_position_,
                              geomtools::geom_id & gid_,
                              double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE);

    public:
      /**! @return the number of the module (>=0).
       */
      uint32_t get_module_number() const;

      /**! @return set the number of the module for this locator.
       */
      void set_module_number(uint32_t module_number_);

      /**! @return the diameter of a cell.
       */
      double get_cell_diameter() const;

      /**! @return the length of a cell (including the base and cathode ring).
       */
      double get_cell_length() const;

      /**! @return the length of anode wires.
       */
      double get_anode_wire_length() const;

      /**! @return the diameter of anode wires.
       */
      double get_anode_wire_diameter() const;

      /**! @return the diameter of field wires.
       */
      double get_field_wire_length() const;

      /**! @return the length field wires.
       */
      double get_field_wire_diameter() const;

      /**! @return the number of sides on the tracking chamber.
       */
      size_t get_number_of_sides() const;

      /**! @return the number of drift cells layers in one side of the tracking chamber.
       */
      size_t get_number_of_layers(uint32_t side_) const;

      /**! @return the number of drift cells rows in one side of the tracking chamber.
       */
      size_t get_number_of_rows(uint32_t side_) const;

      /**! @return the X-position of a cell for specific side and layer (in module coordinate system).
       */
      double get_layer_x(uint32_t side_, uint32_t layer_) const;

      /**! @return the Y-position of a cell for specific side and row (in module coordinate system.
       */
      double get_row_y(uint32_t side_, uint32_t row_) const;


      /**! Compute the position of a cell for specific side, layer and row (in module coordinate system).
       */
      void compute_cell_position(uint32_t side_, uint32_t layer_, uint32_t row_, geomtools::vector_3d & module_position_) const;

      /**! @return the position of a cell for specific side, layer and row (in module coordinate system).
       */
      geomtools::vector_3d get_cell_position(uint32_t side_, uint32_t layer_, uint32_t row_) const;

      /** Tranform a world coordinate system position to the corresponding module coordinate system position.
       */
      void transform_world_to_module(const geomtools::vector_3d & world_position_,
                                     geomtools::vector_3d & module_position_) const;

      /** Tranform a module coordinate system position to the corresponding world coordinate system position.
       */
      void transform_module_to_world(const geomtools::vector_3d & module_position_,
                                     geomtools::vector_3d & world_position_) const;


      /** Check if a world coordinate system position is in the module virtual volume (its bounding envelope).
       */
      bool is_world_position_in_module(const geomtools::vector_3d & world_position_,
                                       double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const;


      /** Check if a module coordinate system position is in the module virtual volume (its bounding envelope).
       */
      bool is_in_module(const geomtools::vector_3d & module_position_,
                        double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const;


      /** Check if a module coordinate system position is in a specific cell virtual volume (its bounding envelope).
       */
      bool is_in_cell(const geomtools::vector_3d & module_position_,
                      uint32_t side_,
                      uint32_t layer_,
                      uint32_t row_,
                      double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const;


      /** Check if a world coordinate system position is in a specific cell virtual volume (its bounding envelope).
       */
      bool is_world_position_in_cell(const geomtools::vector_3d & world_position_,
                                     uint32_t side_,
                                     uint32_t layer_,
                                     uint32_t row_,
                                     double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const;

      /** Given a cell at specific side, layer and row, returns the number of neighbouring cells.
       * If asked, cells on the layer==0 on any side of the chamber are associated to cells on the other
       * side of the source plane.
       */
      size_t get_number_of_neighbours(uint32_t side_,
                                      uint32_t layer_,
                                      uint32_t row_,
                                      bool other_side_ = false) const;

      /** Given a cell with a specific geometry ID, returns the number of neighbouring cells.
       */
      size_t get_number_of_neighbours(const geomtools::geom_id & gid_, bool other_side_ = false) const;

      /** Given a cell at specific side, layer and row, compute the array of geometry IDs for associated neighbouring cells.
       */
      void get_neighbours_ids(uint32_t side_,
                              uint32_t layer_,
                              uint32_t row_,
                              std::vector<geomtools::geom_id> & ids_,
                              bool other_side_ = false) const;

      /** Given a cell with a specific geometry IDs, compute the array of geometry IDs for associated neighbouring cells.
       */
      void get_neighbours_ids(const geomtools::geom_id & gid_,
                              std::vector<geomtools::geom_id> & ids_,
                              bool other_side_ = false) const;

      /** Given a cell with a specific geometry IDs, compute its position in the module coordinate system.
       */
      void get_cell_position(const geomtools::geom_id & gid_,
                             geomtools::vector_3d & position_) const;


      /** Given a cell with a specific side, layer and row, compute its position in the module coordinate system.
       */
      void get_cell_position(uint32_t side_,
                             uint32_t layer_,
                             uint32_t row_,
                             geomtools::vector_3d & position_) const;

      uint32_t extract_module(const geomtools::geom_id & gid_) const;

      uint32_t extract_side(const geomtools::geom_id & gid) const;

      uint32_t extract_layer(const geomtools::geom_id & gid_) const;

      uint32_t extract_row(const geomtools::geom_id & gid_) const;

      /** @arg a_gid the GID to be checked.
       *  @return true if the GID corresponds to a Geiger cell's drift volume.
       */
      bool is_drift_cell_volume(const geomtools::geom_id & gid_) const;

      /** @arg a_gid the GID to be checked
       *  @return true if the GID corresponds to a Geiger cell's drift volume in the
       *  module number associated to the locator.
       */
      bool is_drift_cell_volume_in_current_module(const geomtools::geom_id & gid_) const;

      /// Smart print
      virtual void tree_dump(std::ostream &      out_ = std::clog,
                             const std::string & title_  = "",
                             const std::string & indent_ = "",
                             bool                inherit_ = false) const;

      /// Dump
      void dump(std::ostream & out_ = std::clog) const;

    protected:

      /** Protected contruction method. */
      void _construct();

      /// Set default values
      void _set_defaults();

    private:

      bool _initialized_;

      std::string _module_category_;
      std::string _tracker_volume_category_;
      std::string _tracker_layer_category_;
      std::string _drift_cell_volume_category_;

      int _module_index_;
      int _side_index_;
      int _layer_index_;
      int _row_index_;

      uint32_t    _module_type_;
      uint32_t    _tracker_volume_type_;
      uint32_t    _tracker_layer_type_;
      uint32_t    _cell_type_;
      uint32_t    _module_number_;
      const geomtools::mapping * _mapping_;
      const geomtools::id_mgr  * _id_manager_;

      const geomtools::geom_info * _module_ginfo_;
      const geomtools::placement * _module_world_placement_;
      const geomtools::box       * _module_box_;
      const geomtools::box       * _cell_box_;

      std::vector<double> _back_cell_x_;
      std::vector<double> _front_cell_x_;
      std::vector<double> _back_cell_y_;
      std::vector<double> _front_cell_y_;
      double _anode_wire_length_;
      double _anode_wire_diameter_;
      double _field_wire_length_;
      double _field_wire_diameter_;

      int _module_address_index_;
      int _side_address_index_;
      int _layer_address_index_;
      int _row_address_index_;

      // Submodules are present :
      bool _submodules_[2];

    };

  }  // end of namespace geometry

}  // end of namespace snemo

#endif // FALAISE_SNEMO_GEOMETRY_GG_LOCATOR_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
