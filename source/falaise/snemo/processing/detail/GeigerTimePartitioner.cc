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
#include <bayeux/datatools/logger.h>

namespace snreco {
  namespace detail {
    
    GeigerTimePartitioner::GeigerTimePartitioner(double timeGap, bool classifyPrompt,
                                                 bool classifyDelayed, bool splitChamber)
      : maxDelayedHitTimeGap_{timeGap},
        classifyPromptHits_{classifyPrompt},
        classifyDelayedHits_{classifyDelayed},
        isSplitChamber_{splitChamber}
    {
      auto logging = datatools::logger::PRIO_FATAL;
      logging = datatools::logger::PRIO_DEBUG;
      DT_THROW_IF(maxDelayedHitTimeGap_ <= 1.0 * CLHEP::microsecond, std::logic_error,
                  "Cannot cluster hits with a gap of < 1.0us ("
                  << maxDelayedHitTimeGap_ / CLHEP::microsecond << " us supplied)");
      DT_LOG_DEBUG(logging, "maxDelayedHitTimeGap  = " << maxDelayedHitTimeGap_ / CLHEP::microsecond << " us");
      DT_LOG_DEBUG(logging, "classifyPromptHits_   = " << std::boolalpha << classifyPromptHits_);
      DT_LOG_DEBUG(logging, "classifyDelayedHits_  = " << std::boolalpha << classifyDelayedHits_);
      DT_LOG_DEBUG(logging, "isSplitChamber_       = " << std::boolalpha << isSplitChamber_);
    }

    double GeigerTimePartitioner::getMaxDelayedHitTimeGap() const { return maxDelayedHitTimeGap_; }

    bool GeigerTimePartitioner::classifiesPromptHits() const { return classifyPromptHits_; }

    bool GeigerTimePartitioner::classifiesDelayedHits() const { return classifyDelayedHits_; }

    bool GeigerTimePartitioner::isSplitChamber() const { return isSplitChamber_; }

  }  // namespace detail
}  // namespace snreco
