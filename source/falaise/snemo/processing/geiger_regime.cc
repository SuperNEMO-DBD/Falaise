// -*- mode: c++ ; -*-
/** \file falaise/snemo/processing/geiger_regime.cc
 */

// Ourselves:
#include <falaise/snemo/processing/geiger_regime.h>

// Standard library:
#include <cmath>
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/clhep_units.h>
#include <datatools/properties.h>
#include <datatools/utils.h>
// - Bayeux/mygsl:

#include "falaise/property_set.h"
#include "falaise/quantity.h"

namespace {
// Refactored "basic" time to radius calculation
// calculation dependendent on a transition time
class BasicTimeToRadius {
 public:
  BasicTimeToRadius() = default;
  explicit BasicTimeToRadius(double transitionTime) : transitionTime_{transitionTime} {}
  ~BasicTimeToRadius() = default;
  BasicTimeToRadius(const BasicTimeToRadius&) = default;
  BasicTimeToRadius& operator=(const BasicTimeToRadius&) = default;
  BasicTimeToRadius(BasicTimeToRadius&&) = default;
  BasicTimeToRadius& operator=(BasicTimeToRadius&&) = default;

  /* Fit obtained from:
   *   shell> cd <sncore source dir>/doc/geiger_regime
   *   shell> gnuplot calib_t-r_0.gpl
   *
   */
  double operator()(double time_) const {
    const double A1 = 0.570947153108633;
    const double B1 = 0.580148313540993;
    const double C1 = 1.6567483468611;
    const double A2 = 1.86938462695651;
    const double B2 = 0.949912427483918;
    const double t_usec = time_ / CLHEP::microsecond;
    const double ut = 10. * t_usec;
    double r = A1 * ut / (std::pow(ut, B1) + C1);
    if (time_ > transitionTime_) {
      r = A2 * ut / (std::pow(ut, B2));
    }
    r *= CLHEP::cm;
    return r;
  }

 private:
  double transitionTime_ = std::numeric_limits<double>::max();
};

// Calculates the transition time for time-to-radius function
// based on time cut and radius parameters
// Looks like the calculation is of the drift time corresponding
// to the core cell radius, IAOI this time is less than "timeCut",
// otherwise, return max<double>
double calculateTZero(double timeCut, double cellRadius) {
  double tZero = std::numeric_limits<double>::max();
  BasicTimeToRadius timeToRadius;

  const double timeBegin = 0.0 * CLHEP::microsecond;
  double timeStep = 0.050 * CLHEP::microsecond;
  const double timeEnd = timeCut + 0.5 * timeStep;
  bool tune = false;

  for (double driftTime = timeBegin; driftTime < timeEnd; driftTime += timeStep) {
    const double driftRadius = timeToRadius(driftTime);
    if (driftRadius > cellRadius) {
      if (tune) {
        tZero = driftTime - 0.5 * timeStep;
        break;
      }

      driftTime -= 2 * timeStep;
      timeStep /= 20;
      tune = true;
    }
  }

  return tZero;
}

mygsl::tabulated_function makeTimeFromRadius(double timeCut, BasicTimeToRadius timeToRadius) {
  mygsl::tabulated_function fn;
  const double timeBegin = 0.0 * CLHEP::microsecond;
  const double timeStep = 0.2 * CLHEP::microsecond;
  const double timeEnd = timeCut + 0.5 * timeStep;

  for (double driftTime = timeBegin; driftTime < timeEnd; driftTime += timeStep) {
    fn.add_point(timeToRadius(driftTime), driftTime, false);
  }

  fn.lock_table("linear");
  return fn;
}

}  // namespace

namespace snemo {

namespace processing {

geiger_regime::geiger_regime() {
  // Default cell dimensions:
  cellRadius_ = 44. * 0.5 * CLHEP::mm;
  cellDiagonal_ = cellRadius_ * sqrt(2.0);
  cellLength_ = 2900. * CLHEP::mm;

  // Default TDC electronics resolution:
  anodeTimeResolution_ = 12.5 * CLHEP::ns;
  cathodeTimeResolution_ = 100.0 * CLHEP::ns;

  // Default resolution parameters (see I.Nasteva's work):
  // Longitudinal
  zResolution_ = 1.0 * CLHEP::cm;
  zResolutionSingleCathode_ = 5.0 * CLHEP::cm;

  // Radial (parameterized)
  rResolution_a_ = 0.425 * CLHEP::mm;
  rResolution_b_ = 0.0083;  // dimensionless
  rResolution_r0_ = 12.25 * CLHEP::mm;

  coreAnodeEfficiency_ = 1.0;
  coreCathodeEfficiency_ = 1.0;
  plasmaSpeed_ = 5.0 * CLHEP::cm / CLHEP::microsecond;
  plasmaSpeedError_ = 0.5 * CLHEP::cm / CLHEP::microsecond;

  tCut_ = 10. * CLHEP::microsecond;

  // timeToDriftCellRadius_ and the function are derived.
  timeToDriftCellRadius_ = calculateTZero(tCut_, cellRadius_);
  timeFromRadius_ = makeTimeFromRadius(tCut_, BasicTimeToRadius{timeToDriftCellRadius_});
}

geiger_regime::geiger_regime(const datatools::properties& dps) : geiger_regime::geiger_regime() {
  falaise::property_set ps{dps};

  // NB: these are slightly more complex than ideal, because we
  // can't put the value (a double) in directly:
  // ps.get<falaise::length_t>("param", {value / CLHEP::mm, "mm"})();
  // Can review after clarifying variable names, which should shorten
  // them and help things fit.
  if (ps.has_key("cell_diameter")) {
    cellRadius_ = 0.5 * ps.get<falaise::length_t>("cell_diameter")();
  }
  cellDiagonal_ = cellRadius_ * sqrt(2.0);

  if (ps.has_key("cell_length")) {
    cellLength_ = ps.get<falaise::length_t>("cell_length")();
  }

  if (ps.has_key("tcut")) {
    tCut_ = ps.get<falaise::time_t>("tcut")();
    DT_THROW_IF(tCut_ < 8 * CLHEP::microsecond, std::range_error,
                "Cut drift time is too short (" << tCut_ / CLHEP::microsecond << " us < 8 us) !");
  }

  if (ps.has_key("sigma_anode_time")) {
    anodeTimeResolution_ = ps.get<falaise::time_t>("sigma_anode_time")();
  }

  if (ps.has_key("sigma_cathode_time")) {
    cathodeTimeResolution_ = ps.get<falaise::time_t>("sigma_cathode_time")();
  }

  if (ps.has_key("base_anode_efficiency")) {
    coreAnodeEfficiency_ = ps.get<double>("base_anode_efficiency");
  }

  if (ps.has_key("base_cathode_efficiency")) {
    coreCathodeEfficiency_ = ps.get<double>("base_cathode_efficiency");
  }

  if (ps.has_key("plasma_longitudinal_speed")) {
    plasmaSpeed_ = ps.get<falaise::velocity_t>("plasma_longitudinal_speed")();
  }

  if (ps.has_key("sigma_plasma_longitudinal_speed")) {
    plasmaSpeedError_ = ps.get<falaise::velocity_t>("sigma_plasma_longitudinal_speed")();
  }

  if (ps.has_key("sigma_z")) {
    zResolution_ = ps.get<falaise::length_t>("sigma_z")();
  }

  if (ps.has_key("sigma_z_missing_cathode")) {
    zResolutionSingleCathode_ = ps.get<falaise::length_t>("sigma_z_missing_cathode")();
  }

  if (ps.has_key("sigma_r_a")) {
    rResolution_a_ = ps.get<falaise::length_t>("sigma_r_a")();
  }

  if (ps.has_key("sigma_r_b")) {
    rResolution_b_ = ps.get<double>("sigma_r_b");
  }

  if (ps.has_key("sigma_r_r0")) {
    rResolution_r0_ = ps.get<falaise::length_t>("sigma_r_r0")();
  }

  timeToDriftCellRadius_ = calculateTZero(tCut_, cellRadius_);
  timeFromRadius_ = makeTimeFromRadius(tCut_, BasicTimeToRadius(timeToDriftCellRadius_));
}

double geiger_regime::getCellDiameter() const { return 2.0 * cellRadius_; }

double geiger_regime::getCellRadius() const { return cellRadius_; }

double geiger_regime::getCellLength() const { return cellLength_; }

double geiger_regime::getAnodeTimeResolution(double /* anode_time_ */) const {
  return anodeTimeResolution_;
}

double geiger_regime::getCathodeTimeResolution() const { return cathodeTimeResolution_; }

double geiger_regime::getDriftTimeForCellRadius() const { return timeToDriftCellRadius_; }

double geiger_regime::getMaximumDriftTime() const { return tCut_; }

double geiger_regime::getMaximumRadius() const { return cellDiagonal_; }

double geiger_regime::getCathodeEfficiency() const { return coreCathodeEfficiency_; }

/** Value computed from I.Nasteva's plot in DocDB #843:
 *  see: <sncore source dir>/doc/geiger_regime/sn90cells_anode_efficiency.jpg
 */
double geiger_regime::getAnodeEfficiency(double radius) const {
  if (radius < cellRadius_) {
    return coreAnodeEfficiency_;
  }

  if (radius < cellDiagonal_) {
    const double sr0 = getRadialResolution(cellRadius_);
    return coreAnodeEfficiency_ * exp(-(radius - cellRadius_) / sr0);
  }

  return 0.0;
}

double geiger_regime::getPlasmaSpeed() const { return plasmaSpeed_; }

double geiger_regime::getPlasmaSpeedError() const { return plasmaSpeedError_; }

/** Value computed from I.Nasteva's plot in DocDB #843:
 *  see: <sncore source dir>/doc/geiger_regime/sn90cells_longitudinal_resolution.jpg
 */
double geiger_regime::getLongitudinalResolution(double /* z_ */, size_t missing_cathode) const {
  if (missing_cathode == 0) {
    return zResolution_;
  }
  if (missing_cathode == 1) {
    return zResolutionSingleCathode_;
  }
  return 0.5 * cellLength_;
}

/** Value computed from I.Nasteva's plot in DocDB #843:
 *  see: <sncore source dir>/doc/geiger_regime/sn90cells_sigma_r.jpg
 */
double geiger_regime::getRadialResolution(double r) const {
  const double a = rResolution_a_;
  const double b = rResolution_b_;
  const double r0 = rResolution_r0_;
  const double rResolution = a * (1.0 + b * std::pow((r - r0) / CLHEP::mm, 2));
  return rResolution * CLHEP::mm;
}

double geiger_regime::smearZ(mygsl::rng& ran_, double z_, double sigma_z_) const {
  return ran_.gaussian(z_, sigma_z_);
}

double geiger_regime::smearRadius(mygsl::rng& ran_, double r_) const {
  double r{datatools::invalid_real_double()};
  double sr0 = getRadialResolution(cellRadius_);
  if (r_ < (cellRadius_ + 2. * sr0)) {
    const double sr = getRadialResolution(r_);
    r = ran_.gaussian(r_, sr);
    if (r < 0.0) {
      r = 0.0 * CLHEP::mm;
    }
  } else {
    r = ran_.flat(cellRadius_, cellDiagonal_);
  }
  return r;
}

double geiger_regime::base_t_2_r(double time_, int mode_) const {
  if (mode_ == 0) {
    return BasicTimeToRadius(timeToDriftCellRadius_)(time_);
  }
  return BasicTimeToRadius()(time_);
}

void geiger_regime::calibrateRadiusFromTime(double drift_time_, double& drift_radius_,
                                            double& sigma_drift_radius_) const {
  DT_THROW_IF(drift_time_ < 0.0, std::range_error,
              "Negative drift time (" << drift_time_ / CLHEP::ns << " ns)");
  datatools::invalidate(drift_radius_);
  datatools::invalidate(sigma_drift_radius_);
  if (drift_time_ < tCut_) {
    drift_radius_ = base_t_2_r(drift_time_);
    sigma_drift_radius_ = getRadialResolution(drift_radius_);
  }
}

double geiger_regime::getRandomTimeGivenRadius(mygsl::rng& ran_, double drift_distance_) const {
  DT_THROW_IF(drift_distance_ < 0.0, std::range_error, "Negative drift distance !");

  double drift_time{datatools::invalid_real_double()};

  if (drift_distance_ <= cellDiagonal_) {
    const double rcut = timeFromRadius_.x_max();
    const double tcut = timeFromRadius_(rcut);

    if (drift_distance_ <= rcut) {
      double sr = getRadialResolution(drift_distance_);
      if (drift_distance_ > cellRadius_) {
        sr = getRadialResolution(cellRadius_);
      }
      double r_min = drift_distance_ - sr;
      if (r_min < 0.0) {
        r_min = 0.0;
      }
      const double t_min = timeFromRadius_(r_min);
      const double t_mean = timeFromRadius_(drift_distance_);
      const double mean_time = t_mean;
      const double sigma_time = (t_mean - t_min);
      drift_time = ran_.gaussian(mean_time, sigma_time);
      // protect against pathological times :
      if (drift_distance_ > cellRadius_) {
        const double sr0 = getRadialResolution(cellRadius_);
        const double st0 = timeToDriftCellRadius_ - timeFromRadius_(cellRadius_ - sr0);
        const double tinf = timeToDriftCellRadius_ - 2 * st0;
        if (drift_time < tinf) {
          drift_time = 2 * tinf - drift_time;
        }
      }
    } else {
      const double t2 = tcut;
      const double r2 = rcut;
      const double r1 = (r2 + cellRadius_) / 2;
      const double t1 = timeToDriftCellRadius_;
      if (drift_distance_ < r2) {
        const double sr = getRadialResolution(drift_distance_);
        const double r_min = drift_distance_ - sr;
        const double r_max = drift_distance_ + sr;
        const double t_min = t1 + (r_min - r1) * (t2 - t1) / (r2 - r1);
        const double t_max = t1 + (r_max - r1) * (t2 - t1) / (r2 - r1);
        const double mean_time = 0.5 * (t_max + t_min);
        const double sigma_time = 0.5 * (t_max - t_min);
        drift_time = ran_.gaussian(mean_time, sigma_time);
        drift_time = mean_time;  // XXX
        const double tlim = timeToDriftCellRadius_;
        if (drift_time < tlim) {
          drift_time = 2 * tlim - drift_time;
        }

        double ta = mean_time - 2 * sigma_time;
        if (ta < t1) {
          ta = t1;
        }
        const double tb = mean_time + 2 * sigma_time;
        drift_time = ran_.flat(ta, tb);
      }
    }

    // protection against negative random drift times:
    if (drift_time < 0.0) {
      drift_time = 0.5 * anodeTimeResolution_;
    }
  }

  // 2012/01/06: XG: quickly fix non valid drift time (maybe
  // there is a better by solving some strange 'if' condition
  // before)
  if (!datatools::is_valid(drift_time)) {
    drift_time = tCut_;
  }

  return drift_time;
}

void geiger_regime::tree_dump(std::ostream& out, const std::string& title,
                              const std::string& indent, bool inherit) const {
  if (!title.empty()) {
    out << indent << title << std::endl;
  }
  out << indent << datatools::i_tree_dumpable::tag
      << "Cell diameter = " << 2.0 * cellRadius_ / CLHEP::mm << " mm" << std::endl;
  out << indent << datatools::i_tree_dumpable::tag << "Cell length   = " << cellLength_ / CLHEP::cm
      << " cm" << std::endl;
  out << indent << datatools::i_tree_dumpable::tag << "Sigma z       = " << zResolution_ / CLHEP::mm
      << " mm" << std::endl;
  out << indent << datatools::i_tree_dumpable::tag
      << "Sigma r/a     = " << rResolution_a_ / CLHEP::mm << " mm" << std::endl;
  out << indent << datatools::i_tree_dumpable::tag << "Sigma r/b     = " << rResolution_b_
      << std::endl;
  out << indent << datatools::i_tree_dumpable::tag
      << "Sigma r/r0    = " << rResolution_r0_ / CLHEP::mm << " mm" << std::endl;
  out << indent << datatools::i_tree_dumpable::tag << "t0            = " << timeToDriftCellRadius_ / CLHEP::ns
      << " ns" << std::endl;
  out << indent << datatools::i_tree_dumpable::tag << "tcut          = " << tCut_ / CLHEP::ns
      << " ns" << std::endl;
  out << indent << datatools::i_tree_dumpable::tag << "r0            = " << cellRadius_ / CLHEP::mm
      << " mm" << std::endl;
  out << indent << datatools::i_tree_dumpable::inherit_tag(inherit)
      << "rdiag         = " << cellDiagonal_ / CLHEP::mm << " mm" << std::endl;
}

}  // end of namespace processing
}  // end of namespace snemo

/********************************
 * OCD support : implementation *
 ********************************/

#include <datatools/object_configuration_description.h>

/** Opening macro for implementation
 *  @arg snemo::processing::geiger_regime  the full class name
 *  @arg ocd_ is the identifier of the 'datatools::object_configuration_description'
 *            to be initialized (passed by mutable reference).
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::processing::geiger_regime, ocd_) {
  ocd_.set_class_name("snemo::processing::geiger_regime");
  ocd_.set_class_description("This object describes the Geiger regime of SuperNEMO drift cells");
  ocd_.set_class_library("falaise");
  // ocd_.set_class_documentation("");

  {
    // Description of the 'cell_diameter' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("cell_diameter")
        .set_terse_description("The diameter of drift cell")
        .set_traits(datatools::TYPE_REAL)
        // .set_long_description("Default value: ``44 mm``                \n")
        .set_explicit_unit(true)
        .set_unit_label("length")
        .set_unit_symbol("mm")
        .set_default_value_real(44 * CLHEP::mm)
        .add_example(
            "Set the default value::                          \n"
            "                                                 \n"
            "  cell_diameter : real = 44 mm                   \n"
            "                                                 \n");
  }

  {
    // Description of the 'cell_length' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("cell_length")
        .set_terse_description("The length of drift cell")
        .set_traits(datatools::TYPE_REAL)
        // .set_long_description("Default value: ``2900 mm``              \n")
        .set_explicit_unit(true)
        .set_unit_label("length")
        .set_unit_symbol("mm")
        .set_default_value_real(2900 * CLHEP::mm)
        .add_example(
            "Set the default value::                          \n"
            "                                                 \n"
            "  cell_length : real = 2900 mm                   \n"
            "                                                 \n");
  }

  {
    // Description of the 'sigma_anode_time' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("sigma_anode_time")
        .set_terse_description("The error on anode drift time")
        .set_traits(datatools::TYPE_REAL)
        // .set_long_description("Default value: ``12.5 ns``              \n")
        .set_explicit_unit(true)
        .set_unit_label("time")
        .set_unit_symbol("ns")
        .set_default_value_real(12.5 * CLHEP::ns)
        .add_example(
            "Set the default value::                          \n"
            "                                                 \n"
            "  sigma_anode_time : real = 12.5 ns              \n"
            "                                                 \n");
  }

  {
    // Description of the 'tcut' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("tcut")
        .set_terse_description("Maximum drift time to be calibrated")
        .set_traits(datatools::TYPE_REAL)
        // .set_long_description("Default value: ``10.0 us``              \n")
        .set_explicit_unit(true)
        .set_unit_label("time")
        .set_unit_symbol("ns")
        .set_default_value_real(10 * CLHEP::ns)
        .add_example(
            "Set the default value::                          \n"
            "                                                 \n"
            "  tcut : real as time = 10.0 us                  \n"
            "                                                 \n");
  }

  {
    // Description of the 'sigma_cathode_time' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("sigma_cathode_time")
        .set_terse_description("The error on cathode drift time")
        .set_traits(datatools::TYPE_REAL)
        // .set_long_description("Default value: ``100 ns``               \n")
        .set_explicit_unit(true)
        .set_unit_label("time")
        .set_unit_symbol("ns")
        .set_default_value_real(100 * CLHEP::ns)
        .add_example(
            "Set the default value::                          \n"
            "                                                 \n"
            "  sigma_cathode_time : real = 100 ns             \n"
            "                                                 \n");
  }

  {
    // Description of the 'sigma_z' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("sigma_z")
        .set_terse_description("The error on longitudinal position")
        .set_traits(datatools::TYPE_REAL)
        //.set_long_description("Default value: ``1.0 cm``               \n")
        .set_explicit_unit(true)
        .set_unit_label("length")
        .set_unit_symbol("cm")
        .set_default_value_real(1.0 * CLHEP::cm)
        .add_example(
            "Set the default value::                          \n"
            "                                                 \n"
            "  sigma_z : real as length = 1.0 cm              \n"
            "                                                 \n");
  }

  {
    // Description of the 'sigma_z_missing_cathode' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("sigma_z_missing_cathode")
        .set_terse_description("The error on longitudinal position in case of a missing cathode")
        .set_traits(datatools::TYPE_REAL)
        // .set_long_description("Default value: ``5.0 cm``                  \n")
        .set_explicit_unit(true)
        .set_unit_label("length")
        .set_unit_symbol("cm")
        .set_default_value_real(5.0 * CLHEP::cm)
        .add_example(
            "Set the default value::                             \n"
            "                                                    \n"
            "  sigma_z_missing_cathode : real as length = 5.0 cm \n"
            "                                                    \n");
  }

  {
    // Description of the 'sigma_r_a' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("sigma_r_a")
        .set_terse_description("First parameter of the error on drift radius")
        .set_traits(datatools::TYPE_REAL)
        //.set_long_description("Default value: ``0.425 mm``             \n")
        .set_explicit_unit(true)
        .set_unit_label("length")
        .set_unit_symbol("mm")
        .set_default_value_real(0.425 * CLHEP::mm)
        .add_example(
            "Set the default value::                          \n"
            "                                                 \n"
            "  sigma_r_a : real = 0.425 mm                    \n"
            "                                                 \n");
  }

  {
    // Description of the 'sigma_r_b' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("sigma_r_b")
        .set_terse_description("Second parameter of the error on drift radius")
        .set_traits(datatools::TYPE_REAL)
        //.set_long_description("Default value: ``0.0083``  \n")
        .set_explicit_unit(false)
        .set_default_value_real(0.0083)
        .add_example(
            "Set the default value::     \n"
            "                            \n"
            "  sigma_r_b : real = 0.0083 \n"
            "                            \n");
  }

  {
    // Description of the 'sigma_r_r0' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("sigma_r_r0")
        .set_terse_description("Third parameter of the error on drift radius")
        .set_traits(datatools::TYPE_REAL)
        // .set_long_description("Default value: ``12.25 mm``\n")
        .set_explicit_unit(true)
        .set_unit_label("length")
        .set_unit_symbol("mm")
        .set_default_value_real(12.25 * CLHEP::mm)
        .add_example(
            "Set the default value::          \n"
            "                                 \n"
            "  sigma_r_r0 : real = 12.25 mm   \n"
            "                                 \n");
  }

  {
    // Description of the 'base_anode_efficiency' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("base_anode_efficiency")
        .set_terse_description("Base anode efficiency")
        .set_traits(datatools::TYPE_REAL)
        //.set_long_description("Default value: ``1.0``          \n")
        .set_explicit_unit(false)
        .set_default_value_real(1.0)
        .add_example(
            "Set the default value::              \n"
            "                                     \n"
            "  base_anode_efficiency : real = 1.0 \n"
            "                                     \n");
  }

  {
    // Description of the 'base_cathode_efficiency' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("base_cathode_efficiency")
        .set_terse_description("Base cathode efficiency")
        .set_traits(datatools::TYPE_REAL)
        //.set_long_description("Default value: ``1.0``                      \n")
        .set_explicit_unit(false)
        .set_default_value_real(1.0)
        .add_example(
            "Set the default value::                          \n"
            "                                                 \n"
            "  base_cathode_efficiency : real = 1.0           \n"
            "                                                 \n");
  }

  {
    // Description of the 'plasma_longitudinal_speed' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("plasma_longitudinal_speed")
        .set_terse_description("Plasma longitudinal speed")
        .set_traits(datatools::TYPE_REAL)
        // .set_long_description("Default value: ``5.0 cm/us``                \n")
        .set_explicit_unit(true)
        .set_unit_label("velocity")
        .set_unit_symbol("cm/us")
        .set_default_value_real(5.0 * CLHEP::cm / CLHEP::microsecond)
        .add_example(
            "Set the default value::                                    \n"
            "                                                           \n"
            "  plasma_longitudinal_speed : real as velocity = 5.0 cm/us \n"
            "                                                           \n");
  }

  {
    // Description of the 'sigma_plasma_longitudinal_speed' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("sigma_plasma_longitudinal_speed")
        .set_terse_description("Error on plasma longitudinal speed")
        .set_traits(datatools::TYPE_REAL)
        //.set_long_description("Default value: ``0.5 cm/us``                \n")
        .set_explicit_unit(true)
        .set_unit_label("velocity")
        .set_unit_symbol("cm/us")
        .set_default_value_real(0.5 * CLHEP::cm / CLHEP::microsecond)
        .add_example(
            "Set the default value::                                          \n"
            "                                                                 \n"
            "  sigma_plasma_longitudinal_speed : real as velocity = 0.5 cm/us \n"
            "                                                                 \n");
  }

  // Additionnal configuration hints :
  ocd_.set_configuration_hints(
      "Here is a full configuration example in the                      \n"
      "``datatools::properties`` ASCII format::                         \n"
      "                                                                 \n"
      "  cell_diameter : real as length = 44 mm                         \n"
      "  cell_length : real as length = 2900 mm                         \n"
      "  sigma_anode_time : real as time = 12.5 ns                      \n"
      "  tcut : real as time = 10.0 us                                  \n"
      "  sigma_cathode_time : real = 100 ns                             \n"
      "  sigma_z : real = 1.0 cm                                        \n"
      "  sigma_z_missing_cathode : real as length = 5.0 cm              \n"
      "  sigma_r_a : real as length = 0.425 mm                          \n"
      "  sigma_r_b : real = 0.0083                                      \n"
      "  sigma_r_r0 : real as length = 12.25 mm                         \n"
      "  base_anode_efficiency : real = 1.0                             \n"
      "  base_cathode_efficiency : real = 1.0                           \n"
      "  plasma_longitudinal_speed : real as velocity = 5.0 cm/us       \n"
      "  sigma_plasma_longitudinal_speed : real as velocity = 0.5 cm/us \n"
      "                                                                 \n");

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()  // Closing macro for implementation

// Registration macro for class 'snemo::processing::geiger_regime' :
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::processing::geiger_regime, "snemo::processing::geiger_regime")
