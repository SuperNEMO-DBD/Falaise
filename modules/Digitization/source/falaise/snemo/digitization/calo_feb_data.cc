// snemo/digitization/calo_feb_data.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>

#include "calo_feb_data.h"
#include <iostream>

using namespace std;

namespace snemo {
namespace digitization {

calo_feb_data::calo_feb_data() {  // D-fault C-tor
  reset();
}

bool calo_feb_data::is_initialized() const { return _initialized_; }
void calo_feb_data::initialize(bool has_waveform_, unsigned int nb_sample_,
                               unsigned int first_sample_) {
  bool are_args_ok = true;
  if (has_waveform_ == true && (nb_sample_ == 0 || first_sample_ == 0)) {
    are_args_ok = false;
  }

  DT_THROW_IF(!are_args_ok, std::logic_error,
              "calo feb has waveform BUT nb of sample and/or first_sample are null ! ");

  int total_nb_of_words = NB_OF_WORD + nb_sample_;
  _feb_words_collection.reserve(total_nb_of_words);

  _data_description_.set(WAVEFORM_STATUS) = has_waveform_;
  _number_of_sample = nb_sample_;
  _first_waveform_sample = first_sample_;

  _waveform[0].reserve(_number_of_sample);
  _waveform[1].reserve(_number_of_sample);

  _initialized_ = true;
}

void calo_feb_data::reset() {
  _data_description_.reset();
  // YL to tuned
  _rack_id = 100;
  _crate_id = 100;
  _board_id = 100;
  _samlong_id = 100;

  for (unsigned int i = 0; i < NB_OF_CHANNEL; i++) {
    _baseline[i] = datatools::invalid_real();
    _charge[i] = datatools::invalid_real();
    _max_peak_amplitude[i] = datatools::invalid_real();
    _time_max_peak_amplitude[i] =
        datatools::invalid_real();  // YL : to check, should be an integer uint16_t
    _falling_time[i] = datatools::invalid_real();
    _rising_time[i] = datatools::invalid_real();
    _waveform[i].clear();
    _treshold_status_[i].set(LOW_TRESHOLD, false);
    _treshold_status_[i].set(HIGH_TRESHOLD, false);
  }

  _initialized_ = false;
}

void calo_feb_data::set_address(uint16_t rack_, uint16_t crate_, uint16_t board_,
                                uint16_t samlong_) {
  DT_THROW_IF(
      (rack_ > MAX_RACK_INDEX) || (crate_ > MAX_CRATE_INDEX) || (board_ > MAX_SAMLONG_INDEX),
      std::logic_error, "Incorrect electronic address ! Chek rack, crate, board, samlong.... ");
  _rack_id = rack_;
  _crate_id = crate_;
  _board_id = board_;
  _samlong_id = samlong_;
}

// DETECTED TRESHOLD : true if signal cross treshold
// [1 bit] LT + [1 bit] HT
void calo_feb_data::set_detected_treshold_per_channel(bool lt_, bool ht_, int channel_) {
  DT_THROW_IF((ht_ && !lt_), std::logic_error,
              "In case of high treshold detection, low treshold should be detected ");

  _treshold_status_[channel_].set(LOW_TRESHOLD, lt_);
  _treshold_status_[channel_].set(HIGH_TRESHOLD, ht_);
}

// YL : need to control metadata validity
// BASELINE : Mean value of the first 16 samples of the waveform
// [13 bits]MSB + [3 bits]LSB
void calo_feb_data::set_baseline_per_channel(double value_, int channel_) {
  DT_THROW_IF((value_ > MAX_SAMPLE_VALUE), std::logic_error,
              "Mean channel baseline should not be too high! (should be lower than '"
                  << MAX_SAMPLE_VALUE << "' ");

  _baseline[channel_] = value_;
}

// CHARGE : integration of N samples
void calo_feb_data::set_charge_per_channel(double value_, int channel_) {
  _charge[channel_] = value_;
}

// MAX PEAK : maximum of the waveform
// baseline substracted
// signed value [13 bits]MSB + decimals [3 bits]LSB
void calo_feb_data::set_max_per_channel(double value_, int channel_) {
  DT_THROW_IF((value_ > MAX_SAMPLE_VALUE), std::logic_error,
              "Max sample value should not be too high! (should be lower than '" << MAX_SAMPLE_VALUE
                                                                                 << "' ");

  _max_peak_amplitude[channel_] = value_;
}

// TIME@MAX : Sample number of the MAX PEAK
// [10 bits]
// In case of overflow, return first overflowed sample
void calo_feb_data::set_time_max_per_channel(uint16_t value_, int channel_) {
  DT_THROW_IF((value_ > MAX_NB_OF_SAMPLE), std::logic_error,
              "Max position should not be greater than max number of sample : '" << MAX_SAMPLE_VALUE
                                                                                 << "' ");

  _time_max_peak_amplitude[channel_] = value_;
}

// FALLING TIME : time of the threshold crossing point
// sample nb [10 bits] + time interpolation [9 bits]
void calo_feb_data::set_falling_per_channel(double value_, int channel_) {
  DT_THROW_IF(
      (value_ > MAX_NB_OF_SAMPLE), std::logic_error,
      "Falling time not be greater than max number of sample : '" << MAX_SAMPLE_VALUE << "' ");

  _falling_time[channel_] = value_;
}

// RISING TIME : time of the threshold crossing point
// sample nb [10 bits] + time interpolation [9 bits]
void calo_feb_data::set_rising_per_channel(double value_, int channel_) {
  DT_THROW_IF(
      (value_ > MAX_NB_OF_SAMPLE), std::logic_error,
      "Rising time not be greater than max number of sample : '" << MAX_SAMPLE_VALUE << "' ");

  _rising_time[channel_] = value_;
}

void calo_feb_data::set_metadata(double *baseline_array_, double *charge_array_, double *max_array_,
                                 uint16_t *time_max_array_, double *falling_array_,
                                 double *rising_array_) {
  for (unsigned int i = 0; i < NB_OF_CHANNEL; i++) {
    set_baseline_per_channel(baseline_array_[i], i);
    set_charge_per_channel(charge_array_[i], i);
    set_max_per_channel(max_array_[i], i);
    set_time_max_per_channel(time_max_array_[i], i);
    set_falling_per_channel(falling_array_[i], i);
    set_rising_per_channel(rising_array_[i], i);
  }
}

void calo_feb_data::set_data(std::vector<uint16_t> *waveform_array_) {
  DT_THROW_IF(!is_initialized(), std::logic_error,
              "Current calo_feb_data not initialized ! (it should be my fellow) ");

  bool is_ok_array_size = true;
  if (waveform_array_[0].size() != _number_of_sample) is_ok_array_size = false;
  if (waveform_array_[1].size() != _number_of_sample) is_ok_array_size = false;

  DT_THROW_IF(!is_ok_array_size, std::logic_error,
              "Input waveform array has not the expected array size ! (that's the dark side) ");

  for (unsigned int i = 0; i < NB_OF_CHANNEL; i++) {
    for (unsigned int j = 0; j < _number_of_sample; j++) {
      _waveform[i][j] = waveform_array_[i][j];
    }
  }
}

// void calo_feb_data::set_header(int32_t id_, const geomtools::geom_id & feb_id_, uint64_t
// data_time_){
//   //DT_THROW_IF((data_time_<0), std::logic_error, "Negative timestamp for calo data ! (should be
//   positive) "); set_hit_id(id_); set_geom_id(feb_id_); _data_timestamp = data_time_;
// }

void calo_feb_data::set_header(int32_t id_, uint16_t rack_, uint16_t crate_, uint16_t board_,
                               uint16_t samlong_, uint64_t data_time_) {
  // DT_THROW_IF((data_time_<0), std::logic_error, "Negative timestamp for calo data ! (should be
  // positive) ");
  set_hit_id(id_);
  set_address(rack_, crate_, board_, samlong_);
  // set_rack_id(rack_);
  // set_crate_id(crate_);
  // set_board_id(board_);
  // set_samlong_id(samlong_);
  _data_timestamp = data_time_;
}

void calo_feb_data::tree_dump(std::ostream &out_, const std::string &title_,
                              const std::string &indent_, bool inherit_) const {
  //      base_hit::tree_dump (out_, title_, indent_, true);
  out_ << title_ << inherit_ << std::endl;
  out_ << indent_ << datatools::i_tree_dumpable::tag << "METADATA" << std::endl;
  for (unsigned int i = 0; i < NB_OF_CHANNEL; i++) {
    if (i == 0) out_ << indent_ << datatools::i_tree_dumpable::tag;
    if (i == 1) out_ << indent_ << datatools::i_tree_dumpable::last_tag;
    out_ << "Channel #" << i << " : ";

    if (_treshold_status_[i].test(LOW_TRESHOLD)) out_ << "LT detected ";
    if (_treshold_status_[i].test(HIGH_TRESHOLD)) out_ << ", HT detected ";
    out_ << std::endl;

    out_ << indent_;
    if (i == 0)
      out_ << datatools::i_tree_dumpable::skip_tag;
    else {
      out_ << datatools::i_tree_dumpable::last_skip_tag;
    }
    out_ << datatools::i_tree_dumpable::tag;
    out_ << "Waveform : " << std::boolalpha << _data_description_.test(WAVEFORM_STATUS);

    if (_data_description_.test(WAVEFORM_STATUS)) {
      out_ << " => ";
      for (int j = 0; j < 10; j++) {
        out_ << _waveform[i][j] << " ";
      }
      out_ << "...";
    }

    out_ << std::endl;
    out_ << indent_;
    if (i == 0)
      out_ << datatools::i_tree_dumpable::skip_tag;
    else {
      out_ << datatools::i_tree_dumpable::last_skip_tag;
    }
    out_ << datatools::i_tree_dumpable::tag;
    out_ << "Baseline     : " << _baseline[i] << std::endl;

    out_ << indent_;
    if (i == 0)
      out_ << datatools::i_tree_dumpable::skip_tag;
    else {
      out_ << datatools::i_tree_dumpable::last_skip_tag;
    }
    out_ << datatools::i_tree_dumpable::tag;
    out_ << "Charge       : " << _charge[i] << std::endl;

    out_ << indent_;
    if (i == 0)
      out_ << datatools::i_tree_dumpable::skip_tag;
    else {
      out_ << datatools::i_tree_dumpable::last_skip_tag;
    }
    out_ << datatools::i_tree_dumpable::tag;
    out_ << "Max peak     : " << _max_peak_amplitude[i] << std::endl;

    out_ << indent_;
    if (i == 0)
      out_ << datatools::i_tree_dumpable::skip_tag;
    else {
      out_ << datatools::i_tree_dumpable::last_skip_tag;
    }
    out_ << datatools::i_tree_dumpable::tag;
    out_ << "Time@Max     : " << _time_max_peak_amplitude[i] << std::endl;

    out_ << indent_;
    if (i == 0)
      out_ << datatools::i_tree_dumpable::skip_tag;
    else {
      out_ << datatools::i_tree_dumpable::last_skip_tag;
    }
    out_ << datatools::i_tree_dumpable::tag;
    out_ << "Falling time : " << _falling_time[i] << std::endl;

    out_ << indent_;
    if (i == 0)
      out_ << datatools::i_tree_dumpable::skip_tag;
    else {
      out_ << datatools::i_tree_dumpable::last_skip_tag;
    }
    out_ << datatools::i_tree_dumpable::last_tag;
    out_ << "Rising time  : " << _rising_time[i] << std::endl;
  }
  return;
}

}  // end of namespace digitization
}  // end of namespace snemo
