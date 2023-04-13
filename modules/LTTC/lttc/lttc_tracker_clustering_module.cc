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
    lttc_tracker_clustering_module::lttc_tracker_clustering_module(datatools::logger::priority p_)
    : dpp::base_module(p_)
    {
      _set_defaults();
    }

    // Destructor :
    lttc_tracker_clustering_module::~lttc_tracker_clustering_module()
    {
      if (is_initialized()) {
        lttc_tracker_clustering_module::reset();
      }
    }

    void lttc_tracker_clustering_module::_set_defaults()
    {
      _EH_tag_ = "EH";
      _CD_tag_ = "CD";
      _TCD_tag_ = "TCD";
      _clusterizer_driver_.reset();
    }

    void lttc_tracker_clustering_module::set_eh_label(const std::string & eh_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Module '" << get_name() << "' is already initialized ! ");
      _EH_tag_ = eh_;
    }

    const std::string& lttc_tracker_clustering_module::get_eh_label() const
    {
      return _EH_tag_;
    }

    void lttc_tracker_clustering_module::set_cd_label(const std::string & cd_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Module '" << get_name() << "' is already initialized ! ");
      _CD_tag_ = cd_;
    }

    const std::string& lttc_tracker_clustering_module::get_cd_label() const
    {
      return _CD_tag_;
    }

    void lttc_tracker_clustering_module::set_tcd_label(const std::string & tc_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Module '" << get_name() << "' is already initialized ! ");
      _TCD_tag_ = tc_;
    }

    const std::string& lttc_tracker_clustering_module::get_tcd_label() const
    {
      return _TCD_tag_;
    }

    const snemo::processing::detector_description &
    lttc_tracker_clustering_module::get_detector_description() const
    {
      return _detector_desc_;
    }

    void lttc_tracker_clustering_module::initialize(const datatools::properties & config_,
                                                    datatools::service_manager & services_,
                                                    dpp::module_handle_dict_type & /* unused */)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Module '" << get_name() << "' is already initialized ! ");
      dpp::base_module::_common_initialize(config_);

      falaise::property_set ps{config_};

      _EH_tag_ = ps.get<std::string>("EH_label", snedm::labels::event_header());
      _CD_tag_ = ps.get<std::string>("CD_label", snedm::labels::calibrated_data());
      _TCD_tag_ = ps.get<std::string>("TCD_label", snedm::labels::tracker_clustering_data());

      // Geometry manager :
      if (not _detector_desc_.has_geometry_manager()) {
        snemo::service_handle<snemo::geometry_svc> geoSVC{services_};
        _detector_desc_.set_geometry_manager(*(geoSVC.operator->()));
      }

      // Tracker cell status service :
      if (not _detector_desc_.has_cell_status_service()) {
        snemo::service_handle<snemo::tracker_cell_status_service> cellStatusSVC{services_};
        _detector_desc_.set_cell_status_service(*(cellStatusSVC.operator->()));
      }

      // Clustering algorithm :
      // Initialize the clustering driver :
      _clusterizer_driver_ = std::make_unique<lttc::lttc_driver>();
      {
        // Specific to LTTC driver:
        lttc::lttc_driver * lttcDriver = dynamic_cast<lttc::lttc_driver *>(_clusterizer_driver_.get());
        DT_THROW_IF(! lttcDriver, std::logic_error,
                    "Module '" << get_name() << "' could not instantiate the '" << lttc::lttc_driver::LTTC_ID
                    << "' tracker clusterizer algorithm !");
        lttcDriver->set_detector_description(get_detector_description());
      }
      _clusterizer_driver_->initialize(config_);
      
      _set_initialized(true);
    }

    void lttc_tracker_clustering_module::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error,
                  "Module '" << get_name() << "' is not initialized !");
      _set_initialized(false);
      // Reset the clusterizer driver :
      if (_clusterizer_driver_) {
        if (_clusterizer_driver_->is_initialized()) {
          _clusterizer_driver_->reset();
        }
        _clusterizer_driver_.reset();
      }
      _set_defaults();
    }

    // Processing :
    dpp::base_module::process_status
    lttc_tracker_clustering_module::process(datatools::things & event_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error,
                  "Module '" << get_name() << "' is not initialized !");
      namespace snedm = snemo::datamodel;
      if (datatools::logger::is_debug(get_logging_priority())) {
        std::cerr << "[debug] *********************************" << '\n';
        std::cerr << "[debug] *        NEW LTTC EVENT         *" << '\n';
        std::cerr << "[debug] *********************************" << '\n';
      }
      
     // Get event_header or fail
      if (!event_.has(_EH_tag_)) {
        DT_THROW(std::logic_error, "Missing event header to be processed !");
      }
      const auto & eventHeader = event_.get<snedm::event_header>(_EH_tag_);
      // Get input data or fail
      if (!event_.has(_CD_tag_)) {
        DT_THROW(std::logic_error, "Missing calibrated data to be processed !");
      }
      const auto & calibratedData = event_.get<snedm::calibrated_data>(_CD_tag_);

      // Get or create output data
      auto & clusteringData = ::snedm::getOrAddToEvent<snedm::tracker_clustering_data>(_TCD_tag_, event_);
      clusteringData.clear();

      // Main processing method :
      _process(eventHeader, calibratedData, clusteringData);

      if (datatools::logger::is_debug(get_logging_priority())) {
        std::cerr << "[debug] *********************************" << '\n';
        std::cerr << "[debug] *      END OF LTTC EVENT        *" << '\n';
        std::cerr << "[debug] *********************************" << '\n' << '\n';
      }

      return dpp::base_module::PROCESS_SUCCESS;
    }

    void lttc_tracker_clustering_module::_process(const snemo::datamodel::event_header & event_header_,
                                                  const snemo::datamodel::calibrated_data & calib_data_,
                                                  snemo::datamodel::tracker_clustering_data & clustering_data_)
    {
      DT_LOG_DEBUG(get_logging_priority(), "Entering...");
      if (event_header_.is_simulated()) {
        if (event_header_.has_mc_timestamp()) {
          DT_LOG_DEBUG(get_logging_priority(), "Simulated event has a MC timestamp");
        }
      }

      if (calib_data_.tracker_hits().size() == 0) {
        DT_LOG_DEBUG(get_logging_priority(), "Calibrated data has not tracker hits!");
      } else {
        DT_LOG_DEBUG(get_logging_priority(), "Calibrated data has " << calib_data_.tracker_hits().size() << " tracker hits!");
      }
  
      if (clustering_data_.solutions().size() == 0) {
        DT_LOG_DEBUG(get_logging_priority(), "Tracker clustering data has no solutions yet!");
      }
      
      if (event_header_.has_mc_timestamp()) {
        DT_LOG_DEBUG(get_logging_priority(), "Assign the event timestamp to the LTTC clusterizer driver");
       _clusterizer_driver_->set_event_timestamp(event_header_.get_mc_timestamp());
      }
      
      // Process the clusterizer driver :
      DT_LOG_DEBUG(get_logging_priority(), "Run the LTTC clusterizer driver");
      _clusterizer_driver_->process(calib_data_.tracker_hits(),
                                    calib_data_.calorimeter_hits(),
                                    clustering_data_);
      
      DT_LOG_DEBUG(get_logging_priority(), "Exiting...");
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
  // ::snemo::reconstruction::lttc_driver::init_ocd(ocd_);

  {
    // Description of the 'EH_label' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("EH_label")
      .set_terse_description("The label/name of the 'event_header' bank")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description(
                            "This is the name of the bank to be used \n"
                            "as the event header.    \n")
      .set_default_value_string(snedm::labels::event_header())
      .add_example(
                   "Use an alternative name for the 'event_header' bank:: \n"
                   "                                \n"
                   "  EH_label : string = \"EH2\"   \n"
                   "                                \n");
  }

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
                               "  TPC.processing_prompt_hits   : boolean = true                      \n"
                               "  TPC.processing_delayed_hits  : boolean = true                      \n"
                               "  TPC.split_chamber            : boolean = 0                         \n"
                               "  EH_label                     : string = \"EH\"                     \n"
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
