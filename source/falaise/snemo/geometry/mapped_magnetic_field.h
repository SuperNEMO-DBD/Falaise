/// \file falaise/snemo/geometry/mapped_magnetic_field.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2016-03-11
 * Last modified : 2016-03-11
 *
 * Copyright (C) 2016 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *                    Steve Snow <S.W.Snow@warwick.ac.uk>
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
 *   Mapped magnetic field for the SuperNEMO demonstrator module
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_GEOMETRY_MAPPED_MAGNETIC_FIELD_H
#define FALAISE_SNEMO_GEOMETRY_MAPPED_MAGNETIC_FIELD_H

// Standard library:
#include <string>

// Third party:
// - Bayeux/emfield:
#include <emfield/base_electromagnetic_field.h>

namespace snemo {

  namespace geometry {

    /// \brief Class representing a contant mapped  magnetic field
    class mapped_magnetic_field : public ::emfield::base_electromagnetic_field
    {
    public:

      /// \brief Mapping mode
      enum mapping_mode_type {
        MM_INVALID          = -1, ///< Invalid mapping mode
        MM_IMPORT_CSV_MAP_0 =  0  ///< Build from imported CSV file
      };

      /// Default constructor
      mapped_magnetic_field(uint32_t = 0);

      /// Destructor
      virtual ~mapped_magnetic_field();

      /// Initialization
      virtual void initialize(const ::datatools::properties &,
                              ::datatools::service_manager &,
                              ::emfield::base_electromagnetic_field::field_dict_type &);

      /// Reset
      virtual void reset();

      /// Compute electric field
      virtual int compute_electric_field(const ::geomtools::vector_3d & position_,
                                         double time_,
                                         ::geomtools::vector_3d & electric_field_) const;

      /// Compute magnetic field
      virtual int compute_magnetic_field(const ::geomtools::vector_3d & position_,
                                         double time_,
                                         geomtools::vector_3d & magnetic_field_) const;

      /// Smart print
      virtual void tree_dump (std::ostream & out_         = std::clog,
                              const std::string & title_  = "",
                              const std::string & indent_ = "",
                              bool inherit_               = false) const;

      /// Set the map source filename
      void set_map_filename(const std::string &);

      /// Set the mapping mode
      void set_mapping_mode(mapping_mode_type);

    protected:

      /// Set default attributes values
      void _set_defaults();

    private:

      mapping_mode_type _mapping_mode_;           //!< Mapping mode
      std::string       _map_filename_;           //!< Map filename
      bool              _zero_field_outside_map_; //!< Force zero field outside the interpolated map

      struct _work_type;
      boost::scoped_ptr<_work_type> _work_; //!< PIMPL-ized working data

      // Macro to automate the registration of the EM field :
      EMFIELD_REGISTRATION_INTERFACE(mapped_magnetic_field)

    };

  }  // end of namespace geometry

}  // end of namespace snemo

#endif // FALAISE_SNEMO_GEOMETRY_MAPPED_MAGNETIC_FIELD_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
