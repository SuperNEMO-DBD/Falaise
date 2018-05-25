/// \file falaise/snemo/datamodels/sim_calo_digi_hit.cc

// Ourselves:
#include <falaise/snemo/datamodels/sim_calo_digi_hit.h>

namespace snemo {

  namespace datamodel {

    // Serial tag :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(sim_calo_digi_hit,
						      "snemo::datamodel::sim_calo_digi_hit")

    sim_calo_digi_hit::sim_calo_digi_hit()
    {
      return;
    }

    sim_calo_digi_hit::~sim_calo_digi_hit()
    {
      return;
    }

    bool sim_calo_digi_hit::is_valid() const
    {
      if (!has_hit_id())
	{
	  return false;
	}
      if (!(has_geom_id() || has_elec_id()))
	{
	  return false;
	}

      return true;
    }

    bool sim_calo_digi_hit::has_elec_id() const
    {
      return _elec_id_.is_valid();
    }

    void sim_calo_digi_hit::invalidate_elec_id()
    {
      _elec_id_.invalidate();
      return;
    }

    const geomtools::geom_id & sim_calo_digi_hit::get_elec_id() const
    {
      return _elec_id_;
    }

    geomtools::geom_id & sim_calo_digi_hit::grab_elec_id()
    {
      return _elec_id_;
    }

    void sim_calo_digi_hit::set_elec_id(const geomtools::geom_id & an_eid_)
    {
      _elec_id_ = an_eid_;
      return;
    }

    const uint64_t & sim_calo_digi_hit::get_trigger_id() const
    {
      return _trigger_id_;
    }

    void sim_calo_digi_hit::set_trigger_id(const uint64_t & a_tid_)
    {
      _trigger_id_ = a_tid_;
      return;
    }

    const std::vector<int16_t> & sim_calo_digi_hit::get_waveform() const
    {
      return _waveform_;
    }

    std::vector<int16_t> & sim_calo_digi_hit::grab_waveform()
    {
      return _waveform_;
    }

    void sim_calo_digi_hit::tree_dump(std::ostream & out_,
				      const std::string & title_,
				      const std::string & indent_,
				      bool inherit_) const
    {
      std::string indent;
      if (!indent_.empty())
	{
	  indent = indent_;
	}

      base_hit::tree_dump(out_, title_, indent_, true);

      out_ << indent << datatools::i_tree_dumpable::tag << "Electronic ID : ";
      if (_elec_id_.is_valid()) {
	out_ << _elec_id_;
      } else {
	out_ << "<none>";
      }
      out_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag << "Trigger ID : ";
      out_ << _trigger_id_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
	   << "Validity : " << (is_valid() ? "<yes>" : "<no>") << std::endl;

      return;
    }

  }  // end of namespace datamodel

}  // end of namespace snemo
