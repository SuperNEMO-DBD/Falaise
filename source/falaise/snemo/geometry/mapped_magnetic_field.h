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
#include <memory>
#include <string>

// Third party:
// - Bayeux/emfield:
#include <emfield/base_electromagnetic_field.h>

namespace snemo {

namespace geometry {

/// \brief Class representing a contant mapped  magnetic field
class mapped_magnetic_field : public ::emfield::base_electromagnetic_field {
 public:
  /// \brief Mapping mode
  enum class map_mode_t {
    INVALID = -1,         ///< Invalid mapping mode
    IMPORT_CSV_MAP_0 = 0  ///< Build from imported CSV file
  };

  /// Default constructor
  mapped_magnetic_field(uint32_t flags = 0);

  /// Destructor
  virtual ~mapped_magnetic_field();

  /// Initialization
  virtual void initialize(const datatools::properties &, datatools::service_manager &,
                          emfield::base_electromagnetic_field::field_dict_type &) override;

  /// Reset
  virtual void reset() override;

  /// Compute electric field
  virtual int compute_electric_field(const geomtools::vector_3d &position, double time,
                                     geomtools::vector_3d &efield) const override;

  /// Compute magnetic field
  virtual int compute_magnetic_field(const geomtools::vector_3d &position, double time,
                                     geomtools::vector_3d &magnetic_field) const override;

  /// Smart print
  virtual void tree_dump(std::ostream &out = std::clog, const std::string &title = "",
                         const std::string &indent = "", bool inherit = false) const override;

  /// Set the map source filename
  void setMapFilename(const std::string &);

  /// Set the mapping mode
  void setMapMode(map_mode_t mm);

  /// Set the flag to force a zero B-field outside the mapped domain
  void setZeroFieldOutsideMap(bool);

  /// Set the Z component inversion flag
  void setInvertedZ(bool);

 protected:
  /// Set default attributes values
  void _set_defaults();

 private:
  map_mode_t mapMode_;        //!< Mapping mode
  std::string mapFile_;       //!< Map filename
  bool zeroFieldOutsideMap_;  //!< Force zero field outside the interpolated map
  bool invertFieldAlongZ_;    //!< Invert the Z component of the field

  struct MapImpl;
  std::unique_ptr<MapImpl> fieldMap_;  //!< PIMPL-ized working data

  // Macro to automate the registration of the EM field :
  EMFIELD_REGISTRATION_INTERFACE(mapped_magnetic_field)
};

}  // end of namespace geometry

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_GEOMETRY_MAPPED_MAGNETIC_FIELD_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
