// -*- mode: c++ ; -*-
// falaise/snemo/datamodels/precalibrated_tracker_hit.cc

// Ourselves:
#include <falaise/snemo/datamodels/precalibrated_tracker_hit.h>

// Third party:
// - Bayeux/datatools
#include <datatools/clhep_units.h>
#include <datatools/utils.h>

namespace snemo {

  namespace datamodel {

    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(precalibrated_tracker_hit,
                                                      "snemo::datamodel::precalibrated_tracker_hit")

    double precalibrated_tracker_hit::get_anodic_time() const
    {
      return _anodic_time_;
    }

    void precalibrated_tracker_hit::set_anodic_time(double anodic_time_)
    {
      _anodic_time_ = anodic_time_;
      return;
    }

    bool precalibrated_tracker_hit::has_anodic_time() const
    {
      return datatools::is_valid(_anodic_time_);
    }

    void precalibrated_tracker_hit::reset_anodic_time()
    {
      _anodic_time_ = datatools::invalid_real();
      return;
    }

    double precalibrated_tracker_hit::get_sigma_anodic_time() const
    {
      return _sigma_anodic_time_;
    }

    void precalibrated_tracker_hit::set_sigma_anodic_time(double sigma_anodic_time_)
    {
      _sigma_anodic_time_ = sigma_anodic_time_;
      return;
    }

    bool precalibrated_tracker_hit::has_sigma_anodic_time() const
    {
      return datatools::is_valid(_sigma_anodic_time_);
    }

    void precalibrated_tracker_hit::reset_sigma_anodic_time()
    {
      _sigma_anodic_time_ = datatools::invalid_real();
      return;
    }

    double precalibrated_tracker_hit::get_bottom_cathode_drift_time() const
    {
      return _bottom_cathode_drift_time_;
    }

    void precalibrated_tracker_hit::set_bottom_cathode_drift_time(double bottom_cathode_drift_time_)
    {
      _bottom_cathode_drift_time_ = bottom_cathode_drift_time_;
      return;
    }

    bool precalibrated_tracker_hit::has_bottom_cathode_drift_time() const
    {
      return datatools::is_valid(_bottom_cathode_drift_time_);
    }

    void precalibrated_tracker_hit::reset_bottom_cathode_drift_time()
    {
      _bottom_cathode_drift_time_ = datatools::invalid_real();
      return;
    }

    double precalibrated_tracker_hit::get_sigma_bottom_cathode_drift_time() const
    {
      return _sigma_bottom_cathode_drift_time_;
    }

    void precalibrated_tracker_hit::set_sigma_bottom_cathode_drift_time(double sigma_bottom_cathode_drift_time_)
    {
      _sigma_bottom_cathode_drift_time_ = sigma_bottom_cathode_drift_time_;
      return;
    }

    bool precalibrated_tracker_hit::has_sigma_bottom_cathode_drift_time() const
    {
      return datatools::is_valid(_sigma_bottom_cathode_drift_time_);
    }

    void precalibrated_tracker_hit::reset_sigma_bottom_cathode_drift_time()
    {
      _sigma_bottom_cathode_drift_time_ = datatools::invalid_real();
      return;
    }

    double precalibrated_tracker_hit::get_top_cathode_drift_time() const
    {
      return _top_cathode_drift_time_;
    }

    void precalibrated_tracker_hit::set_top_cathode_drift_time(double top_cathode_drift_time_)
    {
      _top_cathode_drift_time_ = top_cathode_drift_time_;
      return;
    }

    bool precalibrated_tracker_hit::has_top_cathode_drift_time() const
    {
      return datatools::is_valid(_top_cathode_drift_time_);
    }

    void precalibrated_tracker_hit::reset_top_cathode_drift_time()
    {
      _top_cathode_drift_time_ = datatools::invalid_real();
      return;
    }

    double precalibrated_tracker_hit::get_plasma_propagation_time() const
    {
      if (has_bottom_cathode_drift_time() && has_top_cathode_drift_time()) return _bottom_cathode_drift_time_ + _top_cathode_drift_time_;
      else return datatools::invalid_real();
    }

    double precalibrated_tracker_hit::get_sigma_top_cathode_drift_time() const
    {
      return _sigma_top_cathode_drift_time_;
    }

    void precalibrated_tracker_hit::set_sigma_top_cathode_drift_time(double sigma_top_cathode_drift_time_)
    {
      _sigma_top_cathode_drift_time_ = sigma_top_cathode_drift_time_;
      return;
    }

    bool precalibrated_tracker_hit::has_sigma_top_cathode_drift_time() const
    {
      return datatools::is_valid(_sigma_top_cathode_drift_time_);
    }

    void precalibrated_tracker_hit::reset_sigma_top_cathode_drift_time()
    {
      _sigma_top_cathode_drift_time_ = datatools::invalid_real();
      return;
    }

    bool precalibrated_tracker_hit::is_valid() const
    {
      return this->base_hit::is_valid() and has_anodic_time();
    }

    void precalibrated_tracker_hit::invalidate()
    {
      this->base_hit::invalidate();
      datatools::invalidate(_anodic_time_);
      datatools::invalidate(_sigma_anodic_time_);
      datatools::invalidate(_bottom_cathode_drift_time_);
      datatools::invalidate(_sigma_bottom_cathode_drift_time_);
      datatools::invalidate(_top_cathode_drift_time_);
      datatools::invalidate(_sigma_top_cathode_drift_time_);
    }

    void precalibrated_tracker_hit::clear()
    {
      precalibrated_tracker_hit::invalidate();
    }
  
    void precalibrated_tracker_hit::print_tree(std::ostream & out_,
					       const boost::property_tree::ptree & options_) const
    {
      base_hit::print_tree(out_, base_print_options::force_inheritance(options_));
      base_print_options popts;
      popts.configure_from(options_);
      const std::string & indent = popts.indent;

      double tmp_time_sec = _anodic_time_ / CLHEP::second;

      int time_day    = std::floor(tmp_time_sec / 86400.);
      tmp_time_sec   -= 86400. * time_day;
      int time_hour   = std::floor(tmp_time_sec / 3600.);
      tmp_time_sec   -= 3600. * time_hour;
      int time_min    = std::floor(tmp_time_sec / 60.);
      tmp_time_sec   -= 60. * time_min;
      int time_sec    = std::floor(tmp_time_sec);
      tmp_time_sec   -= time_sec;
      int time_msec   = std::floor(tmp_time_sec * 1E3);
      tmp_time_sec   -= 1E-3 * time_msec;
      int time_usec   = std::floor(tmp_time_sec * 1E6);
      tmp_time_sec   -= 1E-6 * time_usec;
      int time_nsec   = std::floor(tmp_time_sec * 1E9);

      out_ << indent << tag << "Anodic time: " << time_day << "d " << time_hour << "h "
	  << time_min << "m " << time_sec << "s " << time_msec << "ms " << time_usec << "us " << time_nsec << " ns +/- "
	  << _sigma_anodic_time_ / CLHEP::nanosecond << " ns \n";

      out_ << indent << tag << "Bottom cathode drift time: ";
      if (has_bottom_cathode_drift_time()) {
	out_ << _bottom_cathode_drift_time_ / CLHEP::microsecond << " us +/- " << _sigma_bottom_cathode_drift_time_ / CLHEP::nanosecond << " ns\n";
      } else {
	out_ << "missing\n";
      }

      out_ << indent << last_tag << "Top cathode drift time: ";
      if (has_top_cathode_drift_time()) {
	out_ << _top_cathode_drift_time_ / CLHEP::microsecond << " us +/- " << _sigma_bottom_cathode_drift_time_ / CLHEP::nanosecond << " ns\n";
      } else {
	out_ << "missing\n";
      }

      return;
    }

  } // end of namespace datamodel

} // end of namespace snemo
