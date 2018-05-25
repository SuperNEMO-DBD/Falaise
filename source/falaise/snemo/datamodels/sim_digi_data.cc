/// \file falaise/snemo/datamodels/sim_digi_data.cc

// Ourselves:
#include <falaise/snemo/datamodels/sim_digi_data.h>

namespace snemo {

  namespace datamodel {

    // Serial tag :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(sim_digi_data, "snemo::datamodel::sim_digi_data")

    sim_digi_data::sim_digi_data()
    {
    }

    sim_digi_data::~sim_digi_data()
    {
    }

    bool sim_digi_data::is_valid() const
    {
      return true;
    }

    void sim_digi_data::reset()
    {
      _calo_digi_hits_.clear();
      _tracker_digi_hits_.clear();
      _trigger_data_.clear();
      _properties_.clear();
      return;
    }

    const sim_digi_data::calo_digi_hit_collection_type & sim_digi_data::get_calo_digi_hits() const
    {
      return _calo_digi_hits_;
    }

    sim_digi_data::calo_digi_hit_collection_type & sim_digi_data::grab_calo_digi_hits()
    {
      return _calo_digi_hits_;
    }

    const sim_digi_data::tracker_digi_hit_collection_type & sim_digi_data::get_tracker_digi_hits() const
    {
      return _tracker_digi_hits_;
    }

    sim_digi_data::tracker_digi_hit_collection_type & sim_digi_data::grab_tracker_digi_hits()
    {
      return _tracker_digi_hits_;
    }

    const sim_digi_data::trigger_digi_data_collection_type & sim_digi_data::get_trigger_digi_data() const
    {
      return _trigger_data_;
    }

    sim_digi_data::trigger_digi_data_collection_type & sim_digi_data::grab_trigger_digi_data()
    {
      return _trigger_data_;
    }

    const datatools::properties & sim_digi_data::get_properties() const
    {
      return _properties_;
    }

    datatools::properties & sim_digi_data::grab_properties()
    {
      return _properties_;
    }

    void sim_digi_data::tree_dump(std::ostream & out_,
				  const std::string & title_,
				  const std::string & indent_,
				  bool inherit_) const
    {
      if (!title_.empty())
	{
	  out_ << indent_ << title_ << std::endl;
	}

      // Auxiliary properties:
      {
	out_ << indent_ << datatools::i_tree_dumpable::tag << "Auxiliary properties : ";
	if (_properties_.size() == 0)
	  {
	    out_ << "<empty>";
	  }
	out_ << std::endl;
	{
	  std::ostringstream indent_oss;
	  indent_oss << indent_;
	  indent_oss << datatools::i_tree_dumpable::skip_tag;
	  _properties_.tree_dump(out_, "", indent_oss.str());
	}
      }

      // Calorimeter hits:
      {
	out_ << indent_ << datatools::i_tree_dumpable::tag << "Calorimeter Hits : " << std::endl;
	out_ << _calo_digi_hits_.size();
	out_ << std::endl;
      }
      // Tracker hits:
      {
	out_ << indent_ << datatools::i_tree_dumpable::tag << "Tracker Hits : " << std::endl;
	out_ << _tracker_digi_hits_.size();
	out_ << std::endl;
      }
      // Trigger data:
      {
	out_ << indent_ << datatools::i_tree_dumpable::tag << "Trigger data : " << std::endl;
	out_ << _trigger_data_.size();
	out_ << std::endl;
      }

      // Validity:
      out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_);
      out_ << "Valid: " << is_valid() << std::endl;

      return;
    }

  }  // end of namespace datamodel

}  // end of namespace snemo
