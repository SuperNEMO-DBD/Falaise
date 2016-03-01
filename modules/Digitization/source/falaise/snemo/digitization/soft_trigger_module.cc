/// \file falaise/snemo/digitization/soft_trigger_module.cc
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves:
#include <snemo/digitization/soft_trigger_module.h>

// Standard library:

// Third party:
// - Bayeux/datatools:
#include <datatools/service_manager.h>

// This project (Falaise):
//#include <falaise/snemo/processing/services.h>
#include <falaise/snemo/datamodels/data_model.h>

// This plugin (soft_trigger_module) :
#include <falaise/snemo/digitization/soft_trigger_algo.h>

namespace snemo { 
  
  namespace digitization {

    // Registration instantiation macro
    DPP_MODULE_REGISTRATION_IMPLEMENT(soft_trigger_module,
                                      "snemo::digitization::soft_trigger_module");
    
    soft_trigger_module::soft_trigger_module(datatools::logger::priority logging_priority_)
      : dpp::base_module(logging_priority_)
    {
      _set_defaults();
      return;
    }

    soft_trigger_module::~soft_trigger_module()
    {
      if (is_initialized()) soft_trigger_module::reset();
      return;
    }    

    void soft_trigger_module::_set_defaults()
    {
      _SD_label_ =  snemo::datamodel::data_info::default_simulated_data_label();

      _STA_.reset();
      return;
    }
    
    void soft_trigger_module::initialize(const datatools::properties  & setup_,
					 datatools::service_manager   & service_manager_,
					 dpp::module_handle_dict_type & module_dict_)
    {
      DT_THROW_IF(is_initialized(),
                  std::logic_error,
                  "Module '" << get_name() << "' is already initialized ! ");
      
      dpp::base_module::_common_initialize(setup_);
      if (setup_.has_key("SD_label")) {
        _SD_label_ = setup_.fetch_string("SD_label");
      }
      // Algo :

      // Tag the module as initialized :
      _set_initialized(true);
    }    

    void soft_trigger_module::reset()
    {
      DT_THROW_IF(! is_initialized(),
                  std::logic_error,
                  "Module '" << get_name() << "' is not initialized !");

      _set_initialized(false);
      _set_defaults();
      return;
    }

    dpp::base_module::process_status soft_trigger_module::process(datatools::things & data_record_)
    {
      DT_THROW_IF(! is_initialized(), std::logic_error,
		  "Module '" << get_name() << "' is not initialized !");

      /*************************
       * Check simulated data *
       *************************/
      const bool abort_at_missing_input = true;
      // Check if some 'simulated_data' are available in the data model:
      if (!data_record_.has(_SD_label_)) 
	{
	  DT_THROW_IF(abort_at_missing_input, std::logic_error, "Missing simulated data to be processed !");
	  // leave the data unchanged.
	  return dpp::base_module::PROCESS_ERROR;
	}
      // Get the 'simulated_data' entry from the data model :
      const  mctools::simulated_data & the_simulated_data = data_record_.get<mctools::simulated_data>(_SD_label_);
      
      /********************
       * Process the data *
       ********************/

      // Main processing method :
      this->_process(the_simulated_data);

      return dpp::base_module::PROCESS_SUCCESS;
    }
    
    dpp::base_module::process_status soft_trigger_module::_process(const mctools::simulated_data & SD_)
    {
      DT_LOG_TRACE(get_logging_priority(), "Entering...");
      bool result_for_a_SD = _STA_->process(SD_);
      if (!result_for_a_SD) return dpp::base_module::PROCESS_ERROR;
      else return dpp::base_module::PROCESS_SUCCESS;
    }

  } // end of namespace digitization

} // end of namespace snemo

/* OCD support */
#include <datatools/object_configuration_description.h>
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::digitization::soft_trigger_module, ocd_)
{
  return;
}

DOCD_CLASS_IMPLEMENT_LOAD_END() // Closing macro for implementation
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::digitization::soft_trigger_module,
			       "snemo::digitization::soft_trigger_module");

// end of falaise/snemo/digitization/soft_trigger_module.cc
