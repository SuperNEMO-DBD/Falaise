/// \file falaise/snemo/reconstruction/sultan_tracker_clustering_module.cc

// Ourselves:
#include <CAT/sultan_tracker_clustering_module.h>

// Standard library:
#include <sstream>
#include <stdexcept>

// Third party:
// - Bayeux/datatools:
#include <bayeux/datatools/service_manager.h>
// - Bayeux/geomtools:
#include <bayeux/geomtools/geometry_service.h>
#include <bayeux/geomtools/manager.h>

// This project:
#include <falaise/property_set.h>

#include <falaise/snemo/datamodels/data_model.h>
#include <falaise/snemo/datamodels/tracker_clustering_data.h>
#include <falaise/snemo/processing/base_tracker_clusterizer.h>
#include <falaise/snemo/services/services.h>
#include "falaise/snemo/services/geometry.h"
#include "falaise/snemo/services/service_handle.h"

// This plugin:
#include <CAT/sultan_driver.h>

namespace snemo {

namespace reconstruction {

// Registration instantiation macro :
DPP_MODULE_REGISTRATION_IMPLEMENT(sultan_tracker_clustering_module,
                                  "snemo::reconstruction::sultan_tracker_clustering_module")

// Constructor :
sultan_tracker_clustering_module::sultan_tracker_clustering_module(
    datatools::logger::priority logging_priority_)
    : dpp::base_module(logging_priority_) {
  _set_defaults();
}

// Destructor :
sultan_tracker_clustering_module::~sultan_tracker_clustering_module() {
  if (is_initialized()) {
    sultan_tracker_clustering_module::reset();
  }
}

void sultan_tracker_clustering_module::_set_defaults() {
  geoManager_ = nullptr;
  CDTag_.clear();
  TCDTag_.clear();
  sultanAlgo_.reset(nullptr);
}

void sultan_tracker_clustering_module::reset() {
  DT_THROW_IF(!is_initialized(), std::logic_error,
              "Module '" << get_name() << "' is not initialized !");
  _set_initialized(false);
  // Reset the clusterizer driver :
  if (sultanAlgo_) {
    if (sultanAlgo_->is_initialized()) {
      sultanAlgo_->reset();
    }
    sultanAlgo_.reset();
  }
  _set_defaults();
}


void sultan_tracker_clustering_module::set_cd_label(const std::string& cdl_) {
  DT_THROW_IF(is_initialized(), std::logic_error,
              "Module '" << get_name() << "' is already initialized ! ");
  CDTag_ = cdl_;
}

const std::string& sultan_tracker_clustering_module::get_cd_label() const { return CDTag_; }

void sultan_tracker_clustering_module::set_tcd_label(const std::string& tcdl_) {
  DT_THROW_IF(is_initialized(), std::logic_error,
              "Module '" << get_name() << "' is already initialized ! ");
  TCDTag_ = tcdl_;
}

const std::string& sultan_tracker_clustering_module::get_tcd_label() const { return TCDTag_; }

const geomtools::manager& sultan_tracker_clustering_module::get_geometry_manager() const {
  return *geoManager_;
}

void sultan_tracker_clustering_module::set_geometry_manager(const geomtools::manager& gmgr_) {
  DT_THROW_IF(is_initialized(), std::logic_error,
              "Module '" << get_name() << "' is already initialized ! ");
  geoManager_ = &gmgr_;

  // Check setup label:
  const std::string& setup_label = geoManager_->get_setup_label();
  DT_THROW_IF(setup_label != "snemo::demonstrator" && setup_label != "snemo::tracker_commissioning",
              std::logic_error, "Setup label '" << setup_label << "' is not supported !");
}


void sultan_tracker_clustering_module::initialize(
    const datatools::properties& config, datatools::service_manager& services,
    dpp::module_handle_dict_type& /* unused */) {
  DT_THROW_IF(is_initialized(), std::logic_error,
              "Module '" << get_name() << "' is already initialized ! ");

  dpp::base_module::_common_initialize(config);

  namespace snedm = snemo::datamodel;

  falaise::property_set ps{config};

  CDTag_ = ps.get<std::string>("CD_label", snedm::data_info::default_calibrated_data_label());
  TCDTag_ =
      ps.get<std::string>("TCD_label", snedm::data_info::default_tracker_clustering_data_label());

  // Geometry manager :
  if (geoManager_ == nullptr) {
    snemo::service_handle<snemo::geometry_svc> geoSVC{services};
    set_geometry_manager(*(geoSVC.operator->()));
  }

  // Clustering algorithm :
  sultanAlgo_.reset(new sultan_driver);
  DT_THROW_IF(!sultanAlgo_, std::logic_error,
              "Module '" << get_name() << "' could not instantiate the '" << sultan_driver::SULTAN_ID
                         << "' tracker clusterizer algorithm !");

  // Initialize the clustering driver :
  sultanAlgo_->set_geometry_manager(get_geometry_manager());
  sultanAlgo_->initialize(config);

  _set_initialized(true);
}



// Processing :
dpp::base_module::process_status sultan_tracker_clustering_module::process(
    datatools::things& event) {
  DT_THROW_IF(!is_initialized(), std::logic_error,
              "Module '" << get_name() << "' is not initialized !");

  namespace snedm = snemo::datamodel;
  // Get input data or fail
  if (!event.has(CDTag_)) {
    DT_THROW_IF(true, std::logic_error, "Missing calibrated data to be processed !");
  }
  const auto& calibratedData = event.get<snedm::calibrated_data>(CDTag_);

  // Get or create output data
  auto& clusteringData = snedm::getOrAddToEvent<snedm::tracker_clustering_data>(TCDTag_, event);

  if (clusteringData.has_solutions()) {
    DT_LOG_WARNING(get_logging_priority(),
                   "Module " << get_name() << " resetting solutions in bank " << TCDTag_)
    clusteringData.reset();
  }

  // Main processing method :
  _process(calibratedData, clusteringData);
  return dpp::base_module::PROCESS_SUCCESS;
}

void sultan_tracker_clustering_module::_process(
    const snemo::datamodel::calibrated_data& calib_data,
    snemo::datamodel::tracker_clustering_data& clustering_data) {
  // Process the clusterizer driver :
  sultanAlgo_->process(calib_data.calibrated_tracker_hits(),
                       calib_data.calibrated_calorimeter_hits(), clustering_data);
}

}  // end of namespace reconstruction

}  // end of namespace snemo

/* OCD support */
#include <datatools/object_configuration_description.h>
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::reconstruction::sultan_tracker_clustering_module, ocd_) {
  ocd_.set_class_name("snemo::reconstruction::sultan_tracker_clustering_module");
  ocd_.set_class_description("A module that performs the SULTAN clusterization of tracker hits");
  ocd_.set_class_library("falaise");
  ocd_.set_class_documentation("This module uses the SULTAN clustering algorithm.");

  // Invoke OCD support from parent class :
  dpp::base_module::common_ocd(ocd_);

  // Invoke specific OCD support from the driver class:
  ::snemo::reconstruction::sultan_driver::init_ocd(ocd_);

  {
    std::ostringstream ldesc;
    ldesc << "This is the name of the bank to be used \n"
          << "as the source of input tracker hits.    \n"
          << "Default value is: \"" << snemo::datamodel::data_info::default_calibrated_data_label()
          << "\".  \n";
    // Description of the 'CD_label' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("CD_label")
        .set_terse_description("The label/name of the 'calibrated data' bank")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_long_description(ldesc.str())
        .set_default_value_string(snemo::datamodel::data_info::default_calibrated_data_label())
        .add_example(
            "Use an alternative name for the 'calibrated data' bank:: \n"
            "                                \n"
            "  CD_label : string = \"CD2\"   \n"
            "                                \n");
  }

  {
    std::ostringstream ldesc;
    ldesc << "This is the name of the bank to be used \n"
          << "as the sink of output clusters of tracker hits.    \n"
          << "Default value is: \""
          << snemo::datamodel::data_info::default_tracker_clustering_data_label() << "\".  \n";
    // Description of the 'TCD_label' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("TCD_label")
        .set_terse_description("The label/name of the 'tracker clustering data' bank")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_long_description(ldesc.str())
        .set_default_value_string(
            snemo::datamodel::data_info::default_tracker_clustering_data_label())
        .add_example(
            "Use an alternative name for the 'tracker clustering data' bank:: \n"
            "                                  \n"
            "  TCD_label : string = \"TCD2\"   \n"
            "                                  \n");
  }

  // Additionnal configuration hints :
  ocd_.set_configuration_hints(
      "Here is a full configuration example in the                          \n"
      "``datatools::properties`` ASCII format::                             \n"
      "                                                                     \n"
      "  TC.logging.priority          : string = \"fatal\"                  \n"
      "  TC.locator_plugin_name       : string = \"locators_driver\"        \n"
      "  TPC.delayed_hit_cluster_time : real as time = 10 us                \n"
      "  TPC.processing_prompt_hits   : boolean = 1                         \n"
      "  TPC.processing_delayed_hits  : boolean = 1                         \n"
      "  TPC.split_chamber            : boolean = 0                         \n"
      "  CD_label                     : string = \"CD\"                     \n"
      "  TCD_label                    : string = \"TCD\"                    \n"
      "  SULTAN.magnetic_field        : real   = 25 gauss                   \n"
      "  SULTAN.max_time              : real   = 5000.0 ms                  \n"
      "  SULTAN.Emin                  : real  = 120 keV                     \n"
      "  SULTAN.Emax                  : real  = 3.3 MeV                     \n"
      "  SULTAN.nsigma_r              : real  = 3.0                         \n"
      "  SULTAN.nsigma_z              : real  = 4.0                         \n"
      "  SULTAN.nofflayers            : integer = 1                         \n"
      "  SULTAN.nsigmas               : real  = 1.0                         \n"
      "  SULTAN.sigma_z_factor        : real  = 1.0                         \n"
      "                                                                     \n"
      "Additional specific parameters can be used to configure              \n"
      "the embedded ``SULTAN`` driver itself; see the OCD support           \n"
      "of the ``snemo::reconstruction::sultan_driver`` class.               \n");

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()  // Closing macro for implementation
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::reconstruction::sultan_tracker_clustering_module,
                               "snemo::reconstruction::sultan_tracker_clustering_module")
