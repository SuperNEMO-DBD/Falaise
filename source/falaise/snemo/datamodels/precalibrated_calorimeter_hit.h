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
 *   Precalibrated calorimeter hit.
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

      /// Return the amplitude associated to the hit
      double get_amplitude() const;

      /// Set the amplitude associated to the hit
      void set_amplitude(double);

      /// Check if the amplitude is valid
      bool has_amplitude() const;

      /// Reset the amplitude
      void reset_amplitude();

      /// Return the charge associated to the hit
      double get_charge() const;

      /// Set the charge associated to the hit
      void set_charge(double);

      /// Check if the charge is valid
      bool has_charge() const;

      /// Reset the charge
      void reset_charge();

      /// Return the time associated to the hit
      double get_time() const;

      /// Set the time associated to the hit
      void set_time(double);

      /// Check if the time is valid
      bool has_time() const;

      /// Reset the time
      void reset_time();

      /// Return the baseline associated to the hit
      double get_baseline() const;

      /// Set the baseline associated to the hit
      void set_baseline(double);

      /// Check if the baseline is valid
      bool has_baseline() const;

      /// Reset the baseline
      void reset_baseline();

      /// Return the rising_time associated to the hit
      double get_rising_time() const;

      /// Set the rising_time associated to the hit
      void set_rising_time(double);

      /// Check if the rising_time is valid
      bool has_rising_time() const;

      /// Reset the rising_time
      void reset_rising_time();

      /// Return the falling_time associated to the hit
      double get_falling_time() const;

      /// Set the falling_time associated to the hit
      void set_falling_time(double);

      /// Check if the falling_time is valid
      bool has_falling_time() const;

      /// Reset the falling_time
      void reset_falling_time();

      /// Return the width associated to the hit
      double get_width() const;

      /// Set the width associated to the hit
      void set_width(double);

      /// Check if the width is valid
      bool has_width() const;

      /// Reset the width
      void reset_width();

      /// Check if the internal data of the hit are valid
      bool is_valid() const override;

      /// Invalidate the internal data of hit
      void invalidate() override;

      /// Smart print
      virtual void tree_dump(std::ostream& out = std::clog, const std::string& title = "",
                             const std::string& indent = "", bool is_last = false) const override;


    private:

      double _amplitude_{datatools::invalid_real()};    //!< Amplitude associated to the pulse
      double _charge_{datatools::invalid_real()};       //!< Charge associated to the pulse
      double _time_{datatools::invalid_real()};         //!< Time associated to the pulse
      double _baseline_{datatools::invalid_real()};     //!< Baseline associated to the pulse
      double _rising_time_{datatools::invalid_real()};  //!< Rising time associated to the pulse
      double _falling_time_{datatools::invalid_real()}; //!< Falling time associated to the pulse
      double _width_{datatools::invalid_real()};        //!< Width of the pulse

      DATATOOLS_SERIALIZATION_DECLARATION()
    };

    /// Alias for a handle of precalibrated calorimeter hit
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
