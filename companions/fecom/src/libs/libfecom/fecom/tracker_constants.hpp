//! \file fecom/tracker_constants.hpp
//
// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2016 by Guillaume Oliviéro <goliviero@lpccaen.in2p3.fr>

#ifndef FECOM_TRACKER_CONSTANTS_HPP
#define FECOM_TRACKER_CONSTANTS_HPP

// Standard library:
#include <cstdint>

// Third party:
#include <bayeux/datatools/clhep_units.h>
#include <bayeux/datatools/units.h>

namespace fecom {

struct tracker_constants {
  // tracker_drift_cell.geom -> z : real as length = 2920.0 mm
  static double geiger_cell_Leff();

  static double tracker_clock_tick();

  // Page 27 SNDER :
  static const uint16_t NUMBER_OF_CHANNEL_PER_FEAST = 54;
  static const uint16_t NUMBER_OF_FEAST_PER_BOARD = 2;
  static constexpr uint16_t NUMBER_OF_CHANNEL_PER_BOARD =
      NUMBER_OF_CHANNEL_PER_FEAST * NUMBER_OF_FEAST_PER_BOARD;
  static const uint16_t NUMBER_OF_CELLS_PER_BOARD = 36;
  static const uint16_t INVALID_BOARD = 20;
  static const uint16_t INVALID_FEAST = NUMBER_OF_FEAST_PER_BOARD;
  static const uint16_t INVALID_CHANNEL = NUMBER_OF_CHANNEL_PER_BOARD;
  static const uint64_t INVALID_TIME = 0xFFFFFFFFFFFFFFFF;

  static const uint64_t NUMBER_OF_LAYERS = 9;
  static const uint64_t NUMBER_OF_ROWS_PER_BOARD = 4;
  static const uint64_t TOTAL_NUMBER_OF_ROWS = 113;
  static const uint64_t NUMBER_OF_TIMES = 7;

  static const uint64_t GEOMETRIC_CELL_TYPE = 1000;
  static const uint64_t LAYER_INDEX = 0;
  static const uint64_t ROW_INDEX = 1;
  static const uint64_t PORT_INDEX = 2;

  static const uint64_t ANODIC_PORT = 2;
  static const uint64_t BOT_CATHODE_PORT = 0;
  static const uint64_t TOP_CATHODE_PORT = 1;

  static const uint64_t ANODIC_CHANNEL_TYPE = 2001;
  static const uint64_t CATHODIC_CHANNEL_TYPE = 2002;

  static const uint64_t SLOT_INDEX = 0;
  static const uint64_t FEAST_INDEX = 1;
  static const uint64_t CHANNEL_INDEX = 2;
};

}  // namespace fecom

#endif  // FECOM_TRACKER_CONSTANTS_HPP

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
