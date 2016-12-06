/// \file snemo/electronics/electronics_service.h
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
 *   SuperNEMO electronics service.
 *
 */

#ifndef FALAISE_SNEMO_ELECTRONICS_ELECTRONICS_SERVICE_H
#define FALAISE_SNEMO_ELECTRONICS_ELECTRONICS_SERVICE_H

// Standard library:
#include <iostream>
#include <string>

// Third party :
// - Bayeux/datatools:
#include <datatools/base_service.h>
#include <datatools/service_tools.h>

namespace datatools {
  class properties;
}

namespace geomtools {
  class manager;
}

namespace snemo {

  namespace electronics {

    class manager;

    /// \brief Electronics service
    class electronics_service
      : public datatools::base_service
    {
    public:

      /// Default constructor
      electronics_service();

      /// Destructor
      virtual ~electronics_service();

      /// Return the name of the 'geometry' service
      const std::string & get_geo_label() const;

      /// Set the name of the 'geometry' service
      void set_geo_label(const std::string & );

      /// Check if the non mutable geometry manager is set
      bool has_geometry_manager() const;

      /// Set a reference to the non mutable geometry manager
      void set_geometry_manager(const  geomtools::manager &);

      /// Return a reference to the non mutable geometry manager
      const geomtools::manager & get_geometry_manager () const;

      /// Check if the non mutable electronics manager is set
      bool has_electronics_manager() const;

      /// Return a reference to the non mutable geoemtry manager
      const manager & get_electronics_manager () const;

      /// Check initialization flag
      virtual bool is_initialized() const;

      /// Initialization
      virtual int initialize(const datatools::properties & config_,
                             datatools::service_dict_type & service_dict_);

      /// Termination
      virtual int reset();

      /// Smart print
      virtual void tree_dump(std::ostream & out_         = std::clog,
                             const std::string & title_  = "",
                             const std::string & indent_ = "",
                             bool inherit_               = false) const;

    private:

      // Management:
      bool                       _initialized_ = false;           //!< Initialization flag

      // Configuration:
      std::string _Geo_label_; //!< Name of the geometry service

      // Working data:
      const geomtools::manager * _geometry_manager_ = nullptr;    //!< Handle to the geometry manager
      const manager *            _electronics_manager_ = nullptr; //!< Handle to the electronics manager

      // Registration :
      DATATOOLS_SERVICE_REGISTRATION_INTERFACE(electronics_service);

    };

  }  // end of namespace electronics

}  // end of namespace snemo

#include <datatools/ocd_macros.h>

// Declare the OCD interface of the module
DOCD_CLASS_DECLARATION(snemo::electronics::electronics_service)

#endif // FALAISE_SNEMO_ELECTRONICS_ELECTRONICS_SERVICE_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
