/// \file lttc/lttc_tracker_clustering_module.cc

// Ourselves:
#include <lttc/lttc_tracker_clustering_module.hh>

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

// LTTC:
#include <lttc/lttc_driver.hh>

namespace snemo {

  namespace reconstruction {

    // Registration instantiation macro :
    DPP_MODULE_REGISTRATION_IMPLEMENT(lttc_tracker_clustering_module,
                                      "snemo::reconstruction::lttc_tracker_clustering_module")

    // Constructor :
    lttc_tracker_clustering_module::lttc_tracker_clustering_module(datatools::logger::priority p)
    : dpp::base_module(p) {
      _set_defaults();
    }

    // Destructor :
    lttc_tracker_clustering_module::~lttc_tracker_clustering_module() {
      if (is_initialized()) {
        lttc_tracker_clustering_module::reset();
      }
    }

    void lttc_tracker_clustering_module::set_cd_label(const std::string& cd) {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Module '" << get_name() << "' is already initialized ! ");
      CDTag_ = cd;
    }

    void lttc_tracker_clustering_module::reset() {
      DT_THROW_IF(!is_initialized(), std::logic_error,
                  "Module '" << get_name() << "' is not initialized !");
      _set_initialized(false);
      // Reset the clusterizer driver :
      if (lttcAlgo_) {
        if (lttcAlgo_->is_initialized()) {
          lttcAlgo_->reset();
        }
        lttcAlgo_.reset();
      }
      _set_defaults();
    }

    void lttc_tracker_clustering_module::_set_defaults() {
      geoManager_ = nullptr;
      CDTag_.clear();
      TCDTag_.clear();
      lttcAlgo_.reset(nullptr);
    }

    const std::string& lttc_tracker_clustering_module::get_cd_label() const { return CDTag_; }

    void lttc_tracker_clustering_module::set_tcd_label(const std::string& tc) {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Module '" << get_name() << "' is already initialized ! ");
      TCDTag_ = tc;
    }

    const std::string& lttc_tracker_clustering_module::get_tcd_label() const { return TCDTag_; }

    const geomtools::manager& lttc_tracker_clustering_module::get_geometry_manager() const {
      return *geoManager_;
    }

    void lttc_tracker_clustering_module::set_geometry_manager(const geomtools::manager& gm) {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Module '" << get_name() << "' is already initialized ! ");
      // Check setup label:
      const std::string& setup_label = gm.get_setup_label();
      DT_THROW_IF(setup_label != "snemo::demonstrator" && setup_label != "snemo::tracker_commissioning",
                  std::logic_error, "Setup label '" << setup_label << "' is not supported !");
      geoManager_ = &gm;
    }

    void lttc_tracker_clustering_module::initialize(const datatools::properties& config,
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
      lttcAlgo_.reset(new lttc_driver);
      DT_THROW_IF(!lttcAlgo_, std::logic_error,
                  "Module '" << get_name() << "' could not instantiate the '" << lttc_driver::LTTC_ID
                  << "' tracker clusterizer algorithm !");
      lttcAlgo_->set_geometry_manager(get_geometry_manager());
      lttcAlgo_->initialize(config);
      _set_initialized(true);
    }

    // Processing :
    dpp::base_module::process_status lttc_tracker_clustering_module::process(datatools::things& event) {
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

    void lttc_tracker_clustering_module::_process(
                                                  const snemo::datamodel::calibrated_data& calib_data,
                                                  snemo::datamodel::tracker_clustering_data& clustering_data) {
      // Process the clusterizer driver :
      lttcAlgo_->process(calib_data.tracker_hits(), calib_data.calorimeter_hits(),
                         clustering_data);
    }

  }  // namespace reconstruction

}  // end of namespace snemo

/* OCD support */
#include <datatools/object_configuration_description.h>
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::reconstruction::lttc_tracker_clustering_module, ocd_) {
  ocd_.set_class_name("snemo::reconstruction::lttc_tracker_clustering_module");
  ocd_.set_class_description("A module that performs the LTTC clusterization of tracker hits");
  ocd_.set_class_library("Falaise_LTTC");
  ocd_.set_class_documentation("This module uses the LTTC clustering algorithm.");

  // Invoke OCD support from parent class :
  dpp::base_module::common_ocd(ocd_);

  // Invoke specific OCD support from the driver class:
  ::snemo::reconstruction::lttc_driver::init_ocd(ocd_);

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
                               "  LTTC.dummy                   : boolean = true                      \n"
                               "                                                                     \n"
                               "Additional specific parameters can be used to configure              \n"
                               "the embedded ``LTTC`` driver itself; see the OCD support             \n"
                               "of the ``snemo::reconstruction::lttc_driver`` class.                 \n");

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()  // Closing macro for implementation
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::reconstruction::lttc_tracker_clustering_module,
                               "snemo::reconstruction::lttc_tracker_clustering_module")
