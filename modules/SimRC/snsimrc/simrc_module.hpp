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
      void _process_impl(mctools::simulated_data & simdata_);

      /// Give default values to specific class members.
      void _set_defaults();

    private:
      
      const geomtools::manager * _geoManager_ = nullptr; ///< The geometry manager
      std::string _SDTag_ = "SD"; ///< The label of the input SD bank
      std::vector<std::string> _caloTypes_{}; ///< Calorimeter hit categories

      // /// Handle to the embedded dead-OM tagger
      // std::unique_ptr<snemo::processing::dead_om_tagger> _deadOmTagger_;
      // /// Handle to the embedded dead-cell tagger
      // std::unique_ptr<snemo::processing::dead_cell_tagger> _deadCellTagger_;

      // Macro to automate the registration of the module :
      DPP_MODULE_REGISTRATION_INTERFACE(simrc_module)
      
    };
    
  }  // end of namespace simulation

}  // end of namespace snemo

#include <datatools/ocd_macros.h>

// @arg snemo::simulation::simrc_module the name the registered class
DOCD_CLASS_DECLARATION(snemo::simulation::simrc_module)

#endif  // FALAISE_SIMRC_PLUGIN_SIMRC_MODULE_HPP
