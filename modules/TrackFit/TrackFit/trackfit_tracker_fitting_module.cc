/// \file falaise/snemo/reconstruction/trackfit_tracker_fitting_module.cc

// Ourselves:
#include <TrackFit/trackfit_tracker_fitting_module.h>

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
#include <falaise/config/property_set.h>
#include <falaise/snemo/datamodels/data_model.h>
#include <falaise/snemo/datamodels/tracker_clustering_data.h>
#include <falaise/snemo/datamodels/tracker_trajectory_data.h>
#include <falaise/snemo/services/services.h>
#include "falaise/snemo/services/geometry.h"
#include "falaise/snemo/services/service_handle.h"

// TrackFit
#include <TrackFit/trackfit_driver.h>

namespace snemo {

namespace reconstruction {

// Registration instantiation macro :
DPP_MODULE_REGISTRATION_IMPLEMENT(trackfit_tracker_fitting_module,
                                  "snemo::reconstruction::trackfit_tracker_fitting_module")

// Constructor :
trackfit_tracker_fitting_module::trackfit_tracker_fitting_module(
    datatools::logger::priority logging_priority_)
    : dpp::base_module(logging_priority_) {
  _set_defaults();
}

// Destructor :
trackfit_tracker_fitting_module::~trackfit_tracker_fitting_module() {
  if (is_initialized()) {
    trackfit_tracker_fitting_module::reset();
  }
}

const geomtools::manager& trackfit_tracker_fitting_module::get_geometry_manager() const {
  return *geoManager_;
}

void trackfit_tracker_fitting_module::set_geometry_manager(const geomtools::manager& gmgr_) {
  DT_THROW_IF(is_initialized(), std::logic_error,
              "Module '" << get_name() << "' is already initialized ! ");
  geoManager_ = &gmgr_;

  // Check setup label:
  const std::string& setup_label = geoManager_->get_setup_label();
  DT_THROW_IF(setup_label != "snemo::demonstrator" && setup_label != "snemo::tracker_commissioning",
              std::logic_error, "Setup label '" << setup_label << "' is not supported !");
}

void trackfit_tracker_fitting_module::_set_defaults() {
  geoManager_ = nullptr;
  TCDTag_.clear();
  TTDTag_.clear();
  fitterAlgo_.reset(nullptr);
}

// Initialization :
void trackfit_tracker_fitting_module::initialize(const datatools::properties& config,
                                                 datatools::service_manager& services,
                                                 dpp::module_handle_dict_type& /* unused */) {
  DT_THROW_IF(is_initialized(), std::logic_error,
              "Module '" << get_name() << "' is already initialized ! ");

  dpp::base_module::_common_initialize(config);

  falaise::config::property_set ps{config};
  TCDTag_ = ps.get<std::string>(
      "TCD_label", snemo::datamodel::data_info::default_tracker_clustering_data_label());
  TTDTag_ = ps.get<std::string>(
      "TTD_label", snemo::datamodel::data_info::default_tracker_trajectory_data_label());

  snemo::service_handle<snemo::geometry_svc> geoSVC{services};
  set_geometry_manager(*(geoSVC.operator->()));

  // Tracking algorithm :
  std::string algorithm_id = trackfit_driver::trackfit_id();
  // Initialize the fitting algo:
  fitterAlgo_.reset(new trackfit_driver);
  fitterAlgo_->set_geometry_manager(get_geometry_manager());
  fitterAlgo_->initialize(config);
  _set_initialized(true);
}

void trackfit_tracker_fitting_module::reset() {
  DT_THROW_IF(!is_initialized(), std::logic_error,
              "Module '" << get_name() << "' is not initialized !");
  _set_initialized(false);
  // Reset the fitter driver :
  if (fitterAlgo_) {
    if (fitterAlgo_->is_initialized()) {
      fitterAlgo_->reset();
    }
    fitterAlgo_.reset();
  }
  _set_defaults();
}

// Processing :
dpp::base_module::process_status trackfit_tracker_fitting_module::process(
    datatools::things& event) {
  DT_THROW_IF(!is_initialized(), std::logic_error,
              "Module '" << get_name() << "' is not initialized !");
  namespace snedm = snemo::datamodel;

  // Check tracker clustering data
  if (!event.has(TCDTag_)) {
    DT_THROW_IF(true, std::logic_error, "Missing tracker clustering data to be processed !");
  }
  const auto& inputClusters = event.get<snedm::tracker_clustering_data>(TCDTag_);

  // Check tracker trajectory data
  auto& outputTrajectories = snedm::getOrAddToEvent<snedm::tracker_trajectory_data>(TTDTag_, event);
  if (outputTrajectories.has_solutions()) {
    DT_LOG_WARNING(get_logging_priority(),
                   "Event bank '" << TTDTag_ << "' already has processed tracker trajectory data");
  }
  outputTrajectories.reset();

  // Main processing method :
  _process(inputClusters, outputTrajectories);

  return dpp::base_module::PROCESS_SUCCESS;
}

void trackfit_tracker_fitting_module::_process(
    const snemo::datamodel::tracker_clustering_data& clusters,
    snemo::datamodel::tracker_trajectory_data& trajectories) {
  // Process isolated tracks using external resource:
  if (!clusters.has_solutions()) {
    DT_LOG_TRACE(get_logging_priority(), "No clustered solution to be fitted");
    return;
  }

  // process the fitter driver :
  fitterAlgo_->process(clusters, trajectories);
}

}  // end of namespace reconstruction

}  // end of namespace snemo

/* OCD support */
#include <datatools/object_configuration_description.h>
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::reconstruction::trackfit_tracker_fitting_module, ocd_) {
  ocd_.set_class_name("snemo::reconstruction::trackfit_tracker_fitting_module");
  ocd_.set_class_description(
      "A module that performs the track fitting using the TrackFit algorithms");
  ocd_.set_class_library("Falaise_TrackFit");
  ocd_.set_class_documentation(
      "This module uses the TrackFit trjectory fitting algorithms for.   \n"
      "charged particles traversing the tracking chamber. See also OCD   \n"
      "support for the ``snemo::reconstruction::trackfit_driver`` class. \n");

  dpp::base_module::common_ocd(ocd_);

  {
    // Description of the 'TCD_label' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("TCD_label")
        .set_terse_description("The label/name of the 'tracker clustering data' bank")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_long_description(
            "This is the name of the bank to be used as    \n"
            "the source of input clusters of tracker hits. \n")
        .set_default_value_string(
            snemo::datamodel::data_info::default_tracker_clustering_data_label())
        .add_example(
            "Use an alternative name for the 'tracker clustering data' bank:: \n"
            "                                  \n"
            "  TCD_label : string = \"TCD2\"   \n"
            "                                  \n");
  }

  {
    // Description of the 'TTD_label' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("TTD_label")
        .set_terse_description("The label/name of the 'tracker trajectory data' bank")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_long_description(
            "This is the name of the bank to be used as \n"
            "the sink of output tracker trajectories.   \n")
        .set_default_value_string(
            snemo::datamodel::data_info::default_tracker_trajectory_data_label())
        .add_example(
            "Use an alternative name for the 'tracker trajectory data' bank:: \n"
            "                                  \n"
            "  TTD_label : string = \"TTD2\"   \n"
            "                                  \n");
  }

  // Additionnal configuration hints :
  ocd_.set_configuration_hints(
      "Here is a full configuration example in the      \n"
      "``datatools::properties`` ASCII format::         \n"
      "                                         \n"
      "  TCD_label : string = \"TCD\"           \n"
      "  TTD_label : string = \"TTD\"           \n"
      "                                         \n"
      "Additional specific parameters are used to configure         \n"
      "the embedded ``TrackFit`` driver itself; see the OCD support \n"
      "of the ``snemo::reconstruction::trackfit_driver`` class.     \n");

  ocd_.set_validation_support(true);
  ocd_.lock();
}
DOCD_CLASS_IMPLEMENT_LOAD_END()  // Closing macro for implementation
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::reconstruction::trackfit_tracker_fitting_module,
                               "snemo::reconstruction::trackfit_tracker_fitting_module")
