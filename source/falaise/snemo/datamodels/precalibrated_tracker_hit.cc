// falaise/snemo/datamodels/precalibrated_tracker_hit.cc

// Ourselves:
#include <falaise/snemo/datamodels/precalibrated_tracker_hit.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/clhep_units.h>
#include <datatools/utils.h>


namespace snemo {

  namespace datamodel {

    // Serial tag for datatools::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(precalibrated_tracker_hit,
                                                      "snemo::datamodel::precalibrated_tracker_hit")


    double precalibrated_tracker_hit::get_reference_time() const
    {
      return _reference_time_;
    }

    void precalibrated_tracker_hit::set_reference_time(double reference_time_)
    {
      _reference_time_ = reference_time_;
      return;
    }

    bool precalibrated_tracker_hit::has_reference_time() const
    {
      return !datatools::is_valid(_reference_time_);
    }

    void precalibrated_tracker_hit::reset_reference_time()
    {
      _reference_time_ = datatools::invalid_real();
      return;
    }

    double precalibrated_tracker_hit::get_anodic_drift_time() const
    {
      return _anodic_drift_time_;
    }

    void precalibrated_tracker_hit::set_anodic_drift_time(double anodic_drift_time_)
    {
      _anodic_drift_time_ = anodic_drift_time_;
      return;
    }

    bool precalibrated_tracker_hit::has_anodic_drift_time() const
    {
      return !datatools::is_valid(_anodic_drift_time_);
    }

    void precalibrated_tracker_hit::reset_anodic_drift_time()
    {
      _anodic_drift_time_ = datatools::invalid_real();
      return;
    }

    double precalibrated_tracker_hit::get_sigma_anodic_drift_time() const
    {
      return _sigma_anodic_drift_time_;
    }

    void precalibrated_tracker_hit::set_sigma_anodic_drift_time(double sigma_anodic_drift_time_)
    {
      _sigma_anodic_drift_time_ = sigma_anodic_drift_time_;
      return;
    }

    void precalibrated_tracker_hit::reset_sigma_anodic_drift_time()
    {
      _sigma_anodic_drift_time_ = datatools::invalid_real();
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
      return !datatools::is_valid(_bottom_cathode_drift_time_);
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
      return !datatools::is_valid(_top_cathode_drift_time_);
    }

    void precalibrated_tracker_hit::reset_top_cathode_drift_time()
    {
      _top_cathode_drift_time_ = datatools::invalid_real();
      return;
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

    void precalibrated_tracker_hit::reset_sigma_top_cathode_drift_time()
    {
      _sigma_top_cathode_drift_time_ = datatools::invalid_real();
      return;
    }

    double precalibrated_tracker_hit::get_plasma_propagation_time() const
    {
      if (has_bottom_cathode_drift_time() && has_top_cathode_drift_time()) return _bottom_cathode_drift_time_ + _top_cathode_drift_time_;
      else return datatools::invalid_real();
    }

    bool precalibrated_tracker_hit::is_delayed() const
    {
      return _is_delayed_;
    }

    void precalibrated_tracker_hit::set_delayed(bool is_delayed_)
    {
      _is_delayed_ = is_delayed_;
      return;
    }

    bool precalibrated_tracker_hit::is_valid() const
    {
      return this->base_hit::is_valid();
      // GO (05/05/2022) - Have to determine when a precalib tracker hit is valid or not ?
    }

    void precalibrated_tracker_hit::invalidate()
    {
      this->base_hit::invalidate();
      datatools::invalidate(_reference_time_);
      datatools::invalidate(_anodic_drift_time_);
      datatools::invalidate(_sigma_anodic_drift_time_);
      datatools::invalidate(_bottom_cathode_drift_time_);
      datatools::invalidate(_sigma_bottom_cathode_drift_time_);
      datatools::invalidate(_top_cathode_drift_time_);
      datatools::invalidate(_sigma_top_cathode_drift_time_);
      _is_delayed_ = false;
    }

    void precalibrated_tracker_hit::clear()
    {
      precalibrated_tracker_hit::invalidate();
    }

    void precalibrated_tracker_hit::tree_dump(std::ostream& out, const std::string& title,
                                              const std::string& indent, bool /* is_last */) const
    {
      base_hit::tree_dump(out, title, indent, true);

      out << indent << datatools::i_tree_dumpable::tag << "Reference time : " << _reference_time_ / CLHEP::microsecond << " us\n"
          << indent << datatools::i_tree_dumpable::tag << "Anodic drift time : " << _anodic_drift_time_ / CLHEP::microsecond << " us\n"
          << indent << datatools::i_tree_dumpable::tag << "Sigma(anodic drift time) : " << _sigma_anodic_drift_time_ / CLHEP::microsecond << " us\n"
          << indent << datatools::i_tree_dumpable::tag << "Bottom cathode drift time : " << _bottom_cathode_drift_time_ / CLHEP::microsecond << " us\n"
          << indent << datatools::i_tree_dumpable::tag << "Sigma(bottom cathode drift time) : " << _sigma_bottom_cathode_drift_time_ / CLHEP::microsecond << " us\n"
          << indent << datatools::i_tree_dumpable::tag << "Top cathode drift time : "    << _top_cathode_drift_time_ / CLHEP::microsecond << " us\n"
          << indent << datatools::i_tree_dumpable::tag << "Sigma(top cathode drift time) : " << _sigma_top_cathode_drift_time_ / CLHEP::microsecond << " us\n"
          << indent << datatools::i_tree_dumpable::tag << "Is delayed : " << _is_delayed_ << std::endl;

    }

  }  // end of namespace datamodel

}  // end of namespace snemo
