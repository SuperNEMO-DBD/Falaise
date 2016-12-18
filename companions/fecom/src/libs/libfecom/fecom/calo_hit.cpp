// fecom/calo_hit.cpp

// Ourselves:
#include <fecom/calo_hit.hpp>

// Third party:
#include <bayeux/datatools/exception.h>

namespace fecom {

  calo_hit::calo_hit()
    : base_hit(SIG_CALORIMETER)
  {
    _reset_();
  }

  calo_hit::~calo_hit()
  {
  }

  bool calo_hit::is_valid() const
  {
    if (! this->base_hit::is_valid()) return false;
    if (channel >= calo_constants::INVALID_BOARD_CHANNEL) return false;
    return (fcr < calo_constants::MAX_NUMBER_OF_SAMPLES) && (raw_tdc < 0xFFFFFFFF);
  }

  void calo_hit::reset()
  {
    this->base_hit::reset();
    _reset_();
    hitmode = SIG_CALORIMETER;
    return;
  }

  void calo_hit::_reset_raw_waveform_data_()
  {
    for (size_t i = 0; i < calo_constants::MAX_NUMBER_OF_SAMPLES; i++) {
      raw_waveform_data[i] = 0xFFFF;
    }
    return;
  }

  void calo_hit::_reset_()
  {
    channel = calo_constants::INVALID_BOARD_CHANNEL;
    raw_tdc = 0xFFFFFFFF;
    event_id = 0;
    low_threshold = false;
    high_threshold = false;
    low_threshold_time_count = 0;
    low_threshold_trig_count = 0;
    fcr = calo_constants::INVALID_SAMPLE;
    waveform_data_size = 0;
    _reset_raw_waveform_data_();
    raw_baseline = 0;
    raw_peak = calo_constants::INVALID_SAMPLE;
    raw_peak_cell = 0;
    raw_baseline = 0;
    raw_charge = 0;
    raw_charge_overflow = false;
    raw_cfd_rising_edge_time = 0;
    raw_cfd_falling_edge_time = 0;
    return;
  }

  int16_t calo_hit::get_raw_sample(const uint16_t cell_) const
  {
    DT_THROW_IF(cell_ >= waveform_data_size, std::range_error,
                "Invalid cell index [" << cell_ << " >= " << waveform_data_size << "]!");
    return raw_waveform_data[cell_];
  }

  void calo_hit::set_raw_sample(const uint16_t cell_, const int16_t adc_)
  {
    DT_THROW_IF(cell_ >= waveform_data_size, std::range_error,
                "Invalid cell index [" << cell_ << " >= " << waveform_data_size << "]!");
    raw_waveform_data[cell_] = adc_;
  }

  void calo_hit::tree_dump(std::ostream & out_,
                           const std::string & title_,
                           const std::string & indent_,
                           bool inherit_) const
  {
    this->base_hit::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << io::tag()
         << "Channel                   : " << (int) channel << std::endl;

    out_ << indent_ << io::tag()
         << "Raw TDC                   : " << raw_tdc << std::endl;

    out_ << indent_ << io::tag()
         << "Event ID                  : " << (int) event_id << std::endl;

    out_ << indent_ << io::tag()
         << "Low threshold flag        : " << std::boolalpha << low_threshold << std::endl;

    out_ << indent_ << io::tag()
         << "High threshold flag       : " << std::boolalpha << high_threshold << std::endl;

    out_ << indent_ << io::tag()
         << "Low threshold trig count  : " << low_threshold_trig_count << std::endl;

    out_ << indent_ << io::tag()
         << "Low threshold time count  : " << low_threshold_time_count << std::endl;

    out_ << indent_ << io::tag()
         << "Waveform data size        : " << waveform_data_size << std::endl;

    out_ << indent_ << io::tag()
         << "FCR                       : " << fcr << std::endl;

    out_ << indent_ << io::tag()
         << "Raw baseline              : " << raw_baseline << std::endl;

    out_ << indent_ << io::tag()
         << "Raw peak                  : " << raw_peak << std::endl;

    out_ << indent_ << io::tag()
         << "Raw peak cell             : " << raw_peak_cell << std::endl;

    out_ << indent_ << io::tag()
         << "Raw charge                : " << raw_charge << std::endl;

    out_ << indent_ << io::tag()
         << "Raw charge overflow       : " << std::boolalpha << raw_charge_overflow << std::endl;

    out_ << indent_ << io::tag()
         << "Raw CFD rising edge time  : " << raw_cfd_rising_edge_time << std::endl;

    out_ << indent_ << io::tag()
         << "Raw CFD falling edge time : " << raw_cfd_falling_edge_time << std::endl;

    out_ << indent_ << io::inherit_last_tag(inherit_)
         << "Validity                  : " << std::boolalpha << is_valid() << std::endl;

    return;
  }

  void calo_hit::print_waveform(std::ostream & out_, const uint32_t flags_) const
  {
    for (std::size_t i = 0; i < waveform_data_size; i++) {
      out_ << i << ' ' << raw_waveform_data[i] << '\n';
    }
    out_ << std::endl << std::endl;
    return;
  }

} // namespace fecom
