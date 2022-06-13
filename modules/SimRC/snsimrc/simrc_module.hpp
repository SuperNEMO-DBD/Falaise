//! \file    simrc_module.hpp
//! \brief   SimRC module
//! \details Process a things object
#ifndef FALAISE_SIMRC_PLUGIN_SIMRC_MODULE_HPP
#define FALAISE_SIMRC_PLUGIN_SIMRC_MODULE_HPP

// Standard library
#include <string>
#include <memory>

// Third party:
// - Bayeux/dpp :
#include <dpp/base_module.h>

#include "event_timestamper.hpp"
#include "tracker_cell_tagger.hpp"
#include "calorimeter_om_tagger.hpp"

namespace geomtools {
  class manager;
}

namespace mctools {
  class simulated_data;
}

namespace snemo {

  namespace simulation {

    /// \brief The data processing module for applying real running conditions to SD data
    class simrc_module
      : public dpp::base_module
    {
    public:
      
      /// Constructor
      simrc_module(datatools::logger::priority = datatools::logger::PRIO_FATAL);

      /// Destructor
      virtual ~simrc_module();

      /// Setting geometry manager
      void set_geometry_manager(const geomtools::manager& gmgr_);

      /// Getting geometry manager
      const geomtools::manager& get_geometry_manager() const;

      /// Initialization
      virtual void initialize(const datatools::properties & config_,
                              datatools::service_manager & services_,
                              dpp::module_handle_dict_type & /* unused */) override;

      /// Reset
      virtual void reset() override;

      /// Data record processing
      virtual process_status process(datatools::things & event_) override;

    protected:
      
      /// Method  to process and generate trajectory data
      void _process_impl(snemo::datamodel::event_header & eh_,
                         mctools::simulated_data & sd_);

      /// Give default values to specific class members.
      void _set_defaults();

    private:
      
      const geomtools::manager * _geoManager_ = nullptr; ///< The geometry manager
      std::string _EHTag_ = "EH"; ///< The label of the input EH bank (event header)
      std::string _SDTag_ = "SD"; ///< The label of the input SD bank (simulated data)
      
      std::vector<std::string> _caloTypes_{}; ///< Calorimeter hit categories
      std::string _ggType_;
      bool _event_timestamping_ = true;
      bool _tracker_cell_tag_ = true;
      bool _calorimeter_om_tag_ = true;
      std::unique_ptr<snemo::simulation::event_timestamper> _event_timestamper_;
      std::unique_ptr<snemo::simulation::tracker_cell_tagger> _tracker_cell_tagger_;
      std::unique_ptr<snemo::simulation::calorimeter_om_tagger> _calorimeter_om_tagger_;
 
      // Macro to automate the registration of the module :
      DPP_MODULE_REGISTRATION_INTERFACE(simrc_module)
      
    };
    
  } // end of namespace simulation

} // end of namespace snemo

#include <datatools/ocd_macros.h>

// @arg snemo::simulation::simrc_module the name the registered class
DOCD_CLASS_DECLARATION(snemo::simulation::simrc_module)

#endif // FALAISE_SIMRC_PLUGIN_SIMRC_MODULE_HPP
