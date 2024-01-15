// -*- mode: c++ ; -*-
/** \file falaise/snemo/processing/udd2pcd_module.cc
 */

// Ourselves:
#include "udd2pcd_module.h"

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

// This project:
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

      // Retrieve calorimeter pre-calibration configuration
      _calo_adc2volt_ = fps.get<double>("calo_adc2volt", 2.5/4096.0) * CLHEP::volt;
      double _calo_sampling_frequency_ = fps.get<double>("calo_sampling_frequency_ghz", 2.56) * 1E9*CLHEP::hertz;
      _calo_sampling_period_ = 1.0/_calo_sampling_frequency_; // fps.get<double>("calo_sampling_period_ns", 0.390625) * 1E-9*CLHEP::second;
      _calo_postrigger_time_ = fps.get<double>("calo_postrigger_ns", 250) * CLHEP::ns;
      _calo_baseline_nsamples_ = fps.get<int>("calo_baseline_nsamples", 16);
      _calo_charge_integration_nsamples_ = fps.get<int>("calo_charge_integration_nsamples", 992);
      _calo_charge_integration_nsamples_before_peak_ = fps.get<int>("calo_charge_integration_nsamples_before_peak", 64);
      _calo_time_cfd_ratio_ = fps.get<double>("calo_time_cfd_ratio", 4./16.);
      _calo_discard_empty_waveform_ = fps.get<bool>("calo_discard_empty_waveform", false);

      std::string udd2pcd_calo_method_label = fps.get<std::string>("calo_method", "fwmeas");
      if (udd2pcd_calo_method_label == "fwmeas") {
	_calo_pcd_algo_ = ALGO_CALO_FWMEASUREMENT;
      } else if (udd2pcd_calo_method_label == "swmeas") {
	_calo_pcd_algo_ = ALGO_CALO_SWMEASUREMENT;
      } else {
	DT_LOG_WARNING(get_logging_priority(), "udd2pcd calo method '" << udd2pcd_calo_method_label << "' not recognised");
	_calo_pcd_algo_ = ALGO_CALO_FWMEASUREMENT;
      }

      std::string udd2pcd_tracker_method_label = fps.get<std::string>("tracker_method", "earliset");
      if (udd2pcd_tracker_method_label == "earliest") {
	_tracker_pcd_algo_ = ALGO_TRACKER_EARLIEST;
      } else {
	DT_LOG_WARNING(get_logging_priority(), "udd2pcd tracker method '" << udd2pcd_tracker_method_label << "' not recognised");
	_tracker_pcd_algo_ = ALGO_TRACKER_EARLIEST;
      }

      DT_LOG_NOTICE(get_logging_priority(), "\ncalorimeter udd2pcd firmware measurement configuration:\n"
		    << "- adc2volt          = " << _calo_adc2volt_/(1E-3*CLHEP::volt) << " mV/tick\n"
		    << "- sampling period   = " << _calo_sampling_period_/(CLHEP::nanosecond) << " ns\n"
		    << "- post trigger time = " << _calo_postrigger_time_/(CLHEP::nanosecond) << " ns\n"
		    << "- baseline nsamples = " << _calo_baseline_nsamples_ << "\n"
		    << "- charge integration nsamples = " << _calo_charge_integration_nsamples_ << "\n"
		    << "- pre-charge nsamples         = " << _calo_charge_integration_nsamples_before_peak_ << "\n"
		    << "- falling time cfd ratio      = " << _calo_time_cfd_ratio_);

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
      DT_LOG_NOTICE(get_logging_priority(), "processing event #" << udd_data.get_event_id());

      // Check if some 'pcd_data' are available in the data model:
      // Precalibrated Data is a single object with each hit collection
      // may, or may not, have it depending on if we run before or after
      // other calibrators
      auto& pcd_data = snedm::getOrAddToEvent<snemo::datamodel::precalibrated_data>(_pcd_output_tag_, event);

      // Always rewrite calorimeter hits
      pcd_data.calorimeter_hits().clear();

      // Always rewrite tracker hits
      pcd_data.tracker_hits().clear();

      // Main calorimeter processing method
      process_calo_impl(udd_data, pcd_data);

      // Main tracker processing method
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
      const double CALO_TIME_WINDOW = 1024 * _calo_sampling_period_;

      // Retrieve UDD calorimeter digitized hits
      const auto& udd_calo_hits = udd_data_.get_calorimeter_hits();

      // Loop over UDD calorimeter digitized hits
      for (auto& a_udd_calo_hit : udd_calo_hits) {

	// Discard empty waveforms
	if (_calo_discard_empty_waveform_) {

	  bool discard_calo_hit = true;

	  // Keep hit with HT flag
	  if (a_udd_calo_hit->is_high_threshold())
	    discard_calo_hit = false;

	  // Keep hit with LT only flag
	  else if (a_udd_calo_hit->is_low_threshold_only())
	    discard_calo_hit = false;

	  // Keep hit with signal >10mV : It (rarely) happens to get signal without HT/LT.
	  // Usually such calorimeter hit was not part of the trigger decision (random coinc.)
	  // and it won't be usefull for physics, but let's keep it in the event just in case..
	  else if (((a_udd_calo_hit->get_fwmeas_peak_amplitude()/8.0) * CALO_ADC2VOLT) > (10E-3 *CLHEP::volt))
	    discard_calo_hit = false;

	  if (discard_calo_hit)
	    continue;

	}

	DT_LOG_WARNING(get_logging_priority(), "Precalibrating calo hit from " << a_udd_calo_hit->get_geom_id());

        // Produce a new precalibrated calorimeter hit
        auto new_pcd_calo = datatools::make_handle<snemo::datamodel::precalibrated_calorimeter_hit>();

	// Keep same hit number for calorimeter's digitized hit and precalibrated hit
        new_pcd_calo->set_hit_id(a_udd_calo_hit->get_hit_id());
        new_pcd_calo->set_geom_id(a_udd_calo_hit->get_geom_id());

	// Retrieve fwmeas digital data from UDD calorimeter hit
	const int16_t & fwmeas_baseline_d  = a_udd_calo_hit->get_fwmeas_baseline();
	const int16_t & fwmeas_amplitude_d = a_udd_calo_hit->get_fwmeas_peak_amplitude();
	const int32_t & fwmeas_charge_d    = a_udd_calo_hit->get_fwmeas_charge();
	const int32_t & fwmeas_falling_time_cfd_d = a_udd_calo_hit->get_fwmeas_falling_cell();
	const int32_t & fwmeas_rising_time_cfd_d = a_udd_calo_hit->get_fwmeas_rising_cell();

	// Convert/calibrate digital data
	const double fwmeas_baseline  = (fwmeas_baseline_d/16.0) * CALO_ADC2VOLT;
        const double fwmeas_amplitude = (fwmeas_amplitude_d/8.0) * CALO_ADC2VOLT;
        const double fwmeas_charge    = fwmeas_charge_d * CALO_ADC2VOLT * CALO_SAMPLING_PERIOD;
	const double fwmeas_time_cfd  = (fwmeas_falling_time_cfd_d/256.0) * CALO_SAMPLING_PERIOD;
	const double time_tdc = a_udd_calo_hit->get_timestamp() * 6.25 * CLHEP::ns;
	const double fwmeas_time = time_tdc - CALO_TIME_WINDOW + CALO_POSTRIGGER_TIME + fwmeas_time_cfd;

	// Store pre-calibrated data into pCD hit
        new_pcd_calo->set_baseline(fwmeas_baseline);
        new_pcd_calo->set_amplitude(fwmeas_amplitude);
        new_pcd_calo->set_charge(fwmeas_charge);
	new_pcd_calo->set_time(fwmeas_time);

	// Additional information into pCD calo datatools::properties
	datatools::properties& pcd_calo_hit_properties = new_pcd_calo->grab_auxiliaries();

	// -> store the time cfd in record window
        pcd_calo_hit_properties.store("time_cfd_ns", fwmeas_time_cfd/CLHEP::ns);

	// -> compute and store the pulse width
	const double fwmeas_rising_time_cfd = (fwmeas_rising_time_cfd_d/256.0) * CALO_SAMPLING_PERIOD;
	const double fwmes_width = fwmeas_rising_time_cfd - fwmeas_time_cfd;
        pcd_calo_hit_properties.store("pulse_width_ns", fwmes_width/CLHEP::ns);

	// Append the new pCD calorimeter hit
        calo_hits_.push_back(new_pcd_calo);

	if (datatools::logger::is_debug(get_logging_priority()))
	  new_pcd_calo->tree_dump(std::clog);
      }
    }

    // Precalibrate calorimeter hits from UDD informations:
    void udd2pcd_module::precalibrate_calo_hits_swmeas(const snemo::datamodel::unified_digitized_data & udd_data_,
						       snemo::datamodel::PreCalibCalorimeterHitHdlCollection & calo_hits_) {

      // Constants
      const double CALO_ADC2VOLT = _calo_adc2volt_;
      const double CALO_SAMPLING_PERIOD = _calo_sampling_period_;
      const double CALO_POSTRIGGER_TIME = _calo_postrigger_time_;
      const double CALO_TIME_WINDOW = 1024 * _calo_sampling_period_;

      // Retrieve UDD calorimeter digitized hits
      const auto& udd_calo_hits = udd_data_.get_calorimeter_hits();

      // Loop over UDD calorimeter digitized hits
      for (auto& a_udd_calo_hit : udd_calo_hits) {

	// Discard empty waveforms
	if (_calo_discard_empty_waveform_) {

	  bool discard_calo_hit = true;

	  // Keep hit with HT flag
	  if (a_udd_calo_hit->is_high_threshold())
	    discard_calo_hit = false;
	  // Keep hit with LT only flag
	  else if (a_udd_calo_hit->is_low_threshold_only())
	    discard_calo_hit = false;
	  // Keep hit with signal >10mV : It (rarely) happens to get signal without HT/LT !
	  // Usually such calorimeter hit was not part of the trigger decision (random coinc.)
	  // and it won't be usefull for physics, but let's keep it in the event just in case..
	  else if (((a_udd_calo_hit->get_fwmeas_peak_amplitude()/8.0) * CALO_ADC2VOLT) > (10E-3 *CLHEP::volt))
	    discard_calo_hit = false;

	  if (discard_calo_hit)
	    continue;
	}

        // Produce a new precalibrated calorimeter hit
        auto new_pcd_calo = datatools::make_handle<snemo::datamodel::precalibrated_calorimeter_hit>();

	// Keep same hit number for calorimeter's digitized hit and precalibrated hit
        new_pcd_calo->set_hit_id(a_udd_calo_hit->get_hit_id());
        new_pcd_calo->set_geom_id(a_udd_calo_hit->get_geom_id());

	const std::vector<int16_t> & a_udd_calo_waveform = a_udd_calo_hit->get_waveform();

	// Baseline
	double swmeas_baseline_sum=0, swmeas_baseline_sum2=0;
	for (int sample=0; sample<_calo_baseline_nsamples_; sample++) {
	  const int16_t & waveform_sample = a_udd_calo_waveform[sample];
	  swmeas_baseline_sum += waveform_sample;
	  swmeas_baseline_sum2 += waveform_sample*waveform_sample;
	}

	const double swmeas_baseline_adc_mean = swmeas_baseline_sum/_calo_baseline_nsamples_;
	const double swmeas_baseline_adc_sigma2 = swmeas_baseline_sum2/_calo_baseline_nsamples_ - swmeas_baseline_adc_mean*swmeas_baseline_adc_mean;
	const double swmeas_baseline_value = (swmeas_baseline_adc_mean - 2048) * CALO_ADC2VOLT;
	const double swmeas_baseline_error = sqrt(swmeas_baseline_adc_sigma2) * CALO_ADC2VOLT;
        new_pcd_calo->set_baseline(swmeas_baseline_value);
	new_pcd_calo->set_sigma_baseline(swmeas_baseline_error);

	// Amplitude
	int16_t min_amplitude_adc = INT16_MAX;
	int16_t min_amplitude_sample = 0;

	const int16_t nsamples = a_udd_calo_waveform.size();

	for (int16_t sample=0; sample<nsamples; sample++) {
	  const int16_t & waveform_sample = a_udd_calo_waveform[sample];
	  if (waveform_sample < min_amplitude_adc) {
	    min_amplitude_adc = waveform_sample;
	    min_amplitude_sample = sample;
	  }
	}

	const double swmeas_amplitude_adc = min_amplitude_adc - swmeas_baseline_adc_mean;
	const double swmeas_amplitude_adc_sigma2 = 0.5*0.5 + swmeas_baseline_adc_sigma2;
	const double swmeas_amplitude_value = swmeas_amplitude_adc * CALO_ADC2VOLT;
	const double swmeas_amplitude_error = sqrt(swmeas_amplitude_adc_sigma2) * CALO_ADC2VOLT;
        new_pcd_calo->set_amplitude(swmeas_amplitude_value);
	new_pcd_calo->set_sigma_amplitude(swmeas_amplitude_error);

	// Charge
	int16_t charge_sample_start = min_amplitude_sample - _calo_charge_integration_nsamples_before_peak_;
	if (charge_sample_start < 0) charge_sample_start = 0;
	int16_t charge_sample_stop = charge_sample_start + _calo_charge_integration_nsamples_;
	if (charge_sample_stop > nsamples) charge_sample_stop = nsamples;

	double swmeas_charge_sum = 0;
	for (int16_t sample=charge_sample_start; sample<charge_sample_stop; sample++) {
	  const int16_t & waveform_sample = a_udd_calo_waveform[sample];
	  swmeas_charge_sum += waveform_sample;
	}

	swmeas_charge_sum -= (charge_sample_stop-charge_sample_start)*swmeas_baseline_adc_mean;

	const double swmeas_charge = swmeas_charge_sum * CALO_ADC2VOLT * CALO_SAMPLING_PERIOD;
	const double swmeas_charge_sigma2 = (0.5*0.5 + swmeas_baseline_adc_sigma2)*(charge_sample_stop-charge_sample_start);
	const double swmeas_charge_error = sqrt(swmeas_charge_sigma2) * CALO_ADC2VOLT * CALO_SAMPLING_PERIOD;
        new_pcd_calo->set_charge(swmeas_charge);
	new_pcd_calo->set_sigma_charge(swmeas_charge_error);

	// Time CFD
	const double cfd_threshold_adc = swmeas_baseline_adc_mean + _calo_time_cfd_ratio_*swmeas_amplitude_adc;

	// Look for waveform sample crossing threshold (front side of the pulse)
	int16_t sample_i = 0;
	for (sample_i=min_amplitude_sample ; sample_i>0; sample_i--) {
	  const int16_t & waveform_sample = a_udd_calo_waveform[sample_i];
	  if (waveform_sample >= cfd_threshold_adc)
	    break;
	}

	// Interpolate with pol1 the two consecutives samples to retrieve the falling time
	double swmeas_falling_time_cfd_sample = 0;
	if (sample_i != (nsamples-2)) {
	  const double sample_x1 = sample_i;
	  const double sample_x2 = sample_i+1;
	  const double sample_y1 = a_udd_calo_waveform[sample_x1];
	  const double sample_y2 = a_udd_calo_waveform[sample_x2];
	  const double pol1_a = (sample_y2-sample_y1)/(sample_x2-sample_x1);
	  const double pol1_b = sample_y1-pol1_a*sample_x1;
	  swmeas_falling_time_cfd_sample = (cfd_threshold_adc-pol1_b)/pol1_a;
	}

	const double time_tdc = a_udd_calo_hit->get_timestamp() * 6.25 * CLHEP::ns;
	const double swmeas_falling_time_cfd = swmeas_falling_time_cfd_sample * CALO_SAMPLING_PERIOD;
	const double swmeas_falling_time = time_tdc - CALO_TIME_WINDOW + CALO_POSTRIGGER_TIME + swmeas_falling_time_cfd;
	new_pcd_calo->set_time(swmeas_falling_time);
	// new_pcd_calo->set_sigma_time();

	// Look for waveform sample crossing threshold (back side of the pulse)
	for (sample_i=min_amplitude_sample ; sample_i<1024; sample_i++) {
	  const int16_t & waveform_sample = a_udd_calo_waveform[sample_i];
	  if (waveform_sample >= cfd_threshold_adc)
	    break;
	}

	// Interpolate with pol1 the two consecutives samples to retrieve the rising time
	double swmeas_rising_time_cfd_sample = 0;
	if (sample_i < (1024-2)) {
	  const double sample_x1 = sample_i-1;
	  const double sample_x2 = sample_i;
	  const double sample_y1 = a_udd_calo_waveform[sample_x1];
	  const double sample_y2 = a_udd_calo_waveform[sample_x2];
	  const double pol1_a = (sample_y2-sample_y1)/(sample_x2-sample_x1);
	  const double pol1_b = sample_y1-pol1_a*sample_x1;
	  swmeas_rising_time_cfd_sample = (cfd_threshold_adc-pol1_b)/pol1_a;
	}

	// Additional information into pCD calo datatools::properties
	datatools::properties& pcd_calo_hit_properties = new_pcd_calo->grab_auxiliaries();

	// -> store the time cfd in record window
        pcd_calo_hit_properties.store("time_cfd_ns", swmeas_falling_time_cfd/CLHEP::ns);

	// -> compute and store the pulse width
	const double swmeas_rising_time_cfd = swmeas_rising_time_cfd_sample * CALO_SAMPLING_PERIOD;
	const double swmes_width = swmeas_rising_time_cfd - swmeas_falling_time_cfd;
        pcd_calo_hit_properties.store("pulse_width_ns", swmes_width/CLHEP::ns);

	// Append the new pCD calorimeter hit
        calo_hits_.push_back(new_pcd_calo);

	if (datatools::logger::is_debug(get_logging_priority()))
	  new_pcd_calo->tree_dump(std::clog);
      }
    }

    void udd2pcd_module::process_calo_impl(const snemo::datamodel::unified_digitized_data & udd_data_,
                                           snemo::datamodel::precalibrated_data & pcd_data_) {

      if (_calo_pcd_algo_ == ALGO_CALO_FWMEASUREMENT)
	precalibrate_calo_hits_fwmeas(udd_data_, pcd_data_.calorimeter_hits());

      else if (_calo_pcd_algo_ == ALGO_CALO_SWMEASUREMENT)
	precalibrate_calo_hits_swmeas(udd_data_, pcd_data_.calorimeter_hits());

    }


    // Precalibrate tracker hits from UDD informations:
    void udd2pcd_module::precalibrate_tracker_hits_earliest(const snemo::datamodel::unified_digitized_data & udd_data_,
							    snemo::datamodel::PreCalibTrackerHitHdlCollection & tracker_hits_) {
      // Constants
      const double TRACKER_TDC_TICK = 12.5 * CLHEP::nanosecond;

      // Retrieve UDD tracker digitized hits
      const auto& udd_tracker_hits = udd_data_.get_tracker_hits();

      // Loop over UDD tracker digitized hits
      for (auto& a_udd_tracker_hit : udd_tracker_hits) {

	// Loop over all gg_times and keep trace of the earliest timestamps
	int64_t first_anode_timestamp[5];
	std::fill(std::begin(first_anode_timestamp), std::end(first_anode_timestamp), std::numeric_limits<int64_t>::max());
	int64_t first_bottom_cathode_timestamp = std::numeric_limits<int64_t>::max();
	int64_t first_top_cathode_timestamp = std::numeric_limits<int64_t>::max();

	for (auto& udd_tracker_times : a_udd_tracker_hit->get_times()) {

	  for (int ANODE_Ri=0; ANODE_Ri<5; ANODE_Ri++) {
	    if (udd_tracker_times.has_anode_time(ANODE_Ri)) {
	      const int64_t & anode_timestamp = udd_tracker_times.get_anode_time(ANODE_Ri);
	      if (anode_timestamp != 0) // bug from RED/UDD bridge (to fix)
		{
		  if (anode_timestamp < first_anode_timestamp[ANODE_Ri])
		    first_anode_timestamp[ANODE_Ri] = anode_timestamp;
		}
	    }
	  }
	  
	  if (udd_tracker_times.has_bottom_cathode_time()) {
	    const int64_t & bottom_cathode_timestamp = udd_tracker_times.get_bottom_cathode_time();
	    if (bottom_cathode_timestamp < first_bottom_cathode_timestamp)
	      first_bottom_cathode_timestamp = bottom_cathode_timestamp;
	  }

	  if (udd_tracker_times.has_top_cathode_time()) {
	    const int64_t & top_cathode_timestamp = udd_tracker_times.get_top_cathode_time();
	    if (top_cathode_timestamp < first_top_cathode_timestamp)
	      first_top_cathode_timestamp = top_cathode_timestamp;
	  }

	}

	// Skip the UDD tracker hit without anode R0 timestamp
	if (first_anode_timestamp[0] == std::numeric_limits<int64_t>::max())
	  continue;

        auto new_pcd_tracker = datatools::make_handle<snemo::datamodel::precalibrated_tracker_hit>();

	// Keep same hit number for tracker's digitized hit and precalibrated hit
        new_pcd_tracker->set_hit_id(a_udd_tracker_hit->get_hit_id());
        new_pcd_tracker->set_geom_id(a_udd_tracker_hit->get_geom_id());

	// Convert and fill the earliest R0 timestamp
	const double first_anode_time = first_anode_timestamp[0] * TRACKER_TDC_TICK;
	new_pcd_tracker->set_anodic_time(first_anode_time);
	new_pcd_tracker->set_sigma_anodic_time(0.5*TRACKER_TDC_TICK);

	// Convert and fill the earliest bottom cathode timestamp (R5)
	if (first_bottom_cathode_timestamp != std::numeric_limits<int64_t>::max()) {
	  const double first_bottom_cathode_time = first_bottom_cathode_timestamp * TRACKER_TDC_TICK;
	  new_pcd_tracker->set_bottom_cathode_drift_time(first_bottom_cathode_time-first_anode_time);
	  new_pcd_tracker->set_sigma_bottom_cathode_drift_time(0.5*sqrt(2)*TRACKER_TDC_TICK);
	}

	// Convert and fill the earliest top cathode timestamp (R6)
	if (first_top_cathode_timestamp != std::numeric_limits<int64_t>::max()) {
	  const double first_top_cathode_time = first_top_cathode_timestamp * TRACKER_TDC_TICK;
	  new_pcd_tracker->set_top_cathode_drift_time(first_top_cathode_time-first_anode_time);
	  new_pcd_tracker->set_sigma_top_cathode_drift_time(0.5*sqrt(2)*TRACKER_TDC_TICK);
	}

	// Convert and fill the earliest R[1-4] timestamp in properties
	for (int ANODE_Ri=1; ANODE_Ri<5; ANODE_Ri++) {
	  if (first_anode_timestamp[ANODE_Ri] != std::numeric_limits<int64_t>::max()) {
	    const double first_anode_ri_time = first_anode_timestamp[ANODE_Ri] * TRACKER_TDC_TICK;
	    std::string ri_key = "R" + std::to_string(ANODE_Ri) + "_us";
	    new_pcd_tracker->grab_auxiliaries().store_real(ri_key, (first_anode_ri_time-first_anode_time)/(CLHEP::microsecond));
	    // std::string sigma_ri_key = "sigma_R" + std::to_string(ANODE_Ri) + "_drift_time";
	    // new_pcd_tracker->grab_auxiliaries().store_real(sigma_ri_key, 0.5*sqrt(2)*TRACKER_TDC_TICK);
	  }
	}

	// Append the new pCD tracker hit
        tracker_hits_.push_back(new_pcd_tracker);

	if (datatools::logger::is_debug(get_logging_priority()))
	  new_pcd_tracker->tree_dump(std::clog);
      }
    }

    void udd2pcd_module::process_tracker_impl(const snemo::datamodel::unified_digitized_data & udd_data_,
					      snemo::datamodel::precalibrated_data & pcd_data_) {

      if (_tracker_pcd_algo_ == ALGO_TRACKER_EARLIEST)
	precalibrate_tracker_hits_earliest(udd_data_, pcd_data_.tracker_hits());

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
