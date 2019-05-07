// \file falaise/snemo/visualization/visu_toy_module.cc

// Ourselves:
#include <snemo/visualization/visu_toy_module.h>

// Standard library:
#include <unistd.h>
#include <sstream>
#include <stdexcept>

// Third party:
// - Bayeux/datatools:
#include <datatools/service_manager.h>
// - Bayeux/geomtools:
#include <geomtools/geometry_service.h>
#include <geomtools/manager.h>

// This project (Falaise):
#include <falaise/snemo/datamodels/data_model.h>
#include <falaise/snemo/services/services.h>
#include <falaise/snemo/visualization/toy_display_driver.h>

namespace snemo {

namespace visualization {

// Registration instantiation macro :
DPP_MODULE_REGISTRATION_IMPLEMENT(visu_toy_module, "snemo::visualization::visu_toy_module")

const geomtools::manager& visu_toy_module::get_geometry_manager() const {
  return *_geometry_manager_;
}

void visu_toy_module::set_geometry_manager(const geomtools::manager& gmgr_) {
  DT_THROW_IF(is_initialized(), std::logic_error,
              "Module '" << get_name() << "' is already initialized ! ");
  _geometry_manager_ = &gmgr_;

  // Check setup label:
  const std::string& setup_label = _geometry_manager_->get_setup_label();
  DT_THROW_IF(setup_label != "snemo::demonstrator" && setup_label != "snemo::tracker_commissioning",
              std::logic_error, "Setup label '" << setup_label << "' is not supported !");
  return;
}

// Constructor :
visu_toy_module::visu_toy_module(datatools::logger::priority logging_priority_)
    : dpp::base_module(logging_priority_) {
  _set_defaults();
  return;
}

// Destructor :
visu_toy_module::~visu_toy_module() {
  if (is_initialized()) {
    visu_toy_module::reset();
  }
  return;
}

void visu_toy_module::_set_defaults() {
  _geometry_manager_ = 0;
  _driver_.reset(0);
  _module_number_ = 0;
  _interactive_ = true;
  _event_counter_ = 0;
  _min_event_count_ = 0;
  _max_event_count_ = -1;
  _usleep_safe_ = 500000;

  return;
}

void visu_toy_module::reset() {
  DT_THROW_IF(!is_initialized(), std::logic_error,
              "Module '" << get_name() << "' is not initialized !");
  _set_initialized(false);
  // Reset the visualization processor :
  if (_driver_) {
    if (_driver_->is_initialized()) {
      _driver_->reset();
    }
    _driver_.reset();
  }
  _set_defaults();
  return;
}

void visu_toy_module::initialize(const datatools::properties& setup_,
                                 datatools::service_manager& service_manager_,
                                 dpp::module_handle_dict_type& /* module_dict_ */) {
  DT_THROW_IF(is_initialized(), std::logic_error,
              "Module '" << get_name() << "' is already initialized ! ");

  dpp::base_module::_common_initialize(setup_);

  if (setup_.has_key("interactive")) {
    _interactive_ = setup_.fetch_boolean("interactive");
  }

  if (setup_.has_key("min_event_count")) {
    int n = setup_.fetch_integer("min_event_count");
    DT_THROW_IF(
        n < 0, std::range_error,
        "Module '" << get_name() << "' : Invalid minimum event counter (" << n << " < 0) !");
    _min_event_count_ = n;
  }

  if (setup_.has_key("max_event_count")) {
    int n = setup_.fetch_integer("max_event_count");
    DT_THROW_IF(n >= 0 && n < _min_event_count_, std::range_error,
                "Module '" << get_name() << "' : Invalid maximum event counter (" << n << " < "
                           << _min_event_count_ << ") !");
    if (n < 0) {
      _max_event_count_ = -1;
    } else {
      _max_event_count_ = n;
    }
  }

  if (setup_.has_key("usleep_safe")) {
    int n = setup_.fetch_integer("usleep_safe");
    DT_THROW_IF(
        n < 1, std::range_error,
        "Module '" << get_name() << "' : Invalid time safe delay (" << n << " < " << 1 << " us) !");
    _usleep_safe_ = n;
  }

  // Geometry manager :
  if (_geometry_manager_ == 0) {
    std::string geo_label = snemo::service_info::default_geometry_service_label();
    if (setup_.has_key("Geo_label")) {
      geo_label = setup_.fetch_string("Geo_label");
    }
    DT_THROW_IF(geo_label.empty(), std::logic_error,
                "Module '" << get_name() << "' has no valid '"
                           << "Geo_label"
                           << "' property !");
    DT_THROW_IF(!service_manager_.has(geo_label) ||
                    !service_manager_.is_a<geomtools::geometry_service>(geo_label),
                std::logic_error,
                "Module '" << get_name() << "' has no '" << geo_label << "' service !");
    const geomtools::geometry_service& Geo =
        service_manager_.get<geomtools::geometry_service>(geo_label);
    set_geometry_manager(Geo.get_geom_manager());
  }

  // Visualization processor :
  _driver_.reset(new toy_display_driver);
  DT_THROW_IF(!_driver_, std::logic_error,
              "Module '" << get_name() << "' could not instantiate the visualization processor !");

  // Set the SuperNEMO module number:
  if (setup_.has_key("snemo.module_number")) {
    int n = setup_.fetch_integer("snemo.module_number");
    DT_THROW_IF(
        n != 0, std::range_error,
        "Module '" << get_name() << "' does not support SuperNEMO module number '" << n << "' ! ");
  }

  // Plug the geometry manager :
  // {
  //   // Const-ness trick:
  //   geomtools::manager & geo = const_cast<geomtools::manager &>(get_geometry_manager());
  //   _driver_.get()->set_geom_manager(geo);
  // }
  _driver_.get()->set_geom_manager(get_geometry_manager());
  _driver_.get()->set_module_number(_module_number_);
  _driver_.get()->set_interactive(_interactive_);

  // Initialize the visualization processor :
  _driver_.get()->initialize();

  _set_initialized(true);
  return;
}

// Processing :
dpp::base_module::process_status visu_toy_module::process(datatools::things& data_record_) {
  DT_THROW_IF(!is_initialized(), std::logic_error,
              "Module '" << get_name() << "' is not initialized !");

  // Main processing method :
  return _process(data_record_);
}

dpp::base_module::process_status visu_toy_module::_process(datatools::things& data_record_) {
  DT_LOG_TRACE(get_logging_priority(), "Entering...");
  int event_counter = _event_counter_;
  _event_counter_++;

  if (event_counter >= _min_event_count_) {
    if (_max_event_count_ < 0 || event_counter <= _max_event_count_) {
      // Process the visualization driver :
      std::ostringstream evid_ss;
      evid_ss << "Event " << event_counter;
      _driver_.get()->set_current_event_id(evid_ss.str());
      toy_display_driver::display_code_type code = _driver_.get()->display(data_record_);
      if (code == toy_display_driver::DISPLAY_INHIBITED) {
        return dpp::base_module::PROCESS_STOP;
      }
      if (!_interactive_) {
        usleep(_usleep_safe_);
      }
    }
  }
  DT_LOG_TRACE(get_logging_priority(), "Exiting.");
  return dpp::base_module::PROCESS_SUCCESS;
}

}  // end of namespace visualization

}  // end of namespace snemo

/* OCD support */
#include <datatools/object_configuration_description.h>
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::visualization::visu_toy_module, ocd_) {
  ocd_.set_class_name("snemo::visualization::visu_toy_module");
  ocd_.set_class_description("A module that display the event records");
  ocd_.set_class_library("Falaise_VisuToy");
  ocd_.set_class_documentation("Gnuplot based 3D visualization\n");

  dpp::base_module::common_ocd(ocd_);

  // {
  //   // Description of the 'CD_label' configuration property :
  //   datatools::configuration_property_description & cpd
  //     = ocd_.add_property_info();
  //   cpd.set_name_pattern("CD_label")
  //     .set_terse_description("The label/name of the 'calibrated data' bank")
  //     .set_traits(datatools::TYPE_STRING)
  //     .set_mandatory(false)
  //     .set_long_description("This is the name of the bank to be used \n"
  //                           "as the source of input tracker hits.    \n"
  //                           )
  //     .set_default_value_string(snemo::datamodel::data_info::default_calibrated_data_label())
  //     .add_example("Use an alternative name for the \n"
  //                  "'calibrated data' bank::        \n"
  //                  "                                \n"
  //                  "  CD_label : string = \"CD2\"   \n"
  //                  "                                \n"
  //                  )
  //     ;
  // }

  // {
  //   // Description of the 'TCD_label' configuration property :
  //   datatools::configuration_property_description & cpd
  //     = ocd_.add_property_info();
  //   cpd.set_name_pattern("TCD_label")
  //     .set_terse_description("The label/name of the 'tracker clustering data' bank")
  //     .set_traits(datatools::TYPE_STRING)
  //     .set_mandatory(false)
  //     .set_long_description("This is the name of the bank to be used as   \n"
  //                           "the sink of output clusters of tracker hits. \n"
  //                           )
  //     .set_default_value_string(snemo::datamodel::data_info::default_tracker_clustering_data_label())
  //     .add_example("Use an alternative name for the   \n"
  //                  "'tracker clustering data' bank::  \n"
  //                  "                                  \n"
  //                  "  TCD_label : string = \"TCD2\"   \n"
  //                  "                                  \n"
  //                  )
  //     ;
  // }

  // {
  //   // Description of the 'Geo_label' configuration property :
  //   datatools::configuration_property_description & cpd
  //     = ocd_.add_property_info();
  //   cpd.set_name_pattern("Geo_label")
  //     .set_terse_description("The label/name of the geometry service")
  //     .set_traits(datatools::TYPE_STRING)
  //     .set_mandatory(false)
  //     .set_long_description("This is the name of the service to be used as the \n"
  //                           "geometry service.                                 \n"
  //                           "This property is only used if no geometry manager \n"
  //                           "as been provided to the module.                   \n"
  //                           )
  //     .set_default_value_string(snemo::processing::service_info::default_geometry_service_label())
  //     .add_example("Use an alternative name for the geometry service:: \n"
  //                  "                                                   \n"
  //                  "  Geo_label : string = \"geometry\"                \n"
  //                  "                                                   \n"
  //                  )
  //     ;
  // }

  // // Additionnal configuration hints :
  // ocd_.set_configuration_hints("Here is a full configuration example in the \n"
  //                              "``datatools::properties`` ASCII format:: \n" "
  //                              \n" "  CD_label  : string = \"CD\"
  //                              \n" "  TCD_label : string = \"TCD\"
  //                              \n" "  Geo_label : string = \"geometry\"
  //                              \n" "
  //                              \n" "Additional specific parameters are used to configure the
  //                              embedded    \n"
  //                              "``mock_tracker_clustering`` driver itself; see the OCD support of
  //                              \n" "the ``snemo::reconstruction::mock_tracker_clustering_driver``
  //                              class. \n"
  //                              );

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()  // Closing macro for implementation
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::visualization::visu_toy_module,
                               "snemo::visualization::visu_toy_module")
