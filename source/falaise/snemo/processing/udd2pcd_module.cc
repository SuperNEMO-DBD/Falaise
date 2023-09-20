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
#include <falaise/snemo/datamodels/unified_digitized_data.h>
#include <falaise/snemo/datamodels/precalibrated_data.h>
#include <falaise/snemo/services/services.h>

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

      _calo_adc2volt_ = fps.get<double>("calo_adc2volt", 2.5/4096.0) * CLHEP::volt;

      double _calo_sampling_frequency_ = fps.get<double>("calo_sampling_frequency_ghz", 2.56) * 1E9*CLHEP::hertz;
      _calo_sampling_period_ = 1.0/_calo_sampling_frequency_;

      _calo_postrigger_time_ = fps.get<double>("calo_postrigger_ns", 250) * CLHEP::ns;

      // _udd2pcd_calo_method_ = fps.get<std::string>("calo_method", "fwmeas")
      // _udd2pcd_tracker_method_ = fps.get<std::string>("tracker_method", "fwmeas")

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

      // Check if some 'pcd_data' are available in the data model:
      // Precalibrated Data is a single object with each hit collection
      // May, or may not, have it depending on if we run before or after
      // other calibrators
      auto& pcd_data = snedm::getOrAddToEvent<snemo::datamodel::precalibrated_data>(_pcd_output_tag_, event);

      // Always rewrite calorimeter hits
      pcd_data.calorimeter_hits().clear();

      // Always rewrite tracker hits
      pcd_data.tracker_hits().clear();

      // Main calorimeter processing method :
      process_calo_impl(udd_data, pcd_data);

      // Main tracker processing method :
      process_tracker_impl(udd_data, pcd_data);

      return dpp::base_module::PROCESS_SUCCESS;
    }

    // Precalibrate calorimeter hits from UDD informations:
    void udd2pcd_module::precalibrate_calo_hits_fwmeas(const snemo::datamodel::unified_digitized_data & udd_data_,
						       snemo::datamodel::PreCalibCalorimeterHitHdlCollection & calo_hits_) {

      // Constants
      const double CALO_ADC2VOLT = _calo_adc2volt_;
      const double CALO_SAMPLING_PERIOD = _calo_sampling_period_;
      const double CALO_POSTRIGGER_TIME = _calo_postrigger_time_;
      const double CALO_TIME_WINDOW = 1024 * CALO_SAMPLING_PERIOD;

      // Retrieve UDD calorimeter digitized hits
      const auto& udd_calo_hits = udd_data_.get_calorimeter_hits();

      // Loop over UDD calorimeter digitized hits
      for (auto& a_udd_calo_hit : udd_calo_hits) {

	// // Retrieve sampling period and postrigger parameters from properties
	// const datatools::properties & udd_calo_hit_properties = a_udd_calo_hit->get_auxiliaries();

        // For each UDD calorimeter digitized hit, produce and fill a precalibrated calorimeter hit
        auto new_pcd_calo = datatools::make_handle<snemo::datamodel::precalibrated_calorimeter_hit>();
        // auto& newHit = newHandle.grab();

        new_pcd_calo->set_hit_id(a_udd_calo_hit->get_hit_id());
        new_pcd_calo->set_geom_id(a_udd_calo_hit->get_geom_id());

        // const double fwmeas_baseline_adc = (2048.0 + a_udd_calo_hit->get_fwmeas_baseline()/16.0);
	const double fwmeas_baseline = (a_udd_calo_hit->get_fwmeas_baseline()/16.0) * CALO_ADC2VOLT;
        new_pcd_calo->set_baseline(fwmeas_baseline);

        // const double fwmeas_amplitude_adc = a_udd_calo_hit->get_fwmeas_peak_amplitude()/8.0;
        const double fwmeas_amplitude = (a_udd_calo_hit->get_fwmeas_peak_amplitude()/8.0) * CALO_ADC2VOLT;
        new_pcd_calo->set_amplitude(fwmeas_amplitude);

        const double fwmeas_charge = a_udd_calo_hit->get_fwmeas_charge() * CALO_ADC2VOLT * CALO_SAMPLING_PERIOD;
        new_pcd_calo->set_charge(fwmeas_charge);

	const double time_tdc = a_udd_calo_hit->get_timestamp() * 6.25 * CLHEP::ns;
	const double fwmeas_falling_time_cfd = (a_udd_calo_hit->get_fwmeas_falling_cell()/256.0) * CALO_SAMPLING_PERIOD;
	const double fwmeas_time = time_tdc - CALO_TIME_WINDOW + CALO_POSTRIGGER_TIME + fwmeas_falling_time_cfd;
	new_pcd_calo->set_time(fwmeas_time);

        // Add additional information in pCD calo datatools::properties
	datatools::properties& pcd_calo_hit_properties = new_pcd_calo->grab_auxiliaries();

        pcd_calo_hit_properties.store("time_cfd", fwmeas_falling_time_cfd);

	const double fwmeas_rising_time_cfd = (a_udd_calo_hit->get_fwmeas_rising_cell()/256.0) * CALO_SAMPLING_PERIOD;
	const double fwmes_width = fwmeas_rising_time_cfd - fwmeas_falling_time_cfd;
        pcd_calo_hit_properties.store("pulse_width", fwmes_width);

        calo_hits_.push_back(new_pcd_calo);
      }
    }

    void udd2pcd_module::process_calo_impl(const snemo::datamodel::unified_digitized_data & udd_data_,
                                           snemo::datamodel::precalibrated_data & pcd_data_) {

      precalibrate_calo_hits_fwmeas(udd_data_, pcd_data_.calorimeter_hits());

    }


    // Precalibrate tracker hits from UDD informations:
    void udd2pcd_module::initialize_precalibrated_tracker_hits(const snemo::datamodel::unified_digitized_data & udd_data_,
							       snemo::datamodel::PreCalibTrackerHitHdlCollection & tracker_hits_) {

      // Constants
      const double TRACKER_TDC_TICK = 12.5 * CLHEP::nanosecond;

      // Retrieve UDD tracker digitized hits
      const auto& udd_tracker_hits = udd_data_.get_tracker_hits();

      // Loop over UDD tracker digitized hits
      for (auto& a_udd_tracker_hit : udd_tracker_hits) {

	// Loop over all gg_times and keep trace of the earliest R0, R5 and R6
	int64_t first_anode_timestamp = std::numeric_limits<int64_t>::max();
	int64_t first_bottom_cathode_timestamp = std::numeric_limits<int64_t>::max();
	int64_t first_top_cathode_timestamp = std::numeric_limits<int64_t>::max();

	for (auto& udd_tracker_times : a_udd_tracker_hit->get_times()) {

	  const uint16_t & ANODE_R0 = snemo::datamodel::tracker_digitized_hit::ANODE_R0;

	  if (udd_tracker_times.has_anode_time(ANODE_R0)) {
	    const int64_t& anode_timestamp = udd_tracker_times.get_anode_time(ANODE_R0);
	    if (anode_timestamp < first_anode_timestamp)
	      first_anode_timestamp = anode_timestamp;
	  }
	  
	  if (udd_tracker_times.has_bottom_cathode_time()) {
	    const int64_t& bottom_cathode_timestamp = udd_tracker_times.get_bottom_cathode_time();
	    if (bottom_cathode_timestamp < first_bottom_cathode_timestamp)
	      first_bottom_cathode_timestamp = bottom_cathode_timestamp;
	  }

	  if (udd_tracker_times.has_top_cathode_time()) {
	    const int64_t& top_cathode_timestamp = udd_tracker_times.get_top_cathode_time();
	    if (top_cathode_timestamp < first_top_cathode_timestamp)
	      first_top_cathode_timestamp = top_cathode_timestamp;
	  }

	}

	// Skip the UDD tracker hit without anode timestamp
	if (first_anode_timestamp == std::numeric_limits<int64_t>::max())
	  continue;

        auto new_pcd_tracker = datatools::make_handle<snemo::datamodel::precalibrated_tracker_hit>();

        new_pcd_tracker->set_hit_id(a_udd_tracker_hit->get_hit_id());
        new_pcd_tracker->set_geom_id(a_udd_tracker_hit->get_geom_id());

	// Convert and fill the earliest R0 timestamp
	const double first_anode_time = first_anode_timestamp * TRACKER_TDC_TICK;
	new_pcd_tracker->set_anodic_time(first_anode_time);

	// Convert and fill the earliest bottom cathode timestamp (R5)
	if (first_bottom_cathode_timestamp != std::numeric_limits<int64_t>::max()) {
	  const double first_bottom_cathode_time = first_bottom_cathode_timestamp * TRACKER_TDC_TICK;
	  new_pcd_tracker->set_bottom_cathode_drift_time(first_bottom_cathode_time-first_anode_time);
	}

	// Convert and fill the earliest top cathode timestamp (R6)
	if (first_top_cathode_timestamp != std::numeric_limits<int64_t>::max()) {
	  const double first_top_cathode_time = first_top_cathode_timestamp * TRACKER_TDC_TICK;
	  new_pcd_tracker->set_top_cathode_drift_time(first_top_cathode_time-first_anode_time);
	}

        tracker_hits_.push_back(new_pcd_tracker);
      }

    }

    // Clusterize precalibrate tracker hits from UDD informations:
    void udd2pcd_module::clusterize_and_precalibrate_tracker_hits(snemo::datamodel::precalibrated_data & pcd_data_) {

      const double tracker_clusterization_radius_threshold = 3.0; // unit in cell_size
      const double tracker_clusterization_radius2_threshold = tracker_clusterization_radius_threshold*tracker_clusterization_radius_threshold;
      const double tracker_clusterization_deltat_threshold = 15E-6 * CLHEP::second;

      // std::vector<int> pcd_tracker_hit_indexes;
      std::vector<std::vector<int>> pcd_tracker_hit_clusters;

      // Retrieve pCD tracker hits
      const auto& pcd_tracker_hits = pcd_data_.tracker_hits();

      // Iterate over all tracker hits
      for (size_t pcd_tracker_hit_index=0; pcd_tracker_hit_index<pcd_tracker_hits.size(); pcd_tracker_hit_index++) {

	auto & pcd_tracker_hit = pcd_tracker_hits.at(pcd_tracker_hit_index).get();

	// Storage of cluster indexe(s) in which at least 1 tracker hit
	// will match the time/space neighboring criteria with pcd_tracker_hit
	// (if more than 1 cluster is there, they will have to be merged!)
	std::vector<size_t> matching_cluster_indexes;

	// Retrieve time/space data
	const double & pcd_tracker_anode_time = pcd_tracker_hit.get_anodic_time();
	const geomtools::geom_id & pcd_tracker_geomid = pcd_tracker_hit.get_geom_id();
	const uint32_t & pcd_tracker_side  = pcd_tracker_geomid.get(1);
	const uint32_t & pcd_tracker_layer = pcd_tracker_geomid.get(2);
	const uint32_t & pcd_tracker_row   = pcd_tracker_geomid.get(3);

	// Iterate over all existing cluster
	for (size_t cluster_index=0; cluster_index<pcd_tracker_hit_clusters.size(); cluster_index++) {

	  // Retrieve the vector of tracker hit indexes
	  const std::vector<int> & cluster_pcd_tracker_hit_indexes = pcd_tracker_hit_clusters.at(cluster_index);

	  // Iterate over all cells from the current cluster
	  for (const int & cluster_pcd_tracker_hit_index : cluster_pcd_tracker_hit_indexes) {

	    const auto & cluster_pcd_tracker_hit = pcd_tracker_hits.at(cluster_pcd_tracker_hit_index).get();

	    // Retrieve time/space data
	    const double & cluster_pcd_tracker_anode_time = cluster_pcd_tracker_hit.get_anodic_time();
	    const geomtools::geom_id & cluster_pcd_tracker_geomid = cluster_pcd_tracker_hit.get_geom_id();
	    const uint32_t & cluster_pcd_tracker_side  = cluster_pcd_tracker_geomid.get(1);
	    const uint32_t & cluster_pcd_tracker_layer = cluster_pcd_tracker_geomid.get(2);
	    const uint32_t & cluster_pcd_tracker_row   = cluster_pcd_tracker_geomid.get(3);

	    // Now perform time/space comparison of pcd_tracker_hit with cluster_pcd_tracker_hit

	    // 1) skip if tracker hits are not on the same side
	    if (pcd_tracker_side != cluster_pcd_tracker_side)
	      continue;

	    // 2) compute and perform space correlation
	    const int delta_row = cluster_pcd_tracker_row - pcd_tracker_row;
	    const int delta_layer = cluster_pcd_tracker_layer - pcd_tracker_layer;
	    const float delta_radius = delta_row*delta_row + delta_layer*delta_layer;

	    if (delta_radius > tracker_clusterization_radius2_threshold)
	      continue;

	    // 3) compute and perform time correlation
	    const double delta_anode_time = cluster_pcd_tracker_anode_time - pcd_tracker_anode_time;

	    if (abs(delta_anode_time) > tracker_clusterization_deltat_threshold)
	      continue;

	    // if we reach this part, pcd_tracker_hit and cluster_pcd_tracker_hit
	    // are considered as neighbor: pcd_tracker_hit will be add in the cluster
	    matching_cluster_indexes.push_back(cluster_index);

	    break;

	  } // for (cluster_pcd_tracker_hit_index)

	} // for (cluster_index)


	// if pcd_tracker_hit does not matching any cluster, we will
	// create a new cluster and fill it with the index of the hit
	if (matching_cluster_indexes.size() == 0) {
	  std::vector<int> new_pcd_tracker_hit_cluster;
	  new_pcd_tracker_hit_cluster.push_back(pcd_tracker_hit_index);
	  pcd_tracker_hit_clusters.push_back(new_pcd_tracker_hit_cluster);
        }

	// if pcd_tracker_hit is matching only 1 single cluster,
	// we just add the index of the tracker hit in the cluster
	else if (matching_cluster_indexes.size() == 1) {
	  const size_t matching_cluster_index = matching_cluster_indexes.front();
	  pcd_tracker_hit_clusters[matching_cluster_index].push_back(pcd_tracker_hit_index);
	}

	// if pcd_tracker_hit is matching several cluster,
	// we add the index of the tracker hit in the first
	// matching cluster, then we merge (and erase) all
	// other clusters into the first one
	else {
	  const size_t matching_cluster_index = matching_cluster_indexes.front();
	  pcd_tracker_hit_clusters[matching_cluster_index].push_back(pcd_tracker_hit_index);

          for (size_t cluster_i=matching_cluster_indexes.size()-1; cluster_i>0; --cluster_i) {

	    const size_t & cluster_index = matching_cluster_indexes[cluster_i];

	    for (const int & tracker_index : pcd_tracker_hit_clusters[cluster_index])
	      pcd_tracker_hit_clusters[matching_cluster_index].push_back(tracker_index);

	    pcd_tracker_hit_clusters.erase(pcd_tracker_hit_clusters.begin() + cluster_index);
	  }

	}

      } // for (pcd_tracker_hit_index)

      // Now that the clusterization is performed, analyse anodic time range
      // and perform an association with calorimeter hits time [TODO]

    }

    void udd2pcd_module::process_tracker_impl(const snemo::datamodel::unified_digitized_data & udd_data_,
					      snemo::datamodel::precalibrated_data & pcd_data_) {

      initialize_precalibrated_tracker_hits(udd_data_, pcd_data_.tracker_hits());

      clusterize_and_precalibrate_tracker_hits(pcd_data_);
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
