// -*- mode: c++ ; -*-
/** \file falaise/snemo/datamodels/calibrated_data.cc
 */

// Ourselves
#include <falaise/snemo/datamodels/calibrated_data.h>

namespace snemo {

  namespace datamodel {

    // Serial tag for datatools::serialization::i_serializable interface :
    // DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(calibrated_data, "snemo::core::model::calibrated_data")
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(calibrated_data, "snemo::datamodel::calibrated_data")

    bool calibrated_data::has_data() const
    {
      return has_calibrated_calorimeter_hits() || has_calibrated_tracker_hits();
    }

    bool calibrated_data::has_calibrated_calorimeter_hits() const
    {
      return _calibrated_calorimeter_hits_.size() != 0;
    }

    const calibrated_data::calorimeter_hit_collection_type & calibrated_data::calibrated_calorimeter_hits() const
    {
      return _calibrated_calorimeter_hits_;
    }

    calibrated_data::calorimeter_hit_collection_type & calibrated_data::calibrated_calorimeter_hits()
    {
      return _calibrated_calorimeter_hits_;
    }

    bool calibrated_data::has_calibrated_tracker_hits() const
    {
      return _calibrated_tracker_hits_.size() != 0;
    }

    const calibrated_data::tracker_hit_collection_type & calibrated_data::calibrated_tracker_hits() const
    {
      return _calibrated_tracker_hits_;
    }

    calibrated_data::tracker_hit_collection_type & calibrated_data::calibrated_tracker_hits()
    {
      return _calibrated_tracker_hits_;
    }

    const datatools::properties & calibrated_data::get_properties() const
    {
      return _properties_;
    }

    datatools::properties & calibrated_data::grab_properties()
    {
      return _properties_;
    }

    calibrated_data::calibrated_data()
    {
      return;
    }

    calibrated_data::~calibrated_data()
    {
      clear();
      return;
    }

    void calibrated_data::reset_calibrated_calorimeter_hits()
    {
      _calibrated_calorimeter_hits_.clear();
      return;
    }

    void calibrated_data::reset_calibrated_tracker_hits()
    {
      _calibrated_tracker_hits_.clear();
      return;
    }

    void calibrated_data::reset()
    {
      reset_calibrated_calorimeter_hits();
      reset_calibrated_tracker_hits();
      _properties_.clear();
      return;
    }

    void calibrated_data::clear()
    {
      reset();
      return;
    }

    void calibrated_data::tree_dump(std::ostream & out_,
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

      // Properties:
      {
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Properties : ";
        if(_properties_.size() == 0) {
          out_ << "<empty>";
        }
        out_ << std::endl;
        {
          std::ostringstream indent_oss;
          indent_oss << indent;
          indent_oss << datatools::i_tree_dumpable::skip_tag;
          _properties_.tree_dump(out_, "", indent_oss.str());
        }
      }

      // Calibrated calorimeter hits:
      out_ << indent << datatools::i_tree_dumpable::tag;
      out_ << "Calibrated calorimeter hits: " << _calibrated_calorimeter_hits_.size() << std::endl;

      // Calibrated tracker hits:
      out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_);
      out_ << "Calibrated tracker hits: " << _calibrated_tracker_hits_.size() << std::endl;

      return;
    }

  } // end of namespace datamodel

} // end of namespace snemo

// end of falaise/snemo/datamodels/calibrated_data.cc
