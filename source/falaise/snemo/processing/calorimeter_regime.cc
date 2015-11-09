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

    // static
    const double & calorimeter_regime::default_energy_resolution()
    {
      static double _r(8. * CLHEP::perCent);
      return _r;
    }

    // static
    const double & calorimeter_regime::default_low_energy_threshold()
    {
      static double _th(50. * CLHEP::keV);
      return _th;
    }

    // static
    const double & calorimeter_regime::default_high_energy_threshold()
    {
      static double _th(150. * CLHEP::keV);
      return _th;
    }

    // static
    const double & calorimeter_regime::default_scintillator_relaxation_time()
    {
      static double _t(6. * CLHEP::ns);
      return _t;
    }

    bool calorimeter_regime::is_initialized() const
    {
      return _initialized_;
    }

    calorimeter_regime::calorimeter_regime()
    {
      _initialized_ = false;
      _init_defaults_();
      return;
    }

    void calorimeter_regime::initialize(const datatools::properties & config_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Calorimeter regime is already initialized !");

      const double energy_unit = CLHEP::keV;
      const double time_unit   = CLHEP::ns;

      // Energy resolution
      {
        const std::string key_name = "energy.resolution";
        if (config_.has_key(key_name)) {
          _resolution_ = config_.fetch_real(key_name);
          if (! config_.has_explicit_unit(key_name)){
            _resolution_ *= CLHEP::perCent;
          }
        }
      }

      // Trigger thresholds
      {
        const std::string key_name = "energy.high_threshold";
        if (config_.has_key(key_name)) {
          _high_threshold_ = config_.fetch_real(key_name);
          if (! config_.has_explicit_unit(key_name)){
            _high_threshold_ *= energy_unit;
          }
        }
      }

      {
        const std::string key_name = "energy.low_threshold";
        if (config_.has_key(key_name)) {
          _low_threshold_ = config_.fetch_real(key_name);
          if (! config_.has_explicit_unit(key_name)){
            _low_threshold_ *= energy_unit;
          }
        }
      }

      // Alpha quenching fit parameters
      {
        const std::string key_name = "alpha_quenching_parameters";
        if (config_.has_key(key_name)) {
          _alpha_quenching_0_ = config_.fetch_real_vector(key_name,0);
          _alpha_quenching_1_ = config_.fetch_real_vector(key_name,1);
          _alpha_quenching_2_ = config_.fetch_real_vector(key_name,2);
        }
      }

      // Scintillator relaxation time for time resolution
      {
        const std::string key_name = "scintillator_relaxation_time";
        if (config_.has_key(key_name)) {
          _scintillator_relaxation_time_ = config_.fetch_real(key_name);
          if (! config_.has_explicit_unit(key_name)){
            _scintillator_relaxation_time_ *= time_unit;
          }
        }
      }

      _initialized_ = true;
      return;
    }

    void calorimeter_regime::_init_defaults_()
    {
      // Default energy resolution:
      _resolution_ = default_energy_resolution();

      // Default trigger thresholds:
      _high_threshold_ = default_high_energy_threshold();
      _low_threshold_  = default_low_energy_threshold();

      // Default scintillation relaxation time:
      _scintillator_relaxation_time_ = default_scintillator_relaxation_time();

      // Default alpha quenching parameters:
      _alpha_quenching_0_ = 77.4;
      _alpha_quenching_1_ =  0.639;
      _alpha_quenching_2_ =  2.34;

      // Default category is empty:
      _category_ = "";

      return;
    }

    void calorimeter_regime::reset()
    {
      DT_THROW_IF(! is_initialized(), std::logic_error, "Not initialized !");
      _init_defaults_();
      _initialized_ = false;
      return;
    }

    void calorimeter_regime::set_category(const std::string & category_)
    {
      _category_ = category_;
    }

    const std::string & calorimeter_regime::get_category() const
    {
      return _category_;
    }

    double calorimeter_regime::randomize_energy(mygsl::rng & ran_, const double energy_) const
    {
      DT_THROW_IF(! is_initialized(), std::logic_error, "Not initialized !");

      // 2015-01-08 XG: Implement a better energy calibration based on Poisson
      // statistics for the number of photons inside scintillator. This
      // technique should be more accurate for low energy deposit.
      const double fwhm2sig = 2*sqrt(2*log(2.0));
      const double nrj2photon = std::pow(fwhm2sig/_resolution_, 2);
      const double mu = energy_ / CLHEP::MeV * nrj2photon;
      const double spread_energy = ran_.poisson(mu) / nrj2photon;

      return spread_energy;
    }

    double calorimeter_regime::get_sigma_energy(const double energy_) const
    {
      DT_THROW_IF(! is_initialized(), std::logic_error, "Not initialized !");

      const double fwhm2sig = 1.0/(2*sqrt(2*log(2.0)));

      return _resolution_ * fwhm2sig * sqrt(energy_ / CLHEP::MeV);
    }

    double calorimeter_regime::quench_alpha_energy(const double energy_) const
    {
      DT_THROW_IF(! is_initialized(), std::logic_error, "Not initialized !");

      const double energy = energy_ * CLHEP::MeV;

      const double par_0 = _alpha_quenching_0_;
      const double par_1 = _alpha_quenching_1_;
      const double par_2 = _alpha_quenching_2_;

      const double mod_energy = 1.0 /(par_1 * energy + 1.0);
      const double quenching_factor = -par_0 *(std::pow(mod_energy, par_2) -
                                               std::pow(mod_energy, par_2 / 2.0));

      const double quenched_energy = energy / quenching_factor;

      return quenched_energy;
    }

    double calorimeter_regime::randomize_time(mygsl::rng & ran_,
                                              const double time_,
                                              const double energy_) const
    {
      DT_THROW_IF(! is_initialized(), std::logic_error, "Not initialized !");

      const double sigma_time  = get_sigma_time(energy_);
      const double spread_time = ran_.gaussian(time_, sigma_time);

      // Negative time are physical since start time does not have
      // physical sense: return (spread_time < 0.0 ? 0.0 : spread_time);
      return spread_time;
    }

    double calorimeter_regime::get_sigma_time(const double energy_) const
    {
      DT_THROW_IF(! is_initialized(), std::logic_error, "Not initialized !");

      // Have a look inside Gregoire Pichenot thesis(NEMO2) and
      // L. Simard parametrization for NEMO3 simulation
      const double scin_time = _scintillator_relaxation_time_;

      const double fwhm2sig = 1.0/(2*sqrt(2*log(2.0)));
      const double sigma_e  = _resolution_ * fwhm2sig;

      const double sigma_time = scin_time * sigma_e / sqrt(energy_ / CLHEP::MeV);

      return sigma_time;
    }

    bool calorimeter_regime::is_high_threshold(const double energy_) const
    {
      DT_THROW_IF(! is_initialized(), std::logic_error, "Not initialized !");
      return(energy_ >= _high_threshold_);
    }

    bool calorimeter_regime::is_low_threshold(const double energy_) const
    {
      DT_THROW_IF(! is_initialized(), std::logic_error, "Not initialized !");
      return(energy_ >= _low_threshold_);
    }


    void calorimeter_regime::tree_dump(std::ostream & out_,
                                       const std::string & title_,
                                       const std::string & indent_,
                                       bool inherit_) const
    {
      std::string indent;
      if (! indent_.empty()) {
        indent = indent_;
      }
      if (! title_.empty()) {
        out_ << indent << title_ << std::endl;
      }

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Initialized          : " << is_initialized() << std::endl;
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Energy resolution     = " << _resolution_ / CLHEP::perCent << " %"  << std::endl;
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Low energy threshold  = " << _low_threshold_ / CLHEP::keV << " keV" << std::endl;
      out_ << indent << datatools::i_tree_dumpable::tag
           << "High energy threshold = " << _high_threshold_ / CLHEP::keV << " keV" << std::endl;
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Relaxation time       = " << _scintillator_relaxation_time_ / CLHEP::ns << " ns" << std::endl;
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Alpha quenching par0  = " << _alpha_quenching_0_ << std::endl;
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Alpha quenching par1  = " << _alpha_quenching_1_ << std::endl;
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Alpha quenching par2  = " << _alpha_quenching_2_ << std::endl;
      out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "Category              = " << _category_ << std::endl;
      return;
    }

  } // end of namespace processing

} // end of namespace snemo

/********************************
 * OCD support : implementation *
 ********************************/

#include <datatools/object_configuration_description.h>

/** Opening macro for implementation
 *  @arg snemo::processing::calorimeter_regime  the full class name
 *  @arg ocd_ is the identifier of the 'datatools::object_configuration_description'
 *            to be initialized (passed by mutable reference).
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::processing::calorimeter_regime,ocd_)
{
  ocd_.set_class_name("snemo::processing::calorimeter_regime");
  ocd_.set_class_description("This object describes the calorimeter regime of SuperNEMO optical line");
  ocd_.set_class_library("falaise");
  //ocd_.set_class_documentation("");

  {
    // Description of the 'energy.resolution' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("energy.resolution")
      .set_terse_description("The optical line energy resolution for electrons at 1 MeV")
      .set_traits(datatools::TYPE_REAL)
      .set_explicit_unit(true)
      .set_unit_label("fraction")
      .set_unit_symbol("%")
      .set_default_value_real(snemo::processing::calorimeter_regime::default_energy_resolution(), "%")
      .add_example("Set the default value::                          \n"
                   "                                                 \n"
                   "  energy.resolution : real as fraction = 8 %     \n"
                   "                                                 \n"
                   )
      ;
  }

  {
    // Description of the 'energy.low_threshold' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("energy.low_threshold")
      .set_terse_description("The optical line low energy threshold")
      .set_traits(datatools::TYPE_REAL)
      //.set_long_description("Default value: ``50 keV``             \n")
      .set_explicit_unit(true)
      .set_unit_label("energy")
      .set_unit_symbol("keV")
      .set_default_value_real(snemo::processing::calorimeter_regime::default_low_energy_threshold(), "keV")
      .add_example("Set the default value::                          \n"
                   "                                                 \n"
                   "  energy.low_threshold : real as energy = 50 keV \n"
                   "                                                 \n"
                   )
      ;
  }

  {
    // Description of the 'energy.high_threshold' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("energy.high_threshold")
      .set_terse_description("The optical line high energy threshold")
      .set_traits(datatools::TYPE_REAL)
      //.set_long_description("Default value: ``150 keV``           \n")
      .set_explicit_unit(true)
      .set_unit_label("energy")
      .set_unit_symbol("keV")
      .set_default_value_real(snemo::processing::calorimeter_regime::default_high_energy_threshold(), "keV")
      .add_example("Set the default value::                            \n"
                   "                                                   \n"
                   "  energy.high_threshold : real as energy = 150 keV \n"
                   "                                                   \n"
                   )
      ;
  }

  {
    // Description of the 'scintillator_relaxation_time' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("scintillator_relaxation_time")
      .set_terse_description("The scintillator relaxation time")
      .set_traits(datatools::TYPE_REAL)
      .set_long_description("This parameter is used to compute the time resolution of the calorimeter. \n")
      .set_explicit_unit(true)
      .set_unit_label("time")
      .set_unit_symbol("ns")
      .set_default_value_real(snemo::processing::calorimeter_regime::default_scintillator_relaxation_time(), "ns")
      .add_example("Set the default value::                                \n"
                   "                                                       \n"
                   "  scintillator_relaxation_time : real as time = 6.0 ns \n"
                   "                                                       \n"
                   )
      ;
  }

  {
    // Description of the 'alpha_quenching_parameters' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("alpha_quenching_parameters")
      .set_terse_description("Activation of the quenching for alpha particles")
      .set_traits(datatools::TYPE_REAL,
                  configuration_property_description::ARRAY,
                  3)
      .set_long_description("The current implementation use a fit with 3 parameters. \n"
                            "The default dimensionless values are: ``77.4  0.639  2.34``    \n")
      .set_explicit_unit(false)
      .add_example("Set the default value::                     \n"
                   "                                            \n"
                   "  alpha_quenching_parameters : real[3] = \\ \n"
                   "      77.4  0.639  2.34                     \n"
                   "                                            \n"
                   )
      ;
  }


  // Additionnal configuration hints :
  ocd_.set_configuration_hints("Here is a full configuration example in the      \n"
                               "``datatools::properties`` ASCII format::         \n"
                               "                                                 \n"
                               "  energy.resolution     : real as fraction = 8 %         \n"
                               "  energy.low_threshold  : real as energy = 50 keV        \n"
                               "  energy.high_threshold : real as energy = 150 keV       \n"
                               "  scintillator_relaxation_time : real as time = 6.0 ns   \n"
                               "  alpha_quenching_parameters : real[3] = 77.4 0.639 2.34 \n"
                               "                                                         \n"
                               );

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END() // Closing macro for implementation

// Registration macro for class 'snemo::processing::calorimeter_regime' :
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::processing::calorimeter_regime,"snemo::processing::calorimeter_regime")

// end of falaise/snemo/processing/calorimeter_regime.cc
