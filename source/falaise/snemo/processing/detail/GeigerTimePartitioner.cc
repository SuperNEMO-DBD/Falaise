/// \file falaise/TrackerPreClustering/GeigerTimePartitioner.cc

// Ourselves:
#include "GeigerTimePartitioner.h"

// Standard library:
#include <cstdlib>
#include <limits>
#include <sstream>
#include <stdexcept>

// Third party:
// - Bayeux/datatools:
#include <bayeux/datatools/exception.h>

namespace snreco {
namespace detail {
GeigerTimePartitioner::GeigerTimePartitioner(double timeGap, bool classifyPrompt,
                                             bool classifyDelayed, bool splitChamber)
    : maxDelayedHitTimeGap_{timeGap},
      classifyPromptHits_{classifyPrompt},
      classifyDelayedHits_{classifyDelayed},
      isSplitChamber_{splitChamber} {
  DT_THROW_IF(maxDelayedHitTimeGap_ <= 1.0 * CLHEP::microsecond, std::logic_error,
              "Cannot cluster hits with a gap of < 1.0us ("
                  << maxDelayedHitTimeGap_ / CLHEP::microsecond << " supplied)");
}

double GeigerTimePartitioner::getMaxDelayedHitTimeGap() const { return maxDelayedHitTimeGap_; }

bool GeigerTimePartitioner::classifiesPromptHits() const { return classifyPromptHits_; }

bool GeigerTimePartitioner::classifiesDelayedHits() const { return classifyDelayedHits_; }

bool GeigerTimePartitioner::isSplitChamber() const { return isSplitChamber_; }

}  // namespace detail
}  // namespace snreco
