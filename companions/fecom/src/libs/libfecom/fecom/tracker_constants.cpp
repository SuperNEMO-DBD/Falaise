// fecom/tracker_constants.cpp

// Ourselves:
#include <fecom/tracker_constants.hpp>

// Third party:
#include <bayeux/datatools/exception.h>

namespace fecom {

  // statics:
  const uint16_t tracker_constants::NUMBER_OF_CHANNEL_PER_FEAST;
  const uint16_t tracker_constants::NUMBER_OF_FEAST_PER_BOARD;
  constexpr uint16_t tracker_constants::NUMBER_OF_CHANNEL_PER_BOARD;
  const uint16_t tracker_constants::NUMBER_OF_CELLS_PER_BOARD;
  const uint16_t tracker_constants::INVALID_FEAST_INDEX;
  const uint16_t tracker_constants::INVALID_CHANNEL_INDEX;
  const uint64_t tracker_constants::INVALID_TIME;
  const uint64_t tracker_constants::GEOMETRIC_CELL_TYPE;
  const uint64_t tracker_constants::LAYER_INDEX;
  const uint64_t tracker_constants::ROW_INDEX;
  const uint64_t tracker_constants::PORT_INDEX;
  const uint64_t tracker_constants::ANODIC_PORT;
  const uint64_t tracker_constants::BOT_CATHODE_PORT;
  const uint64_t tracker_constants::TOP_CATHODE_PORT;
  const uint64_t tracker_constants::ELECTRONIC_CELL_TYPE;
  const uint64_t tracker_constants::FEAST_INDEX;
  const uint64_t tracker_constants::CHANNEL_INDEX;

} // namespace fecom
