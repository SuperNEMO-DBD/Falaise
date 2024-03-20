// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/precalibrated_calorimeter_hit.h
/* Author(s) :    Guillaume Oliviero <oliviero@cenbg.in2p3.fr>
 *                Emmanuel Chauveau <chauveau@cenbg.in2p3.fr>
 * Creation date: 2022-05-03
 * Last modified: 2024-03-13
 *
 * Description:
 *
 *   Precalibrated calorimeter hit.
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
    class precalibrated_calorimeter_hit
      : public geomtools::base_hit
    {

    public:

      /// Return the baseline associated to the hit
      double get_baseline() const;

      /// Set the baseline associated to the hit
      void set_baseline(double);

      /// Check if the baseline is valid
      bool has_baseline() const;

      /// Reset the baseline
      void reset_baseline();

      /// Return the error on the baseline associated to the hit
      double get_sigma_baseline() const;

      /// Set the error on the baseline associated to the hit
      void set_sigma_baseline(double);

      /// Check if the error on the baseline is valid
      bool has_sigma_baseline() const;

      /// Reset the error on the baseline
      void reset_sigma_baseline();

      /// Return the amplitude associated to the hit
      double get_amplitude() const;

      /// Set the amplitude associated to the hit
      void set_amplitude(double);

      /// Check if the amplitude is valid
      bool has_amplitude() const;

      /// Reset the amplitude
      void reset_amplitude();

      /// Return the error on the amplitude associated to the hit
      double get_sigma_amplitude() const;

      /// Set the error on the amplitude associated to the hit
      void set_sigma_amplitude(double);

      /// Check if the error on the amplitude is valid
      bool has_sigma_amplitude() const;

      /// Reset the error on the amplitude
      void reset_sigma_amplitude();

      /// Return the charge associated to the hit
      double get_charge() const;

      /// Set the charge associated to the hit
      void set_charge(double);

      /// Check if the charge is valid
      bool has_charge() const;

      /// Reset the charge
      void reset_charge();

      /// Return the error on the charge associated to the hit
      double get_sigma_charge() const;

      /// Set the error on the charge associated to the hit
      void set_sigma_charge(double);

      /// Check if the error on the charge is valid
      bool has_sigma_charge() const;

      /// Reset the error on the charge
      void reset_sigma_charge();

      /// Return the time associated to the hit
      double get_time() const;

      /// Set the time associated to the hit
      void set_time(double);

      /// Check if the time is valid
      bool has_time() const;

      /// Reset the time
      void reset_time();

      /// Return the error on the time associated to the hit
      double get_sigma_time() const;

      /// Set the error on the time associated to the hit
      void set_sigma_time(double);

      /// Check if the error on the time is valid
      bool has_sigma_time() const;

      /// Reset the error on the time
      void reset_sigma_time();

      /// Check if the internal data of the hit are valid
      bool is_valid() const override;

      /// Invalidate the internal data of hit
      void invalidate() override;

      /// Smart print
      void print_tree(std::ostream & out_ = std::clog,
                      const boost::property_tree::ptree & options_ = empty_options()) const override;
 
    private:

      double _baseline_{datatools::invalid_real()};           //!< Baseline associated to the hit
      double _sigma_baseline_{datatools::invalid_real()};     //!< Error on the baseline associated to the hit
      double _amplitude_{datatools::invalid_real()};          //!< Amplitude associated to the hit
      double _sigma_amplitude_{datatools::invalid_real()};    //!< Error on the amplitude associated to the hit
      double _charge_{datatools::invalid_real()};             //!< Charge associated to the hit
      double _sigma_charge_{datatools::invalid_real()};       //!< Error on the charge associated to the hit
      double _time_{datatools::invalid_real()};               //!< Time associated to the hit
      double _sigma_time_{datatools::invalid_real()};         //!< Error on the time associated to the hit

      DATATOOLS_SERIALIZATION_DECLARATION()
    };

    // Aliases for handle and collection of precalibrated calorimeter hit
    using PreCalibratedCalorimeterHit = precalibrated_calorimeter_hit;
    using PreCalibratedCalorimeterHitCollection = std::vector<PreCalibratedCalorimeterHit>;
    using PreCalibratedCalorimeterHitHdl = datatools::handle<PreCalibratedCalorimeterHit>;
    using PreCalibratedCalorimeterHitHdlCollection = std::vector<PreCalibratedCalorimeterHitHdl>;

    using precalibrated_calorimeter_hit_collection = std::vector<precalibrated_calorimeter_hit>;
    using precalibrated_calorimeter_hit_handle = datatools::handle<precalibrated_calorimeter_hit>;
    using precalibrated_calorimeter_hit_handle_collection = std::vector<precalibrated_calorimeter_hit_handle>;

  } // end of namespace datamodel

} // end of namespace snemo

#endif // FALAISE_SNEMO_DATAMODELS_PRECALIBRATED_CALORIMETER_HIT_H
