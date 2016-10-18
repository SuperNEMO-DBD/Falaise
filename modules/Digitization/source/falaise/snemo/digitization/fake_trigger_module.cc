/// \file falaise/snemo/digitization/fake_trigger_module.cc
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves:
#include <snemo/digitization/fake_trigger_module.h>

// Standard library:

// Third party:
// - Bayeux/datatools:
#include <datatools/service_manager.h>

// This project (Falaise):
//#include <falaise/snemo/processing/services.h>
#include <falaise/snemo/datamodels/data_model.h>

// This plugin (fake_trigger_module) :
#include <falaise/snemo/digitization/fake_trigger_algo.h>

namespace snemo { 
  
  namespace digitization {

    // Registration instantiation macro
    DPP_MODULE_REGISTRATION_IMPLEMENT(fake_trigger_module,
                                      "snemo::digitization::fake_trigger_module")
    
    fake_trigger_module::fake_trigger_module(datatools::logger::priority logging_priority_)
      : dpp::base_module(logging_priority_)
    {
      _set_defaults();
      return;
    }

    fake_trigger_module::~fake_trigger_module()
    {
      if (is_initialized()) fake_trigger_module::reset();
      return;
    }    

    void fake_trigger_module::_set_defaults()
    {
      _SD_label_ = snemo::datamodel::data_info::default_simulated_data_label();
      return;
    }
    
    void fake_trigger_module::initialize(const datatools::properties  & setup_,
					 datatools::service_manager   & /* service_manager_*/,
					 dpp::module_handle_dict_type & /* module_dict_ */)
					 
    {
      DT_THROW_IF(is_initialized(),
                  std::logic_error,
                  "Module '" << get_name() << "' is already initialized ! ");
      
      dpp::base_module::_common_initialize(setup_);
      if (setup_.has_key("SD_label")) {
        _SD_label_ = setup_.fetch_string("SD_label");
      }
      // Algo :
      _algo_.reset(new snemo::digitization::fake_trigger_algo);
      _algo_->initialize();
      // Tag the module as initialized :
      _set_initialized(true);
    }    

    void fake_trigger_module::reset()
    {
      DT_THROW_IF(! is_initialized(),
                  std::logic_error,
                  "Module '" << get_name() << "' is not initialized !");

      _set_initialized(false);
      if (_algo_.get() != 0) {
	if (_algo_->is_initialized()) {
	  _algo_->reset();
	}
	_algo_.reset();
      }
      _set_defaults();
      return;
    }

    dpp::base_module::process_status fake_trigger_module::process(datatools::things & data_record_)
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
      const mctools::simulated_data & the_simulated_data = data_record_.get<mctools::simulated_data>(_SD_label_);
      
      /********************
       * Process the data *
       ********************/

      // Main processing method :
      return this->_process(the_simulated_data);
    }
    
    dpp::base_module::process_status fake_trigger_module::_process(const mctools::simulated_data & SD_)
    {
      DT_LOG_TRACE(get_logging_priority(), "Entering...");
      bool result_for_a_SD = _algo_->process(SD_);
      if (!result_for_a_SD) return dpp::base_module::PROCESS_STOP;
      else return dpp::base_module::PROCESS_SUCCESS;
    }

  } // end of namespace digitization

} // end of namespace snemo

/* OCD support */
#include <datatools/object_configuration_description.h>
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::digitization::fake_trigger_module, ocd_)
{
  ocd_.set_class_name("snemo::digitization::fake_trigger_module");
  ocd_.set_class_description("A module that performs a selection on simulated data based on the number of PMT and GG cells hit");
  ocd_.set_class_library("Falaise_FakeTrigger");
  ocd_.set_class_documentation(" \n"
			       " \n"
                               );

  // Invoke OCD support from parent class :
  dpp::base_module::common_ocd(ocd_);

  {
    // Description of the 'SD_label' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("SD_label")
      .set_terse_description("The label/name of the 'simulated data' bank")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("This is the name of the bank to be used  \n"
                            "after a simulation. \n")
      .set_default_value_string(snemo::datamodel::data_info::default_simulated_data_label())
      .add_example("Use an alternative name for the \n"
                   "'simulated data' bank::        \n"
                   "                                \n"
                   "  SD_label : string = \"SD2\"   \n"
                   "                                \n"
                   )
      ;
  }

  ocd_.set_validation_support(true);
  ocd_.lock();

  return;
}

DOCD_CLASS_IMPLEMENT_LOAD_END() // Closing macro for implementation
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::digitization::fake_trigger_module,
			       "snemo::digitization::fake_trigger_module")

// end of falaise/snemo/digitization/fake_trigger_module.cc
