// -*- mode: c++ ; -*-
/** \file falaise/snemo/datamodels/calorimeter_digitized_hit.cc
*/

// Ourselves
#include <falaise/snemo/datamodels/calorimeter_digitized_hit.h>

// Third party:
// - Bayeux/datatools
#include <bayeux/datatools/exception.h>

namespace snemo {

  namespace datamodel {

    DATATOOLS_SERIALIZATION_IMPLEMENTATION(calorimeter_digitized_hit,
                                           "snemo::datamodel::calorimeter_digitized_hit")

    /*** calorimeter_digitized_hit::rtd_origin ***/

    calorimeter_digitized_hit::rtd_origin::rtd_origin(int32_t hit_number_,
                                                      int32_t trigger_id_)
    : _hit_number_(hit_number_)
      , _trigger_id_(trigger_id_)
    {
      return;
    }

    int32_t calorimeter_digitized_hit::rtd_origin::get_hit_number() const
    {
      return _hit_number_;
    }

    int32_t calorimeter_digitized_hit::rtd_origin::get_trigger_id() const
    {
      return _trigger_id_;
    }

    bool calorimeter_digitized_hit::rtd_origin::is_valid() const
    {
      if (_hit_number_ < 0) return false;
      if (_trigger_id_ < 0) return false;
      return true;
    }

    void calorimeter_digitized_hit::rtd_origin::invalidate()
    {
      _hit_number_ = INVALID_HIT_ID;
      _trigger_id_ = INVALID_TRIGGER_ID;
      return;
    }

    // virtual
    void calorimeter_digitized_hit::rtd_origin::print_tree(std::ostream & out_,
                                                           const boost::property_tree::ptree & options_) const
    {
      base_print_options popts;
      popts.configure_from(options_);

      if (popts.title.length()) {
// #include <falaise/snemo/datamodels/sncabling_bridge.h>
// #include <falaise/snemo/datamodels/snfee_bridge.h>
        out_ << popts.indent << popts.title << std::endl;
      }

      out_ << popts.indent << tag
           << "Hit number : " << _hit_number_ << std::endl;

      out_ << popts.indent << inherit_tag(popts.inherit)
           << "Trigger ID : " << _trigger_id_ << std::endl;

      return;
    }

    /*** calorimeter_digitized_hit ***/

    bool calorimeter_digitized_hit::is_low_threshold_only() const
    {
      return _low_threshold_only_;
    }

    void calorimeter_digitized_hit::set_low_threshold_only(bool lt_)
    {
      _low_threshold_only_ = lt_;
      return;
    }

    bool calorimeter_digitized_hit::is_high_threshold() const
    {
      return _high_threshold_;
    }

    void calorimeter_digitized_hit::set_high_threshold(bool ht_)
    {
      _high_threshold_ = ht_;
      return;
    }

    bool calorimeter_digitized_hit::has_waveform() const
    {
      return _store_check(STORE_DIGIFLAGS_WAVEFORM);
    }

    bool calorimeter_digitized_hit::has_fwmeas_baseline() const
    {
      return _store_check(STORE_DIGIFLAGS_FWMEAS_BASELINE);
    }

    void calorimeter_digitized_hit::set_fwmeas_baseline(int16_t baseline_)
    {
      _fwmeas_.baseline = baseline_;
      _store_set(STORE_DIGIFLAGS_FWMEAS_BASELINE);
      return;
    }

    void calorimeter_digitized_hit::reset_fwmeas_baseline()
    {
      _fwmeas_.baseline = 0;
      _store_unset(STORE_DIGIFLAGS_FWMEAS_BASELINE);
      return;
    }

    int16_t calorimeter_digitized_hit::get_fwmeas_baseline() const
    {
      return _fwmeas_.baseline;
    }

    bool calorimeter_digitized_hit::has_fwmeas_peak_amplitude() const
    {
      return _store_check(STORE_DIGIFLAGS_FWMEAS_PEAK_AMPLITUDE);
    }

    void calorimeter_digitized_hit::set_fwmeas_peak_amplitude(int16_t peak_amplitude_)
    {
      _fwmeas_.peak_amplitude = peak_amplitude_;
      _store_set(STORE_DIGIFLAGS_FWMEAS_PEAK_AMPLITUDE);
      return;
    }

    void calorimeter_digitized_hit::reset_fwmeas_peak_amplitude()
    {
      _fwmeas_.peak_amplitude = 0;
      _store_unset(STORE_DIGIFLAGS_FWMEAS_PEAK_AMPLITUDE);
      return;
    }

    int16_t calorimeter_digitized_hit::get_fwmeas_peak_amplitude() const
    {
      return _fwmeas_.peak_amplitude;
    }

    bool calorimeter_digitized_hit::has_fwmeas_peak_cell() const
    {
      return _store_check(STORE_DIGIFLAGS_FWMEAS_PEAK_CELL);
    }

    void calorimeter_digitized_hit::set_fwmeas_peak_cell(int16_t peak_cell_)
    {
      _fwmeas_.peak_cell = peak_cell_;
      _store_set(STORE_DIGIFLAGS_FWMEAS_PEAK_CELL);
      return;
    }

    void calorimeter_digitized_hit::reset_fwmeas_peak_cell()
    {
      _fwmeas_.peak_cell = 0;
      _store_unset(STORE_DIGIFLAGS_FWMEAS_PEAK_CELL);
      return;
    }

    int16_t calorimeter_digitized_hit::get_fwmeas_peak_cell() const
    {
      return _fwmeas_.peak_cell;
    }

    bool calorimeter_digitized_hit::has_fwmeas_charge() const
    {
      return _store_check(STORE_DIGIFLAGS_FWMEAS_CHARGE);
    }

    void calorimeter_digitized_hit::set_fwmeas_charge(int32_t charge_)
    {
      _fwmeas_.charge = charge_;
      _store_set(STORE_DIGIFLAGS_FWMEAS_CHARGE);
      return;
    }

    void calorimeter_digitized_hit::reset_fwmeas_charge()
    {
      _fwmeas_.charge = 0;
      _store_unset(STORE_DIGIFLAGS_FWMEAS_CHARGE);
      return;
    }

    int32_t calorimeter_digitized_hit::get_fwmeas_charge() const
    {
      return _fwmeas_.charge;
    }

    bool calorimeter_digitized_hit::has_fwmeas_rising_cell() const
    {
      return _store_check(STORE_DIGIFLAGS_FWMEAS_RISING_CELL);
    }

    void calorimeter_digitized_hit::set_fwmeas_rising_cell(int32_t rc_)
    {
      _fwmeas_.rising_cell = rc_;
      _store_set(STORE_DIGIFLAGS_FWMEAS_RISING_CELL);
      return;
    }

    void calorimeter_digitized_hit::reset_fwmeas_rising_cell()
    {
      _fwmeas_.rising_cell = 0;
      _store_unset(STORE_DIGIFLAGS_FWMEAS_RISING_CELL);
      return;
    }

    int32_t calorimeter_digitized_hit::get_fwmeas_rising_cell() const
    {
      return _fwmeas_.rising_cell;
    }

    bool calorimeter_digitized_hit::has_fwmeas_falling_cell() const
    {
      return _store_check(STORE_DIGIFLAGS_FWMEAS_FALLING_CELL);
    }

    void calorimeter_digitized_hit::set_fwmeas_falling_cell(int32_t fc_)
    {
      _fwmeas_.falling_cell = fc_;
      _store_set(STORE_DIGIFLAGS_FWMEAS_FALLING_CELL);
      return;
    }

    void calorimeter_digitized_hit::reset_fwmeas_falling_cell()
    {
      _fwmeas_.falling_cell = 0;
      _store_unset(STORE_DIGIFLAGS_FWMEAS_FALLING_CELL);
      return;
    }

    int32_t calorimeter_digitized_hit::get_fwmeas_falling_cell() const
    {
      return _fwmeas_.falling_cell;
    }

    const calorimeter_digitized_hit::rtd_origin & calorimeter_digitized_hit::get_origin() const
    {
      return _origin_;
    }

    void calorimeter_digitized_hit::set_origin(const rtd_origin & orig_)
    {
      _store_set(STORE_DIGIFLAGS_ORIGIN);
      _origin_ = orig_;
      return;
    }

    void calorimeter_digitized_hit::reset_origin()
    {
      _store_unset(STORE_DIGIFLAGS_ORIGIN);
      return;
      _origin_.invalidate();
    }

    bool calorimeter_digitized_hit::has_origin() const
    {
      return _origin_.is_valid();
    }

    void calorimeter_digitized_hit::set_timestamp(const int64_t & timestamp_)
    {
      _timestamp_ = timestamp_;
      return;
    }

    const int64_t & calorimeter_digitized_hit::get_timestamp() const
    {
      return _timestamp_;
    }

    void calorimeter_digitized_hit::set_fcr(uint16_t fcr_)
    {
      _fcr_ = fcr_;
      return;
    }

    uint16_t calorimeter_digitized_hit::get_fcr() const
    {
      return _fcr_;
    }

    void calorimeter_digitized_hit::set_lt_trigger_counter(uint16_t lt_trigger_counter_)
    {
      _lt_trigger_counter_ = lt_trigger_counter_;
      return;
    }

    uint16_t calorimeter_digitized_hit::get_lt_trigger_counter() const
    {
      return _lt_trigger_counter_;
    }

    void calorimeter_digitized_hit::set_lt_time_counter(uint32_t lt_time_counter_)
    {
      _lt_time_counter_ = lt_time_counter_;
      return;
    }

    uint32_t calorimeter_digitized_hit::get_lt_time_counter() const
    {
      return _lt_time_counter_;
    }

    const std::vector<int16_t> & calorimeter_digitized_hit::get_waveform() const
    {
      return _waveform_;
    }

    std::vector<int16_t> & calorimeter_digitized_hit::grab_waveform()
    {
      _store_set(STORE_DIGIFLAGS_WAVEFORM);
      return _waveform_;
    }

    void calorimeter_digitized_hit::set_waveform(std::vector<int16_t> & wf_)
    {
      _waveform_ = wf_;
      return;
    }

    bool calorimeter_digitized_hit::is_valid() const
    {
      if (not this->geomtools::base_hit::is_valid()) return true;
      if (_timestamp_ == INVALID_TIME_TICKS) return false;
      if (_fcr_ == INVALID_FCR) return false;
      // No invalid value for 'lt_time_counter'
      if (_waveform_.size() == 0) return false;
      return true;
    }

    void calorimeter_digitized_hit::invalidate()
    {
      this->geomtools::base_hit::invalidate();
      _timestamp_ = INVALID_TIME_TICKS;
      _low_threshold_only_ = false;
      _high_threshold_ = false;
      _fcr_ = INVALID_FCR;
      _waveform_.clear();
      reset_fwmeas_baseline();
      reset_fwmeas_peak_amplitude();
      reset_fwmeas_peak_cell();
      reset_fwmeas_charge();
      reset_fwmeas_rising_cell();
      reset_fwmeas_falling_cell();
      reset_origin();
      return;
    }

    // virtual
    void calorimeter_digitized_hit::print_tree(std::ostream & out_,
                                               const boost::property_tree::ptree & options_) const
    {
      base_print_options popts;
      popts.configure_from(options_);

      this->geomtools::base_hit::print_tree(out_,
                                            base_print_options::force_inheritance(options_));

      out_ << popts.indent << tag
           << "Timestamp : " << _timestamp_ << std::endl;

      out_ << popts.indent << tag
           << "Low threshold only : " << std::boolalpha << _low_threshold_only_ << std::endl;

      out_ << popts.indent << tag
           << "High threshold : " << std::boolalpha << _high_threshold_ << std::endl;

      out_ << popts.indent << tag
           << "FCR : " << (_fcr_== INVALID_FCR ? "invalid" : std::to_string(_fcr_))  << std::endl;

      out_ << popts.indent << tag
           << "LT time counter : " << _lt_time_counter_  << std::endl;

      out_ << popts.indent << tag
           << "Waveform : " << _waveform_.size() << " samples" << std::endl;

      out_ << popts.indent << tag
           << "FW measurements : " << std::endl;

      out_ << popts.indent << skip_tag
           << tag << "Baseline       : ";
      out_ << (has_fwmeas_baseline() ? std::to_string(_fwmeas_.baseline) : "undefined");
      out_ << std::endl;

      out_ << popts.indent << skip_tag
           << tag << "Peak amplitude : ";
      out_ << (has_fwmeas_peak_amplitude() ? std::to_string(_fwmeas_.peak_amplitude) : "undefined");
      out_ << std::endl;

      out_ << popts.indent << skip_tag
           << tag << "Peak cell      : ";
      out_ << (has_fwmeas_peak_cell() ? std::to_string(_fwmeas_.peak_cell) : "undefined");
      out_ << std::endl;

      out_ << popts.indent << skip_tag
           << tag << "Charge cell    : ";
      out_ << (has_fwmeas_charge() ? std::to_string(_fwmeas_.charge) : "undefined");
      out_ << std::endl;

      out_ << popts.indent << skip_tag
           << tag << "Rising cell    : ";
      out_ << (has_fwmeas_rising_cell() ? std::to_string(_fwmeas_.rising_cell) : "undefined");
      out_ << std::endl;

      out_ << popts.indent << skip_tag
           << last_tag << "Falling cell   : ";
      out_ << (has_fwmeas_falling_cell() ? std::to_string(_fwmeas_.falling_cell) : "undefined");
      out_ << std::endl;

      out_ << popts.indent << tag
           << "RTD Origin : " << (_origin_.is_valid() ? "" : "undefined") << std::endl;
      if (_origin_.is_valid()) {
        boost::property_tree::ptree opts2;
        opts2.put("indent", popts.indent + "|   ");
        _origin_.print_tree(out_, opts2);
      }

      out_ << popts.indent << inherit_tag(popts.inherit)
           << "Valid : " << std::boolalpha << is_valid() << std::endl;

      return;
    }

  } // end of namespace datamodel

} // end of namespace snemo
