// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/raw_data.cc

// Ourselves:
#include <falaise/snemo/datamodels/raw_data.h>

namespace snemo {

  namespace datamodel {

    // Serial tag for datatools::serialization::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(raw_data, "snemo::datamodel::raw_data")

    const raw_trigger_infos & raw_data::get_raw_trigger_infos() const
    {
      return _raw_trigger_infos_;
    }

    raw_trigger_infos & raw_data::grab_raw_trigger_infos()
    {
      return _raw_trigger_infos_;
    }

    const raw_data::raw_calorimeter_hit_col & raw_data::get_raw_calorimeter_hits() const
    {
      return _raw_calorimeter_hits_;
    }

    raw_data::raw_calorimeter_hit_col & raw_data::grab_raw_calorimeter_hits()
    {
      return _raw_calorimeter_hits_;
    }

    const raw_data::raw_tracker_hit_col & raw_data::get_raw_tracker_hits() const
    {
      return _raw_tracker_hits_;
    }

    raw_data::raw_tracker_hit_col & raw_data::grab_raw_tracker_hits()
    {
      return _raw_tracker_hits_;
    }

    const datatools::properties & raw_data::get_auxiliaries() const
    {
      return _auxiliaries_;
    }

    datatools::properties & raw_data::grab_auxiliaries()
    {
      return _auxiliaries_;
    }

    raw_data::raw_data()
    {
      return;
    }

    raw_data::~raw_data()
    {
      return;
    }

    void raw_data::reset()
    {
      _raw_trigger_infos_.reset();
      _raw_calorimeter_hits_.clear();
      _raw_tracker_hits_.clear();
      _auxiliaries_.clear();
      return;
    }

    void raw_data::tree_dump(std::ostream & out_,
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

      // Auxiliary properties:
      {
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Auxiliary properties : ";
        if(_auxiliaries_.size() == 0) {
          out_ << "<empty>";
        }
        out_ << std::endl;
        {
          std::ostringstream indent_oss;
          indent_oss << indent;
          indent_oss << datatools::i_tree_dumpable::skip_tag;
          _auxiliaries_.tree_dump(out_, "", indent_oss.str());
        }
      }

      {
        // Trigger informations:
        out_ << indent << datatools::i_tree_dumpable::tag;
        out_ << "Trigger informations: " << std::endl;
        {
          std::ostringstream indent_oss;
          indent_oss << indent;
          indent_oss << datatools::i_tree_dumpable::skip_tag;
          _raw_trigger_infos_.tree_dump(out_, "", indent_oss.str());
        }
      }

      // Raw calorimeter hits:
      out_ << indent << datatools::i_tree_dumpable::tag;
      out_ << "Raw calorimeter hits: " << _raw_calorimeter_hits_.size() << std::endl;

      // Raw tracker hits:
      out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_);
      out_ << "Raw tracker hits: " << _raw_tracker_hits_.size() << std::endl;

      return;
    }


  } // end of namespace datamodel

} // end of namespace snemo
