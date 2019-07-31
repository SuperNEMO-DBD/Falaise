// -*- mode: c++ ; -*-
/** \file falaise/snemo/processing/CalorimeterModel.cc
 */

// Ourselves:
#include <falaise/snemo/processing/calorimeter_regime.h>

// Standard library:
#include <cmath>

// Third party:
// - Bayeux/datatools:
#include <bayeux/datatools/clhep_units.h>
#include <bayeux/datatools/exception.h>
// - Bayeux/mygsl:

namespace {
const double fwhm2sig{1.0 / (2 * sqrt(2 * log(2.0)))};
}

namespace snemo {

namespace processing {

CalorimeterModel::CalorimeterModel(falaise::property_set const& ps)
    : CalorimeterModel::CalorimeterModel() {
  if (ps.has_key("energy.resolution")) {
    energyResolution = ps.get<falaise::fraction_t>("energy.resolution")();
  }

  if (ps.has_key("energy.high_threshold")) {
    highEnergyThreshold = ps.get<falaise::energy_t>("energy.high_threshold")();
  }

  if (ps.has_key("energy.low_threshold")) {
    lowEnergyThreshold = ps.get<falaise::energy_t>("energy.low_threshold")();
  }

  const std::string key_name = "alpha_quenching_parameters";
  if (ps.has_key(key_name)) {
    auto tmp = ps.get<std::vector<double>>(key_name);
    DT_THROW_IF(tmp.size() != 3, std::domain_error,
                "alpha_quenching_parameters must be array of size 3");
    alphaQuenching_0 = tmp[0];
    alphaQuenching_1 = tmp[1];
    alphaQuenching_2 = tmp[2];
  }

  // Scintillator relaxation time for time resolution
  if (ps.has_key("scintillator_relaxation_time")) {
    relaxationTime = ps.get<falaise::time_t>("scintillator_relaxation_time")();
  }
}

double CalorimeterModel::smearEnergy(mygsl::rng& rng, const double energy) const {
  // 2015-01-08 XG: Implement a better energy calibration based on Poisson
  // statistics for the number of photons inside scintillator. This
  // technique should be more accurate for low energy deposit.
  // const double fwhm2sig = 2*sqrt(2*log(2.0));
  // const double nrj2photon = std::pow(fwhm2sig/energyResolution, 2);
  // const double mu = energy_ / CLHEP::MeV * nrj2photon;
  // const double spread_energy = ran_.poisson(mu) / nrj2photon;
  // return spread_energy;

  // 2016-06-01 XG: Get back to gaussian fluctuation to avoid fixed number
  // of photon-electron due to Poisson distribution
  const double sigma_energy = getSigmaEnergy(energy);
  const double spread_energy = rng.gaussian(energy, sigma_energy);
  return (spread_energy < 0.0 ? 0.0 : spread_energy);
}

double CalorimeterModel::getSigmaEnergy(const double energy) const {
  return energyResolution * fwhm2sig * sqrt(energy / CLHEP::MeV);
}

double CalorimeterModel::quenchAlphaParticle(const double energy) const {
  const double raw_energy = energy * CLHEP::MeV;

  const double mod_energy = 1.0 / (alphaQuenching_1 * raw_energy + 1.0);
  const double quenching_factor =
      -alphaQuenching_0 *
      (std::pow(mod_energy, alphaQuenching_2) - std::pow(mod_energy, alphaQuenching_2 / 2.0));

  return raw_energy / quenching_factor;
}

double CalorimeterModel::smearTime(mygsl::rng& rng, const double time,
                                        const double energy) const {
  const double sigma_time = getSigmaTime(energy);
  // Negative time are physical since input time is relative
  return rng.gaussian(time, sigma_time);
}

double CalorimeterModel::getSigmaTime(const double energy) const {
  // Have a look inside Gregoire Pichenot thesis(NEMO2) and
  // L. Simard parametrization for NEMO3 simulation
  const double sigma_e = energyResolution * fwhm2sig;
  return relaxationTime * sigma_e / sqrt(energy / CLHEP::MeV);
}

bool CalorimeterModel::aboveHighThreshold(const double energy) const {
  return (energy >= highEnergyThreshold);
}

bool CalorimeterModel::aboveLowThreshold(const double energy) const {
  return (energy >= lowEnergyThreshold);
}

}  // end of namespace processing

}  // end of namespace snemo

/********************************
 * OCD support : implementation *
 ********************************/

#include <datatools/object_configuration_description.h>

/** Opening macro for implementation
 *  @arg snemo::processing::CalorimeterModel  the full class name
 *  @arg ocd_ is the identifier of the 'datatools::object_configuration_description'
 *            to be initialized (passed by mutable reference).
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::processing::CalorimeterModel, ocd_) {
  ocd_.set_class_name("snemo::processing::CalorimeterModel");
  ocd_.set_class_description(
      "A model of the energy/time smearing in SuperNEMO Wall and Veto Calorimeters");
  ocd_.set_class_library("falaise");
  // ocd_.set_class_documentation("");

  {
    // Description of the 'energy.resolution' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("energy.resolution")
        .set_terse_description("The optical line energy resolution for electrons at 1 MeV")
        .set_traits(datatools::TYPE_REAL)
        .set_explicit_unit(true)
        .set_unit_label("fraction")
        .set_unit_symbol("%")
        .add_example(
            "Set the default value::                          \n"
            "                                                 \n"
            "  energy.resolution : real as fraction = 8 %     \n"
            "                                                 \n");
  }

  {
    // Description of the 'energy.low_threshold' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("energy.low_threshold")
        .set_terse_description("The optical line low energy threshold")
        .set_traits(datatools::TYPE_REAL)
        //.set_long_description("Default value: ``50 keV``             \n")
        .set_explicit_unit(true)
        .set_unit_label("energy")
        .set_unit_symbol("keV")
        .add_example(
            "Set the default value::                          \n"
            "                                                 \n"
            "  energy.low_threshold : real as energy = 50 keV \n"
            "                                                 \n");
  }

  {
    // Description of the 'energy.high_threshold' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("energy.high_threshold")
        .set_terse_description("The optical line high energy threshold")
        .set_traits(datatools::TYPE_REAL)
        //.set_long_description("Default value: ``150 keV``           \n")
        .set_explicit_unit(true)
        .set_unit_label("energy")
        .set_unit_symbol("keV")
        .add_example(
            "Set the default value::                            \n"
            "                                                   \n"
            "  energy.high_threshold : real as energy = 150 keV \n"
            "                                                   \n");
  }

  {
    // Description of the 'scintillator_relaxation_time' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("scintillator_relaxation_time")
        .set_terse_description("The scintillator relaxation time")
        .set_traits(datatools::TYPE_REAL)
        .set_long_description(
            "This parameter is used to compute the time resolution of the calorimeter. \n")
        .set_explicit_unit(true)
        .set_unit_label("time")
        .set_unit_symbol("ns")
        .add_example(
            "Set the default value::                                \n"
            "                                                       \n"
            "  scintillator_relaxation_time : real as time = 6.0 ns \n"
            "                                                       \n");
  }

  {
    // Description of the 'alpha_quenching_parameters' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("alpha_quenching_parameters")
        .set_terse_description("Activation of the quenching for alpha particles")
        .set_traits(datatools::TYPE_REAL, configuration_property_description::ARRAY, 3)
        .set_long_description(
            "The current implementation use a fit with 3 parameters. \n"
            "The default dimensionless values are: ``77.4  0.639  2.34``    \n")
        .set_explicit_unit(false)
        .add_example(
            "Set the default value::                     \n"
            "                                            \n"
            "  alpha_quenching_parameters : real[3] = \\ \n"
            "      77.4  0.639  2.34                     \n"
            "                                            \n");
  }

  // Additionnal configuration hints :
  ocd_.set_configuration_hints(
      "Here is a full configuration example in the      \n"
      "``datatools::properties`` ASCII format::         \n"
      "                                                 \n"
      "  energy.resolution     : real as fraction = 8 %         \n"
      "  energy.low_threshold  : real as energy = 50 keV        \n"
      "  energy.high_threshold : real as energy = 150 keV       \n"
      "  scintillator_relaxation_time : real as time = 6.0 ns   \n"
      "  alpha_quenching_parameters : real[3] = 77.4 0.639 2.34 \n"
      "                                                         \n");

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()  // Closing macro for implementation

// Registration macro for class 'snemo::processing::CalorimeterModel' :
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::processing::CalorimeterModel,
                               "snemo::processing::CalorimeterModel")
