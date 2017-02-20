//! \file fecom/calo_constants.hpp
//
// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>

#ifndef FECOM_CALO_CONSTANTS_HPP
#define FECOM_CALO_CONSTANTS_HPP

// Standard library:
#include <cstdint>

// Third party:
#include <bayeux/datatools/units.h>
#include <bayeux/datatools/clhep_units.h>

namespace fecom {

  struct calo_constants
  {

    static const uint16_t MAX_NUMBER_OF_SAMPLES = 1024;
    static const uint16_t INVALID_SAMPLE = 1024;
    static const uint16_t NUMBER_OF_CHANNELS_PER_SAMLONG = 2;
    static const uint16_t NUMBER_OF_SAMLONGS_PER_BOARD = 8;
    static constexpr uint16_t NUMBER_OF_CHANNELS_PER_BOARD
    = NUMBER_OF_CHANNELS_PER_SAMLONG
      * NUMBER_OF_SAMLONGS_PER_BOARD;
    static const uint16_t INVALID_BOARD_CHANNEL = NUMBER_OF_CHANNELS_PER_BOARD;

    static double adc_dynamics();
    static double adc_unit();
    static double adc_offset_unit();
    static double default_sampling_frequency();
    static double time_sample_duration(int clock_ = 0);
    static double time_offset_unit();

		static const uint16_t NUMBERS_OF_CALO_PER_COLUMN = 13;

		static const uint64_t GEOMETRIC_CALO_TYPE = 3000;
		static const uint16_t COLUMN_INDEX = 0;
		static const uint16_t ROW_INDEX = 1;

		static const uint64_t CALO_CHANNEL_TYPE = 4000;
		static const uint16_t SLOT_INDEX = 0;
		static const uint16_t CHANNEL_INDEX = 1;

  };

} // namespace fecom

#endif // FECOM_CALO_CONSTANTS_HPP

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
