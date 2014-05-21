/** \file falaise/snemo/geometry/gveto_locator.h */
/* Author(s)     : Xavier Garrido <garrido@lal.in2p3.fr>
 * Creation date : 2012-06-12
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
 *   Gamma-veto scintillator block locator for one module of the SuperNEMO detector
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_GEOMETRY_GVETO_LOCATOR_H
#define FALAISE_SNEMO_GEOMETRY_GVETO_LOCATOR_H 1

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
  class i_shape_3d;
  class box;
}

namespace snemo {

  namespace geometry {

    /// \brief Fast locator class for SuperNEMO gamma-veto scintillator block volumes
    class gveto_locator : public geomtools::base_locator
    {
    public:

      static const unsigned int NWALLS_PER_SIDE = 2;

      /// \brief Wall identifier constants (SuperNEMO module Z axis)
      enum wall_type
        {
          WALL_INVALID = -1, //!< Invalid/undefined wall
          WALL_BOTTOM  = 0,  //!< Bottom wall
          WALL_TOP     = 1   //!< Top wall
        };

      virtual bool is_initialized () const;

      /// Constructor
      gveto_locator ();

      /// Constructor:
      gveto_locator (const ::geomtools::manager & mgr_,
                     uint32_t module_number_);

      /// Destructor
      virtual ~gveto_locator ();

      virtual void initialize (const datatools::properties & config_);

      virtual void reset ();

      void initialize ();

      bool has_submodule (uint32_t side_) const;

      bool id_is_valid (uint32_t side_, uint32_t wall_, uint32_t column_) const;

      // Interfaces from geomtools::i_locator :
      virtual bool find_geom_id (const geomtools::vector_3d & world_position_,
                                 int type_,
                                 geomtools::geom_id & gid_,
                                 double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const;

      bool find_block_geom_id (const geomtools::vector_3d & world_position_,
                               geomtools::geom_id & gid_,
                               double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const;
    protected:

      /**
       */
      bool find_block_geom_id_ (const geomtools::vector_3d & in_module_position_,
                                geomtools::geom_id & gid_,
                                double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE);

      void _set_defaults_ ();

    public:
      /**! @return the number of the module (>=0).
       */
      uint32_t get_module_number () const;

      /**! set the number of the module for this locator.
       */
      void set_module_number (uint32_t module_number_);

      /**! check if block is partitioned in the current setup.
       */
      bool is_block_partitioned () const;

      void dump (std::ostream & out_ = std::clog) const;

      /**! @return the width of a calorimeter block.
       */
      double get_block_width () const;

      /**! @return the height of a calorimeter block.
       */
      double get_block_height () const;

      /**! @return the thickness of a block.
       */
      double get_block_thickness () const;

      /**! @return the number of sides on the calorimeter.
       */
      size_t get_number_of_sides () const;

      /**! @return the number of walls on the calorimeter.
       */
      size_t get_number_of_walls () const;

      /**! @return the number of columns in one side and one wall of the calorimeter.
       */
      size_t get_number_of_columns (uint32_t side_, uint32_t wall_) const;

      /**! @return the Z-position of a wall for specific side and wall (in module coordinate system).
       */
      double get_wall_z (uint32_t side_, uint32_t wall_) const;

      /**! @return the Z-position of the entrance window of a wall for specific side and wall (in module coordinate system).
       */
      double get_wall_window_z (uint32_t side_, uint32_t wall_) const;

      /**! @return the X-position of a block for specific side, wall and column (in module coordinate system).
       */
      double get_column_x (uint32_t side_, uint32_t wall_, uint32_t column_) const;

      /**! @return the Y-position of a block for specific side, wall and column (in module coordinate system).
       */
      double get_column_y (uint32_t side_, uint32_t wall_, uint32_t column_) const;

      /**! Compute the position of a block
       *   for specific side, wall and column (in module coordinate system).
       */
      void compute_block_position (uint32_t side_,
                                   uint32_t wall_,
                                   uint32_t column_,
                                   geomtools::vector_3d & module_position_) const;

      /**! Compute the position of the center of the entrance window of a block
       *   for specific side and column (in module coordinate system).
       */
      void compute_block_window_position (uint32_t side_,
                                          uint32_t wall_,
                                          uint32_t column_,
                                          geomtools::vector_3d & module_position_) const;

      /**! @return the position of a block for specific side, wall and column (in module coordinate system).
       */
      geomtools::vector_3d get_block_position (uint32_t side_, uint32_t wall_, uint32_t column_) const;

      /**! @return the position of the center of the entrance window of a block
       *   for specific side, wall and column (in module coordinate system).
       */
      geomtools::vector_3d get_block_window_position (uint32_t side_, uint32_t wall_, uint32_t column_) const;

      /** Tranform a world coordinate system position to the corresponding module coordinate system position.
       */
      void transform_world_to_module (const geomtools::vector_3d & world_position_,
                                      geomtools::vector_3d & module_position_) const;

      /** Tranform a module coordinate system position to the corresponding world coordinate system position.
       */
      void transform_module_to_world (const geomtools::vector_3d & module_position_,
                                      geomtools::vector_3d & world_position_) const;


      /** Check if a world coordinate system position is in the module virtual volume (its bounding envelope).
       */
      bool is_world_position_in_module (const geomtools::vector_3d & world_position_,
                                        double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const;


      /** Check if a module coordinate system position is in the module virtual volume (its bounding envelope).
       */
      bool is_in_module (const geomtools::vector_3d & module_position_,
                         double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const;


      /** Check if a module coordinate system position is in a specific block virtual volume (its bounding envelope).
       */
      bool is_in_block (const geomtools::vector_3d & module_position_,
                        uint32_t side_,
                        uint32_t wall_,
                        uint32_t column_,
                        double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const;


      /** Check if a world coordinate system position is in a specific block virtual volume (its bounding envelope).
       */
      bool is_world_position_in_block (const geomtools::vector_3d & world_position_,
                                       uint32_t side_,
                                       uint32_t wall_,
                                       uint32_t column_,
                                       double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const;

      /** Given a block at specific side, wall and column, returns the number of neighbouring blocks.
       */
      size_t get_number_of_neighbours (uint32_t side_,
                                       uint32_t wall_,
                                       uint32_t column_,
                                       uint8_t mask_ = utils::NEIGHBOUR_FIRST) const;

      /** Given a block with a specific geometry ID, returns the number of neighbouring blocks.
       */
      size_t get_number_of_neighbours (const geomtools::geom_id & gid_,
                                       uint8_t mask_ = utils::NEIGHBOUR_FIRST) const;

      /** Given a block at specific side, wall and column, compute the array of geometry IDs for associated neighbouring blocks.
       */
      void get_neighbours_ids (uint32_t side_,
                               uint32_t wall_,
                               uint32_t column_,
                               std::vector<geomtools::geom_id> & ids_,
                               uint8_t mask_ = utils::NEIGHBOUR_FIRST) const;

      /** Given a block with a specific geometry IDs, compute the array of geometry IDs for associated neighbouring blocks.
       */
      void get_neighbours_ids (const geomtools::geom_id & gid_,
                               std::vector<geomtools::geom_id> & ids_,
                               uint8_t mask_ = utils::NEIGHBOUR_FIRST) const;

      /** Given a block with a specific geometry IDs, compute its position in the module coordinate system.
       */
      void get_block_position (const geomtools::geom_id & gid_,
                               geomtools::vector_3d & position_) const;


      /** Given a block with a specific side, wall and column, compute its position in the module coordinate system.
       */
      void get_block_position (uint32_t side_,
                               uint32_t wall_,
                               uint32_t column_,
                               geomtools::vector_3d & position_) const;

      int get_module_address_index () const;

      int get_side_address_index () const;

      int get_wall_address_index () const;

      int get_column_address_index () const;

      int get_part_address_index () const;

      uint32_t extract_module (const geomtools::geom_id & gid_) const;

      uint32_t extract_side (const geomtools::geom_id & gid_) const;

      uint32_t extract_wall (const geomtools::geom_id & gid_) const;

      uint32_t extract_column (const geomtools::geom_id & gid_) const;

      uint32_t extract_part (const geomtools::geom_id & gid_) const;

      bool is_calo_block (const geomtools::geom_id & gid_) const;

      bool is_calo_block_in_current_module (const geomtools::geom_id & gid_) const;

    protected:

      void _construct ();

    private:

      bool _initialized_;

      // Configuration parameters :
      uint32_t        _module_number_;
      uint32_t        _block_part_;

      // Running values :
      uint32_t    _module_type_;
      uint32_t    _tracker_submodule_type_;
      uint32_t    _block_type_;
      uint32_t    _wrapper_type_;
      bool        _block_partitioned_;

      const geomtools::mapping    * _mapping_;
      const geomtools::id_mgr     * _id_manager_;

      const geomtools::geom_info  * _module_ginfo_;
      const geomtools::placement  * _module_world_placement_;
      const geomtools::box        * _module_box_;
      const geomtools::i_shape_3d * _block_shape_;
      bool                          _composite_block_shape_;
      const geomtools::box        * _block_box_;
      double              _block_z_[2][NWALLS_PER_SIDE];
      double              _block_window_z_[2][NWALLS_PER_SIDE];
      std::vector<double> _back_block_x_[NWALLS_PER_SIDE];
      std::vector<double> _front_block_x_[NWALLS_PER_SIDE];
      std::vector<double> _back_block_y_[NWALLS_PER_SIDE];
      std::vector<double> _front_block_y_[NWALLS_PER_SIDE];

      double _block_width_;
      double _block_height_;
      double _block_thickness_;

      // Block GID addressing :
      int _module_address_index_;
      int _side_address_index_;
      int _wall_address_index_;
      int _column_address_index_;
      int _part_address_index_;

      // Submodules are present :
      bool _submodules_[2];

    };

  }  // end of namespace geometry

}  // end of namespace snemo

#endif // FALAISE_SNEMO_GEOMETRY_GVETO_LOCATOR_H

// end of falaise/snemo/geometry/gveto_locator.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
