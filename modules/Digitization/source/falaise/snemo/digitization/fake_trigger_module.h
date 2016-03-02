/// \file falaise/snemo/digitization/fake_trigger_module.h
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
 * A module which goal is to process 'fake_trigger_algo' to cut SD files
 * from a datatools::things coming from flsimulate
 * The physical selection for a fake trigger is configurable but the 
 * default configuration is 1 PMT hit and 3 Geiger cells hits.
 *
 * History:
 *
 */

// Third party :
// - Bayeux/dpp:
#include <dpp/base_module.h>
// - Bayeux/mctools:
#include <mctools/simulated_data.h>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_FAKE_TRIGGER_MODULE_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_FAKE_TRIGGER_MODULE_H

namespace snemo { 
  
  namespace digitization {

    class fake_trigger_algo;
    
    /// \brief Fake trigger algorithm class to cut SD files.
    class fake_trigger_module :  public dpp::base_module
    {
    public :

      /// Constructor
      fake_trigger_module(datatools::logger::priority = datatools::logger::PRIO_FATAL);

      /// Destructor
      virtual ~fake_trigger_module();

      /// Initialization
      virtual void initialize(const datatools::properties  & setup_,
                              datatools::service_manager   & service_manager_,
                              dpp::module_handle_dict_type & module_dict_);
      /// Reset
      virtual void reset();

      /// Data record processing
      virtual process_status process(datatools::things & data_);

    protected : 
      
      /// Measure particle track physical parameters such as charge, vertices
      process_status _process(const mctools::simulated_data & SD_);
      
      /// Give default values to specific class members.
      void _set_defaults();
      
    private :
      
      std::string _SD_label_; //!< The label of the simulated data bank

      /// Fake Trigger Algo :
      boost::scoped_ptr<snemo::digitization::fake_trigger_algo> _STA_;

      // Macro to automate the registration of the module :
      DPP_MODULE_REGISTRATION_INTERFACE(fake_trigger_module);
    };
  } // end of namespace digitization

} // end of namespace snemo

#include <datatools/ocd_macros.h>

// Declare the OCD interface of the module
DOCD_CLASS_DECLARATION(snemo::digitization::fake_trigger_module)

#endif // FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_FAKE_TRIGGER_MODULE_H

/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
