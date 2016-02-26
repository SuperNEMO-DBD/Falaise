/// \file falaise/snemo/digitization/soft_trigger_algo.h
/* Author(s)     : Guillaume Oliviero <goliviero@lpccaen.in2p3.fr>
 *                 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *                 Yves Lemiere <lemiere@lpccaen.in2p3.fr>
 * Creation date : 2016-02-26
 * Last modified : 2014-02-26
 *
 * Copyright (C) 2012-2016 Guillaume Oliviero <goliviero@lpccaen.in2p3.fr>
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
 * An algorithm class that implements a soft trigger to cut SD files.  
 * The physical selection for a soft trigger is configurable but the 
 * default configuration is 1 PMT hit and 3 Geiger cells hits.
 *
 * History:
 *
 */


#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_SOFT_TRIGGER_ALGO_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_SOFT_TRIGGER_ALGO_H

// Standard library :
#include <string>
#include <bitset>

namespace datatools {
  class properties;
}

namespace snemo { 
  
  namespace digitization {

    /// \brief Soft trigger algorithm class to cut SD files.
    class soft_trigger_algo
    {        
      /// Getting initialization flag
      bool is_initialized() const;
      
      /// Setting logging priority
      void set_logging_priority(const datatools::logger::priority priority_);

      /// Getting logging priority
      datatools::logger::priority get_logging_priority() const;

      /// Check the geometry manager
      bool has_geometry_manager() const;

      /// Address the geometry manager
      void set_geometry_manager(const geomtools::manager & gmgr_);
      
      /// Initialize the driver through configuration properties
      void initialize(const datatools::properties & setup_);
      
      /// Reset
      void reset();
      
       /// Data record processing
      void process(datatools::things & data_record_);
      
   protected:

      /// Soft trigger algo process
      void _process(datatools::things & data_record_);

    private :
     
      // Configuration :
      bool _initialized_;                               //!< Initialization flag
      datatools::logger::priority _logging_priority_;  //!< Logging priority
      const geomtools::manager * _geometry_manager_;   //!< The SuperNEMO geometry manager

    };

  } // end of namespace digitization

} // end of namespace snemo






    
#endif // FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_TRIGGER_ALGORITHM_H

/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
