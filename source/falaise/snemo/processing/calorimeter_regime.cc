// -*- mode: c++ ; -*-
/** \file falaise/snemo/processing/calorimeter_regime.cc
 */

// Ourselves:
#include <falaise/snemo/processing/calorimeter_regime.h>

// Standard library:
#include <cmath>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/clhep_units.h>
// - Bayeux/mygsl:
#include <mygsl/rng.h>

namespace snemo {

  namespace processing {

    bool calorimeter_regime::is_initialized () const
    {
      return _initialized_;
    }

    calorimeter_regime::calorimeter_regime ()
    {
      _initialized_ = false;
      _init_defaults_ ();
      return;
    }

    void calorimeter_regime::initialize (const datatools::properties & config_)
    {
      DT_THROW_IF (is_initialized (), std::logic_error, "Calorimeter regime is already initialized !");
      DT_THROW_IF (_category_.empty (), std::logic_error, "Category is not defined !");

      // Energy resolution
      {
        const std::string & key_name = this->get_category () + ".resolution";

        if (config_.has_key (key_name)) {
          _resolution_ = config_.fetch_real (key_name);
        }
      }

      // Trigger thresholds
      {
        const std::string & key_name = this->get_category () + ".high_threshold";

        if (config_.has_key (key_name)) {
          _high_threshold_ = config_.fetch_real (key_name);
          if (! config_.has_explicit_unit (key_name)){
            _high_threshold_ *= CLHEP::keV;
          }
        }
      }

      {
        const std::string & key_name = this->get_category () + ".low_threshold";

        if (config_.has_key (key_name)) {
          _low_threshold_ = config_.fetch_real (key_name);
          if (! config_.has_explicit_unit (key_name)){
            _low_threshold_ *= CLHEP::keV;
          }
        }
      }

      // Alpha quenching fit parameters
      {
        const std::string & key_name = "alpha_quenching_parameters";

        if (config_.has_key (key_name)) {
          _alpha_quenching_0_ = config_.fetch_real_vector (key_name,0);
          _alpha_quenching_1_ = config_.fetch_real_vector (key_name,1);
          _alpha_quenching_2_ = config_.fetch_real_vector (key_name,2);
        }
      }

      // Scintillator relaxation time for time resolution
      {
        const std::string & key_name = "scintillator_relaxation_time";
        if (config_.has_key (key_name)) {
          _scintillator_relaxation_time_ = config_.fetch_real (key_name);
          if (! config_.has_explicit_unit (key_name)){
            _scintillator_relaxation_time_ *= CLHEP::ns;
          }
        }
      }

      _initialized_ = true;
      return;
    }

    void calorimeter_regime::_init_defaults_ ()
    {
     // Default energy resolution:
      _resolution_ = 0.08;

      // Default trigger thresholds:
      _high_threshold_ = 150.0 * CLHEP::keV;
      _low_threshold_  = 50.0 * CLHEP::keV;

      // Default alpha quenching parameters:
      _alpha_quenching_0_ = 77.4;
      _alpha_quenching_1_ = 0.639;
      _alpha_quenching_2_ = 2.34;

      // Default scintillation relaxation time:
      _scintillator_relaxation_time_ = 6.0 * CLHEP::ns;

      // Default category is empty:
      _category_ = "";

      return;
    }

    void calorimeter_regime::reset ()
    {
      DT_THROW_IF (! is_initialized (), std::logic_error, "Not initialized !");
      _init_defaults_ ();
      _initialized_ = false;
      return;
    }

    void calorimeter_regime::set_category (const std::string & category_)
    {
      _category_ = category_;
    }

    const std::string & calorimeter_regime::get_category () const
    {
      return _category_;
    }

    double calorimeter_regime::randomize_energy (mygsl::rng & ran_, const double energy_) const
    {
      DT_THROW_IF (! is_initialized (), std::logic_error, "Not initialized !");

      const double sigma_energy  = get_sigma_energy (energy_);
      const double spread_energy = ran_.gaussian (energy_, sigma_energy);

      return (spread_energy < 0.0 ? 0.0 : spread_energy);
    }

    double calorimeter_regime::get_sigma_energy (const double energy_) const
    {
      DT_THROW_IF (! is_initialized (), std::logic_error, "Not initialized !");

      const double fwhm2sig = 1.0/(2*sqrt (2*log (2.0)));

      return _resolution_ * fwhm2sig * sqrt (energy_);
    }

    double calorimeter_regime::quench_alpha_energy (const double energy_) const
    {
      DT_THROW_IF (! is_initialized (), std::logic_error, "Not initialized !");

      const double energy = energy_ * CLHEP::MeV;

      const double par_0 = _alpha_quenching_0_;
      const double par_1 = _alpha_quenching_1_;
      const double par_2 = _alpha_quenching_2_;

      const double mod_energy = 1.0 / (par_1 * energy + 1.0);
      const double quenching_factor = -par_0 * (std::pow (mod_energy, par_2) -
                                                std::pow (mod_energy, par_2 / 2.0));

      const double quenched_energy = energy / quenching_factor;

      return quenched_energy;
    }

    double calorimeter_regime::randomize_time (mygsl::rng & ran_,
                                               const double time_,
                                               const double energy_) const
    {
      DT_THROW_IF (! is_initialized (), std::logic_error, "Not initialized !");

      const double sigma_time  = get_sigma_time (energy_);
      const double spread_time = ran_.gaussian (time_, sigma_time);

      // Negative time are physical since start time does not have
      // physical sense: return (spread_time < 0.0 ? 0.0 :
      // spread_time);
      return spread_time;
    }

    double calorimeter_regime::get_sigma_time (const double energy_) const
    {
      DT_THROW_IF (! is_initialized (), std::logic_error, "Not initialized !");

      // have a look inside Gregoire Pichenot thesis (NEMO2) and
      // L. Simard parametrization for NEMO3 simulation
      const double scin_time = _scintillator_relaxation_time_;

      const double fwhm2sig = 1.0/(2*sqrt (2*log (2.0)));
      const double sigma_e  = _resolution_ * fwhm2sig;

      const double sigma_time = scin_time * sigma_e / sqrt (energy_);

      return sigma_time;
    }

    bool calorimeter_regime::is_high_threshold (const double energy_) const
    {
      DT_THROW_IF (! is_initialized (), std::logic_error, "Not initialized !");
      return (energy_ >= _high_threshold_);
    }

    bool calorimeter_regime::is_low_threshold (const double energy_) const
    {
      DT_THROW_IF (! is_initialized (), std::logic_error, "Not initialized !");
      return (energy_ >= _low_threshold_);
    }


    void calorimeter_regime::tree_dump(std::ostream & out_,
                                       const std::string & title_,
                                       const std::string & indent_,
                                       bool inherit_) const
    {
      std::string indent;
      if(! indent_.empty()) {
        indent = indent_;
      }
      if(! title_.empty()) {
        out_ << indent << title_ << std::endl;
      }

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Initialized          : " << is_initialized () << std::endl;
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Energy resolution    = " << _resolution_ << std::endl;
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Low threshold        = " << _low_threshold_ / CLHEP::keV << " keV" << std::endl;
      out_ << indent << datatools::i_tree_dumpable::tag
           << "High threshold       = " << _high_threshold_ / CLHEP::keV << " keV" << std::endl;
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Relaxation time      = " << _scintillator_relaxation_time_ / CLHEP::ns << " ns" << std::endl;
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Alpha quenching par0 = " << _alpha_quenching_0_ << std::endl;
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Alpha quenching par1 = " << _alpha_quenching_1_ << std::endl;
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Alpha quenching par2 = " << _alpha_quenching_2_ << std::endl;
      out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "Category             = " << _category_ << std::endl;
      return;
    }

  } // end of namespace processing

} // end of namespace snemo

// end of falaise/snemo/processing/calorimeter_regime.cc
