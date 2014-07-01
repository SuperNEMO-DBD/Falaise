// -*- mode: c++ ; -*-
// falaise/snemo/datamodels/mock_raw_tracker_hit.cc

// Ourselves:
#include <falaise/snemo/datamodels/mock_raw_tracker_hit.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/clhep_units.h>

namespace snemo {

  namespace datamodel {

    /*
    // Serial tag for datatools::serialization::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(mock_raw_tracker_hit,
                                                      "snemo::datamodel::mock_raw_tracker_hit")
    */

    const double mock_raw_tracker_hit::INVALID_VALUE = std::numeric_limits<double>::quiet_NaN ();

    const std::string mock_raw_tracker_hit::NOISY_FLAG = "noisy";

    bool mock_raw_tracker_hit::is_ref_time_missing () const
    {
      return ! datatools::is_valid (_ref_time_);
    }

    bool mock_raw_tracker_hit::is_drift_time_missing () const
    {
      return ! datatools::is_valid (_drift_time_);
    }

    bool mock_raw_tracker_hit::is_top_time_missing () const
    {
      return ! datatools::is_valid (_top_time_);
    }

    bool mock_raw_tracker_hit::is_bottom_time_missing () const
    {
      return ! datatools::is_valid (_bottom_time_);
    }

    void mock_raw_tracker_hit::invalidate_ref_time ()
    {
      _ref_time_          = INVALID_VALUE;
      _sigma_ref_time_    = INVALID_VALUE;
      _store &= ~STORE_REF_TIME;
      return;
    }

    double mock_raw_tracker_hit::get_ref_time () const
    {
      return _ref_time_;
    }

    void mock_raw_tracker_hit::set_ref_time (double ref_time_)
    {
      _ref_time_ = ref_time_;
      _store |= STORE_REF_TIME;
      return;
    }

    double mock_raw_tracker_hit::get_sigma_ref_time () const
    {
      return _sigma_ref_time_;
    }

    void mock_raw_tracker_hit::set_sigma_ref_time (double sigma_ref_time_)
    {
      _sigma_ref_time_ = sigma_ref_time_;
      _store |= STORE_REF_TIME;
      return;
    }

    double mock_raw_tracker_hit::get_drift_time () const
    {
      return _drift_time_;
    }

    void mock_raw_tracker_hit::set_drift_time (double drift_time_)
    {
      _drift_time_ = drift_time_;
      _store |= STORE_TIMES;
      return;
    }

    double mock_raw_tracker_hit::get_sigma_drift_time () const
    {
      return _sigma_drift_time_;
    }

    void mock_raw_tracker_hit::set_sigma_drift_time (double sigma_drift_time_)
    {
      _sigma_drift_time_ = sigma_drift_time_;
      _store |= STORE_TIMES;
      return;
    }

    double mock_raw_tracker_hit::get_top_time () const
    {
      return _top_time_;
    }

    void mock_raw_tracker_hit::set_top_time (double top_time_)
    {
      _top_time_ = top_time_;
      _store |= STORE_TIMES;
      return;
    }

    double mock_raw_tracker_hit::get_sigma_top_time () const
    {
      return _sigma_top_time_;
    }

    void mock_raw_tracker_hit::set_sigma_top_time (double sigma_top_time_)
    {
      _sigma_top_time_ = sigma_top_time_;
      _store |= STORE_TIMES;
      return;
    }

    double mock_raw_tracker_hit::get_bottom_time () const
    {
      return _bottom_time_;
    }

    void mock_raw_tracker_hit::set_bottom_time (double bottom_time_)
    {
      _bottom_time_ = bottom_time_;
      _store |= STORE_TIMES;
      return;
    }

    double mock_raw_tracker_hit::get_sigma_bottom_time () const
    {
      return _sigma_bottom_time_;
    }

    void mock_raw_tracker_hit::set_sigma_bottom_time (double sigma_bottom_time_)
    {
      _sigma_bottom_time_ = sigma_bottom_time_;
      _store |= STORE_TIMES;
      return;
    }

    void mock_raw_tracker_hit::invalidate_times ()
    {
      _drift_time_        = INVALID_VALUE;
      _bottom_time_       = INVALID_VALUE;
      _top_time_          = INVALID_VALUE;
      _sigma_drift_time_  = INVALID_VALUE;
      _sigma_bottom_time_ = INVALID_VALUE;
      _sigma_top_time_    = INVALID_VALUE;
      _store &= ~STORE_TIMES;
      return;
    }

    mock_raw_tracker_hit::mock_raw_tracker_hit () : base_hit ()
    {
      _ref_time_          = INVALID_VALUE;
      _sigma_ref_time_    = INVALID_VALUE;
      _drift_time_        = INVALID_VALUE;
      _bottom_time_       = INVALID_VALUE;
      _top_time_          = INVALID_VALUE;
      _sigma_drift_time_  = INVALID_VALUE;
      _sigma_bottom_time_ = INVALID_VALUE;
      _sigma_top_time_    = INVALID_VALUE;
      return;
    }

    mock_raw_tracker_hit::~mock_raw_tracker_hit ()
    {
      return;
    }

    bool mock_raw_tracker_hit::is_valid () const
    {
      return this->base_hit::is_valid () && std::isnormal (_ref_time_);
    }

    void mock_raw_tracker_hit::invalidate ()
    {
      this->base_hit::invalidate ();
      invalidate_ref_time ();
      invalidate_times ();
      return;
    }

    void mock_raw_tracker_hit::clear ()
    {
      mock_raw_tracker_hit::invalidate ();
      return;
    }

    void mock_raw_tracker_hit::tree_dump (std::ostream & out_,
                                          const std::string & title_,
                                          const std::string & indent_,
                                          bool inherit_) const
    {
      std::string indent;
      if (! indent_.empty ()) indent = indent_;
      base_hit::tree_dump (out_, title_, indent_, true);

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Reference time    : " << _ref_time_ / CLHEP::ns << " ns" << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Sigma reference time    : " << _sigma_ref_time_ / CLHEP::ns << " ns" << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Drift time        : " << _drift_time_ / CLHEP::ns << " ns" << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Sigma drift time  : " << _sigma_drift_time_ / CLHEP::ns << " ns" << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Bottom time       : " << _bottom_time_ / CLHEP::microsecond << " us" << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Sigma bottom time : " << _sigma_bottom_time_ / CLHEP::microsecond << " us" << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Top time          : " << _top_time_ / CLHEP::microsecond << " us" << std::endl;

      out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
           << "Sigma top time    : " << _sigma_top_time_ / CLHEP::microsecond << " us" << std::endl;

      return;
    }

    void mock_raw_tracker_hit::dump () const
    {
      tree_dump (std::clog, "snemo::datamodel::mock_raw_tracker_hit");
      return;
    }

  } // end of namespace datamodel

} // end of namespace snemo
