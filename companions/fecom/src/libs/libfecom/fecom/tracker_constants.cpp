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

} // namespace fecom
