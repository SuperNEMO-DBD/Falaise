// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/precalibrated_tracker_hit.h
/* Author(s) :    Guillaume Oliviero <goliviero@ucl.ac.uk>
 *                Emmanuel Chauveau <chauveau@cenbg.in2p3.fr>
 * Creation date: 2022-05-03
 * Last modified: 2022-05-03
 *
 * Description:
 *
 *   Precalibrated tracker hit
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_DATAMODELS_PRECALIBRATED_TRACKER_HIT_H
#define FALAISE_SNEMO_DATAMODELS_PRECALIBRATED_TRACKER_HIT_H 1

// Standard library:
#include <string>
#include <vector>

// Third party:
// - Bayeux/datatools:
#include <datatools/handle.h>
// - Bayeux/geomtools:
#include <geomtools/base_hit.h>

namespace snemo {

  namespace datamodel {

    /// \brief Model of a precalibrated tracker hit (Geiger regime)
    class precalibrated_tracker_hit : public geomtools::base_hit {
    public:

      /// Check if minimal calibration informations are present to consider the hit as valid and usable
      bool is_valid() const override;

      /// Invalidate calibration informations stored in the hit
      void invalidate() override;

      /// Invalidate calibration informations stored in the hit
      virtual void clear() override;

      /// Smart print method
      virtual void tree_dump(std::ostream& out = std::clog, const std::string& title = "",
                             const std::string& indent = "", bool is_last = false) const override;


    private:
      // double r_{datatools::invalid_real()};        //!< Transverse drift distance
      // double sigma_r_{datatools::invalid_real()};  //!< Transverse drift distance error

      DATATOOLS_SERIALIZATION_DECLARATION()
    };


    /// Alias for a handle on a precalibrated tracker hit
    // typedef datatools::handle<precalibrated_tracker_hit> handle_type;
    /// Alias for a collection of handles on precalibrated tracker hits
    // typedef std::vector<handle_type> collection_type;

    using PreCalibTrackerHit = precalibrated_tracker_hit;
    using PreCalibTrackerHitCollection = std::vector<PreCalibTrackerHit>;

    using PreCalibTrackerHitHdl = datatools::handle<PreCalibTrackerHit>;
    using PreCalibTrackerHitHdlCollection = std::vector<PreCalibTrackerHitHdl>;

  }  // end of namespace datamodel

}  // end of namespace snemo

// Class version:
#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(snemo::datamodel::precalibrated_tracker_hit, 1)

#endif  // FALAISE_SNEMO_DATAMODELS_PRECALIBRATED_TRACKER_HIT_H
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
