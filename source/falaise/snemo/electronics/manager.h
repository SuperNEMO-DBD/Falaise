/// \file snemo/electronics/manager.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2016-12-06
 * Last modified : 2016-12-06
 *
 * Copyright (C) 2016 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *   SuperNEMO electronics manager.
 *
 */

#ifndef FALAISE_SNEMO_ELECTRONICS_MANAGER_H
#define FALAISE_SNEMO_ELECTRONICS_MANAGER_H

// Standard library:
#include <iostream>
#include <string>

// Third party :
// - Bayeux/datatools:
#include <datatools/i_tree_dump.h>

// This project :
#include <snemo/electronics/cabling.h>

namespace datatools {
  class properties;
}

namespace geomtools {
  class manager;
}

namespace snemo {

  namespace electronics {

    /// \brief Electronics manager
    class manager
      : public datatools::i_tree_dumpable
    {
    public:

      /// Default constructor
      manager();

      /// Destructor
      virtual ~manager();

      /// Check if the non mutable geometry manager is set
      bool has_geometry_manager() const;

      /// Set a reference to the non mutable geometry manager
      void set_geometry_manager(const geomtools::manager &);

      /// Return a reference to the non mutable geometry manager
      const geomtools::manager & get_geometry_manager() const;

      /// Check if cabling is setup
      bool has_cabling() const;

      /// Check initialization flag
      virtual bool is_initialized() const;

      /// Initialization
      virtual void initialize(const datatools::properties & config_);

      /// Termination
      virtual void reset();

      /// Smart print
      virtual void tree_dump(std::ostream & out_         = std::clog,
                             const std::string & title_  = "",
                             const std::string & indent_ = "",
                             bool inherit_               = false) const;

    private:

      void _init_cabling_(const datatools::properties & cabling_config_);

      void _reset_cabling_();

    private:

      // Management:
      bool _initialized_ = false; //!< Initialization flag

      // Configuration:
      const geomtools::manager * _geometry_manager_ = nullptr;
      bool _no_cabling_ = false; //!< Inhibit cabling service

      // Working data:
      cabling _cabling_; //!< Cabling (geometry <-> electronics ID converter)

    };

  }  // end of namespace electronics

}  // end of namespace snemo

#include <datatools/ocd_macros.h>

// Declare the OCD interface of the module
DOCD_CLASS_DECLARATION(snemo::electronics::manager)

#endif // FALAISE_SNEMO_ELECTRONICS_MANAGER_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
