/// \file falaise/snemo/reconstruction/gamma_clustering_module.cc

// Ourselves:
#include <snemo/reconstruction/gamma_clustering_module.h>

// Standard library:
#include <sstream>
#include <stdexcept>

// Third party:
// - Bayeux/datatools:
#include <datatools/service_manager.h>
// - Bayeux/geomtools:
#include <geomtools/geometry_service.h>
#include <geomtools/manager.h>

//- GSL:
#include <gsl/gsl_cdf.h>

// This project:
#include <falaise/snemo/datamodels/data_model.h>
#include <falaise/snemo/datamodels/particle_track_data.h>
#include <falaise/snemo/geometry/locator_plugin.h>
#include <falaise/snemo/services/services.h>

// Gamma Clustering
#include <snemo/reconstruction/gamma_clustering_driver.h>

namespace snemo {

namespace reconstruction {

// Registration instantiation macro :
DPP_MODULE_REGISTRATION_IMPLEMENT(gamma_clustering_module,
                                  "snemo::reconstruction::gamma_clustering_module")

const geomtools::manager& gamma_clustering_module::get_geometry_manager() const {
  return *_geometry_manager_;
}

void gamma_clustering_module::set_geometry_manager(const geomtools::manager& gmgr_) {
  DT_THROW_IF(is_initialized(), std::logic_error,
              "Module '" << get_name() << "' is already initialized ! ");
  _geometry_manager_ = &gmgr_;

  // Check setup label:
  const std::string& setup_label = _geometry_manager_->get_setup_label();
  DT_THROW_IF(setup_label != "snemo::demonstrator" && setup_label != "snemo::tracker_commissioning",
              std::logic_error, "Setup label '" << setup_label << "' is not supported !");
  return;
}

void gamma_clustering_module::_set_defaults() {
  _geometry_manager_ = 0;
  _PTD_label_ = snemo::datamodel::data_info::default_particle_track_data_label();
  _driver_.reset();
  return;
}

// Initialization :
void gamma_clustering_module::initialize(const datatools::properties& setup_,
                                         datatools::service_manager& service_manager_,
                                         dpp::module_handle_dict_type& /* module_dict_ */) {
  DT_THROW_IF(is_initialized(), std::logic_error,
              "Module '" << get_name() << "' is already initialized ! ");

  dpp::base_module::_common_initialize(setup_);

  if (setup_.has_key("PTD_label")) {
    _PTD_label_ = setup_.fetch_string("PTD_label");
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

  // Gamma clustering algorithm :
  std::string algorithm_id;
  if (setup_.has_key("driver")) {
    algorithm_id = setup_.fetch_string("driver");
  } else {
    // Provide default driver name
    algorithm_id = snemo::reconstruction::gamma_clustering_driver::get_id();
  }
  if (algorithm_id == snemo::reconstruction::gamma_clustering_driver::get_id()) {
    _driver_.reset(new snemo::reconstruction::gamma_clustering_driver);
  } else {
    DT_THROW_IF(true, std::logic_error,
                "Unsupported '" << algorithm_id << "'gamma clustering algorithm ");
  }

  // Plug the geometry manager :
  _driver_.get()->set_geometry_manager(get_geometry_manager());

  // Initialize the clustering driver :
  _driver_.get()->initialize(setup_);

  _set_initialized(true);
  return;
}

void gamma_clustering_module::reset() {
  DT_THROW_IF(!is_initialized(), std::logic_error,
              "Module '" << get_name() << "' is not initialized !");
  _set_initialized(false);
  _set_defaults();
  return;
}

// Constructor :
gamma_clustering_module::gamma_clustering_module(datatools::logger::priority logging_priority_)
    : dpp::base_module(logging_priority_) {
  _set_defaults();
  return;
}

// Destructor :
gamma_clustering_module::~gamma_clustering_module() {
  if (is_initialized()) gamma_clustering_module::reset();
  return;
}

// Processing :
dpp::base_module::process_status gamma_clustering_module::process(datatools::things& data_record_) {
  DT_THROW_IF(!is_initialized(), std::logic_error,
              "Module '" << get_name() << "' is not initialized !");

  // Main processing method :
  _process(data_record_);

  return dpp::base_module::PROCESS_SUCCESS;
}

void gamma_clustering_module::_process(datatools::things& data_record_) {
  DT_LOG_TRACE(get_logging_priority(), "Entering...");

  snemo::datamodel::particle_track_data* ptr_particle_track_data = 0;
  if (!data_record_.has(_PTD_label_)) {
    ptr_particle_track_data =
        &(data_record_.add<snemo::datamodel::particle_track_data>(_PTD_label_));
  } else {
    ptr_particle_track_data =
        &(data_record_.grab<snemo::datamodel::particle_track_data>(_PTD_label_));
  }
  snemo::datamodel::particle_track_data& ptd = *ptr_particle_track_data;

  // process the fitter driver :
  _driver_.get()->process(ptd.get_non_associated_calorimeters(), ptd);

  DT_LOG_TRACE(get_logging_priority(), "Exiting.");
  return;
}

}  // end of namespace reconstruction

}  // end of namespace snemo

/* OCD support */
#include <datatools/object_configuration_description.h>
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::reconstruction::gamma_clustering_module, ocd_) {
  ocd_.set_class_name("snemo::reconstruction::gamma_clustering_module");
  ocd_.set_class_description(
      "A module that performs the gamma clustering using the Gamma_Clustering algorithm");
  ocd_.set_class_library("Falaise_GammaClustering");
  ocd_.set_class_documentation(
      "This module uses the Gamma Clustering algorithms for  \n"
      "gamma involved in non associated calorimeter hits");

  dpp::base_module::common_ocd(ocd_);

  {
    // Description of the 'PTD_label' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("PTD_label")
        .set_terse_description("The label/name of the 'particle track data' bank")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_long_description(
            "This is the name of the bank to be used as    \n"
            "the source of calorimeter hits and reconstructed vertices. \n")
        .set_default_value_string(snemo::datamodel::data_info::default_particle_track_data_label())
        .add_example(
            "Use an alternative name for the 'particle track data' bank:: \n"
            "                                  \n"
            "  PTD_label : string = \"PTD2\"   \n"
            "                                  \n");
  }

  {
    // Description of the 'Geo_label' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("Geo_label")
        .set_terse_description("The label/name of the geometry service")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_long_description(
            "This is the name of the service to be used as the \n"
            "geometry service.                                 \n"
            "This property is only used if no geometry manager \n"
            "as been provided to the module.                   \n")
        .set_default_value_string(snemo::service_info::default_geometry_service_label())
        .add_example(
            "Use an alternative name for the geometry service:: \n"
            "                                     \n"
            "  Geo_label : string = \"geometry2\" \n"
            "                                     \n");
  }

  // Invoke specific OCD support from the driver class:
  ::snemo::reconstruction::gamma_clustering_driver::init_ocd(ocd_);

  // Additionnal configuration hints :
  ocd_.set_configuration_hints(
      "Here is a full configuration example in the                        \n"
      "``datatools::properties`` ASCII format::                           \n"
      "                                                                   \n"
      "  PTD_label : string = \"PTD\"                                     \n"
      "  Geo_label : string = \"geometry\"                                \n"
      "  BGB.add_foil_vertex_extrapolation : boolean = 1                  \n"
      "  BGB.add_foil_vertex_minimal_probability : real as fraction = 1 % \n"
      "  driver : string = \"GC\"                                         \n"
      "  GC.cluster_time_range : real as time = 2.5 ns                    \n"
      "  GC.cluster_grid_mask : string = \"first\"                        \n"
      "                                                                   \n");

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()  // Closing macro for implementation
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::reconstruction::gamma_clustering_module,
                               "snemo::reconstruction::gamma_clustering_module")
