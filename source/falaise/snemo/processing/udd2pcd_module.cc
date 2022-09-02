// -*- mode: c++ ; -*-
/** \file falaise/snemo/processing/udd2pcd_module.cc
 */

// Ourselves:
#include "udd2pcd_module.h"
// #include "udd2pcd_module_utils.h"

// Standard library:
#include <sstream>
#include <stdexcept>

// Third party:
// - Bayeux/datatools:
#include <datatools/service_manager.h>
#include <datatools/utils.h>
// - Bayeux/geomtools:
#include <geomtools/geometry_service.h>
#include <geomtools/manager.h>
// - Bayeux/mctools:
#include <mctools/utils.h>

// This project :
#include <falaise/snemo/datamodels/data_model.h>
#include <falaise/snemo/services/services.h>
#include <falaise/snemo/rc/calorimeter_om_status.h>

namespace snemo {

  namespace processing {

    // Registration instantiation macro :
    DPP_MODULE_REGISTRATION_IMPLEMENT(udd2pcd_module,
                                      "snemo::processing::udd2pcd_module")

    void udd2pcd_module::initialize(const datatools::properties& ps,
                                    datatools::service_manager& /*unused*/,
                                    dpp::module_handle_dict_type& /*unused*/) {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Module '" << get_name() << "' is already initialized ! ");

      this->base_module::_common_initialize(ps);
      falaise::property_set fps{ps};

      _udd_input_tag_  = fps.get<std::string>("UDD_label", snedm::labels::unified_digitized_data());
      _pcd_output_tag_ = fps.get<std::string>("pCD_label", snedm::labels::precalibrated_data());

      this->base_module::_set_initialized(true);
    }

    void udd2pcd_module::reset() { this->base_module::_set_initialized(false); }

    // Processing :
    dpp::base_module::process_status udd2pcd_module::process(datatools::things& event) {
      DT_THROW_IF(!is_initialized(), std::logic_error,
                  "Module '" << get_name() << "' is not initialized !");

      // Check Input unified digitized data exists, or fail
      if (!event.has(_udd_input_tag_)) {
        throw std::logic_error("Missing unified digitized data to be processed !");
        return dpp::base_module::PROCESS_ERROR;
      }
      auto& udd_data = event.get<snemo::datamodel::unified_digitized_data>(_udd_input_tag_);

      // Check if some 'precalibrated_data' are available in the data model:
      // Precalibrated Data is a single object with each hit collection
      // May, or may not, have it depending on if we run before or after
      // other calibrators
      auto& precalibrated_data = snedm::getOrAddToEvent<snemo::datamodel::precalibrated_data>(_pcd_output_tag_, event);

      // Always rewrite calorimeter hits
      precalibrated_data.calorimeter_hits().clear();

      // Always rewrite tracker hits
      precalibrated_data.tracker_hits().clear();

      // Main calorimeter processing method :
      process_calo_impl(udd_data, precalibrated_data.calorimeter_hits());

      // Main tracker processing method :
      process_tracker_impl(udd_data, precalibrated_data.tracker_hits());

      return dpp::base_module::PROCESS_SUCCESS;
    }

    // Precalibrate calorimeter hits from UDD informations:
    void udd2pcd_module::precalibrate_calo_hits(const snemo::datamodel::unified_digitized_data & udd_data_,
                                                snemo::datamodel::PreCalibCalorimeterHitHdlCollection & calo_hits_) {

      // Retrieve UDD calorimeter digitized hits
      const auto& udd_calo_hits = udd_data_.get_calorimeter_hits();

      // Loop over UDD calorimeter digitized hits
      for (auto& a_udd_calo_hit : udd_calo_hits) {
        // For each UDD calorimeter digitized hit, produce and fill a precalibrated calorimeter hit

        auto new_pcd_calo = datatools::make_handle<snemo::datamodel::precalibrated_calorimeter_hit>();
        // auto& newHit = newHandle.grab();

        new_pcd_calo->set_hit_id(a_udd_calo_hit.get().get_hit_id());
        new_pcd_calo->set_geom_id(a_udd_calo_hit->get_geom_id());


       // Warning: timestamp is integer while time is double
        const double time = a_udd_calo_hit->get_timestamp();
        const double sigma_time = std::sqrt(time);
        new_pcd_calo->set_time(time);
        new_pcd_calo->set_sigma_time(sigma_time);

        // Preliminary version: Fill with Firmware Measurement values
        const double fwmeas_amplitude = a_udd_calo_hit->get_fwmeas_peak_amplitude();
        const double sigma_fwmeas_amplitude = std::sqrt(fwmeas_amplitude);
        new_pcd_calo->set_amplitude(fwmeas_amplitude);
        new_pcd_calo->set_sigma_amplitude(sigma_fwmeas_amplitude);

        const double fwmeas_charge = a_udd_calo_hit->get_fwmeas_charge();
        const double sigma_fwmeas_charge = std::sqrt(fwmeas_charge);
        new_pcd_calo->set_charge(fwmeas_charge);
        new_pcd_calo->set_sigma_charge(sigma_fwmeas_charge);

        const double fwmeas_baseline = a_udd_calo_hit->get_fwmeas_baseline();
        const double sigma_fwmeas_baseline = std::sqrt(fwmeas_baseline);
        new_pcd_calo->set_baseline(fwmeas_baseline);
        new_pcd_calo->set_sigma_baseline(sigma_fwmeas_baseline);

        // Warning: Have to convert rising cell into a time and add it to the continuous hit timestamp ?
        const double fwmeas_rising_time = time + a_udd_calo_hit->get_fwmeas_rising_cell();
        const double sigma_fwmeas_rising_time = std::sqrt(fwmeas_rising_time);
        new_pcd_calo->set_rising_time(fwmeas_rising_time);
        new_pcd_calo->set_sigma_rising_time(sigma_fwmeas_rising_time);

        // Warning: Have to convert falling cell into a time and add it to the continuous hit timestamp ?
        const double fwmeas_falling_time = time + a_udd_calo_hit->get_fwmeas_falling_cell();
        const double sigma_fwmeas_falling_time = std::sqrt(fwmeas_falling_time);
        new_pcd_calo->set_falling_time(fwmeas_falling_time);
        new_pcd_calo->set_sigma_falling_time(sigma_fwmeas_falling_time);

        const double fwmeas_time_width = fwmeas_falling_time - fwmeas_rising_time;
        const double sigma_fwmeas_time_width = std::sqrt(fwmeas_time_width);
        new_pcd_calo->set_time_width(fwmeas_time_width);
        new_pcd_calo->set_sigma_time_width(sigma_fwmeas_time_width);


        // Add additional information in pCD calo datatools::properties
        datatools::properties& pcd_calo_hit_properties = new_pcd_calo->grab_auxiliaries();
        pcd_calo_hit_properties.store("pCD_calorimeter_algorithm", calorimeter_precalibration_algorithm::ALGO_FWMEASUREMENT);


        calo_hits_.push_back(new_pcd_calo);
      }
    }

    void udd2pcd_module::process_calo_impl(const snemo::datamodel::unified_digitized_data & udd_data_,
                                           snemo::datamodel::PreCalibCalorimeterHitHdlCollection & calo_hits_) {
      precalibrate_calo_hits(udd_data_, calo_hits_);
    }


    // Precalibrate tracker hits from UDD informations:
    void udd2pcd_module::precalibrate_tracker_hits(const snemo::datamodel::unified_digitized_data & udd_data_,
                                                snemo::datamodel::PreCalibTrackerHitHdlCollection & tracker_hits_) {

      // Retrieve UDD tracker digitized hits
      const auto& udd_tracker_hits = udd_data_.get_tracker_hits();

      // Loop over UDD tracker digitized hits
      for (auto& a_udd_tracker_hit : udd_tracker_hits) {
        // For each UDD tracker digitized hit, produce and fill a precalibrated tracker hit

        auto new_pcd_tracker = datatools::make_handle<snemo::datamodel::precalibrated_tracker_hit>();
        // auto& newHit = newHandle.grab();

        new_pcd_tracker->set_hit_id(a_udd_tracker_hit.get().get_hit_id());
        new_pcd_tracker->set_geom_id(a_udd_tracker_hit->get_geom_id());


        // FILL ALL pCD TRACKER TIMES WITH UDD TRACKER TIMESTAMPS
        // How to deal with vector of gg_times ?

        // a_udd_tracker_hit

        // new_pcd_tracker->set_reference_time();
        // new_pcd_tracker->set_anodic_drift_time();
        // new_pcd_tracker->sigma_anodic_drift_time();
        // new_pcd_tracker->set_bottom_cathode_drift_time();
        // new_pcd_tracker->sigma_bottom_cathode_drift_time();
        // new_pcd_tracker->set_top_cathode_drift_time();
        // new_pcd_tracker->sigma_top_cathode_drift_time();
        //
        // new_pcd_tracker->set_delayed();


        tracker_hits_.push_back(new_pcd_tracker);
      }

    }

    void udd2pcd_module::process_tracker_impl(const snemo::datamodel::unified_digitized_data & udd_data_,
                                           snemo::datamodel::PreCalibTrackerHitHdlCollection & tracker_hits_) {
      precalibrate_tracker_hits(udd_data_, tracker_hits_);
    }





  }  // end of namespace processing

}  // end of namespace snemo

/********************************
 * OCD support : implementation *
 ********************************/

#include <datatools/object_configuration_description.h>

/** Opening macro for implementation
 *  @arg snemo::processing::udd2pcd_module the full class name
 *  @arg ocd_ is the identifier of the 'datatools::object_configuration_description'
 *            to be initialized (passed by mutable reference).
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::processing::udd2pcd_module, ocd_) {
  ocd_.set_class_name("snemo::processing::udd2pcd_module");
  ocd_.set_class_description(
                             "A module that performs a precalibration of the unified digitized data (UDD) bank of the calorimeter and tracker data writing in the precalibrated data (pCD) bank");
  ocd_.set_class_library("falaise");
  // ocd_.set_class_documentation("");

  dpp::base_module::common_ocd(ocd_);

  {
    // Description of the 'SD_label' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("UDD_label")
      .set_terse_description("The label/name of the 'unified digitized data' bank")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description(
                            "This is the name of the bank to be used   \n"
                            "as the input unified  calorimeter and tracker hits.  \n")
      .set_default_value_string(snedm::labels::unified_digitized_data())
      .add_example(
                   "Use an alternative name for the 'unified digitized data' bank:: \n"
                   "                                \n"
                   "  UDD_label : string = \"UDD2\" \n"
                   "                                \n");
  }

  {
    // Description of the 'CD_label' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("pCD_label")
      .set_terse_description("The label/name of the 'precalibrated data' bank")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description(
                            "This is the name of the bank to be used    \n"
                            "as the output precalibrated calorimeter and tracker hits. \n")
      .set_default_value_string(snedm::labels::precalibrated_data())
      .add_example(
                   "Use an alternative name for the 'precalibrated data' bank:: \n"
                   "                                  \n"
                   "  pCD_label : string = \"pCD2\"   \n"
                   "                                  \n");
  }

  // Additionnal configuration hints :
  ocd_.set_configuration_hints(
                               "Here is a full configuration example in the \n"
                               "``datatools::properties`` ASCII format::    \n"
                               "                                            \n"
                               "  UDD_label    : string = \"UDD\"           \n"
                               "  pCD_label    : string = \"pCD\"           \n"
                               "                                            \n");

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()  // Closing macro for implementation

// Registration macro for class 'snemo::processing::udd2pcd_module' :
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::processing::udd2pcd_module,
                               "snemo::processing::udd2pcd_module")

// end of falaise/snemo/processing/udd2pcd_module.cc
