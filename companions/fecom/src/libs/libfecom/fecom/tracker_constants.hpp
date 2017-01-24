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
    static const uint16_t NUMBER_OF_CHANNEL_PER_FEAST = 18;
    static const uint16_t NUMBER_OF_FEAST_PER_BOARD = 2;
    static constexpr uint16_t NUMBER_OF_CHANNEL_PER_BOARD
    = NUMBER_OF_CHANNEL_PER_FEAST
      * NUMBER_OF_FEAST_PER_BOARD;
    static const uint16_t INVALID_BOARD_CHANNEL = NUMBER_OF_CHANNEL_PER_BOARD;
    static const uint64_t INVALID_TIME = 0xFFFFFFFF;
  };

} // namespace fecom

#endif // FECOM_TRACKER_CONSTANTS_HPP

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
