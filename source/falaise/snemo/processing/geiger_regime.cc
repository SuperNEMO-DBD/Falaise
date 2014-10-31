// -*- mode: c++ ; -*-
/** \file falaise/snemo/processing/geiger_regime.cc
 */

// Ourselves:
#include <falaise/snemo/processing/geiger_regime.h>

// Standard library:
#include <sstream>
#include <cmath>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/utils.h>
#include <datatools/clhep_units.h>
// - Bayeux/mygsl:
#include <mygsl/rng.h>

namespace snemo {

  namespace processing {

    bool geiger_regime::is_initialized () const
    {
      return _initialized_;
    }

    void geiger_regime::reset ()
    {
      DT_THROW_IF (! is_initialized (), std::logic_error, "Not initialized !");
      _init_defaults_ ();
      _initialized_ = false;
      return;
    }

    void geiger_regime::initialize (const datatools::properties & config_)
    {
      DT_THROW_IF (is_initialized (), std::logic_error, "Already initialized !");

      double length_unit = CLHEP::mm;
      double time_unit   = CLHEP::microsecond;
      double drift_speed_unit = (CLHEP::cm / CLHEP::microsecond);

      if (config_.has_key ("cell_diameter")) {
        _cell_diameter_ = config_.fetch_real ("cell_diameter");
        if (! config_.has_explicit_unit ("cell_diameter")) {
          _cell_diameter_ *= length_unit;
        }
      }

      if (config_.has_key ("cell_length")) {
        _cell_length_ = config_.fetch_real ("cell_length");
        if (! config_.has_explicit_unit ("cell_length")) {
          _cell_length_ *= length_unit;
        }
      }

      if (config_.has_key ("tcut")) {
        _tcut_ = config_.fetch_real ("tcut");
        if (! config_.has_explicit_unit ("tcut")) {
          _tcut_ *= time_unit;
        }
      }

      if (config_.has_key ("sigma_anode_time")) {
        _sigma_anode_time_ = config_.fetch_real ("sigma_anode_time");
        if (! config_.has_explicit_unit ("sigma_anode_time")) {
          _sigma_anode_time_ *= time_unit;
        }
      }

      if (config_.has_key ("sigma_cathode_time")) {
        _sigma_cathode_time_ = config_.fetch_real ("sigma_cathode_time");
        if (! config_.has_explicit_unit ("sigma_cathode_time")) {
          _sigma_cathode_time_ *= time_unit;
        }
      }

      if (config_.has_key ("base_anode_efficiency")) {
        _base_anode_efficiency_ = config_.fetch_real ("base_anode_efficiency");
      }

      if (config_.has_key ("base_cathode_efficiency")) {
        _base_cathode_efficiency_ = config_.fetch_real ("base_cathode_efficiency");
      }

      if (config_.has_key ("plasma_longitudinal_speed")) {
        _plasma_longitudinal_speed_ = config_.fetch_real ("plasma_longitudinal_speed");
        if (config_.has_explicit_unit ("plasma_longitudinal_speed")) {
          _plasma_longitudinal_speed_ *= drift_speed_unit;
        }
      }

      if (config_.has_key ("sigma_plasma_longitudinal_speed")) {
        _sigma_plasma_longitudinal_speed_ = config_.fetch_real ("sigma_plasma_longitudinal_speed");
        if (config_.has_explicit_unit ("sigma_plasma_longitudinal_speed")) {
          _sigma_plasma_longitudinal_speed_ *= drift_speed_unit;
        }
      }

      if (config_.has_key ("sigma_z")) {
        _sigma_z_ = config_.fetch_real ("sigma_z");
        if (! config_.has_explicit_unit ("sigma_z")) {
          _sigma_z_ *= length_unit;
        }
      }

      if (config_.has_key ("sigma_z_missing_cathode")) {
        _sigma_z_missing_cathode_ = config_.fetch_real ("sigma_z_missing_cathode");
        if (! config_.has_explicit_unit ("sigma_z_missing_cathode")) {
          _sigma_z_missing_cathode_ *= length_unit;
        }
      }

      if (config_.has_key ("sigma_r_a")) {
        _sigma_r_a_ = config_.fetch_real ("sigma_r_a");
        if (! config_.has_explicit_unit ("sigma_r_a")){
          _sigma_r_a_ *= length_unit;
        }
      }

      if (config_.has_key ("sigma_r_b")) {
        _sigma_r_b_ = config_.fetch_real ("sigma_r_b");
      }

      if (config_.has_key ("sigma_r_r0")) {
        _sigma_r_r0_ = config_.fetch_real ("sigma_r_r0");
        if (! config_.has_explicit_unit ("sigma_r_r0")){
          _sigma_r_r0_ *= length_unit;
        }
      }


      DT_THROW_IF (_tcut_ < 8 * time_unit, std::range_error,
                   "Cut drift time is too short (" << _tcut_ / time_unit
                   << " us < 8 us) !");

      const double r_cell = 0.5 * _cell_diameter_;
      // 2011-05-12 FM : Compute a valid t0 before :
      double step_drift_time1 = 0.050 * time_unit;
      bool tune = false;
      for (double drift_time = 0.0 * time_unit;
           drift_time < (_tcut_ + 0.5 * step_drift_time1);
           drift_time += step_drift_time1) {
        const double drift_radius = base_t_2_r (drift_time, 1);
        if (drift_radius > r_cell) {
          if (tune) {
            _t0_ = drift_time - 0.5 * step_drift_time1;
            break;
          } else {
            drift_time -= 2 * step_drift_time1;
            step_drift_time1 /= 20;
            tune = true;
          }
        }
      }

      const double step_drift_time = 0.2 * time_unit;
      for (double drift_time = 0.0 * time_unit;
           drift_time < (_tcut_ + 0.5 * step_drift_time);
           drift_time += step_drift_time) {
        const double drift_radius = base_t_2_r (drift_time);
        _base_rt_.add_point (drift_radius, drift_time, false);
      }
      _base_rt_.lock_table ("linear");
      _r0_    = r_cell;
      _rdiag_ = r_cell * sqrt (2.0);

      _initialized_ = true;
      return;
    }

    void geiger_regime::_init_defaults_ ()
    {
      // Default cell size:
      _cell_diameter_ = 44. * CLHEP::mm;
      _cell_length_   = 2900. * CLHEP::mm;

      // Default TDC electronics resolution:
      _sigma_anode_time_   =  12.5 * CLHEP::ns;
      _sigma_cathode_time_ = 100.0 * CLHEP::ns;

      // Default resolution parameters (see I.Nasteva's work):
      _sigma_z_    =  1.0 * CLHEP::cm;
      _sigma_z_missing_cathode_  = 5.0 * CLHEP::cm;
      _sigma_r_a_  =  0.425 * CLHEP::mm;
      _sigma_r_b_  =  0.0083; // dimensionless
      _sigma_r_r0_ = 12.25 * CLHEP::mm;

      _base_anode_efficiency_     = 1.0;
      _base_cathode_efficiency_   = 1.0;
      _plasma_longitudinal_speed_ = 5.0 * CLHEP::cm / CLHEP::microsecond;
      _sigma_plasma_longitudinal_speed_ = 0.5 * CLHEP::cm / CLHEP::microsecond;

      // Reset internals:
      _tcut_   = 10. * CLHEP::microsecond;
      datatools::invalidate (_t0_);
      datatools::invalidate (_r0_);
      datatools::invalidate (_rdiag_);
      _base_rt_.reset ();

      return;
    }

    geiger_regime::geiger_regime ()
    {
      _initialized_ = false;
      _init_defaults_ ();
      return;
    }

    geiger_regime::~geiger_regime()
    {
      return;
    }

    double geiger_regime::get_cell_diameter () const
    {
      return _cell_diameter_;
    }

    double geiger_regime::get_cell_radius () const
    {
      return 0.5 * _cell_diameter_;
    }

    double geiger_regime::get_cell_length () const
    {
      return _cell_length_;
    }

    double geiger_regime::get_sigma_anode_time (double /* anode_time_ */) const
    {
      return _sigma_anode_time_;
    }

    double geiger_regime::get_sigma_cathode_time () const
    {
      return _sigma_cathode_time_;
    }

    double geiger_regime::get_t0 () const
    {
      return _t0_;
    }

    double geiger_regime::get_tcut () const
    {
      return _tcut_;
    }

    double geiger_regime::get_r0 () const
    {
      return _r0_;
    }

    double geiger_regime::get_rdiag () const
    {
      return _rdiag_;
    }

    double geiger_regime::get_base_anode_efficiency () const
    {
      return _base_anode_efficiency_;
    }

    double geiger_regime::get_base_cathode_efficiency () const
    {
      return _base_cathode_efficiency_;
    }

    /** Value computed from I.Nasteva's plot in DocDB #843:
     *  see: <sncore source dir>/doc/geiger_regime/sn90cells_anode_efficiency.jpg
     */
    double geiger_regime::get_anode_efficiency (double r_) const
    {
      DT_THROW_IF (! is_initialized (), std::logic_error, "Not initialized !");
      const double max_eff = _base_anode_efficiency_;
      if (r_ < _r0_) return max_eff;
      if (r_ < _rdiag_) {
        const double sr0 = get_sigma_r (_r0_);
        return max_eff * exp (-(r_ - _r0_) / sr0);
      }
      return 0.0;
    }

    double geiger_regime::get_cathode_efficiency () const
    {
      return _base_cathode_efficiency_;
    }

    double geiger_regime::get_plasma_longitudinal_speed () const
    {
      return _plasma_longitudinal_speed_;
    }

    double geiger_regime::get_sigma_plasma_longitudinal_speed () const
    {
      return _sigma_plasma_longitudinal_speed_;
    }

    mygsl::tabulated_function & geiger_regime::grab_base_rt ()
    {
      return _base_rt_;
    }

    /** Value computed from I.Nasteva's plot in DocDB #843:
     *  see: <sncore source dir>/doc/geiger_regime/sn90cells_longitudinal_resolution.jpg
     */
    double geiger_regime::get_sigma_z (double /* z_ */, size_t missing_cathode_) const
    {
      DT_THROW_IF (! is_initialized (), std::logic_error, "Not initialized !");
      if (missing_cathode_ == 0 ) return _sigma_z_;
      if (missing_cathode_ == 1 ) return _sigma_z_missing_cathode_;
      return 0.5 * _cell_length_;
    };

    /** Value computed from I.Nasteva's plot in DocDB #843:
     *  see: <sncore source dir>/doc/geiger_regime/sn90cells_sigma_r.jpg
     */
    double geiger_regime::get_sigma_r (double r_) const
    {
      DT_THROW_IF (! is_initialized (), std::logic_error, "Not initialized !");
      const double a  = _sigma_r_a_;
      const double b  = _sigma_r_b_;
      const double r0 = _sigma_r_r0_;
      const double sr = a * (1.0 + b * std::pow ((r_ - r0) / CLHEP::mm, 2));
      return sr * CLHEP::mm;
    }

    double geiger_regime::randomize_z (mygsl::rng & ran_, double z_, double sigma_z_) const
    {
      DT_THROW_IF (! is_initialized (), std::logic_error, "Not initialized !");
      const double z = ran_.gaussian (z_, sigma_z_);
      return z;
    }

    double geiger_regime::randomize_r (mygsl::rng & ran_, double r_) const
    {
      DT_THROW_IF (! is_initialized (), std::logic_error, "Not initialized !");
      double r;
      datatools::invalidate (r);
      double sr0 = get_sigma_r (_r0_);
      if (r_ < (_r0_ + 2. * sr0)) {
        const double sr = get_sigma_r (r_);
        r = ran_.gaussian (r_, sr);
        if (r < 0.0) {
          r = 0.0 * CLHEP::mm;
        }
      } else {
        r = ran_.flat (_r0_, _rdiag_);
      }
      return r;
    }

    double geiger_regime::base_t_2_r (double time_, int mode_) const
    {
      DT_THROW_IF (time_ < 0.0, std::range_error, "Invalid drift time !");
      /* Fit obtained from:
       *   shell> cd <sncore source dir>/doc/geiger_regime
       *   shell> gnuplot calib_t-r_0.gpl
       *
       */
      const double A1 = 0.570947153108633;
      const double B1 = 0.580148313540993;
      const double C1 = 1.6567483468611;
      const double A2 = 1.86938462695651;
      const double B2 = 0.949912427483918;
      const double t_usec = time_ / CLHEP::microsecond;
      const double ut = 10. * t_usec;
      double r = A1 * ut / ( std::pow (ut, B1) + C1);
      if (mode_ == 0) {
        if (time_ > _t0_) {
          r = A2 * ut / ( std::pow (ut, B2));
        }
      }
      r *= CLHEP::cm;
      return r;
    }

    void geiger_regime::calibrate_drift_radius_from_drift_time (double   drift_time_,
                                                                double & drift_radius_,
                                                                double & sigma_drift_radius_) const
    {
      DT_THROW_IF (! is_initialized (), std::logic_error, "Not initialized !");
      DT_THROW_IF (drift_time_ < 0.0, std::range_error,
                   "Invalid drift time (" << drift_time_ / CLHEP::ns << " ns) !");
      datatools::invalidate (drift_radius_);
      datatools::invalidate (sigma_drift_radius_);
      if (drift_time_ < _tcut_) {
        drift_radius_ = base_t_2_r (drift_time_);
        sigma_drift_radius_ = get_sigma_r (drift_radius_);
      }
      return;
    }

    double geiger_regime::randomize_drift_time_from_drift_distance (mygsl::rng & ran_,
                                                                    double drift_distance_) const
    {
      DT_THROW_IF (! is_initialized (), std::logic_error, "Not initialized !");
      DT_THROW_IF (drift_distance_ < 0.0, std::range_error, "Invalid drift distance !");
      datatools::logger::priority local_priority = datatools::logger::PRIO_WARNING;

      double drift_time;
      datatools::invalidate (drift_time);

      if (drift_distance_ <= _rdiag_) {
        const double rcut = _base_rt_.x_max ();
        const double tcut = _base_rt_ (rcut);

        DT_LOG_TRACE (local_priority, "drift_distance_ = " << drift_distance_ << " "
                      << "rdiag = " << _rdiag_ << " " << "rcut = " << rcut);

        //if (drift_distance_ <= __r0 + sr0)
        if (drift_distance_ <= rcut) {
          double sr = get_sigma_r (drift_distance_);
          if (drift_distance_ > _r0_) sr = get_sigma_r (_r0_);
          double r_min = drift_distance_ - sr;
          if (r_min < 0.0) r_min = 0.0;
          const double t_min = _base_rt_ (r_min);
          const double t_mean = _base_rt_ (drift_distance_);
          const double mean_time  = t_mean;
          const double sigma_time = (t_mean - t_min);
          drift_time = ran_.gaussian (mean_time, sigma_time);
          // protect against pathological times :
          if (drift_distance_ > _r0_) {
            const double sr0 = get_sigma_r (_r0_);
            const double st0 = _t0_ - _base_rt_ (_r0_ - sr0);
            const double tinf = _t0_ - 2 * st0;
            if (drift_time < tinf) {
              drift_time = 2 * tinf - drift_time;
            }
          }
        } else {
          const double t2 = tcut;
          const double r2 = rcut;
          const double r1 = (r2 + _r0_) / 2;
          const double t1 = _t0_;
          DT_LOG_TRACE (local_priority, "t0 = " << _t0_);
          DT_LOG_TRACE (local_priority, "tcut = " << _tcut_);
          DT_LOG_TRACE (local_priority, "t1 = " << t1 << " t2 = " << t2);
          DT_LOG_TRACE (local_priority, "r1 = " << r1 << " r2 = " << r2);
          if (drift_distance_ < r2) {
            const double sr = get_sigma_r (drift_distance_);
            const double r_min = drift_distance_ - sr;
            const double r_max = drift_distance_ + sr;
            const double t_min = t1 + (r_min - r1) * (t2 - t1) / (r2  - r1);
            const double t_max = t1 + (r_max - r1) * (t2 - t1) / (r2  - r1);
            const double mean_time  = 0.5 * (t_max + t_min);
            const double sigma_time = 0.5 * (t_max - t_min);
            drift_time = ran_.gaussian (mean_time, sigma_time);
            drift_time = mean_time; // XXX
            DT_LOG_TRACE (local_priority, "drift_distance_ = " << drift_distance_
                          << " mean_time = " << mean_time);
            const double tlim = _t0_;
            if (drift_time < tlim) {
              drift_time = 2 * tlim - drift_time;
            }

            double ta = mean_time - 2 * sigma_time;
            if (ta < t1) {
              ta = t1;
            }
            const double tb = mean_time + 2 * sigma_time;
            drift_time = ran_.flat (ta, tb);

          }
        }

        // protection against negative random drift times:
        if (drift_time < 0.0) {
          drift_time = 0.5 * _sigma_anode_time_;
        }
      }

      // 2012/01/06: XG: quickly fix non valid drift time (maybe
      // there is a better by solving some strange 'if' condition
      // before)
      if (!datatools::is_valid (drift_time)) {
        drift_time = _tcut_;
      }

      DT_LOG_TRACE (local_priority, "drift_time = " << drift_time);
      return drift_time;
    }


    void geiger_regime::tree_dump(std::ostream & out_,
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
           << "Initialized   = " << is_initialized () << std::endl;
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Cell diameter = " << _cell_diameter_ / CLHEP::mm << " mm" << std::endl;
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Cell length   = " << _cell_length_ / CLHEP::cm << " cm" << std::endl;
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Sigma z       = " << _sigma_z_ / CLHEP::mm << " mm" << std::endl;
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Sigma r/a     = " << _sigma_r_a_ / CLHEP::mm << " mm" << std::endl;
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Sigma r/b     = " << _sigma_r_b_  << std::endl;
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Sigma r/r0    = " << _sigma_r_r0_ / CLHEP::mm << " mm" << std::endl;
      out_ << indent << datatools::i_tree_dumpable::tag
           << "t0            = " << _t0_ / CLHEP::ns << " ns" << std::endl;
      out_ << indent << datatools::i_tree_dumpable::tag
           << "tcut          = " << _tcut_ / CLHEP::ns << " ns" << std::endl;
      out_ << indent << datatools::i_tree_dumpable::tag
           << "r0            = " << _r0_ / CLHEP::mm << " mm" << std::endl;
      out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "rdiag         = " << _rdiag_ / CLHEP::mm << " mm" << std::endl;
      return;
    }

  } // end of namespace processing

} // end of namespace snemo

/********************************
 * OCD support : implementation *
 ********************************/

#include <datatools/object_configuration_description.h>

/** Opening macro for implementation
 *  @arg snemo::processing::geiger_regime  the full class name
 *  @arg ocd_ is the identifier of the 'datatools::object_configuration_description'
 *            to be initialized (passed by mutable reference).
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::processing::geiger_regime,ocd_)
{
  ocd_.set_class_name("snemo::processing::geiger_regime");
  ocd_.set_class_description("This object describes the Geiger regime of SuperNEMO drift cells");
  ocd_.set_class_library("falaise");
  // ocd_.set_class_documentation("");

  {
    // Description of the 'cell_diameter' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("cell_diameter")
      .set_terse_description("The diameter of drift cell")
      .set_traits(datatools::TYPE_REAL)
      // .set_long_description("Default value: ``44 mm``                \n")
      .set_explicit_unit(true)
      .set_unit_label("length")
      .set_unit_symbol("mm")
      .set_default_value_real(44 * CLHEP::mm)
      .add_example("Set the default value::                          \n"
                   "                                                 \n"
                   "  cell_diameter : real = 44 mm                   \n"
                   "                                                 \n"
                   )
      ;
  }

  {
    // Description of the 'cell_length' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("cell_length")
      .set_terse_description("The length of drift cell")
      .set_traits(datatools::TYPE_REAL)
      // .set_long_description("Default value: ``2900 mm``              \n")
      .set_explicit_unit(true)
      .set_unit_label("length")
      .set_unit_symbol("mm")
      .set_default_value_real(2900 * CLHEP::mm)
      .add_example("Set the default value::                          \n"
                   "                                                 \n"
                   "  cell_length : real = 2900 mm                   \n"
                   "                                                 \n"
                   )
      ;
  }

  {
    // Description of the 'sigma_anode_time' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("sigma_anode_time")
      .set_terse_description("The error on anode drift time")
      .set_traits(datatools::TYPE_REAL)
      // .set_long_description("Default value: ``12.5 ns``              \n")
      .set_explicit_unit(true)
      .set_unit_label("time")
      .set_unit_symbol("ns")
      .set_default_value_real(12.5 * CLHEP::ns)
      .add_example("Set the default value::                          \n"
                   "                                                 \n"
                   "  sigma_anode_time : real = 12.5 ns              \n"
                   "                                                 \n"
                   )
      ;
  }

  {
    // Description of the 'tcut' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("tcut")
      .set_terse_description("Maximum drift time to be calibrated")
      .set_traits(datatools::TYPE_REAL)
      // .set_long_description("Default value: ``10.0 us``              \n")
      .set_explicit_unit(true)
      .set_unit_label("time")
      .set_unit_symbol("ns")
      .set_default_value_real(10 * CLHEP::ns)
      .add_example("Set the default value::                          \n"
                   "                                                 \n"
                   "  tcut : real as time = 10.0 us                  \n"
                   "                                                 \n"
                   )
      ;
  }

  {
    // Description of the 'sigma_cathode_time' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("sigma_cathode_time")
      .set_terse_description("The error on cathode drift time")
      .set_traits(datatools::TYPE_REAL)
      // .set_long_description("Default value: ``100 ns``               \n")
      .set_explicit_unit(true)
      .set_unit_label("time")
      .set_unit_symbol("ns")
      .set_default_value_real(100 * CLHEP::ns)
      .add_example("Set the default value::                          \n"
                   "                                                 \n"
                   "  sigma_cathode_time : real = 100 ns             \n"
                   "                                                 \n"
                   )
      ;
  }

  {
    // Description of the 'sigma_z' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("sigma_z")
      .set_terse_description("The error on longitudinal position")
      .set_traits(datatools::TYPE_REAL)
      //.set_long_description("Default value: ``1.0 cm``               \n")
      .set_explicit_unit(true)
      .set_unit_label("length")
      .set_unit_symbol("cm")
      .set_default_value_real(1.0 * CLHEP::cm)
      .add_example("Set the default value::                          \n"
                   "                                                 \n"
                   "  sigma_z : real as length = 1.0 cm              \n"
                   "                                                 \n"
                   )
      ;
  }

  {
    // Description of the 'sigma_z_missing_cathode' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("sigma_z_missing_cathode")
      .set_terse_description("The error on longitudinal position in case of a missing cathode")
      .set_traits(datatools::TYPE_REAL)
      // .set_long_description("Default value: ``5.0 cm``                  \n")
      .set_explicit_unit(true)
      .set_unit_label("length")
      .set_unit_symbol("cm")
      .set_default_value_real(5.0 * CLHEP::cm)
      .add_example("Set the default value::                             \n"
                   "                                                    \n"
                   "  sigma_z_missing_cathode : real as length = 5.0 cm \n"
                   "                                                    \n"
                   )
      ;
  }

  {
    // Description of the 'sigma_r_a' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("sigma_r_a")
      .set_terse_description("First parameter of the error on drift radius")
      .set_traits(datatools::TYPE_REAL)
      //.set_long_description("Default value: ``0.425 mm``             \n")
      .set_explicit_unit(true)
      .set_unit_label("length")
      .set_unit_symbol("mm")
      .set_default_value_real(0.425 * CLHEP::mm)
      .add_example("Set the default value::                          \n"
                   "                                                 \n"
                   "  sigma_r_a : real = 0.425 mm                    \n"
                   "                                                 \n"
                   )
      ;
  }

  {
    // Description of the 'sigma_r_b' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("sigma_r_b")
      .set_terse_description("Second parameter of the error on drift radius")
      .set_traits(datatools::TYPE_REAL)
      //.set_long_description("Default value: ``0.0083``  \n")
      .set_explicit_unit(false)
      .set_default_value_real(0.0083)
      .add_example("Set the default value::     \n"
                   "                            \n"
                   "  sigma_r_b : real = 0.0083 \n"
                   "                            \n"
                   )
      ;
  }

  {
    // Description of the 'sigma_r_r0' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("sigma_r_r0")
      .set_terse_description("Third parameter of the error on drift radius")
      .set_traits(datatools::TYPE_REAL)
      // .set_long_description("Default value: ``12.25 mm``\n")
      .set_explicit_unit(true)
      .set_unit_label("length")
      .set_unit_symbol("mm")
      .set_default_value_real(12.25 * CLHEP::mm)
      .add_example("Set the default value::          \n"
                   "                                 \n"
                   "  sigma_r_r0 : real = 12.25 mm   \n"
                   "                                 \n"
                   )
      ;
  }


  {
    // Description of the 'base_anode_efficiency' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("base_anode_efficiency")
      .set_terse_description("Base anode efficiency")
      .set_traits(datatools::TYPE_REAL)
      //.set_long_description("Default value: ``1.0``          \n")
      .set_explicit_unit(false)
      .set_default_value_real(1.0)
      .add_example("Set the default value::              \n"
                   "                                     \n"
                   "  base_anode_efficiency : real = 1.0 \n"
                   "                                     \n"
                   )
      ;
  }

  {
    // Description of the 'base_cathode_efficiency' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("base_cathode_efficiency")
      .set_terse_description("Base cathode efficiency")
      .set_traits(datatools::TYPE_REAL)
      //.set_long_description("Default value: ``1.0``                      \n")
      .set_explicit_unit(false)
      .set_default_value_real(1.0)
      .add_example("Set the default value::                          \n"
                   "                                                 \n"
                   "  base_cathode_efficiency : real = 1.0           \n"
                   "                                                 \n"
                   )
      ;
  }

  {
    // Description of the 'plasma_longitudinal_speed' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("plasma_longitudinal_speed")
      .set_terse_description("Plasma longitudinal speed")
      .set_traits(datatools::TYPE_REAL)
      // .set_long_description("Default value: ``5.0 cm/us``                \n")
      .set_explicit_unit(true)
      .set_unit_label("velocity")
      .set_unit_symbol("cm/us")
      .set_default_value_real(5.0 * CLHEP::cm / CLHEP::microsecond)
      .add_example("Set the default value::                                    \n"
                   "                                                           \n"
                   "  plasma_longitudinal_speed : real as velocity = 5.0 cm/us \n"
                   "                                                           \n"
                   )
      ;
  }

  {
    // Description of the 'sigma_plasma_longitudinal_speed' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("sigma_plasma_longitudinal_speed")
      .set_terse_description("Error on plasma longitudinal speed")
      .set_traits(datatools::TYPE_REAL)
      //.set_long_description("Default value: ``0.5 cm/us``                \n")
      .set_explicit_unit(true)
      .set_unit_label("velocity")
      .set_unit_symbol("cm/us")
      .set_default_value_real(0.5 * CLHEP::cm / CLHEP::microsecond)
      .add_example("Set the default value::                                          \n"
                   "                                                                 \n"
                   "  sigma_plasma_longitudinal_speed : real as velocity = 0.5 cm/us \n"
                   "                                                                 \n"
                   )
      ;
  }

  // Additionnal configuration hints :
  ocd_.set_configuration_hints("Here is a full configuration example in the                      \n"
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
                               "                                                                 \n"
                               );

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END() // Closing macro for implementation

// Registration macro for class 'snemo::processing::geiger_regime' :
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::processing::geiger_regime,"snemo::processing::geiger_regime")

// end of falaise/snemo/processing/geiger_regime.cc
