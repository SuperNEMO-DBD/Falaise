/* gg_locator.cc
 *
 * Copyright (C) 2011-2012 Francois Mauger <mauger@lpccaen.in2p3.fr>
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

#include <stdexcept>

#include <sngeometry/gg_locator.h>

#include <datatools/utils.h>
#include <datatools/units.h>
#include <datatools/version_id.h>

#include <geomtools/box.h>
#include <geomtools/cylinder.h>
#include <geomtools/manager.h>

namespace snemo {

  namespace geometry {

    bool gg_locator::is_initialized () const
    {
      return _initialized_;
    }

    double gg_locator::get_cell_diameter () const
    {
      DT_THROW_IF (! is_initialized (), std::logic_error, "Locator is not initialized !");
      return _cell_box_->get_x ();
    }

    double gg_locator::get_cell_length () const
    {
      DT_THROW_IF (! is_initialized (), std::logic_error, "Locator is not initialized !");
      return _cell_box_->get_z ();
    }

    double gg_locator::get_anode_wire_length () const
    {
      DT_THROW_IF (! is_initialized (), std::logic_error, "Locator is not initialized !");
      return _anode_wire_length_;
    }

    double gg_locator::get_anode_wire_diameter () const
    {
      DT_THROW_IF (! is_initialized (), std::logic_error, "Locator is not initialized !");
      return _anode_wire_diameter_;
    }

    double gg_locator::get_field_wire_length () const
    {
      DT_THROW_IF (! is_initialized (), std::logic_error, "Locator is not initialized !");
      return _field_wire_length_;
    }

    double gg_locator::get_field_wire_diameter () const
    {
      DT_THROW_IF (! is_initialized (), std::logic_error, "Locator is not initialized !");
      return _field_wire_diameter_;
    }

    size_t gg_locator::get_number_of_sides () const
    {
      DT_THROW_IF (! is_initialized (), std::logic_error, "Locator is not initialized !");
      return NSIDES;
    }

    double gg_locator::get_layer_x (uint32_t side_, uint32_t layer_) const
    {
      DT_THROW_IF (! is_initialized (), std::logic_error, "Locator is not initialized !");
      if (side_ == 0)
        {
          DT_THROW_IF (layer_ >= _back_cell_x_.size (),
                       std::logic_error,
                       "Invalid layer number (" << layer_ << ">" << _back_cell_x_.size () - 1 << ")!");
          return _back_cell_x_[layer_];
        }
      else
        {
          DT_THROW_IF (layer_ >= _front_cell_x_.size (),
                       std::logic_error,
                       "Invalid layer number (" << layer_ << ">" << _front_cell_x_.size () - 1 << ")!");
          return _front_cell_x_[layer_];
        }
    }

    double gg_locator::get_row_y (uint32_t side_, uint32_t row_) const
    {
      DT_THROW_IF (! is_initialized (), std::logic_error, "Locator is not initialized !");
      DT_THROW_IF (side_ > 1, std::logic_error, "Invalid side number (" << side_ << "> 1)!");
      if (side_ == 0)
        {
          DT_THROW_IF (row_ >= _back_cell_y_.size (),
                       std::logic_error,
                       "Invalid row number (" << row_ << ">" << _back_cell_y_.size () - 1 << ")!");
          return _back_cell_y_[row_];
        }
      else
        {
          DT_THROW_IF (row_ >= _front_cell_y_.size (),
                       std::logic_error,
                       "Invalid row number (" << row_ << ">" << _front_cell_y_.size () - 1 << ")!");
          return _front_cell_y_[row_];
        }
    }

    void gg_locator::compute_cell_position (uint32_t side_,
                                            uint32_t layer_,
                                            uint32_t row_,
                                            geomtools::vector_3d & module_position_) const
    {
      geomtools::invalidate (module_position_);
      module_position_.set (get_layer_x (side_, layer_), get_row_y (side_, row_), 0.0);
      return;
    }


    geomtools::vector_3d gg_locator::get_cell_position (uint32_t side_, uint32_t layer_, uint32_t row_) const
    {
      geomtools::vector_3d module_cell_pos;
      compute_cell_position (side_, layer_, row_, module_cell_pos);
      return module_cell_pos;
    }

    size_t gg_locator::get_number_of_neighbours (const geomtools::geom_id & gid_,
                                                 bool other_side_) const
    {
      DT_THROW_IF (! is_initialized (), std::logic_error, "Locator is not initialized !");
      DT_THROW_IF (gid_.get (0) != _module_number_,
                   std::logic_error,
                   "Invalid module number (" << gid_.get (0) << "!=" <<_module_number_ << ")!");
      return get_number_of_neighbours (gid_.get (1),
                                       gid_.get (2),
                                       gid_.get (3),
                                       other_side_);
    }

    void gg_locator::get_cell_position (const geomtools::geom_id & gid_,
                                        geomtools::vector_3d & position_) const
    {
      DT_THROW_IF (! is_initialized (), std::logic_error, "Locator is not initialized !");
      DT_THROW_IF (gid_.get (0) != _module_number_,
                   std::logic_error,
                   "Invalid module number (" << gid_.get (0) << "!=" <<_module_number_ << ")!");
      return get_cell_position (gid_.get (1),
                                gid_.get (2),
                                gid_.get (3),
                                position_);
      return;
    }

    void gg_locator::get_cell_position (uint32_t side_,
                                        uint32_t layer_,
                                        uint32_t row_,
                                        geomtools::vector_3d & position_) const
    {
      DT_THROW_IF (! is_initialized (), std::logic_error, "Locator is not initialized !");
      DT_THROW_IF (side_ > 1, std::logic_error, "Invalid side number (" << side_ << "> 1)!");
      geomtools::invalidate (position_);
      if (side_ == 0 )
        {
          DT_THROW_IF (layer_ >= _back_cell_x_.size (),
                       std::logic_error,
                       "Invalid layer number (" << layer_ << ">" << _back_cell_x_.size () - 1 << ")!");
          DT_THROW_IF (row_ >= _back_cell_y_.size (),
                       std::logic_error,
                       "Invalid row number (" << row_ << ">" << _back_cell_y_.size () - 1 << ")!");
          position_.set (_back_cell_x_[layer_], _back_cell_y_[row_], 0.0);
        }
      else
        {
          DT_THROW_IF (layer_ >= _front_cell_x_.size (),
                       std::logic_error,
                       "Invalid layer number (" << layer_ << ">" << _front_cell_x_.size () - 1 << ")!");
          DT_THROW_IF (row_ >= _front_cell_y_.size (),
                       std::logic_error,
                       "Invalid row number (" << row_ << ">" << _front_cell_y_.size () - 1 << ")!");
          position_.set (_front_cell_x_[layer_], _front_cell_y_[row_], 0.0);
        }
      return;
    }

    void gg_locator::get_neighbours_ids (const geomtools::geom_id & gid_,
                                         std::vector<geomtools::geom_id> & ids_,
                                         bool other_side_) const
    {
      DT_THROW_IF (! is_initialized (), std::logic_error, "Locator is not initialized !");
      DT_THROW_IF (gid_.get (0) != _module_number_,
                   std::logic_error,
                   "Invalid module number (" << gid_.get (0) << "!=" << _module_number_ << ")!");
      get_neighbours_ids (gid_.get (1),
                          gid_.get (2),
                          gid_.get (3),
                          ids_,
                          other_side_);
      return;
    }

    void gg_locator::get_neighbours_ids (uint32_t side_,
                                         uint32_t layer_,
                                         uint32_t row_,
                                         std::vector<geomtools::geom_id> & ids_,
                                         bool other_side_) const
    {
      DT_THROW_IF (! is_initialized (), std::logic_error, "Locator is not initialized !");
      DT_THROW_IF (side_ > 1, std::logic_error, "Invalid side number (" << side_ << "> 1)!");

      ids_.clear ();
      ids_.reserve (8);

      // back
      if (side_ == 0)
        {
          DT_THROW_IF (layer_ >= _back_cell_x_.size (),
                       std::logic_error,
                       "Invalid layer number (" << layer_ << ">" << _back_cell_x_.size () - 1 << ")!");
          DT_THROW_IF (row_ >= _back_cell_y_.size (),
                       std::logic_error,
                       "Invalid row number (" << row_ << ">" << _back_cell_y_.size () - 1 << ")!");
          if (layer_ > 0)
            {
              /*  L+1 L L-1
               *  [ ][ ][ ]
               *  [ ][.][x]
               *  [ ][ ][ ]
               */
              ids_.push_back (geomtools::geom_id (_cell_type_,
                                                  _module_number_,
                                                  side_,
                                                  layer_ - 1,
                                                  row_));
            }
          if (layer_ < (_back_cell_x_.size () - 1))
            {
              /*  L+1 L L-1
               *  [ ][ ][ ] R+1
               *  [x][.][ ] R
               *  [ ][ ][ ] R-1
               */
              ids_.push_back (geomtools::geom_id (_cell_type_,
                                                  _module_number_,
                                                  side_,
                                                  layer_ + 1,
                                                  row_));
            }
          if (row_ > 0)
            {
              /*  L+1 L L-1
               *  [ ][ ][ ] R+1
               *  [ ][.][ ] R
               *  [ ][x][ ] R-1
               */
              ids_.push_back (geomtools::geom_id (_cell_type_,
                                                  _module_number_,
                                                  side_,
                                                  layer_,
                                                  row_ - 1));
            }
          if (row_ < (_back_cell_y_.size () - 1))
            {
              /*  L+1 L L-1
               *  [ ][x][ ] R+1
               *  [ ][.][ ] R
               *  [ ][ ][ ] R-1
               */
              ids_.push_back (geomtools::geom_id (_cell_type_,
                                                  _module_number_,
                                                  side_,
                                                  layer_,
                                                  row_ + 1));
            }

          if ((layer_ < (_back_cell_x_.size () - 1)) && (row_ > 0))
            {
              /*  L+1 L L-1
               *  [ ][ ][ ] R+1
               *  [ ][.][ ] R
               *  [x][ ][ ] R-1
               */
              ids_.push_back (geomtools::geom_id (_cell_type_,
                                                  _module_number_,
                                                  side_,
                                                  layer_ + 1,
                                                  row_ - 1));
            }
          if ((layer_ < (_back_cell_x_.size () - 1)) && (row_ < (_back_cell_y_.size () - 1)))
            {
              /*  L+1 L L-1
               *  [x][ ][ ] R+1
               *  [ ][.][ ] R
               *  [ ][ ][ ] R-1
               */
              ids_.push_back (geomtools::geom_id (_cell_type_,
                                                  _module_number_,
                                                  side_,
                                                  layer_ + 1,
                                                  row_ + 1));
            }
          if ((layer_ > 0) && (row_ > 0))
            {
              /*  L+1 L L-1
               *  [ ][ ][ ] R+1
               *  [ ][.][ ] R
               *  [ ][ ][x] R-1
               */
              ids_.push_back (geomtools::geom_id (_cell_type_,
                                                  _module_number_,
                                                  side_,
                                                  layer_ - 1,
                                                  row_ - 1));
            }
          if ((layer_ > 0) && (row_ < (_back_cell_y_.size () - 1)))
            {
              /*  L+1 L L-1
               *  [ ][ ][x] R+1
               *  [ ][.][ ] R
               *  [ ][ ][ ] R-1
               */
              ids_.push_back (geomtools::geom_id (_cell_type_,
                                                  _module_number_,
                                                  side_,
                                                  layer_ - 1,
                                                  row_ + 1));
            }
          if ((layer_ == 0) && (row_ > 0) && other_side_)
            {
              /*   1  0     0
               *  [ ][ ] | [ ] R+1
               *  [ ][.] | [ ] R
               *  [ ][ ] | [x] R-1
               */
              ids_.push_back (geomtools::geom_id (_cell_type_,
                                                  _module_number_,
                                                  side_ + 1,
                                                  0,
                                                  row_ - 1));
            }
          if ((layer_ == 0) && other_side_)
            {
              /*   1  0     0
               *  [ ][ ] | [ ] R+1
               *  [ ][.] | [x] R
               *  [ ][ ] | [ ] R-1
               */
              ids_.push_back (geomtools::geom_id (_cell_type_,
                                                  _module_number_,
                                                  side_ + 1,
                                                  0,
                                                  row_));
            }
          if ((layer_ == 0) && (row_ < (_back_cell_y_.size () - 1)) && other_side_)
            {
              /*   1  0     0
               *  [ ][ ] | [x] R+1
               *  [ ][.] | [ ] R
               *  [ ][ ] | [ ] R-1
               */
              ids_.push_back (geomtools::geom_id (_cell_type_,
                                                  _module_number_,
                                                  side_ + 1,
                                                  0,
                                                  row_ + 1));
            }
        }
      // front:
      if (side_ == 1)
        {
          DT_THROW_IF (layer_ >= _front_cell_x_.size (),
                       std::logic_error,
                       "Invalid layer number (" << layer_ << ">" << _front_cell_x_.size () - 1 << ")!");
          DT_THROW_IF (row_ >= _front_cell_y_.size (),
                       std::logic_error,
                       "Invalid row number (" << row_ << ">" << _front_cell_y_.size () - 1 << ")!");
          if (layer_ > 0)
            {
              /*  L-1 L L+1
               *  [ ][ ][ ]
               *  [x][.][ ]
               *  [ ][ ][ ]
               */
              ids_.push_back (geomtools::geom_id (_cell_type_,
                                                  _module_number_,
                                                  side_,
                                                  layer_ - 1,
                                                  row_));
            }
          if (layer_ < (_front_cell_x_.size () - 1))
            {
              /*  L-1 L L+1
               *  [ ][ ][ ] R+1
               *  [ ][.][x] R
               *  [ ][ ][ ] R-1
               */
              ids_.push_back (geomtools::geom_id (_cell_type_,
                                                  _module_number_,
                                                  side_,
                                                  layer_ + 1,
                                                  row_));
            }
          if (row_ > 0)
            {
              /*  L-1 L L+1
               *  [ ][ ][ ] R+1
               *  [ ][.][ ] R
               *  [ ][x][ ] R-1
               */
              ids_.push_back (geomtools::geom_id (_cell_type_,
                                                  _module_number_,
                                                  side_,
                                                  layer_,
                                                  row_ - 1));
            }
          if (row_ < (_front_cell_y_.size () - 1))
            {
              /*  L-1 L L+1
               *  [ ][x][ ] R+1
               *  [ ][.][ ] R
               *  [ ][ ][ ] R-1
               */
              ids_.push_back (geomtools::geom_id (_cell_type_,
                                                  _module_number_,
                                                  side_,
                                                  layer_,
                                                  row_ + 1));
            }

          if ((layer_ < (_front_cell_x_.size () - 1)) && (row_ > 0))
            {
              /*  L-1 L L+1
               *  [ ][ ][ ] R+1
               *  [ ][.][ ] R
               *  [ ][ ][x] R-1
               */
              ids_.push_back (geomtools::geom_id (_cell_type_,
                                                  _module_number_,
                                                  side_,
                                                  layer_ + 1,
                                                  row_ - 1));
            }
          if ((layer_ < (_front_cell_x_.size () - 1)) && (row_ < (_front_cell_y_.size () - 1)))
            {
              /*  L-1 L L+1
               *  [ ][ ][x] R+1
               *  [ ][.][ ] R
               *  [ ][ ][ ] R-1
               */
              ids_.push_back (geomtools::geom_id (_cell_type_,
                                                  _module_number_,
                                                  side_,
                                                  layer_ + 1,
                                                  row_ + 1));
            }
          if ((layer_ > 0) && (row_ > 0))
            {
              /*  L-1 L L+1
               *  [ ][ ][ ] R+1
               *  [ ][.][ ] R
               *  [x][ ][ ] R-1
               */
              ids_.push_back (geomtools::geom_id (_cell_type_,
                                                  _module_number_,
                                                  side_,
                                                  layer_ - 1,
                                                  row_ - 1));
            }
          if ((layer_ > 0) && (row_ < (_front_cell_y_.size () - 1)))
            {
              /*  L-1 L L+1
               *  [x][ ][ ] R+1
               *  [ ][.][ ] R
               *  [ ][ ][ ] R-1
               */
              ids_.push_back (geomtools::geom_id (_cell_type_,
                                                  _module_number_,
                                                  side_,
                                                  layer_ - 1,
                                                  row_ + 1));
            }
          if ((layer_ == 0) && (row_ > 0) && other_side_)
            {
              /*   0     0  1
               *  [ ] | [ ][ ] R+1
               *  [ ] | [.][ ] R
               *  [x] | [ ]| [x] R-1
               */
              ids_.push_back (geomtools::geom_id (_cell_type_,
                                                  _module_number_,
                                                  side_ - 1,
                                                  0,
                                                  row_ - 1));
            }
          if ((layer_ == 0) && other_side_)
            {
              /*   0     0  1
               *  [ ] | [ ][ ] R+1
               *  [x] | [.][ ] R
               *  [ ] | [ ]| [x] R-1
               */
              ids_.push_back (geomtools::geom_id (_cell_type_,
                                                  _module_number_,
                                                  side_ - 1,
                                                  0,
                                                  row_));
            }
          if ((layer_ == 0) && (row_ < (_front_cell_y_.size () - 1)) && other_side_)
            {
              /*   0     0  1
               *  [x] | [ ][ ] R+1
               *  [ ] | [.][ ] R
               *  [ ] | [ ]| [x] R-1
               */
              ids_.push_back (geomtools::geom_id (_cell_type_,
                                                  _module_number_,
                                                  side_ - 1,
                                                  0,
                                                  row_ + 1));
            }
        }
      return;
    }

    size_t gg_locator::get_number_of_neighbours (uint32_t side_,
                                                 uint32_t layer_,
                                                 uint32_t row_,
                                                 bool     other_side_) const
    {
      DT_THROW_IF (! is_initialized (), std::logic_error, "Locator is not initialized !");
      bool corner = false;
      bool side = false;
      size_t plus = 0;
      if (side_ == 0)
        {
          if ( (layer_ == 0) || (layer_ == _back_cell_x_.size () - 1) )
            {
              if ( (row_ == 0) || (row_ == _back_cell_y_.size () - 1) )
                {
                  corner = true;
                  if (other_side_ && (layer_ == 0))
                    {
                      plus = 2;
                    }
                }
              else
                {
                  if (other_side_ && (layer_ == 0))
                    {
                      plus = 3;
                    }
                  side = true;
                }
            }
        }
      if (side_ == 1)
        {
          if ( (layer_ == 0) || (layer_ == _front_cell_x_.size () - 1) )
            {
              if ( (row_ == 0) || (row_ == _front_cell_y_.size () - 1) )
                {
                  corner = true;
                  if (other_side_ && (layer_ == 0))
                    {
                      plus = 2;
                    }
                }
              else
                {
                  if (other_side_ && (layer_ == 0))
                    {
                      plus = 3;
                    }
                  side = true;
                }
            }
        }
      if (corner) return 3 + plus;
      if (side) return 5 + plus;
      return 8;
    }

    size_t gg_locator::get_number_of_layers (uint32_t side_) const
    {
      DT_THROW_IF (! is_initialized (), std::logic_error, "Locator is not initialized !");
      DT_THROW_IF (side_ > 1, std::logic_error, "Invalid side number " << side_ << " !");
      size_t nbr_layers = 0;
      if (side_ == 0)
        {
          nbr_layers = _back_cell_x_.size ();
        }
      else if (side_ == 1)
        {
          nbr_layers = _front_cell_x_.size ();
        }
      return nbr_layers;
    }

    size_t gg_locator::get_number_of_rows (uint32_t side_) const
    {
      DT_THROW_IF (! is_initialized (), std::logic_error, "Locator is not initialized !");
      DT_THROW_IF (side_ > 1, std::logic_error, "Invalid side number " << side_ << " !");
      size_t nbr_rows = 0;
      if (side_ == 0)
        {
          nbr_rows = _back_cell_y_.size ();
        }
      else if (side_ == 1)
        {
          nbr_rows = _front_cell_y_.size ();
        }
      return nbr_rows;
    }

    uint32_t gg_locator::extract_module (const geomtools::geom_id & gid_) const
    {
      return gid_.get (_module_address_index_);
    }

    uint32_t gg_locator::extract_side (const geomtools::geom_id & gid_) const
    {
      return gid_.get (_side_address_index_);
    }

    uint32_t gg_locator::extract_layer (const geomtools::geom_id & gid_) const
    {
      return gid_.get (_layer_address_index_);
    }

    uint32_t gg_locator::extract_row (const geomtools::geom_id & gid_) const
    {
      return gid_.get (_row_address_index_);
    }

    bool gg_locator::is_drift_cell_volume (const geomtools::geom_id & gid_) const
    {
      return gid_.get_type () == _cell_type_;
    }

    bool gg_locator::is_drift_cell_volume_in_current_module (const geomtools::geom_id & gid_) const
    {
      return is_drift_cell_volume (gid_)
        && (extract_module (gid_) == _module_number_);
    }

    void gg_locator::_set_defaults ()
    {
      _module_category_            = "module";
      _tracker_volume_category_    = "tracker_volume";
      _tracker_layer_category_     = "tracker_layer";
      _drift_cell_volume_category_ = "drift_cell_core";

      _module_type_         = geomtools::geom_id::INVALID_TYPE;
      _tracker_volume_type_ = geomtools::geom_id::INVALID_TYPE;
      _tracker_layer_type_  = geomtools::geom_id::INVALID_TYPE;
      _cell_type_           = geomtools::geom_id::INVALID_TYPE;

      // Cell GID subaddresses :
      _module_index_        = -1;
      _side_index_          = -1;
      _layer_index_         = -1;
      _row_index_           = -1;

      _module_number_ = geomtools::geom_id::INVALID_ADDRESS;
      _mapping_       = 0;
      _id_manager_    = 0;
      _module_ginfo_  = 0;
      _module_box_    = 0;
      _cell_box_      = 0;

      _tolerance_ = GEOMTOOLS_DEFAULT_TOLERANCE;
      datatools::invalidate (_anode_wire_length_);
      datatools::invalidate (_anode_wire_diameter_);
      datatools::invalidate (_field_wire_length_);
      datatools::invalidate (_field_wire_diameter_);

      _initialized_ = false;
      return;
    }


    // ctor:
    gg_locator::gg_locator () : base_locator ()
    {
      _set_defaults ();
      return;
    }

    // ctor:
    gg_locator::gg_locator (const ::geomtools::manager & mgr_,
                            uint32_t module_number_) :
      base_locator ()
    {
      _set_defaults ();

      // Setup :
      set_geo_manager (mgr_);
      set_module_number (module_number_);

      return;
    }

    // dtor:
    gg_locator::~gg_locator ()
    {
      if (is_initialized ())
        {
          reset ();
        }
      return;
    }

    void gg_locator::_construct ()
    {
      _mapping_ = & get_geo_manager ().get_mapping ();
      _id_manager_ = & get_geo_manager ().get_id_mgr ();

      const geomtools::id_mgr::categories_by_name_col_type & categories
        =  _id_manager_->categories_by_name ();

      DT_THROW_IF (categories.find (_module_category_) == categories.end (),
                   std::logic_error,
                   "No category named '" << _module_category_ << "' !");
      _module_type_ = categories.find (_module_category_)->second.get_type ();

      DT_THROW_IF (categories.find (_tracker_volume_category_) == categories.end (),
                   std::logic_error,
                   "No category named '" << _tracker_volume_category_ << "' !");
      _tracker_volume_type_ = categories.find (_tracker_volume_category_)->second.get_type ();

      DT_THROW_IF (categories.find (_tracker_layer_category_) == categories.end (),
                   std::logic_error,
                   "No category named '" << _tracker_layer_category_ << "' !");
      _tracker_layer_type_  = categories.find (_tracker_layer_category_)->second.get_type ();

      DT_THROW_IF (categories.find (_drift_cell_volume_category_) == categories.end (),
                   std::logic_error,
                   "No category named '" << _drift_cell_volume_category_ << "' !");

      const geomtools::id_mgr::category_info & cell_cat_info = categories.find (_drift_cell_volume_category_)->second;
      _cell_type_    = cell_cat_info.get_type ();
      _module_index_ = cell_cat_info.get_subaddress_index ("module");
      _side_index_   = cell_cat_info.get_subaddress_index ("side");
      _layer_index_  = cell_cat_info.get_subaddress_index ("layer");
      _row_index_    = cell_cat_info.get_subaddress_index ("row");

      const geomtools::geom_id module_gid (_module_type_, _module_number_);
      DT_THROW_IF (! _mapping_->validate_id (module_gid),
                   std::logic_error,
                   "No module with ID = " << module_gid << " !");
      _module_ginfo_ = & _mapping_->get_geom_info (module_gid);

      const geomtools::i_shape_3d * a_shape = & _module_ginfo_->get_logical ().get_shape ();
      DT_THROW_IF (a_shape->get_shape_name () != "box",
                   std::logic_error,
                   "Cannot extract the box shape from module with ID = " << module_gid << " !");
      _module_box_ = dynamic_cast<const geomtools::box * > (a_shape);
      _module_world_placement_ = & _module_ginfo_->get_world_placement ();

      // pick up the first available cell :
      const geomtools::geom_id cell_gid (_cell_type_, _module_number_, 0, 0, 0);
      DT_THROW_IF (! _mapping_->validate_id (cell_gid),
                   std::logic_error,
                   "Cannot extract information about a cell with ID = " << cell_gid << " !");

      const geomtools::geom_info & cell_ginfo = _mapping_->get_geom_info (cell_gid);
      const geomtools::i_shape_3d & b_shape = cell_ginfo.get_logical ().get_shape ();
      DT_THROW_IF (b_shape.get_shape_name () != "box",
                   std::logic_error,
                   "Cannot extract the box shape from cell with ID = " << cell_gid << " !");
      _cell_box_ = dynamic_cast<const geomtools::box * > (&b_shape);

      const size_t NSIDES=  2;

      std::vector<double> * vlx[NSIDES];
      vlx[0] = &_back_cell_x_;
      vlx[1] = &_front_cell_x_;
      for (unsigned int side = 0; side < NSIDES; side++)
        {
          size_t i_layer = 0;
          vlx[side]->reserve (10);
          while (true)
            {
              geomtools::geom_id a_cell_gid (_cell_type_, _module_number_, side, i_layer, 0);
              if (! _mapping_->validate_id (a_cell_gid))
                {
                  break;
                }
              const geomtools::geom_info & a_cell_ginfo = _mapping_->get_geom_info (a_cell_gid);
              const geomtools::placement & a_cell_world_placement = a_cell_ginfo.get_world_placement ();
              geomtools::placement a_cell_module_placement;
              _module_world_placement_->relocate (a_cell_world_placement, a_cell_module_placement);
              const double x = a_cell_module_placement.get_translation ().x ();
              vlx[side]->push_back (x);
              i_layer++;
            }
        }

      std::vector<double> * vcy[NSIDES];
      vcy[0] = &_back_cell_y_;
      vcy[1] = &_front_cell_y_;
      for (unsigned int side = 0; side < NSIDES; side++)
        {
          size_t i_cell = 0;
          vlx[side]->reserve (130);
          while (true)
            {
              geomtools::geom_id a_cell_gid (_cell_type_, _module_number_, side, 0, i_cell);
              if (! _mapping_->validate_id (a_cell_gid))
                {
                  break;
                }
              const geomtools::geom_info & a_cell_ginfo = _mapping_->get_geom_info (a_cell_gid);
              const geomtools::placement & a_cell_world_placement = a_cell_ginfo.get_world_placement ();
              geomtools::placement a_cell_module_placement;
              _module_world_placement_->relocate (a_cell_world_placement, a_cell_module_placement);
              const double y = a_cell_module_placement.get_translation ().y ();
              vcy[side]->push_back (y);
              i_cell++;
            }
        }

      // analyse the geometry versioning :
      datatools::version_id geom_mgr_setup_vid;
      get_geo_manager ().fetch_setup_version_id (geom_mgr_setup_vid);

      {
        // extract the geometry model associated to the "anode_wire" :
        const geomtools::i_model * anode_wire_model = 0;
        std::string model_name = "anode_wire.model"; // default model name
        if (geom_mgr_setup_vid.has_major ())
          {
            if (geom_mgr_setup_vid.get_major () < 2)
              {
                model_name = "anode_wire";
              }
          }

        geomtools::models_col_type::const_iterator found
          = get_geo_manager ().get_factory ().get_models ().find (model_name);
        DT_THROW_IF (found == get_geo_manager ().get_factory ().get_models ().end (),
                     std::logic_error,
                     "You should have found the '" << model_name << "' model here !");
        anode_wire_model = found->second;

        // get the associated "logical volume" :
        const geomtools::logical_volume & anode_wire_log = anode_wire_model->get_logical ();
        // check if there is a concrete shape (it should !):
        DT_THROW_IF (! anode_wire_log.has_shape (), std::logic_error, "You should find a shape here !");

        // get the 3D shape :
        const geomtools::i_shape_3d & anode_wire_shape = anode_wire_log.get_shape ();

        // we know the concret shape class is a box, so we cast it :
        const geomtools::cylinder & anode_wire_cylinder
          = dynamic_cast<const geomtools::cylinder &> (anode_wire_shape);

        _anode_wire_length_   = anode_wire_cylinder.get_z ();
        _anode_wire_diameter_ = anode_wire_cylinder.get_diameter ();
      }

      {
        // extract the geometry model associated to the "field_wire" :
        const geomtools::i_model * field_wire_model = 0;
        std::string model_name = "field_wire.model"; // default model name
        if (geom_mgr_setup_vid.has_major ())
          {
            if (geom_mgr_setup_vid.get_major () < 2)
              {
                model_name = "field_wire";
              }
          }

        geomtools::models_col_type::const_iterator found
          = get_geo_manager ().get_factory ().get_models ().find (model_name);
        DT_THROW_IF (found == get_geo_manager ().get_factory ().get_models ().end (),
                     std::logic_error,
                     "You should have found the '" << model_name << "' model here !");
        field_wire_model = found->second;

        // get the associated "logical volume" :
        const geomtools::logical_volume & field_wire_log = field_wire_model->get_logical ();
        // check if there is a concrete shape (it should !):
        DT_THROW_IF (! field_wire_log.has_shape (), std::logic_error,"You should find a shape here !");

        // get the 3D shape :
        const geomtools::i_shape_3d & field_wire_shape = field_wire_log.get_shape ();

        // we know the concret shape class is a cylinder, so we cast it :
        const geomtools::cylinder & field_wire_cylinder
          = dynamic_cast<const geomtools::cylinder &> (field_wire_shape);

        _field_wire_length_   = field_wire_cylinder.get_z ();
        _field_wire_diameter_ = field_wire_cylinder.get_diameter ();
      }

      const geomtools::id_mgr::category_info & module_ci = categories.find ("drift_cell_core")->second;
      DT_THROW_IF (! module_ci.has_subaddress ("module"),
                   std::logic_error,
                   "Category 'drift_cell_core' has no subaddress 'module' !");
      _module_address_index_ = module_ci.get_subaddress_index ("module");

      DT_THROW_IF (! module_ci.has_subaddress ("side"),
                   std::logic_error,
                   "Category 'drift_cell_core' has no subaddress 'side' !");
      _side_address_index_ = module_ci.get_subaddress_index ("side");

      DT_THROW_IF (! module_ci.has_subaddress ("layer"),
                   std::logic_error,
                   "Category 'drift_cell_core' has no subaddress 'layer' !");
      _layer_address_index_ = module_ci.get_subaddress_index ("layer");

      DT_THROW_IF (! module_ci.has_subaddress ("row"),
                   std::logic_error,
                   "Category 'drift_cell_core' has no subaddress 'row' !");
      _row_address_index_ = module_ci.get_subaddress_index ("row");

      return;
    }

    void gg_locator::set_module_number (uint32_t module_number_)
    {
      _module_number_ = module_number_;
      return;
    }

    uint32_t gg_locator::get_module_number () const
    {
      return _module_number_;
    }

    void gg_locator::initialize ()
    {
      datatools::properties dummy;
      initialize (dummy);
      return;
    }

    void gg_locator::initialize (const datatools::properties & config_)
    {
      base_locator::_basic_initialize (config_);
      DT_THROW_IF (_module_number_ == geomtools::geom_id::INVALID_ADDRESS,
                   std::logic_error,
                   "Missing module number ! Use the 'set_module_number' method before !");

      // Check if the geometry manager is for the SuperNEMO setup :
      if (get_geo_manager ().get_setup_label () != "snemo")
        {
          // DT_THROW_IF (true,
          //              std::logic_error,
          //              "This locator is designed to use a SuperNEMO geometry setup labeled 'snemo', not '"
          //              << get_geo_manager ().get_setup_label () << "' !");
          DT_LOG_WARNING (get_logging_priority (),
                          "This locator is going to be used with a SuperNEMO geometry setup labeled '"
                          << get_geo_manager ().get_setup_label () << " which is NOT the expected official 'snemo' label ! ");
        }

      _construct ();
      _initialized_ = true;
      return;
    }

    void gg_locator::dump (std::ostream & out_) const
    {
      out_ << std::endl << "snemo::geometry:gg_locator::dump: " << std::endl;
      const std::string tag = "|-- ";
      const std::string ltag = "`-- ";
      if (! is_initialized ())
        {
          out_ << ltag << "Initialized               = " << _initialized_ << std::endl;
          return;
        }

      out_ << tag << "Initialized                = " << _initialized_ << std::endl;
      out_ << tag << "Logging priority threshold = "
            << datatools::logger::get_priority_label (get_logging_priority ()) << std::endl;
      out_ << tag << "Module number              = " << _module_number_ << std::endl;
      out_ << tag << "Manager @                  = " << & get_geo_manager () << std::endl;
      out_ << tag << "Mapping @                  = " << _mapping_ << std::endl;
      out_ << tag << "ID manager @               = " << _id_manager_ << std::endl;
      out_ << tag << "Module type                = " << _module_type_ << std::endl;
      out_ << tag << "Tracker volume type        = " << _tracker_volume_type_ << std::endl;
      out_ << tag << "Tracker layer type         = " << _tracker_layer_type_ << std::endl;
      out_ << tag << "Cell type                  = " << _cell_type_ << std::endl;
      out_ << tag << "Module placement    : " << std::endl;
      if (_module_world_placement_ != 0)
        {
          _module_world_placement_->tree_dump (out_, "", "|   ");
        }
      out_ << tag << "Module box : " << std::endl;
      if (_module_box_ != 0)
        {
          _module_box_->tree_dump (out_, "", "|   ");
        }
      out_ << tag << "Cell box : " << std::endl;
      if (_cell_box_ != 0)
        {
          _cell_box_->tree_dump (out_, "", "|   ");
        }
      out_ << tag << "Back layer X-pos [" << _back_cell_x_.size () << "] = ";
      for (size_t i = 0; i < _back_cell_x_.size (); i++)
        {
          out_ << _back_cell_x_[i] / CLHEP::mm << " ";
        }
      out_ << " (mm)" << std::endl;
      out_ << tag << "Back cell Y-pos [" << _back_cell_y_.size () << "] = ";
      for (unsigned int i = 0; i < _back_cell_y_.size (); i++)
        {
          if ((i < 4) || (i > _back_cell_y_.size () - 4))
            {
              out_ << _back_cell_y_[i] / CLHEP::mm << " ";
            }
          else if (i == 4)
            {
              out_ << " ... ";
            }
        }
      out_ << " (mm)" << std::endl;
      out_ << tag << "Front layer X-pos [" << _front_cell_x_.size () << "] =  ";
      for (unsigned int i = 0; i < _front_cell_x_.size (); i++)
        {
          out_ << _front_cell_x_[i] / CLHEP::mm << " ";
        }
      out_ << " (mm)" << std::endl;
      out_ << tag << "Front cell Y-pos [" << _front_cell_y_.size () << "] = ";
      for (unsigned int i = 0; i < _front_cell_y_.size (); i++)
        {
          if ((i < 4) || (i > _back_cell_y_.size () - 4))
            {
              out_ << _front_cell_y_[i] / CLHEP::mm << " ";
            }
          else if (i == 4)
            {
              out_ << " ... ";
            }
        }
      out_ << " (mm)" << std::endl;

      out_ << tag << "Anode wire length   = " << _anode_wire_length_ / CLHEP::mm  << " (mm)" << std::endl;
      out_ << tag << "Anode wire diameter = " << _anode_wire_diameter_ / CLHEP::mm  << " (mm)" << std::endl;
      out_ << tag << "Field wire length   = " << _field_wire_length_ / CLHEP::mm  << " (mm)" << std::endl;
      out_ << tag << "Field wire diameter = " << _field_wire_diameter_ / CLHEP::mm  << " (mm)" << std::endl;
      out_ << tag << "Module address GID index = " << _module_address_index_ << std::endl;
      out_ << tag << "Side address GID index   = " << _side_address_index_ << std::endl;
      out_ << tag << "Layer address GID index  = " << _layer_address_index_ << std::endl;
      out_ << tag << "Row address GID index    = " << _row_address_index_ << std::endl;

      out_ << ltag << "End" << std::endl;
      return;
    }

    void gg_locator::reset ()
    {
      DT_THROW_IF (! is_initialized (), std::logic_error, "Locator is not initialized !");

      _set_defaults ();

      _back_cell_x_.clear ();
      _front_cell_x_.clear ();
      _back_cell_y_.clear ();
      _front_cell_y_.clear ();

      _initialized_ = false;
      return;
    }

    void gg_locator::transform_world_to_module (const geomtools::vector_3d & world_position_,
                                                geomtools::vector_3d & module_position_) const
    {
      DT_THROW_IF (! is_initialized (), std::logic_error, "Locator is not initialized !");
      _module_world_placement_->mother_to_child (world_position_, module_position_);
      return;
    }

    void gg_locator::transform_module_to_world (const geomtools::vector_3d & module_position_,
                                                geomtools::vector_3d & world_position_) const
    {
      DT_THROW_IF (! is_initialized (), std::logic_error, "Locator is not initialized !");
      _module_world_placement_->child_to_mother (module_position_, world_position_);
      return;
    }

    bool gg_locator::is_in_module (const geomtools::vector_3d & module_position_,
                                   double tolerance_) const
    {
      DT_THROW_IF (! is_initialized (), std::logic_error, "Locator is not initialized !");
      return _module_box_->is_inside (module_position_, tolerance_);
    }

    bool gg_locator::is_in_cell (const geomtools::vector_3d & module_position_,
                                 uint32_t side_,
                                 uint32_t layer_,
                                 uint32_t row_,
                                 double tolerance_) const
    {
      DT_THROW_IF (! is_initialized (), std::logic_error, "Locator is not initialized !");
      geomtools::vector_3d to_cell_pos = module_position_;
      to_cell_pos -= get_cell_position (side_, layer_, row_);
      // here one misses one transformation step (rotation) but it is ok :
      return _cell_box_->is_inside (to_cell_pos, tolerance_);
    }

    bool gg_locator::is_world_position_in_cell (const geomtools::vector_3d & world_position_,
                                                uint32_t side_,
                                                uint32_t layer_,
                                                uint32_t row_,
                                                double tolerance_) const
    {
      DT_THROW_IF (! is_initialized (), std::logic_error, "Locator is not initialized !");
      geomtools::vector_3d in_module_position;
      this->transform_world_to_module (world_position_, in_module_position);
      return is_in_cell (in_module_position, side_, layer_, row_, tolerance_);
    }

    bool gg_locator::is_world_position_in_module (const geomtools::vector_3d & world_position_,
                                                  double tolerance_) const
    {
      DT_THROW_IF (! is_initialized (), std::logic_error, "Locator is not initialized !");
      geomtools::vector_3d in_module_position;
      this->transform_world_to_module (world_position_, in_module_position);
      return is_in_module (in_module_position, tolerance_);
    }

    bool gg_locator::find_geom_id (const geomtools::vector_3d & world_position_,
                                   int type_,
                                   geomtools::geom_id & gid_,
                                   double tolerance_) const
    {
      DT_THROW_IF (! is_initialized (), std::logic_error, "Locator is not initialized !");
      DT_THROW_IF (type_ != (int)_cell_type_,
                   std::logic_error,
                   "Only works with type " << _cell_type_ << " for now !");
      gid_.invalidate ();

      // Compute the position in the module coordinate system :
      geomtools::vector_3d in_module_position;
      this->transform_world_to_module (world_position_, in_module_position);

      // First check if it is inside the module :
      if (! _module_box_->is_inside (in_module_position, tolerance_))
        {
          // Not in this module :
          return false;
        }
      return const_cast<gg_locator *> (this)->gg_locator::_find_cell_geom_id (in_module_position, gid_);
    }

    bool gg_locator::find_cell_geom_id (const geomtools::vector_3d & world_position_,
                                        geomtools::geom_id & gid_,
                                        double tolerance_) const
    {
      return find_geom_id (world_position_, _cell_type_, gid_, tolerance_);
    }

    bool gg_locator::_find_cell_geom_id (const geomtools::vector_3d & in_module_position_,
                                         geomtools::geom_id & gid_)
    {
      //_logging_priority = datatools::logger::PRIO_TRACE;
      DT_LOG_TRACE (get_logging_priority (), "Entering...");

      geomtools::geom_id & gid  = gid_;
      gid.invalidate ();
      //_id_manager_->make_id (_drift_cell_volume_category_, gid);
      gid.set_type (_cell_type_);
      uint32_t side_number  (geomtools::geom_id::INVALID_ADDRESS);
      uint32_t layer_number (geomtools::geom_id::INVALID_ADDRESS);
      uint32_t cell_number  (geomtools::geom_id::INVALID_ADDRESS);
      gid.set (_module_index_, geomtools::geom_id::INVALID_ADDRESS);
      gid.set (_side_index_,   geomtools::geom_id::INVALID_ADDRESS);
      gid.set (_layer_index_,  geomtools::geom_id::INVALID_ADDRESS);
      gid.set (_row_index_,    geomtools::geom_id::INVALID_ADDRESS);
      const double z = in_module_position_.z ();
      // 2012-06-05 FM: add tolerance for z-testing
      if (std::abs (z) < (_cell_box_->get_half_z () + 0.5 * _tolerance_))
        {
          gid.set (_module_index_, _module_number_);
          const double y = in_module_position_.y ();
          const double x = in_module_position_.x ();
          double first_cell_x;
          double cell_delta_x;
          double first_cell_y;
          double cell_delta_y;
          size_t nlayers;
          size_t nrows;
          if (x < 0.0)
            {
              side_number = 0;
              nlayers = _back_cell_x_.size ();
              nrows = _back_cell_y_.size ();
              first_cell_x = _back_cell_x_.front ();
              cell_delta_x = (_back_cell_x_.back () - _back_cell_x_.front ()) / (_back_cell_x_.size () - 1);
              first_cell_y = _back_cell_y_.front ();
              cell_delta_y = (_back_cell_y_.back () - _back_cell_y_.front ()) / (_back_cell_y_.size () - 1);
            }
          else
            {
              side_number = 1;
              nlayers = _front_cell_x_.size ();
              nrows = _front_cell_y_.size ();
              first_cell_x = _front_cell_x_.front ();
              cell_delta_x = (_front_cell_x_.back () - _front_cell_x_.front ()) / (_front_cell_x_.size () - 1);
              first_cell_y = _front_cell_y_.front ();
              cell_delta_y = (_front_cell_y_.back () - _front_cell_y_.front ()) / (_front_cell_y_.size () - 1);
            }
          gid.set (_side_index_, side_number);
          const int ix = (int) (((x - first_cell_x) / cell_delta_x) + 0.5);
          if ((ix >= 0) && (ix < (int)nlayers))
            {
              layer_number = ix;
            }
          gid.set (_layer_index_, layer_number);
          const int iy = (int) (((y - first_cell_y) / cell_delta_y) + 0.5);
          if ((iy >= 0) && (iy < (int)nrows))
            {
              cell_number = iy;
            }
          gid.set (_row_index_, cell_number);
          if (gid.is_valid ())
            {
              // 2012-05-31 FM : use ginfo from mapping (see below)
              const geomtools::geom_info * ginfo_ptr = _mapping_->get_geom_info_ptr (gid);
              if (ginfo_ptr == 0)
                {
                  DT_LOG_TRACE (get_logging_priority (), "Unmapped gid = " << gid);
                  gid.invalidate ();
                  return false;
                }
              // 2012-05-31 FM : we check if the 'world' position is in the volume:
              geomtools::vector_3d world_position;
              transform_module_to_world (in_module_position_, world_position);
              if (_mapping_->check_inside (*ginfo_ptr, world_position, _tolerance_))
                {
                  DT_LOG_TRACE (get_logging_priority (), "Position inside gid = " << gid);
                  return true;
                }
            }
          gid.invalidate ();
        }
      // 2012-06-05 FM: add missing invalidate call
      gid.invalidate ();
      return false;
    }

  }  // end of namespace geometry

}  // end of namespace snemo

// end of gg_locator.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
