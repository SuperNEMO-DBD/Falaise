// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/precalibrated_tracker_hit.h
/* Author(s) :    Guillaume Oliviero <oliviero@cenbg.in2p3.fr>
 *                Emmanuel Chauveau <chauveau@cenbg.in2p3.fr>
 *
 * Creation date: 2022-05-03
 * Last modified: 2023-07-20
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

      /// Return the anodic time associated to the hit
      double get_anodic_time() const;

      /// Set the anodic time associated to the hit
      void set_anodic_time(double);

      /// Check if the anodic time is valid
      bool has_anodic_time() const;

      /// Reset the anodic time
      void reset_anodic_time();


      /// Return the sigma anodic drift time associated to the hit
      double get_sigma_anodic_time() const;

      /// Set the sigma anodic drift time associated to the hit
      void set_sigma_anodic_time(double);

      /// Check if the sigma anodic drift time is valid
      bool has_sigma_anodic_time() const;

      /// Reset the sigma anodic drift time
      void reset_sigma_anodic_time();


      // /// Return the anodic drift time associated to the hit
      // double get_anodic_drift_time() const;

      // /// Set the anodic drift time associated to the hit
      // void set_anodic_drift_time(double);

      // /// Check if the anodic drift time is valid
      // bool has_anodic_drift_time() const;

      // /// Reset the anodic drift time
      // void reset_anodic_drift_time();


      // /// Return the sigma anodic drift time associated to the hit
      // double get_sigma_anodic_drift_time() const;

      // /// Set the sigma anodic drift time associated to the hit
      // void set_sigma_anodic_drift_time(double);

      // /// Check if the sigma anodic drift time is valid
      // bool has_sigma_anodic_drift_time() const;

      // /// Reset the sigma anodic drift time
      // void reset_sigma_anodic_drift_time();


      /// Return the bottom cathode drift time associated to the hit
      double get_bottom_cathode_drift_time() const;

      /// Set the bottom cathode drift time associated to the hit
      void set_bottom_cathode_drift_time(double);

      /// Check if the bottom cathode drift time is valid
      bool has_bottom_cathode_drift_time() const;

      /// Reset the bottom cathode drift time
      void reset_bottom_cathode_drift_time();


      /// Return the sigma bottom cathode drift time associated to the hit
      double get_sigma_bottom_cathode_drift_time() const;

      /// Set the sigma bottom cathode drift time associated to the hit
      void set_sigma_bottom_cathode_drift_time(double);

      /// Check if the sigma bottom cathode drift time is valid
      bool has_sigma_bottom_cathode_drift_time() const;

      /// Reset the sigma bottom cathode drift time
      void reset_sigma_bottom_cathode_drift_time();


      /// Return the top cathode drift time associated to the hit
      double get_top_cathode_drift_time() const;

      /// Set the top cathode drift time associated to the hit
      void set_top_cathode_drift_time(double);

      /// Check if the top cathode drift time is valid
      bool has_top_cathode_drift_time() const;

      /// Reset the top cathode drift time
      void reset_top_cathode_drift_time();


      /// Return the sigma top cathode drift time associated to the hit
      double get_sigma_top_cathode_drift_time() const;

      /// Set the sigma top cathode drift time associated to the hit
      void set_sigma_top_cathode_drift_time(double);

      /// Check if the sigma top cathode drift time is valid
      bool has_sigma_top_cathode_drift_time() const;

      /// Reset the sigma top cathode drift time
      void reset_sigma_top_cathode_drift_time();


      /// Return the plasma propagation time associated to the hit (bottom + top cathode times)
      double get_plasma_propagation_time() const;


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
      double _anodic_time_{datatools::invalid_real()};                      //!< Anodic absolute time of the cell
      double _sigma_anodic_time_{datatools::invalid_real()};                //!< Error on anodic absolute time of the cell
      // double _anodic_drift_time_{datatools::invalid_real()};                //!< Anodic drift time of the cell
      // double _sigma_anodic_drift_time_{datatools::invalid_real()};          //!< Error on anodic drift time of the cell
      double _bottom_cathode_drift_time_{datatools::invalid_real()};        //!< Bottom cathode drift time of the cell
      double _sigma_bottom_cathode_drift_time_{datatools::invalid_real()};  //!< Error on bottom cathode drift time of the cell
      double _top_cathode_drift_time_{datatools::invalid_real()};           //!< Top cathode drift time of the cell
      double _sigma_top_cathode_drift_time_{datatools::invalid_real()};     //!< Error on top cathode drift time of the cell

      DATATOOLS_SERIALIZATION_DECLARATION()
    };

    /// Aliases for handle and collection of precalibrated tracker hit
    using PreCalibTrackerHit = precalibrated_tracker_hit;
    using PreCalibTrackerHitCollection = std::vector<PreCalibTrackerHit>;
    using PreCalibTrackerHitHdl = datatools::handle<PreCalibTrackerHit>;
    using PreCalibTrackerHitHdlCollection = std::vector<PreCalibTrackerHitHdl>;

    using precalibrated_tracker_hit_collection = std::vector<precalibrated_tracker_hit>;
    using precalibrated_tracker_hit_handle = datatools::handle<precalibrated_tracker_hit>;
    using precalibrated_tracker_hit_handle_collection = std::vector<precalibrated_tracker_hit_handle>;

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
