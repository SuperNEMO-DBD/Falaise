// -*- mode: c++ ; -*-
/* event_header_utils_module.cc
 */

// Ourselves:
#include <falaise/snemo/processing/event_header_utils_module.h>

// Third party:
// - Bayeux/mctools:
#include <mctools/simulated_data.h>
#include <mctools/utils.h>

// This project:
#include <falaise/snemo/datamodels/data_model.h>
#include <falaise/snemo/datamodels/event_header.h>

namespace snemo {

  namespace processing {

    // Registration instantiation macro :
    DPP_MODULE_REGISTRATION_IMPLEMENT(event_header_utils_module,
                                      "snemo::processing::event_header_utils_module");


    // Initialization :
    void event_header_utils_module::initialize(const datatools::properties  & setup_,
                                               datatools::service_manager   & /*service_manager_*/,
                                               dpp::module_handle_dict_type & /*module_dict_*/)
    {
      DT_THROW_IF(is_initialized(),
                  std::logic_error,
                  "Module '" << get_name() << "' is already initialized ! ");

      dpp::base_module::_common_initialize(setup_);

      if (setup_.has_key("mode")) {
        const std::string mode_label = setup_.fetch_string("mode");
        if (mode_label == "add_header") {
          _mode_ = MODE_ADD_HEADER;
        }
        DT_THROW_IF(_mode_ == MODE_INVALID,
                    std::domain_error,
                    "Module '" << get_name() << "' has no mode '" << mode_label << "' !");
      }

      if (_mode_ == MODE_ADD_HEADER) {
        if (setup_.has_flag("mode.add_header.update")) {
          _add_header_update_ = true;
        }

        if (setup_.has_key("mode.add_header.bank_label")) {
          _add_header_bank_label_ = setup_.fetch_string("mode.add_header.bank_label");
        }

        if (setup_.has_key("mode.add_header.run_number")) {
          _add_header_run_number_ = setup_.fetch_integer("mode.add_header.run_number");
          DT_THROW_IF(_add_header_run_number_ < 0, std::logic_error,
                      "Invalid run number '" << _add_header_run_number_
                      << "' for module '" << get_name() << "' using mode '" << _mode_ << "' !");
        }

        if (setup_.has_key("mode.add_header.event_number")) {
          _add_header_event_number_ = setup_.fetch_integer("mode.add_header.event_number");
          DT_THROW_IF(_add_header_event_number_ < 0, std::logic_error,
                      "Invalid first event number '" << _add_header_event_number_
                      << "' for module '" << get_name() << "' using mode '" << _mode_ << "' !");
        }

        if (setup_.has_flag("mode.add_header.use_genbb_weight")) {
          _add_header_use_genbb_weight_ = true;
        }

        if (setup_.has_flag("mode.add_header.use_genbb_label")) {
          _add_header_use_genbb_label_ = true;
        }

        if (setup_.has_key("mode.add_header.external_properties_path")) {
          _add_header_properties_path_
            = setup_.fetch_string("mode.add_header.external_properties_path");
          datatools::fetch_path_with_env(_add_header_properties_path_);
        }

        if (setup_.has_key("mode.add_header.external_properties_prefix")) {
          _add_header_properties_prefix_
            = setup_.fetch_string("mode.add_header.external_properties_prefix");
        }
      }

      _set_initialized(true);
      return;
    }

    void event_header_utils_module::reset()
    {
      DT_THROW_IF(! is_initialized(),
                  std::logic_error,
                  "Module '" << get_name() << "' is not initialized !");
      _set_initialized(false);
      _set_defaults();
      return;
    }

    void event_header_utils_module::_set_defaults()
    {
      _mode_ = MODE_INVALID;

      _add_header_bank_label_ = snemo::datamodel::data_info::default_event_header_label();

      _add_header_update_       = false;
      _add_header_run_number_   = 0;
      _add_header_event_number_ = 0;

      _add_header_use_genbb_label_   = false;
      _add_header_use_genbb_weight_  = false;
      _add_header_properties_path_   = "";
      _add_header_properties_prefix_ = "";

      return;
    }

    // Constructor :
    event_header_utils_module::event_header_utils_module(datatools::logger::priority logging_priority_)
      : dpp::base_module(logging_priority_)
    {
      _set_defaults();
      return;
    }

    // Destructor
    event_header_utils_module::~event_header_utils_module()
    {
      if (is_initialized()) event_header_utils_module::reset();
      return;
    }

    // Processing :
    dpp::base_module::process_status event_header_utils_module::process(datatools::things & data_record_)
    {
      DT_THROW_IF(! is_initialized(),
                  std::logic_error,
                  "Module '" << get_name() << "' is not initialized !");

      // Main processing method :
      if (_mode_ == MODE_ADD_HEADER) {
        _process_add_header(data_record_);
      }

      return dpp::base_module::PROCESS_SUCCESS;
    }

    void event_header_utils_module::_process_add_header(datatools::things & data_record_)
    {
      DT_THROW_IF(_add_header_bank_label_.empty(),
                  std::logic_error,
                  "Missing bank label to be enriched !");
      snemo::datamodel::event_header * ptr_event_header = 0;
      if (data_record_.has(_add_header_bank_label_)) {
        DT_THROW_IF(! _add_header_update_,
                    std::logic_error,
                    "Event record already has a header '" << _add_header_bank_label_ << "' !");
        ptr_event_header = &(data_record_.grab<snemo::datamodel::event_header>(_add_header_bank_label_));
      } else {
        ptr_event_header = &(data_record_.add<snemo::datamodel::event_header>(_add_header_bank_label_));
      }

      snemo::datamodel::event_header & the_event_header = *ptr_event_header;
      // the_event_header.clear();
      the_event_header.grab_id().set_run_number(_add_header_run_number_);
      the_event_header.grab_id().set_event_number(_add_header_event_number_++);

      // default timestamp :
      the_event_header.grab_timestamp().set_seconds(0);
      the_event_header.grab_timestamp().set_picoseconds(0);

      // Retrieve properties from external files
      if (!_add_header_properties_path_.empty()) {
        datatools::properties & the_properties = the_event_header.grab_properties();
        datatools::properties::read_config(_add_header_properties_path_,
                                           the_properties);
        if (!_add_header_properties_prefix_.empty()) {
          the_properties.erase_all_not_starting_with(_add_header_properties_prefix_);
        }
      }

      // Store GENBB event weight
      if (_add_header_use_genbb_weight_ || _add_header_use_genbb_label_) {
        const std::string sd_label = snemo::datamodel::data_info::default_simulated_data_label();
        DT_THROW_IF(! data_record_.has(sd_label), std::logic_error,
                    "Event record has no '"<< sd_label << "' bank!");
        const mctools::simulated_data & the_simulated_data
          = data_record_.get<mctools::simulated_data>(sd_label);

        if (_add_header_use_genbb_weight_) {
          const double weight = the_simulated_data.get_primary_event().get_genbb_weight();
          the_event_header.grab_properties().update(mctools::event_utils::EVENT_GENBB_WEIGHT, weight);
        }

        if (_add_header_use_genbb_label_) {
          const std::string label = the_simulated_data.get_primary_event().get_label();
          the_event_header.grab_properties().update("event.genbb_label", label);
        }
      }

      return;
    }

  } // end of namespace processing

} // end of namespace snemo

// end of event_header_utils_module.cc
