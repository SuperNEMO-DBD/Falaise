// fecom/calo_constants.cpp

// Ourselves:
#include <fecom/calo_constants.hpp>

// Third party:
#include <bayeux/datatools/exception.h>

namespace fecom {

// statics:
const uint16_t calo_constants::MAX_NUMBER_OF_SAMPLES;
const uint16_t calo_constants::INVALID_SAMPLE;
const uint16_t calo_constants::NUMBER_OF_CHANNELS_PER_SAMLONG;
const uint16_t calo_constants::NUMBER_OF_SAMLONGS_PER_BOARD;
constexpr uint16_t calo_constants::NUMBER_OF_CHANNELS_PER_BOARD;
const uint16_t calo_constants::INVALID_BOARD_CHANNEL;

double calo_constants::adc_dynamics() { return 2.5 * CLHEP::volt; }

double calo_constants::adc_unit() { return adc_dynamics() / 4096; }

double calo_constants::adc_offset_unit() { return adc_unit() / 16; }

double calo_constants::default_sampling_frequency() { return 2.56e9 * CLHEP::hertz; }

double calo_constants::time_sample_duration(int /* clock_*/) {
  double clock = 1. / default_sampling_frequency();
  return clock;
}

double calo_constants::time_offset_unit() { return time_sample_duration() / 256; }
const uint16_t calo_constants::NUMBERS_OF_CALO_PER_COLUMN;
const uint64_t calo_constants::GEOMETRIC_CALO_TYPE;
const uint16_t calo_constants::COLUMN_INDEX;
const uint16_t calo_constants::ROW_INDEX;

const uint64_t calo_constants::CALO_CHANNEL_TYPE;
const uint16_t calo_constants::SLOT_INDEX;
const uint16_t calo_constants::CHANNEL_INDEX;

}  // namespace fecom
