//! \file    falaise/detail/falaise_sys.h
//! \brief   Provide Falaise library system singleton
//! \details
//
// Copyright (c) 2017 by Francois Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2017 by Université de Caen Normandie
//
// This file is part of Falaise.
//
// Falaise is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Falaise is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Falaise.  If not, see <http://www.gnu.org/licenses/>.

#ifndef FALAISE_DETAIL_FALAISE_SYS_H
#define FALAISE_DETAIL_FALAISE_SYS_H

// Standard Library
// #include <string>

// Third party:
#include <boost/core/noncopyable.hpp>

// This project:
#include <datatools/logger.h>
#include <datatools/service_manager.h>

namespace falaise {

  namespace detail {

    //! \brief Falaise system singleton
    class falaise_sys
     : private boost::noncopyable
    {
    public:

      /// Return the name of the Falaise library URN database service for supported setups (geometry, simulation...)
      static const std::string & fl_setup_db_name();

      /// Return the name of the Falaise library URN to resource path resolver service
      static const std::string & fl_resource_resolver_name();

      /// Extract the verbosity from the FALAISE_SYS_LOGGING environment variable (if any)
      static datatools::logger::priority process_logging_env();

      /// Default constructor
      falaise_sys();

      /// Destructor
      virtual ~falaise_sys();

      /// Return the logging priority
      datatools::logger::priority get_logging() const;

      /// Check initialization flag
      bool is_initialized() const;

      /// Initialize
      void initialize();

      /// Shutdown
      void shutdown();

      /// Return a mutable reference to the embedded service manager
      datatools::service_manager & grab_services();

      /// Return a non mutable reference to the embedded service manager
      const datatools::service_manager & get_services() const;

      /// Check if the Falaise system singleton is instantiated
      static bool is_instantiated();

      /// Return a mutable reference to the Falaise system singleton instance
      static falaise_sys & instance();

      /// Return a non-mutable reference to the Falaise system singleton instance
      static const falaise_sys & const_instance();

      /// Instantiate the Falaise system singleton
      static falaise_sys & instantiate();

    private:

      void _libinfo_registration_();

      void _libinfo_deregistration_();

      void _initialize_urn_services_();

      void _shutdown_urn_services_();

    private:

      // Management:
      bool _initialized_ = false;            //!< Initialization flag
      datatools::logger::priority _logging_; //!< Logging priority threshold

      // Working internal data:
      datatools::service_manager _services_; //!< Embedded services

      // Singleton:
      static falaise_sys * _instance_;       //!< Falaise system singleton handle

    };

  } // end of namespace detail

} // namespace falaise

#endif // FALAISE_DETAIL_FALAISE_SYS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
