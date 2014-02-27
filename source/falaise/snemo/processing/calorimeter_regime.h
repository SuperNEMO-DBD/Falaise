// -*- mode: c++ ; -*-
/** \file falaise/snemo/processing/calorimeter_regime.h
 * Author (s) :     Mathieu Bongrand <bongrand@lal.in2p3.fr>
 *                  Xavier Garrido <garrido@lal.in2p3.fr>
 * Creation date: 2011-01-12
 * Last modified: 2014-02-24
 *
 * License:
 *
 * Description:
 *
 *   Tools to model the calorimeter regime in SuperNEMO.
 *
 * History:
 *
 */
#ifndef FALAISE_SNEMO_PROCESSING_CALORIMETER_REGIME_H
#define FALAISE_SNEMO_PROCESSING_CALORIMETER_REGIME_H 1

// Standard library:
#include <string>
#include <iostream>

// Third party
// - Bayeux/datatools
#include <datatools/i_tree_dump.h>

namespace datatools {
  class properties;
}

namespace mygsl {
  class rng;
}

namespace snemo {

  namespace processing {

    /// \brief Simple modelling of the energy and time measurement with the SuperNEMO calorimeter optical lines
    class calorimeter_regime : public datatools::i_tree_dumpable
    {
    public:

      /// Return the default energy resolution
      static const double & default_energy_resolution();

      /// Return the default low energy threshold
      static const double & default_low_energy_threshold();

      /// Return the default high energy threshold
      static const double & default_high_energy_threshold();

      /// Return the default scintillator relaxation time
      static const double & default_scintillator_relaxation_time();

      /// Check initialization flag
      bool is_initialized() const;

      /// Default constructor
      calorimeter_regime();

      /// Initialization from parameters
      void initialize(const datatools::properties & config_);

      /// Reset
      void reset();

      /// Randomize the measured energy value given the true energy
      double randomize_energy(mygsl::rng & ran_, const double energy_) const;

      /// Return the error on energy
      double get_sigma_energy(const double energy_) const;

      /// Compute the effective quenched energy for alpha particle
      double quench_alpha_energy(const double energy_) const;

      /// Randomize the measured time value given the true time and energy
      double randomize_time(mygsl::rng & ran_,
                             const double time_, const double energy_) const;

      /// Return the error on time
      double get_sigma_time(const double energy_) const;

      /// Check if a given energy passes the high threshold
      bool is_high_threshold(const double energy_) const;

      /// Check if a given energy passes the low threshold
      bool is_low_threshold (const double energy_) const;

      /// Set the category of the optical modules associated to this calorimeter regime
      void set_category(const std::string & category_);

      /// Return the category of the optical modules associated to this calorimeter regime
      const std::string & get_category() const;

      /// Smart print
      virtual void tree_dump(std::ostream & a_out         = std::clog,
                              const std::string & a_title  = "",
                              const std::string & a_indent = "",
                              bool a_inherit               = false) const;

    private:

      bool        _initialized_;       //!< Initialization flag
      double      _resolution_;        //!< Energy resolution for electrons at 1 MeV
      double      _high_threshold_;    //!< High energy threshold
      double      _low_threshold_;     //!< Low energy threshold
      double      _alpha_quenching_0_; //!< Parameter 0 for alpha quenching
      double      _alpha_quenching_1_; //!< Parameter 1 for alpha quenching
      double      _alpha_quenching_2_; //!< Parameter 2 for alpha quenching
      double      _scintillator_relaxation_time_; //!< Scintillator relaxation time
      std::string _category_; //!< The category of the optical modules associated to this calorimeter regime

    private:

      void _init_defaults_();
    };

  } // end of namespace processing

} // end of namespace snemo

/***************************
 * OCD support : interface *
 ***************************/

#include <datatools/ocd_macros.h>

// @arg snemo::processing::calorimeter_regime the name the registered class
DOCD_CLASS_DECLARATION(snemo::processing::calorimeter_regime)

#endif // FALAISE_SNEMO_PROCESSING_CALORIMETER_REGIME_H

// end of falaise/snemo/processing/calorimeter_regime.h
