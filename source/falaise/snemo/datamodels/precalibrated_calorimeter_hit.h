// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/precalibrated_calorimeter_hit.h
/* Author(s) :    Guillaume Oliviero <goliviero@ucl.ac.uk>
 *                Emmanuel Chauveau <chauveau@cenbg.in2p3.fr>
 * Creation date: 2022-05-03
 * Last modified: 2022-05-03
 *
 * License:
 *
 * Description:
 *   Precalibrated calorimeter hit
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_DATAMODELS_PRECALIBRATED_CALORIMETER_HIT_H
#define FALAISE_SNEMO_DATAMODELS_PRECALIBRATED_CALORIMETER_HIT_H 1

// Standard library:
#include <vector>

// Third party:
// - Bayeux/datatools:
#include <datatools/handle.h>

// - Bayeux/geomtools:
#include <geomtools/base_hit.h>

namespace snemo {

  namespace datamodel {

    /// \brief Model of a precalibrated calorimeter hit
    class precalibrated_calorimeter_hit : public geomtools::base_hit {
    public:
      // /// Return the time associated to the hit
      // double get_time() const;

      /// Check if the internal data of the hit are valid
      bool is_valid() const override;

      /// Invalidate the internal data of hit
      void invalidate() override;

      /// Smart print
      virtual void tree_dump(std::ostream& out = std::clog, const std::string& title = "",
                             const std::string& indent = "", bool is_last = false) const override;


      struct reconstructed_pulse {

      public:

        /// Return the energy associated to the hit
        double get_amplitude() const;

        /// Set the energy associated to the hit
        void set_amplitude(double);

        /// Check if the amplitude is valid
        bool has_amplitude() const;

        /// Reset the amplitude
        void reset_amplitude();



      private:
        double amplitude_{datatools::invalid_real()};    //!< Amplitude associated to the pulse
        double charge_{datatools::invalid_real()};       //!< Charge associated to the pulse
        double time_{datatools::invalid_real()};         //!< Time (in ns) associated to the pulse
        double baseline_{datatools::invalid_real()};     //!< Baseline (in ns) associated to the pulse
        double rising_time_{datatools::invalid_real()};  //!< Rising time (in ns) associated to the pulse
        double falling_time_{datatools::invalid_real()}; //!< Falling time (in ns) associated to the pulse
        double width_{datatools::invalid_real()};        //!< Width of the pulse
      };

    private:

      std::vector<reconstructed_pulse>  reco_pulses_models_; // Same pulse in a calo hit but different reconstruction models,  [0] of each vector -> direct from RED fwmeasurements


      DATATOOLS_SERIALIZATION_DECLARATION()
    };

    /// Handle of precalibrated calorimeter hit
    // typedef datatools::handle<precalibrated_calorimeter_hit> handle_type;
    /// Collection of handles of precalibrated calorimeter hit
    // typedef std::vector<handle_type> collection_type;
    using PreCalibCalorimeterHit = precalibrated_calorimeter_hit;
    using PreCalibCalorimeterHitCollection = std::vector<PreCalibCalorimeterHit>;

    using PreCalibCalorimeterHitHdl = datatools::handle<PreCalibCalorimeterHit>;
    using PreCalibCalorimeterHitHdlCollection = std::vector<PreCalibCalorimeterHitHdl>;

  }  // end of namespace datamodel

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_DATAMODELS_PRECALIBRATED_CALORIMETER_HIT_H
