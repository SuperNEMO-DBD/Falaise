/// \file falaise/snemo/datamodels/sim_tracker_digi_hit.cc

// Ourselves:
#include <falaise/snemo/datamodels/sim_tracker_digi_hit.h>

namespace snemo {

  namespace datamodel {

    // Serial tag :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(sim_tracker_digi_hit,
						      "snemo::datamodel::sim_tracker_digi_hit")

    sim_tracker_digi_hit::sim_tracker_digi_hit()
    {
      _set_defaults();
    }

    sim_tracker_digi_hit::~sim_tracker_digi_hit() {}

    bool sim_tracker_digi_hit::is_valid() const
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

    bool sim_tracker_digi_hit::has_elec_id() const
    {
      return _elec_id_.is_valid();
    }

    void sim_tracker_digi_hit::invalidate_elec_id()
    {
      _elec_id_.invalidate();
      return;
    }

    const geomtools::geom_id & sim_tracker_digi_hit::get_elec_id() const
    {
      return _elec_id_;
    }

    geomtools::geom_id & sim_tracker_digi_hit::grab_elec_id()
    {
      return _elec_id_;
    }

    void sim_tracker_digi_hit::set_elec_id(const geomtools::geom_id & an_eid_)
    {
      _elec_id_ = an_eid_;
      return;
    }

    const uint64_t & sim_tracker_digi_hit::get_trigger_id() const
    {
      return _trigger_id_;
    }

    void sim_tracker_digi_hit::set_trigger_id(const uint64_t & a_tid_)
    {
      _trigger_id_ = a_tid_;
      return;
    }

    void sim_tracker_digi_hit::set_anode_R0(int64_t t_)
    {
      _anode_R0_ = t_;
      return;
    }

    int64_t sim_tracker_digi_hit::get_anode_R0() const
    {
      return _anode_R0_;
    }

    void sim_tracker_digi_hit::set_anode_R1(int64_t t_)
    {
      _anode_R1_ = t_;
      return;
    }

    int64_t sim_tracker_digi_hit::get_anode_R1() const
    {
      return _anode_R1_;
    }

    void sim_tracker_digi_hit::set_anode_R2(int64_t t_)
    {
      _anode_R2_ = t_;
      return;
    }

    int64_t sim_tracker_digi_hit::get_anode_R2() const
    {
      return _anode_R2_;
    }

    void sim_tracker_digi_hit::set_anode_R3(int64_t t_)
    {
      _anode_R3_ = t_;
      return;
    }

    int64_t sim_tracker_digi_hit::get_anode_R3() const
    {
      return _anode_R3_;
    }

    void sim_tracker_digi_hit::set_anode_R4(int64_t t_)
    {
      _anode_R4_ = t_;
      return;
    }

    int64_t sim_tracker_digi_hit::get_anode_R4() const
    {
      return _anode_R4_;
    }

    void sim_tracker_digi_hit::set_cathode_R5(int64_t t_)
    {
      _cathode_R5_ = t_;
      return;
    }

    int64_t sim_tracker_digi_hit::get_cathode_R5() const
    {
      return _cathode_R5_;
    }

    void sim_tracker_digi_hit::set_cathode_R6(int64_t t_)
    {
      _cathode_R6_ = t_;
      return;
    }

    int64_t sim_tracker_digi_hit::get_cathode_R6() const
    {
      return _cathode_R6_;
    }

    void sim_tracker_digi_hit::_set_defaults()
    {
      _trigger_id_ = -1;
      _anode_R0_   = INVALID_TIME;
      _anode_R1_   = INVALID_TIME;
      _anode_R2_   = INVALID_TIME;
      _anode_R3_   = INVALID_TIME;
      _anode_R4_   = INVALID_TIME;
      _cathode_R5_ = INVALID_TIME;
      _cathode_R6_ = INVALID_TIME;
      return;
    }

    void sim_tracker_digi_hit::tree_dump(std::ostream& out_,
					 const std::string& title_,
					 const std::string& indent_,
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

      out_ << indent_ << datatools::i_tree_dumpable::tag << "Anode t0 : ";
      out_ << _anode_R0_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag << "Anode t1 : ";
      out_ << _anode_R1_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag << "Anode t2 : ";
      out_ << _anode_R2_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag << "Anode t3 : ";
      out_ << _anode_R3_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag << "Anode t4 : ";
      out_ << _anode_R4_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag << "Cathode t5 : ";
      out_ << _cathode_R5_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag << "Cathode t6 : ";
      out_ << _cathode_R6_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
	   << "Validity : " << (is_valid() ? "<yes>" : "<no>") << std::endl;

      return;
    }

  }  // end of namespace datamodel

}  // end of namespace snemo
