//! \file fecom/tracker_constants.hpp
//
// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2016 by Guillaume Oliviéro <goliviero@lpccaen.in2p3.fr>

#ifndef FECOM_TRACKER_CONSTANTS_HPP
#define FECOM_TRACKER_CONSTANTS_HPP

// Standard library:
#include <cstdint>

// Third party:
#include <bayeux/datatools/units.h>
#include <bayeux/datatools/clhep_units.h>

namespace fecom {

  struct tracker_constants
  {
		// Page 27 SNDER :
    static const uint16_t NUMBER_OF_CHANNEL_PER_FEAST = 54;
    static const uint16_t NUMBER_OF_FEAST_PER_BOARD = 2;
    static constexpr uint16_t NUMBER_OF_CHANNEL_PER_BOARD
    = NUMBER_OF_CHANNEL_PER_FEAST
      * NUMBER_OF_FEAST_PER_BOARD;
		static const uint16_t NUMBER_OF_CELLS_PER_BOARD = 36;
		static const uint16_t INVALID_FEAST_INDEX = NUMBER_OF_FEAST_PER_BOARD;
    static const uint16_t INVALID_CHANNEL_INDEX = NUMBER_OF_CHANNEL_PER_BOARD;
    static const uint64_t INVALID_TIME = 0xFFFFFFFF;

		static const uint64_t GEOMETRIC_CELL_TYPE = 1000;
		static const uint64_t LAYER_INDEX = 0;
		static const uint64_t ROW_INDEX = 1;
		static const uint64_t PORT_INDEX = 2;

		static const uint64_t ANODIC_PORT = 2;
		static const uint64_t BOT_CATHODE_PORT = 0;
		static const uint64_t TOP_CATHODE_PORT = 1;

		static const uint64_t ELECTRONIC_CELL_TYPE = 666;
		static const uint64_t FEAST_INDEX = 0;
		static const uint64_t CHANNEL_INDEX = 1;
  };

} // namespace fecom

#endif // FECOM_TRACKER_CONSTANTS_HPP

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
