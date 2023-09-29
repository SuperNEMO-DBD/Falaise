// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/precalibrated_calorimeter_hit.cc

// Ourselves:
#include <falaise/snemo/datamodels/precalibrated_calorimeter_hit.h>

// Third party:
// - Bayeux/datatools
#include <datatools/clhep_units.h>
#include <datatools/utils.h>

namespace snemo {

  namespace datamodel {

    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(precalibrated_calorimeter_hit,
                                                      "snemo::datamodel::precalibrated_calorimeter_hit")

    double precalibrated_calorimeter_hit::get_baseline() const
    {
      return _baseline_;
    }

    bool precalibrated_calorimeter_hit::has_baseline() const
    {
      return datatools::is_valid(_baseline_);
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

    double precalibrated_calorimeter_hit::get_sigma_baseline() const
    {
      return _sigma_baseline_;
    }

    bool precalibrated_calorimeter_hit::has_sigma_baseline() const
    {
      return datatools::is_valid(_sigma_baseline_);
    }

    void precalibrated_calorimeter_hit::set_sigma_baseline(double sigma_baseline_)
    {
      _sigma_baseline_ = sigma_baseline_;
      return;
    }

    void precalibrated_calorimeter_hit::reset_sigma_baseline()
    {
      _sigma_baseline_ = datatools::invalid_real();
      return;
    }

    double precalibrated_calorimeter_hit::get_amplitude() const
    {
      return _amplitude_;
    }

    bool precalibrated_calorimeter_hit::has_amplitude() const
    {
      return datatools::is_valid(_amplitude_);
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

    double precalibrated_calorimeter_hit::get_sigma_amplitude() const
    {
      return _sigma_amplitude_;
    }

    bool precalibrated_calorimeter_hit::has_sigma_amplitude() const
    {
      return datatools::is_valid(_sigma_amplitude_);
    }

    void precalibrated_calorimeter_hit::set_sigma_amplitude(double sigma_amplitude_)
    {
      _sigma_amplitude_ = sigma_amplitude_;
      return;
    }

    void precalibrated_calorimeter_hit::reset_sigma_amplitude()
    {
      _sigma_amplitude_ = datatools::invalid_real();
      return;
    }

    double precalibrated_calorimeter_hit::get_charge() const
    {
      return _charge_;
    }

    bool precalibrated_calorimeter_hit::has_charge() const
    {
      return datatools::is_valid(_charge_);
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

    double precalibrated_calorimeter_hit::get_sigma_charge() const
    {
      return _sigma_charge_;
    }

    bool precalibrated_calorimeter_hit::has_sigma_charge() const
    {
      return datatools::is_valid(_sigma_charge_);
    }

    void precalibrated_calorimeter_hit::set_sigma_charge(double sigma_charge_)
    {
      _sigma_charge_ = sigma_charge_;
      return;
    }

    void precalibrated_calorimeter_hit::reset_sigma_charge()
    {
      _sigma_charge_ = datatools::invalid_real();
      return;
    }

    double precalibrated_calorimeter_hit::get_time() const
    {
      return _time_;
    }

    bool precalibrated_calorimeter_hit::has_time() const
    {
      return datatools::is_valid(_time_);
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

    double precalibrated_calorimeter_hit::get_sigma_time() const
    {
      return _sigma_time_;
    }

    bool precalibrated_calorimeter_hit::has_sigma_time() const
    {
      return datatools::is_valid(_sigma_time_);
    }

    void precalibrated_calorimeter_hit::set_sigma_time(double sigma_time_)
    {
      _sigma_time_ = sigma_time_;
      return;
    }

    void precalibrated_calorimeter_hit::reset_sigma_time()
    {
      _sigma_time_ = datatools::invalid_real();
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
      datatools::invalidate(_baseline_);
      datatools::invalidate(_sigma_baseline_);
      datatools::invalidate(_amplitude_);
      datatools::invalidate(_sigma_amplitude_);
      datatools::invalidate(_charge_);
      datatools::invalidate(_sigma_charge_);
      datatools::invalidate(_time_);
      datatools::invalidate(_sigma_time_);
    }

    void precalibrated_calorimeter_hit::tree_dump(std::ostream& out, const std::string& title,
                                                  const std::string& indent, bool /* is_last */) const {
      base_hit::tree_dump(out, title, indent, true);

      out << indent << datatools::i_tree_dumpable::tag << "Baseline: " << _baseline_ /CLHEP::volt;
      if (has_sigma_baseline())
	out << " +/- " << _sigma_baseline_ /CLHEP::volt;
      out << " V\n";

      out << indent << datatools::i_tree_dumpable::tag << "Amplitude: " << _amplitude_/(1E-3*CLHEP::volt);
      if (has_sigma_amplitude())
	out << " +/- " << _sigma_amplitude_ /(1E-3 * CLHEP::volt);
      out << " mV\n";

      out << indent << datatools::i_tree_dumpable::tag << "Charge: " << _charge_/(1E-9*CLHEP::volt*CLHEP::second);
      if (has_sigma_charge())
	out << " +/- " << _sigma_charge_ /(1E-9*CLHEP::volt*CLHEP::second);
      out << " nV.s\n";

      // convert absolute time into day/hour/min/sec/msec/usec/nsec format
      double tmp_time_sec = _time_ / CLHEP::second;
      int time_day    = std::floor(tmp_time_sec/86400.);
      tmp_time_sec   -= 86400.*time_day;
      int time_hour   = std::floor(tmp_time_sec/3600.);
      tmp_time_sec   -= 3600.*time_hour;
      int time_min    = std::floor(tmp_time_sec/60.);
      tmp_time_sec   -= 60.*time_min;
      int time_sec    = std::floor(tmp_time_sec);
      tmp_time_sec   -= time_sec;
      int time_msec   = std::floor(tmp_time_sec*1E3);
      tmp_time_sec   -= 1E-3*time_msec;
      int time_usec   = std::floor(tmp_time_sec*1E6);
      tmp_time_sec   -= 1E-6*time_usec;
      double time_nsec = tmp_time_sec*1E9;

      out << indent << datatools::i_tree_dumpable::last_tag << "Time: "
	  << time_day << "d " << time_hour << "h " << time_min << "m " << time_sec << "s "
	  << time_msec << "ms " << time_usec << "us " << time_nsec;
      if (has_sigma_time())
	out << "ns +- " << _sigma_time_/(1E-9*CLHEP::second);
      out << "ns\n";

      // << indent << datatools::i_tree_dumpable::tag << "Time: " << _time_ / CLHEP::second << " s\n";
    }

  }  // end of namespace datamodel

}  // end of namespace snemo
