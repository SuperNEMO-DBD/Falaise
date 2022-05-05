// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/precalibrated_tracker_hit.h
/* Author(s) :    Guillaume Oliviero <goliviero@ucl.ac.uk>
 *                Emmanuel Chauveau <chauveau@cenbg.in2p3.fr>
 * Creation date: 2022-05-03
 * Last modified: 2022-05-03
 *
 * Description:
 *
 *   Precalibrated tracker hit.
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

    /// \brief Model of a precalibrated tracker hit
    class precalibrated_tracker_hit : public geomtools::base_hit {

    public:

      /// Return the drift reference_time associated to the hit
      double get_reference_time() const;

      /// Set the drift reference_time associated to the hit
      void set_reference_time(double);

      /// Check if the drift reference_time is valid
      bool has_reference_time() const;

      /// Reset the drift reference_time
      void reset_reference_time();

      /// Return the drift time associated to the hit
      double get_anodic_drift_time() const;

      /// Set the drift time associated to the hit
      void set_anodic_drift_time(double);

      /// Check if the drift time is valid
      bool has_anodic_drift_time() const;

      /// Reset the drift time
      void reset_anodic_drift_time();

      /// Return the drift time associated to the hit
      double get_bottom_cathode_drift_time() const;

      /// Set the drift time associated to the hit
      void set_bottom_cathode_drift_time(double);

      /// Check if the drift time is valid
      bool has_bottom_cathode_drift_time() const;

      /// Reset the drift time
      void reset_bottom_cathode_drift_time();

      /// Return the drift time associated to the hit
      double get_top_cathode_drift_time() const;

      /// Set the drift time associated to the hit
      void set_top_cathode_drift_time(double);

      /// Check if the drift time is valid
      bool has_top_cathode_drift_time() const;

      /// Reset the drift time
      void reset_top_cathode_drift_time();

      /// Return the plasma propagation time associated to the hit (bottom + top cathode times)
      double get_plasma_propagation_time() const;

      /// Check if the hit is marked as delayed
      bool is_delayed() const;

      /// Mark/unmark the hit as delayed
      void set_delayed(bool);

      /// Check if the internal data of the hit are valid
      bool is_valid() const override;

      /// Invalidate calibration informations stored in the hit
      void invalidate() override;

      /// Invalidate calibration informations stored in the hit
      virtual void clear() override;

      /// Smart print method
      virtual void tree_dump(std::ostream& out = std::clog, const std::string& title = "",
                             const std::string& indent = "", bool is_last = false) const override;

    private:

      double _reference_time_{datatools::invalid_real()}; //!< Time reference T0 (from calorimeter or tracker depending of the hypothesis)
      double _anodic_drift_time_{datatools::invalid_real()}; //!< Anodic drift time of the cell
      double _bottom_cathode_drift_time_{datatools::invalid_real()}; //!< Bottom cathode drift time of the cell
      double _top_cathode_drift_time_{datatools::invalid_real()}; //!< Top cathode drift time of the cell
      bool   _is_delayed_ = false;  //!< Check if a cell is delayed


      DATATOOLS_SERIALIZATION_DECLARATION()
    };

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
