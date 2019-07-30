// -*- mode: c++ ; -*-
/* event_header_utils_module.cc
 */

// Ourselves:
#include <falaise/snemo/processing/event_header_utils_module.h>

// Third party:
// // - Bayeux/datatools:
// #include <datatools/event_id.h>
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
                                  "snemo::processing::event_header_utils_module")

// Initialization :
void event_header_utils_module::initialize(const datatools::properties& setup_,
                                           datatools::service_manager& /*service_manager_*/,
                                           dpp::module_handle_dict_type& /*module_dict_*/) {
  DT_THROW_IF(is_initialized(), std::logic_error,
              "Module '" << get_name() << "' is already initialized ! ");

  dpp::base_module::_common_initialize(setup_);

  if (setup_.has_key("mode")) {
    const std::string mode_label = setup_.fetch_string("mode");
    if (mode_label == "add_header") {
      _mode_ = MODE_ADD_HEADER;
    }
    DT_THROW_IF(_mode_ == MODE_INVALID, std::domain_error,
                "Module '" << get_name() << "' has no mode '" << mode_label << "' !");
  }

  if (_mode_ == MODE_ADD_HEADER) {
    if (setup_.has_flag("add_header.generation")) {
      const std::string& genlabel = setup_.fetch_string("add_header.generation");
      if (genlabel == "real") {
        _add_header_gentype_ = snemo::datamodel::event_header::GENERATION_REAL;
      } else if (genlabel == "simulated") {
        _add_header_gentype_ = snemo::datamodel::event_header::GENERATION_SIMULATED;
      } else {
        DT_THROW(std::logic_error, "Invalid generation type '" << genlabel << "'!");
      }
    }

    if (setup_.has_flag("add_header.update")) {
      _add_header_update_ = true;
    }

    if (setup_.has_key("add_header.bank_label")) {
      _add_header_bank_label_ = setup_.fetch_string("add_header.bank_label");
    }

    if (setup_.has_key("add_header.run_number")) {
      _add_header_run_number_ = setup_.fetch_integer("add_header.run_number");
      DT_THROW_IF(_add_header_run_number_ < 0, std::logic_error,
                  "Invalid current run number '" << _add_header_run_number_ << "' for module '"
                                                 << get_name() << "' using mode '" << _mode_
                                                 << "' !");
    }

    if (setup_.has_key("add_header.event_number")) {
      _add_header_event_number_ = setup_.fetch_integer("add_header.event_number");
      DT_THROW_IF(_add_header_event_number_ < 0, std::logic_error,
                  "Invalid first event number '" << _add_header_event_number_ << "' for module '"
                                                 << get_name() << "' using mode '" << _mode_
                                                 << "' !");
    }

    if (ah_is_simulated()) {
      if (setup_.has_flag("add_header.use_genbb_weight")) {
        _add_header_use_genbb_weight_ = true;
      }

      if (setup_.has_flag("add_header.use_genbb_label")) {
        _add_header_use_genbb_label_ = true;
      }
    }

    if (setup_.has_key("add_header.external_properties_path")) {
      _add_header_properties_path_ = setup_.fetch_string("add_header.external_properties_path");
      datatools::fetch_path_with_env(_add_header_properties_path_);
    }

    if (setup_.has_key("add_header.external_properties_prefix")) {
      _add_header_properties_prefix_ = setup_.fetch_string("add_header.external_properties_prefix");
    }
  }

  // Post-initialization:
  if (is_add_header_mode()) {
    _ah_current_run_number_ = _add_header_run_number_;
    _ah_current_event_number_ = _add_header_event_number_;
  }

  _set_initialized(true);
}

void event_header_utils_module::reset() {
  DT_THROW_IF(!is_initialized(), std::logic_error,
              "Module '" << get_name() << "' is not initialized !");
  _set_initialized(false);
  _set_defaults();
}

void event_header_utils_module::_set_defaults() {
  _mode_ = MODE_INVALID;

  _add_header_bank_label_ = snemo::datamodel::data_info::default_event_header_label();
  _add_header_gentype_ = snemo::datamodel::event_header::GENERATION_INVALID;
  _add_header_update_ = false;
  _add_header_run_number_ = 0;
  _add_header_event_number_ = 0;

  _add_header_use_genbb_label_ = false;
  _add_header_use_genbb_weight_ = false;
  _add_header_properties_path_ = "";
  _add_header_properties_prefix_ = "";

  _ah_current_run_number_ = _add_header_run_number_;
  _ah_current_event_number_ = _add_header_event_number_;
}

// Constructor :
event_header_utils_module::event_header_utils_module(datatools::logger::priority logging_priority_)
    : dpp::base_module(logging_priority_) {
  _set_defaults();
}

// Destructor
event_header_utils_module::~event_header_utils_module() {
  if (is_initialized()) {
    event_header_utils_module::reset();
  }
}

bool event_header_utils_module::is_add_header_mode() const { return _mode_ == MODE_ADD_HEADER; }

void event_header_utils_module::ah_increment_run_number(int incr_) {
  DT_THROW_IF(!is_add_header_mode(), std::logic_error, "Invalid mode!");
  _ah_current_run_number_ += incr_;
}

void event_header_utils_module::ah_increment_event_number(int incr_) {
  DT_THROW_IF(!is_add_header_mode(), std::logic_error, "Invalid mode!");
  _ah_current_event_number_ += incr_;
}

bool event_header_utils_module::ah_is_real() const {
  return _add_header_gentype_ == snemo::datamodel::event_header::GENERATION_REAL;
}

bool event_header_utils_module::ah_is_simulated() const {
  return _add_header_gentype_ == snemo::datamodel::event_header::GENERATION_SIMULATED;
}

// Processing :
dpp::base_module::process_status event_header_utils_module::process(
    datatools::things& data_record_) {
  DT_THROW_IF(!is_initialized(), std::logic_error,
              "Module '" << get_name() << "' is not initialized !");

  // Main processing method :
  if (is_add_header_mode()) {
    _process_add_header(data_record_);
  }

  return dpp::base_module::PROCESS_SUCCESS;
}

void event_header_utils_module::_process_add_header(datatools::things& data_record_) {
  DT_THROW_IF(_add_header_bank_label_.empty(), std::logic_error,
              "Missing bank label to be enriched !");
  snemo::datamodel::event_header* ptr_event_header = nullptr;
  if (data_record_.has(_add_header_bank_label_)) {
    DT_THROW_IF(!_add_header_update_, std::logic_error,
                "Event record already has a header '" << _add_header_bank_label_ << "' !");
    ptr_event_header =
        &(data_record_.grab<snemo::datamodel::event_header>(_add_header_bank_label_));
  } else {
    ptr_event_header = &(data_record_.add<snemo::datamodel::event_header>(_add_header_bank_label_));
  }

  snemo::datamodel::event_header& the_event_header = *ptr_event_header;
  // the_event_header.clear();
  the_event_header.get_id().set_run_number(_ah_current_run_number_);
  the_event_header.get_id().set_event_number(_ah_current_event_number_);
  ah_increment_event_number(1);

  the_event_header.set_generation(_add_header_gentype_);

  // default timestamp :
  the_event_header.get_timestamp().set_seconds(0);
  the_event_header.get_timestamp().set_picoseconds(0);

  // Retrieve properties from external files
  if (!_add_header_properties_path_.empty()) {
    datatools::properties& the_properties = the_event_header.get_properties();
    datatools::properties::read_config(_add_header_properties_path_, the_properties);
    if (!_add_header_properties_prefix_.empty()) {
      the_properties.erase_all_not_starting_with(_add_header_properties_prefix_);
    }
  }

  // Store GENBB event weight
  if (ah_is_simulated()) {
    if (_add_header_use_genbb_weight_ || _add_header_use_genbb_label_) {
      const std::string sd_label = snemo::datamodel::data_info::default_simulated_data_label();
      DT_THROW_IF(!data_record_.has(sd_label), std::logic_error,
                  "Event record has no '" << sd_label << "' bank!");
      const auto& the_simulated_data = data_record_.get<mctools::simulated_data>(sd_label);

      if (_add_header_use_genbb_weight_) {
        const double weight = the_simulated_data.get_primary_event().get_genbb_weight();
        the_event_header.get_properties().update(mctools::event_utils::EVENT_GENBB_WEIGHT, weight);
      }

      if (_add_header_use_genbb_label_) {
        const std::string label = the_simulated_data.get_primary_event().get_label();
        the_event_header.get_properties().update(mctools::event_utils::EVENT_GENBB_LABEL, label);
      }
    }
  }
}

}  // end of namespace processing

}  // end of namespace snemo

/********************************
 * OCD support : implementation *
 ********************************/

#include <datatools/object_configuration_description.h>

/** Opening macro for implementation
 *  @arg snemo::processing::event_header_utils_module the full class name
 *  @arg ocd_ is the identifier of the 'datatools::object_configuration_description'
 *            to be initialized (passed by mutable reference).
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::processing::event_header_utils_module, ocd_) {
  ocd_.set_class_name("snemo::processing::event_header_utils_module");
  ocd_.set_class_description("A module that add information into the event header data bank");
  ocd_.set_class_library("falaise");
  // ocd_.set_class_documentation("");

  dpp::base_module::common_ocd(ocd_);

  {
    // Description of the 'mode'' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("mode")
        .set_terse_description("The processing mode")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(true)
        .set_long_description("This is the name of the processing mode to be used")
        .add_example(
            "Use the 'add_header' mode:: \n"
            "                                \n"
            "  mode : string = \"add_header\"\n"
            "                                \n");
  }

  {
    // Description of the 'add_header.bank_label' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("add_header.bank_label")
        .set_terse_description("The label/name of the 'event header' bank")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_long_description(
            "This is the name of the bank to be used \n"
            "to store the event header informations. \n")
        .set_default_value_string(snemo::datamodel::data_info::default_event_header_label())
        .add_example(
            "Use an alternative name for the 'event_ header' bank:: \n"
            "                                                       \n"
            "  add_header.bank_label : string = \"EH2\"             \n"
            "                                                       \n");
  }

  {
    // Description of the 'add_header.update' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("add_header.update")
        .set_terse_description("Flag to activate the update of an existing event header bank")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_mandatory(false)
        .set_default_value_boolean(false)
        .add_example(
            "Use the default value::               \n"
            "                                      \n"
            "  add_header.update : boolean = false \n"
            "                                      \n");
  }

  {
    // Description of the 'add_header.bank_label' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("add_header.generation")
        .set_terse_description("The event generation type")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_long_description(R"(This is the type of generation ("real" or "simulated"))")
        .set_default_value_string(snemo::datamodel::data_info::default_event_header_label())
        .add_example(
            "Indicate simulated event:: \n"
            "                                                       \n"
            "  add_header.generation : string = \"simulated\"       \n"
            "                                                       \n");
  }

  {
    // Description of the 'add_header.run_number' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("add_header.run_number")
        .set_terse_description("The run number to assign")
        .set_traits(datatools::TYPE_INTEGER)
        .set_mandatory(false)
        .set_default_value_integer(0)
        .add_example(
            "Use an alternative run number::         \n"
            "                                        \n"
            "  add_header.run_number : integer = 666 \n"
            "                                        \n");
  }

  {
    // Description of the 'add_header.event_number' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("add_header.event_number")
        .set_terse_description("The event number to assign")
        .set_traits(datatools::TYPE_INTEGER)
        .set_mandatory(false)
        .set_default_value_integer(0)
        .add_example(
            "Use an alternative event number::         \n"
            "                                          \n"
            "  add_header.event_number : integer = 666 \n"
            "                                          \n");
  }

  {
    // Description of the 'add_header.use_genbb_weight' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("add_header.use_genbb_weight")
        .set_terse_description("Flag to activate the saving of GENBB weight")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_mandatory(false)
        .set_long_description(
            "GENBB can provide a weight for each simulated event    \n"
            "when, for instance, double beta decay spectrum is only \n"
            "simulated above 2 MeV.")
        .set_default_value_boolean(false)
        .add_example(
            "Use the default value::                     \n"
            "                                            \n"
            "  add_header.use_genbb_weight : boolean = 0 \n"
            "                                            \n");
  }

  {
    // Description of the 'add_header.use_genbb_label' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("add_header.use_genbb_label")
        .set_terse_description("Flag to activate the saving of GENBB event generator label")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_mandatory(false)
        .set_default_value_boolean(false)
        .add_example(
            "Use the default value::                     \n"
            "                                            \n"
            "  add_header.use_genbb_label : boolean = 0  \n"
            "                                            \n");
  }

  {
    // Description of the 'add_header.external_properties_path' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("add_header.external_properties_path")
        .set_terse_description("Path to an external properties file")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_path(true)
        .set_long_description(
            "The external properties path allows to store miscellaneous \n"
            "informations extracted from a formated properties file     \n")
        .add_example(
            "Set a path to Use the default value::                                                 "
            "\n"
            "                                                                                      "
            "\n"
            "  add_header.external_properties_path : string as path = \"config/event_header.conf\" "
            "\n"
            "                                                                                      "
            "\n");
  }

  {
    // Description of the 'add_header.external_properties_prefix' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("add_header.external_properties_prefix")
        .set_terse_description("The prefix of properties keys to be stored")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_long_description("Only properties with the given prefix will be stored")
        .add_example(
            "Use a specific prefix::                                         \n"
            "                                                                \n"
            "  add_header.external_properties_prefix : string = \"analysis\" \n"
            "                                                                \n");
  }

  // Additionnal configuration hints :
  ocd_.set_configuration_hints(
      "Here is a full configuration example in the                                            \n"
      "``datatools::properties`` ASCII format::                                               \n"
      "                                                                                       \n"
      "  mode : string = \"add_header\"                                                       \n"
      "  add_header.generation : string = \"simulated\"                                       \n"
      "  add_header.bank_label : string = \"EH\"                                              \n"
      "  add_header.run_number : integer = 666                                                \n"
      "  add_header.event_number : integer = 0                                                \n"
      "                                                                                       \n"
      "  add_header.use_genbb_weight : boolean = true                                         \n"
      "  add_header.use_genbb_label  : boolean = true                                         \n"
      "                                                                                       \n"
      "  add_header.external_properties_path  : string as path = \"config/event_header.conf\" \n"
      "  add_header.external_properties_prefix  : string = \"analysis\"                       \n"
      "                                                                                       \n");

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()  // Closing macro for implementation

// Registration macro for class 'snemo::processing::event_header_utils_module' :
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::processing::event_header_utils_module,
                               "snemo::processing::event_header_utils_module")

// end of event_header_utils_module.cc
