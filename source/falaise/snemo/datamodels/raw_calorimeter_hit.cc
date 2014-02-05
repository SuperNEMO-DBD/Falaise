// -*- mode: c++ ; -*-
/** \file falaise/snemo/datamodels/raw_calorimeter_hit.cc
 */

// Ourselves
#include <falaise/snemo/datamodels/raw_calorimeter_hit.h>

// Third party
// - Bayeux/datatools
//#include <datatools/clhep_units.h>
//#include <datatools/utils.h>

namespace snemo {

  namespace datamodel {

    // Serial tag for datatools::serialization::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(raw_calorimeter_hit, "snemo::datamodel::raw_calorimeter_hit")

    bool raw_calorimeter_hit::is_valid() const
    {
      return _data_description_ != DATA_DESC_UNDEFINED;
    }

    void raw_calorimeter_hit::set_trigger_id(uint16_t tid_)
    {
      _trigger_id_ = tid_;
      return;
    }

    uint16_t raw_calorimeter_hit::get_trigger_id() const
    {
      return _trigger_id_;
    }

    void raw_calorimeter_hit::set_channel_address(uint16_t ca_)
    {
      _channel_address_ = ca_;
      return;
    }

    uint16_t raw_calorimeter_hit::get_channel_address() const
    {
      return _channel_address_;
    }

    uint32_t raw_calorimeter_hit::get_module_id() const
    {
      uint32_t id = 0;
      id = _channel_address_ & ADDRESS_MODULE_ID;
      id >>= 11;
      return id;
    }

    uint32_t raw_calorimeter_hit::get_crate_id() const
    {
      uint32_t id = 0;
      id = _channel_address_ & ADDRESS_CRATE_ID;
      id >>= 9;
      return id;
    }

    uint32_t raw_calorimeter_hit::get_board_id() const
    {
      uint32_t id = 0;
      id = _channel_address_ & ADDRESS_BOARD_ID;
      id >>= 5;
      return id;
    }

    uint32_t raw_calorimeter_hit::get_channel_id() const
    {
      uint32_t id = 0;
      id = _channel_address_ & ADDRESS_CHANNEL_ID;
      return id;
    }

    void raw_calorimeter_hit::set_time_info(uint64_t ti_)
    {
      _time_info_ = ti_;
      return;
    }

    uint64_t raw_calorimeter_hit::get_time_info() const
    {
      return _time_info_;
    }

    raw_calorimeter_hit::sampling_frequency_type
    raw_calorimeter_hit::get_sampling_frequency() const
    {
      return SAMPLING_FREQUENCY_UNDEF;
    }

    uint16_t raw_calorimeter_hit::get_posttrig() const
    {

      return TDC_UNDEFINED;
    }

    void raw_calorimeter_hit::set_data_description(uint16_t dd_)
    {
      _data_description_ = dd_;
      return;
    }

    uint16_t raw_calorimeter_hit::get_data_description() const
    {
      return _data_description_;
    }

    bool raw_calorimeter_hit::has_waveform() const
    {
      return _data_description_ & DATA_DESC_SIG_HASWAVEFORM;
    }

    bool raw_calorimeter_hit::has_aux_baseline() const
    {
      return _data_description_ & DATA_DESC_AUX_HASBASELINE;
    }

    bool raw_calorimeter_hit::has_aux_peak_charge() const
    {
      return _data_description_ & DATA_DESC_AUX_HASPEAKCHARGE;
    }

    bool raw_calorimeter_hit::has_aux_peak_data() const
    {
      return _data_description_ & DATA_DESC_AUX_HASPEAKDATA;
    }

    bool raw_calorimeter_hit::has_aux_cfd_interpolated_time() const
    {
      return _data_description_ & DATA_DESC_AUX_HASTIMECFDINTERP;
    }

    void raw_calorimeter_hit::set_waveform_first_sample(uint16_t val_)
    {
      _waveform_first_sample_ = val_;
      return;
    }

    uint16_t raw_calorimeter_hit::get_waveform_first_sample() const
    {
      return _waveform_first_sample_;
    }

    void raw_calorimeter_hit::set_waveform_last_sample(uint16_t val_)
    {
      _waveform_last_sample_ = val_;
      return;
    }

    uint16_t raw_calorimeter_hit::get_waveform_last_sample() const
    {
      return _waveform_last_sample_;
    }

    const raw_calorimeter_hit::waveform_type & raw_calorimeter_hit::get_waveform() const
    {
      return _waveform_;
    }

    raw_calorimeter_hit::waveform_type & raw_calorimeter_hit::grab_waveform()
    {
      return _waveform_;
    }

    void raw_calorimeter_hit::set_aux_baseline(uint16_t val_)
    {
      _aux_baseline_ = val_;
      return;
    }

    uint16_t raw_calorimeter_hit::get_aux_baseline() const
    {
      return _aux_baseline_;
    }

    void raw_calorimeter_hit::set_aux_peak_charge(uint16_t val_)
    {
      _aux_peak_charge_ = val_;
      return;
    }

    uint16_t raw_calorimeter_hit::get_aux_peak_charge() const
    {
      return _aux_peak_charge_;
    }

    void raw_calorimeter_hit::set_aux_peak_amplitude(uint16_t val_)
    {
      _aux_peak_amplitude_ = val_;
      return;
    }

    uint16_t raw_calorimeter_hit::get_aux_peak_amplitude() const
    {
      return _aux_peak_amplitude_;
    }

    void raw_calorimeter_hit::set_aux_peak_time_max_amplitude(uint16_t val_)
    {
      _aux_peak_time_max_amplitude_ = val_;
      return;
    }

    uint16_t raw_calorimeter_hit::get_aux_peak_time_max_amplitude() const
    {
      return _aux_peak_time_max_amplitude_;
    }

    void raw_calorimeter_hit::set_aux_underflow_tot(uint16_t val_)
    {
      _aux_underflow_tot_ = val_;
      return;
    }

    uint16_t raw_calorimeter_hit::get_aux_underflow_tot() const
    {
      return _aux_underflow_tot_;
    }

    void raw_calorimeter_hit::set_aux_time_cfd_interp(uint16_t val_)
    {
      _aux_time_cfd_interp_ = val_;
      return;
    }

    uint16_t raw_calorimeter_hit::get_aux_time_cfd_interp() const
    {
      return _aux_time_cfd_interp_;
    }


    raw_calorimeter_hit::raw_calorimeter_hit()
    {
      _set_defaults();
      _waveform_.reserve(TDC_MAX_SAMPLING_DEPTH);
      return;
    }

    raw_calorimeter_hit::~raw_calorimeter_hit()
    {
      return;
    }

    void raw_calorimeter_hit::reset()
    {
      _waveform_.clear();
      _set_defaults();
      return;
    }

    void raw_calorimeter_hit::_set_defaults()
    {
      _trigger_id_ = 0;
      _channel_address_ = 0;
      _time_info_ = 0;
      _data_description_ = DATA_DESC_UNDEFINED;
      _waveform_first_sample_ = TDC_UNDEFINED;
      _waveform_last_sample_ = TDC_UNDEFINED;
      _aux_baseline_ = ADC_UNDEFINED;
      _aux_peak_charge_ = ADC_UNDEFINED;
      _aux_peak_amplitude_ = ADC_UNDEFINED;
      _aux_peak_time_max_amplitude_ = TDC_UNDEFINED;
      _aux_underflow_tot_ = TDC_UNDEFINED;
      _aux_time_cfd_interp_ = TDC_UNDEFINED;
      return;
    }

    void raw_calorimeter_hit::tree_dump (std::ostream & out_,
                                         const std::string & title_,
                                         const std::string & indent_,
                                         bool inherit_) const
    {
      std::string indent;
      if (! indent_.empty ()) indent = indent_;
      base_hit::tree_dump (out_, title_, indent_, true);

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Trigger ID       : " << _trigger_id_ << std::endl;
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Channel address  : " << _channel_address_ << std::endl;
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Time information :  " << std::hex << _time_info_ << std::dec << std::endl;
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Data description : " << _data_description_ << std::endl;
      if (has_waveform()) {
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Waveform : " << std::endl;
        out_ << indent << datatools::i_tree_dumpable::skip_tag << datatools::i_tree_dumpable::tag
             << "Waveform first sample : " << _waveform_first_sample_ << std::endl;
        out_ << indent << datatools::i_tree_dumpable::skip_tag << datatools::i_tree_dumpable::tag
             << "Waveform last sample : " << _waveform_last_sample_ << std::endl;
        out_ << indent << datatools::i_tree_dumpable::skip_tag << datatools::i_tree_dumpable::last_tag
             << "Waveform samples : " << _waveform_.size() << std::endl;
      }
      if (has_aux_baseline()) {
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Baseline : " << _aux_baseline_ << std::endl;
      }
      if (has_aux_peak_charge()) {
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Peak charge : " << _aux_peak_charge_ << std::endl;
      }
      if (has_aux_peak_data()) {
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Peak amplitude : " << _aux_peak_amplitude_ << std::endl;
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Peak time @ max amplitude : " << _aux_peak_time_max_amplitude_ << std::endl;
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Underflow TOT : " << _aux_underflow_tot_ << std::endl;
      }
      if (has_aux_cfd_interpolated_time()) {
        out_ << indent << datatools::i_tree_dumpable::tag
             << "CFD interpolated time : " << _aux_time_cfd_interp_ << std::endl;
      }

      out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
           << "Valid : " << is_valid() << std::endl;

      return;
    }

  } // end of namespace datamodel

} // end of namespace snemo
