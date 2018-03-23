// fecom/calo_hit.cpp

// Ourselves:
#include <fecom/calo_hit.hpp>

// Third party:
#include <bayeux/datatools/exception.h>

namespace fecom {

DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(calo_hit, "fecom::calo_hit")

calo_hit::calo_hit() : base_hit(SIG_CALORIMETER) { _reset_(); }

calo_hit::~calo_hit() {}

bool calo_hit::is_valid() const {
  if (!this->base_hit::is_valid()) return false;
  if (electronic_id.get(calo_constants::CHANNEL_INDEX) >= calo_constants::INVALID_BOARD_CHANNEL)
    return false;
  else
    return true;
  // return (fcr < calo_constants::MAX_NUMBER_OF_SAMPLES) && (raw_tdc < 0xFFFFFFFF);
}

double calo_hit::get_timestamp() const {
  // WARNING : impossible to use falling cell * tdc_cell_step_ns because we don't have the CFD
  // interpolation from Jihanne and falling cell * tdc_cell_step_ns != falling_time_ns and will
  // induce a shift when we want to set the t_start of a commissioning event in the hc_event_builder
  // return tdc_ns + falling_cell * tdc_cell_step_ns;

  // Must use :
  return tdc_ns + falling_time_ns;
}

void calo_hit::reset() {
  this->base_hit::reset();
  _reset_();
  hitmode = SIG_INVALID;
  return;
}

void calo_hit::_reset_raw_waveform_data_() {
  for (size_t i = 0; i < calo_constants::MAX_NUMBER_OF_SAMPLES; i++) {
    raw_waveform_data[i] = 0xFFF;
  }
  return;
}

void calo_hit::_reset_() {
  raw_tdc = 0xFFFFFFFF;
  tdc_ns = 0;
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
  peak_volt = 0;
  raw_baseline = 0;
  baseline_volt = 0;
  raw_charge = 0;
  charge_picocoulomb = 0;
  raw_charge_overflow = false;
  rising_cell = calo_constants::INVALID_SAMPLE;
  rising_offset = 0;
  rising_time_ns = 0;
  falling_cell = calo_constants::INVALID_SAMPLE;
  falling_offset = 0;
  falling_time_ns = 0;
  return;
}

int16_t calo_hit::get_raw_sample(const uint16_t cell_) const {
  DT_THROW_IF(cell_ >= waveform_data_size, std::range_error,
              "Invalid cell index [" << cell_ << " >= " << waveform_data_size << "]!");
  return raw_waveform_data[cell_];
}

void calo_hit::set_raw_sample(const uint16_t cell_, const int16_t adc_) {
  DT_THROW_IF(cell_ >= waveform_data_size, std::range_error,
              "Invalid cell index [" << cell_ << " >= " << waveform_data_size << "]!");
  raw_waveform_data[cell_] = adc_;
}

void calo_hit::tree_dump(std::ostream& out_, const std::string& title_, const std::string& indent_,
                         bool inherit_) const {
  this->base_hit::tree_dump(out_, title_, indent_, true);

  out_ << indent_ << io::tag() << "Raw TDC                   : " << raw_tdc << std::endl;

  out_ << indent_ << io::tag() << "TDC (in ns)               : " << tdc_ns << std::endl;

  out_ << indent_ << io::tag() << "TDC cell step (in ns)     : " << tdc_cell_step_ns << std::endl;

  out_ << indent_ << io::tag() << "Event ID                  : " << (int)event_id << std::endl;

  out_ << indent_ << io::tag() << "Low threshold flag        : " << std::boolalpha << low_threshold
       << std::endl;

  out_ << indent_ << io::tag() << "High threshold flag       : " << std::boolalpha << high_threshold
       << std::endl;

  out_ << indent_ << io::tag() << "Low threshold trig count  : " << low_threshold_trig_count
       << std::endl;

  out_ << indent_ << io::tag() << "Low threshold time count  : " << low_threshold_time_count
       << std::endl;

  out_ << indent_ << io::tag() << "Waveform data size        : " << waveform_data_size << std::endl;

  out_ << indent_ << io::tag() << "FCR                       : " << fcr << std::endl;

  out_ << indent_ << io::tag() << "Raw baseline              : " << raw_baseline << std::endl;

  out_ << indent_ << io::tag() << "Baseline (in V)           : " << baseline_volt << std::endl;

  out_ << indent_ << io::tag() << "Raw peak                  : " << raw_peak << std::endl;

  out_ << indent_ << io::tag() << "Peak (in V)               : " << peak_volt << std::endl;

  out_ << indent_ << io::tag() << "Raw charge                : " << raw_charge << std::endl;

  out_ << indent_ << io::tag() << "Charge (in pC)            : " << charge_picocoulomb << std::endl;

  out_ << indent_ << io::tag() << "Raw charge overflow       : " << std::boolalpha
       << raw_charge_overflow << std::endl;

  out_ << indent_ << io::tag() << "Rising cell               : " << rising_cell << std::endl;

  out_ << indent_ << io::tag() << "Rising offset             : " << rising_offset << std::endl;

  out_ << indent_ << io::tag() << "Rising time (in ns)       : " << rising_time_ns << std::endl;

  out_ << indent_ << io::tag() << "Falling cell              : " << falling_cell << std::endl;

  out_ << indent_ << io::tag() << "Falling offset            : " << falling_offset << std::endl;

  out_ << indent_ << io::tag() << "Falling time (in ns)      : " << falling_time_ns << std::endl;

  out_ << indent_ << io::inherit_last_tag(inherit_)
       << "Validity                  : " << std::boolalpha << is_valid() << std::endl;

  return;
}

void calo_hit::print_waveform(std::ostream& out_, const uint32_t /*flags_*/) const {
  for (std::size_t i = 0; i < waveform_data_size; i++) {
    out_ << i << ' ' << raw_waveform_data[i] << '\n';
  }
  out_ << std::endl << std::endl;
  return;
}

}  // namespace fecom
