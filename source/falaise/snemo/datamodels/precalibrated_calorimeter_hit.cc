// -*- mode: c++ ; -*-
/** \file falaise/snemo/datamodel/precalibrated_calorimeter_hit.cc
 */

// Ourselves
#include <falaise/snemo/datamodels/precalibrated_calorimeter_hit.h>

// Third party
// - Bayeux/datatools
#include <datatools/clhep_units.h>
#include <datatools/utils.h>

namespace snemo {

  namespace datamodel {

    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(precalibrated_calorimeter_hit,
                                                      "snemo::datamodel::precalibrated_calorimeter_hit")

    double precalibrated_calorimeter_hit::get_amplitude() const
    {
      return _amplitude_;
    }

    bool precalibrated_calorimeter_hit::has_amplitude() const
    {
      return !datatools::is_valid(_amplitude_);
    }

    void precalibrated_calorimeter_hit::set_amplitude(double amplitude_)
    {
      _amplitude_ = amplitude_;
      return;
    }

    void precalibrated_calorimeter_hit::reset_amplitude()
    {
      _amplitude_ = datatools::invalid_real();
      return;
    }

    double precalibrated_calorimeter_hit::get_charge() const
    {
      return _charge_;
    }

    bool precalibrated_calorimeter_hit::has_charge() const
    {
      return !datatools::is_valid(_charge_);
    }

    void precalibrated_calorimeter_hit::set_charge(double charge_)
    {
      _charge_ = charge_;
      return;
    }

    void precalibrated_calorimeter_hit::reset_charge()
    {
      _charge_ = datatools::invalid_real();
      return;
    }

    double precalibrated_calorimeter_hit::get_time() const
    {
      return _time_;
    }

    bool precalibrated_calorimeter_hit::has_time() const
    {
      return !datatools::is_valid(_time_);
    }

    void precalibrated_calorimeter_hit::set_time(double time_)
    {
      _time_ = time_;
      return;
    }

    void precalibrated_calorimeter_hit::reset_time()
    {
      _time_ = datatools::invalid_real();
      return;
    }

    double precalibrated_calorimeter_hit::get_baseline() const
    {
      return _baseline_;
    }

    bool precalibrated_calorimeter_hit::has_baseline() const
    {
      return !datatools::is_valid(_baseline_);
    }

    void precalibrated_calorimeter_hit::set_baseline(double baseline_)
    {
      _baseline_ = baseline_;
      return;
    }

    void precalibrated_calorimeter_hit::reset_baseline()
    {
      _baseline_ = datatools::invalid_real();
      return;
    }

    double precalibrated_calorimeter_hit::get_rising_time() const
    {
      return _rising_time_;
    }

    bool precalibrated_calorimeter_hit::has_rising_time() const
    {
      return !datatools::is_valid(_rising_time_);
    }

    void precalibrated_calorimeter_hit::set_rising_time(double rising_time_)
    {
      _rising_time_ = rising_time_;
      return;
    }

    void precalibrated_calorimeter_hit::reset_rising_time()
    {
      _rising_time_ = datatools::invalid_real();
      return;
    }

    double precalibrated_calorimeter_hit::get_falling_time() const
    {
      return _falling_time_;
    }

    bool precalibrated_calorimeter_hit::has_falling_time() const
    {
      return !datatools::is_valid(_falling_time_);
    }

    void precalibrated_calorimeter_hit::set_falling_time(double falling_time_)
    {
      _falling_time_ = falling_time_;
      return;
    }

    void precalibrated_calorimeter_hit::reset_falling_time()
    {
      _falling_time_ = datatools::invalid_real();
      return;
    }


    double precalibrated_calorimeter_hit::get_width() const
    {
      return _width_;
    }

    bool precalibrated_calorimeter_hit::has_width() const
    {
      return !datatools::is_valid(_width_);
    }

    void precalibrated_calorimeter_hit::set_width(double width_)
    {
      _width_ = width_;
      return;
    }

    void precalibrated_calorimeter_hit::reset_width()
    {
      _width_ = datatools::invalid_real();
      return;
    }

    bool precalibrated_calorimeter_hit::is_valid() const
    {
      return this->base_hit::is_valid();
      // GO (05/05/2022) - Have to determine when a precalib calo hit is valid or not ?
    }

    void precalibrated_calorimeter_hit::invalidate()
    {
      this->base_hit::invalidate();
    }

    void precalibrated_calorimeter_hit::tree_dump(std::ostream& out, const std::string& title,
                                                  const std::string& indent, bool /* is_last */) const {
      base_hit::tree_dump(out, title, indent, true);

      out << indent << datatools::i_tree_dumpable::tag << "Amplitude : " << _amplitude_ / CLHEP::volt << " V\n"
          << indent << datatools::i_tree_dumpable::tag << "Charge : " << _charge_/ (CLHEP::volt * CLHEP::second) << " V.s\n"
          << indent << datatools::i_tree_dumpable::tag << "Time : " << _time_ / CLHEP::nanosecond << " ns\n"
          << indent << datatools::i_tree_dumpable::tag << "Baseline : " << _baseline_ / CLHEP::volt << " V\n"
          << indent << datatools::i_tree_dumpable::tag << "Rising time : " << _rising_time_ / CLHEP::nanosecond << " ns\n"
          << indent << datatools::i_tree_dumpable::tag << "Falling time : " << _falling_time_ / CLHEP::nanosecond << " ns\n"
          << indent << datatools::i_tree_dumpable::tag << "Width : " << _width_ / CLHEP::nanosecond << " ns" << std::endl;

        }

  }  // end of namespace datamodel

}  // end of namespace snemo
