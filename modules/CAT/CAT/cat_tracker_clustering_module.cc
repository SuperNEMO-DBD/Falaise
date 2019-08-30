/// \file falaise/snemo/reconstruction/cat_tracker_clustering_module.cc

// Ourselves:
#include <CAT/cat_tracker_clustering_module.h>

// Standard library:
#include <sstream>
#include <stdexcept>

// Third party:
// - Bayeux/datatools:
#include <datatools/service_manager.h>
// - Bayeux/geomtools:
#include <geomtools/geometry_service.h>
#include <geomtools/manager.h>

// This project:
#include <falaise/property_set.h>

#include <falaise/snemo/datamodels/data_model.h>
#include <falaise/snemo/datamodels/tracker_clustering_data.h>
#include <falaise/snemo/processing/base_tracker_clusterizer.h>
#include <falaise/snemo/services/services.h>
#include "falaise/snemo/services/geometry.h"
#include "falaise/snemo/services/service_handle.h"

// CAT:
#include <CAT/cat_driver.h>

namespace snemo {

namespace reconstruction {

// Registration instantiation macro :
DPP_MODULE_REGISTRATION_IMPLEMENT(cat_tracker_clustering_module,
                                  "snemo::reconstruction::cat_tracker_clustering_module")

// Constructor :
cat_tracker_clustering_module::cat_tracker_clustering_module(datatools::logger::priority p)
    : dpp::base_module(p) {
  _set_defaults();
}

// Destructor :
cat_tracker_clustering_module::~cat_tracker_clustering_module() {
  if (is_initialized()) {
    cat_tracker_clustering_module::reset();
  }
}

void cat_tracker_clustering_module::set_cd_label(const std::string& cd) {
  DT_THROW_IF(is_initialized(), std::logic_error,
              "Module '" << get_name() << "' is already initialized ! ");
  CDTag_ = cd;
}

void cat_tracker_clustering_module::reset() {
  DT_THROW_IF(!is_initialized(), std::logic_error,
              "Module '" << get_name() << "' is not initialized !");
  _set_initialized(false);
  // Reset the clusterizer driver :
  if (catAlgo_) {
    if (catAlgo_->is_initialized()) {
      catAlgo_->reset();
    }
    catAlgo_.reset();
  }
  _set_defaults();
}

void cat_tracker_clustering_module::_set_defaults() {
  geoManager_ = nullptr;
  CDTag_.clear();
  TCDTag_.clear();
  catAlgo_.reset(nullptr);
}

const std::string& cat_tracker_clustering_module::get_cd_label() const { return CDTag_; }

void cat_tracker_clustering_module::set_tcd_label(const std::string& tc) {
  DT_THROW_IF(is_initialized(), std::logic_error,
              "Module '" << get_name() << "' is already initialized ! ");
  TCDTag_ = tc;
}

const std::string& cat_tracker_clustering_module::get_tcd_label() const { return TCDTag_; }

const geomtools::manager& cat_tracker_clustering_module::get_geometry_manager() const {
  return *geoManager_;
}

void cat_tracker_clustering_module::set_geometry_manager(const geomtools::manager& gm) {
  DT_THROW_IF(is_initialized(), std::logic_error,
              "Module '" << get_name() << "' is already initialized ! ");
  // Check setup label:
  const std::string& setup_label = gm.get_setup_label();
  DT_THROW_IF(setup_label != "snemo::demonstrator" && setup_label != "snemo::tracker_commissioning",
              std::logic_error, "Setup label '" << setup_label << "' is not supported !");
  geoManager_ = &gm;
}

void cat_tracker_clustering_module::initialize(const datatools::properties& config,
                                               datatools::service_manager& services,
                                               dpp::module_handle_dict_type& /* unused */) {
  DT_THROW_IF(is_initialized(), std::logic_error,
              "Module '" << get_name() << "' is already initialized ! ");
  dpp::base_module::_common_initialize(config);

  falaise::property_set ps{config};

  CDTag_ = ps.get<std::string>("CD_label", snedm::labels::calibrated_data());
  TCDTag_ = ps.get<std::string>("TCD_label", snedm::labels::tracker_clustering_data());

  // Geometry manager :
  if (geoManager_ == nullptr) {
    snemo::service_handle<snemo::geometry_svc> geoSVC{services};
    set_geometry_manager(*(geoSVC.operator->()));
  }

  // Clustering algorithm :
  // Initialize the clustering driver :
  catAlgo_.reset(new cat_driver);
  DT_THROW_IF(!catAlgo_, std::logic_error,
              "Module '" << get_name() << "' could not instantiate the '" << cat_driver::CAT_ID
                         << "' tracker clusterizer algorithm !");
  catAlgo_->set_geometry_manager(get_geometry_manager());
  catAlgo_->initialize(config);
  _set_initialized(true);
}

// Processing :
dpp::base_module::process_status cat_tracker_clustering_module::process(datatools::things& event) {
  DT_THROW_IF(!is_initialized(), std::logic_error,
              "Module '" << get_name() << "' is not initialized !");
  namespace snedm = snemo::datamodel;
  // Get input data or fail
  if (!event.has(CDTag_)) {
    DT_THROW_IF(true, std::logic_error, "Missing calibrated data to be processed !");
  }
  const auto& calibratedData = event.get<snedm::calibrated_data>(CDTag_);

  // Get or create output data
  auto& clusteringData = ::snedm::getOrAddToEvent<snedm::tracker_clustering_data>(TCDTag_, event);
  clusteringData.clear();

  // Main processing method :
  _process(calibratedData, clusteringData);

  return dpp::base_module::PROCESS_SUCCESS;
}

void cat_tracker_clustering_module::_process(
    const snemo::datamodel::calibrated_data& calib_data,
    snemo::datamodel::tracker_clustering_data& clustering_data) {
  // Process the clusterizer driver :
  catAlgo_->process(calib_data.calibrated_tracker_hits(), calib_data.calibrated_calorimeter_hits(),
                    clustering_data);
}

}  // namespace reconstruction

}  // end of namespace snemo

/* OCD support */
#include <datatools/object_configuration_description.h>
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::reconstruction::cat_tracker_clustering_module, ocd_) {
  ocd_.set_class_name("snemo::reconstruction::cat_tracker_clustering_module");
  ocd_.set_class_description("A module that performs the CAT clusterization of tracker hits");
  ocd_.set_class_library("Falaise_CAT");
  ocd_.set_class_documentation("This module uses the CAT clustering algorithm.");

  // Invoke OCD support from parent class :
  dpp::base_module::common_ocd(ocd_);

  // Invoke specific OCD support from the driver class:
  ::snemo::reconstruction::cat_driver::init_ocd(ocd_);

  {
    // Description of the 'CD_label' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("CD_label")
        .set_terse_description("The label/name of the 'calibrated data' bank")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_long_description(
            "This is the name of the bank to be used \n"
            "as the source of input tracker hits.    \n")
        .set_default_value_string(snedm::labels::calibrated_data())
        .add_example(
            "Use an alternative name for the 'calibrated data' bank:: \n"
            "                                \n"
            "  CD_label : string = \"CD2\"   \n"
            "                                \n");
  }

  {
    // Description of the 'TCD_label' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("TCD_label")
        .set_terse_description("The label/name of the 'tracker clustering data' bank")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_long_description(
            "This is the name of the bank to be used as   \n"
            "the sink of output clusters of tracker hits. \n")
        .set_default_value_string(snedm::labels::tracker_clustering_data())
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
      "  CAT.magnetic_field           : real   = 25 gauss                   \n"
      "  CAT.max_time                 : real   = 5000.0 ms                  \n"
      "  CAT.nofflayers               : integer = 1                         \n"
      "                                                                     \n"
      "Additional specific parameters can be used to configure              \n"
      "the embedded ``CAT`` driver itself; see the OCD support              \n"
      "of the ``snemo::reconstruction::cat_driver`` class.                  \n");

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()  // Closing macro for implementation
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::reconstruction::cat_tracker_clustering_module,
                               "snemo::reconstruction::cat_tracker_clustering_module")
