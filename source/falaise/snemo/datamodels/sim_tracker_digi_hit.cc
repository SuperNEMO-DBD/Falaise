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
      return;
    }

    sim_tracker_digi_hit::~sim_tracker_digi_hit()
    {
      return;
    }

    bool sim_tracker_digi_hit::is_valid() const
    {
      if (!has_hit_id())
	{
	  return false;
	}
      if (!has_geom_id())
	{
	  return false;
	}
      if (!has_anode_R0())
	{
	  return false;
	}
      return true;
    }

    void sim_tracker_digi_hit::reset()
    {
      reset_anode_R0();
      reset_anode_R1();
      reset_anode_R2();
      reset_anode_R3();
      reset_anode_R4();
      reset_cathode_R5();
      reset_cathode_R6();
      return;
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

    void sim_tracker_digi_hit::set_elec_id(const geomtools::geom_id & a_gid)
    {
      _elec_id_ = a_gid;
      return;
    }

    bool sim_tracker_digi_hit::has_anode_R0() const
    {
      return (_store & STORE_ANODE_R0);
    }

    bool sim_tracker_digi_hit::has_anode_R1() const
    {
      return (_store & STORE_ANODE_R1);
    }

    bool sim_tracker_digi_hit::has_anode_R2() const
    {
      return (_store & STORE_ANODE_R3);
    }

    bool sim_tracker_digi_hit::has_anode_R3() const
    {
      return (_store & STORE_ANODE_R3);
    }

    bool sim_tracker_digi_hit::has_anode_R4() const
    {
      return (_store & STORE_ANODE_R4);
    }

    bool sim_tracker_digi_hit::has_cathode_R5() const
    {
      return (_store & STORE_CATHODE_R5);
    }

    bool sim_tracker_digi_hit::has_cathode_R6() const
    {
      return (_store & STORE_CATHODE_R6);
    }

    void sim_tracker_digi_hit::set_anode_R0(int64_t t_)
    {
      _store_set(STORE_ANODE_R0);
      _anode_R0_ = t_;
      return;
    }

    void sim_tracker_digi_hit::reset_anode_R0()
    {
      _store_unset(STORE_ANODE_R0);
      _anode_R0_ = INVALID_TIME;
      return;
    }

    int64_t sim_tracker_digi_hit::get_anode_R0() const
    {
      DT_THROW_IF(!has_anode_R0(), std::logic_error, "Missing anode t0");
      return _anode_R0_;
    }

    void sim_tracker_digi_hit::set_anode_R1(int64_t t_)
    {
      _store_set(STORE_ANODE_R1);
      _anode_R1_ = t_;
      return;
    }

    void sim_tracker_digi_hit::reset_anode_R1()
    {
      _store_unset(STORE_ANODE_R1);
      _anode_R1_ = INVALID_TIME;
      return;
    }

    int64_t sim_tracker_digi_hit::get_anode_R1() const
    {
      DT_THROW_IF(!has_anode_R1(), std::logic_error, "Missing anode t1");
      return _anode_R1_;
    }

    void sim_tracker_digi_hit::set_anode_R2(int64_t t_)
    {
      _store_set(STORE_ANODE_R2);
      _anode_R2_ = t_;
      return;
    }

    void sim_tracker_digi_hit::reset_anode_R2() {
      _store_unset(STORE_ANODE_R2);
      _anode_R2_ = INVALID_TIME;
      return;
    }

    int64_t sim_tracker_digi_hit::get_anode_R2() const
    {
      DT_THROW_IF(!has_anode_R2(), std::logic_error, "Missing anode t2");
      return _anode_R2_;
    }

    void sim_tracker_digi_hit::set_anode_R3(int64_t t_)
    {
      _store_set(STORE_ANODE_R3);
      _anode_R3_ = t_;
      return;
    }

    void sim_tracker_digi_hit::reset_anode_R3()
    {
      _store_unset(STORE_ANODE_R3);
      _anode_R3_ = INVALID_TIME;
      return;
    }

    int64_t sim_tracker_digi_hit::get_anode_R3() const
    {
      DT_THROW_IF(!has_anode_R3(), std::logic_error, "Missing anode t3");
      return _anode_R3_;
    }

    void sim_tracker_digi_hit::set_anode_R4(int64_t t_)
    {
      _store_set(STORE_ANODE_R4);
      _anode_R4_ = t_;
      return;
    }

    void sim_tracker_digi_hit::reset_anode_R4()
    {
      _store_unset(STORE_ANODE_R4);
      _anode_R4_ = INVALID_TIME;
      return;
    }

    int64_t sim_tracker_digi_hit::get_anode_R4() const
    {
      DT_THROW_IF(!has_anode_R4(), std::logic_error, "Missing anode t4");
      return _anode_R4_;
    }

    void sim_tracker_digi_hit::set_cathode_R5(int64_t t_)
    {
      _store_set(STORE_CATHODE_R5);
      _cathode_R5_ = t_;
      return;
    }

    void sim_tracker_digi_hit::reset_cathode_R5()
    {
      _store_unset(STORE_CATHODE_R5);
      _cathode_R5_ = INVALID_TIME;
      return;
    }

    int64_t sim_tracker_digi_hit::get_cathode_R5() const
    {
      DT_THROW_IF(!has_cathode_R5(), std::logic_error, "Missing cathode t5");
      return _cathode_R5_;
    }

    void sim_tracker_digi_hit::set_cathode_R6(int64_t t_)
    {
      _store_set(STORE_CATHODE_R6);
      _cathode_R6_ = t_;
      return;
    }

    void sim_tracker_digi_hit::reset_cathode_R6()
    {
      _store_unset(STORE_CATHODE_R6);
      _cathode_R6_ = INVALID_TIME;
      return;
    }

    int64_t sim_tracker_digi_hit::get_cathode_R6() const
    {
      DT_THROW_IF(!has_cathode_R6(), std::logic_error, "Missing cathode t6");
      return _cathode_R6_;
    }

    void sim_tracker_digi_hit::tree_dump(std::ostream& out_,
					 const std::string& title_,
					 const std::string& indent_,
					 bool inherit_) const
    {
      this->geomtools::base_hit::tree_dump(out_, title_, indent_, true);

      out_ << indent_ << datatools::i_tree_dumpable::tag << "Anode t0 : ";
      if (has_anode_R0()) {
	out_ << _anode_R0_;
      } else {
	out_ << "<none>";
      }
      out_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag << "Anode t1 : ";
      if (has_anode_R1()) {
	out_ << _anode_R1_;
      } else {
	out_ << "<none>";
      }
      out_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag << "Anode t2 : ";
      if (has_anode_R2()) {
	out_ << _anode_R2_;
      } else {
	out_ << "<none>";
      }
      out_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag << "Anode t3 : ";
      if (has_anode_R3()) {
	out_ << _anode_R3_;
      } else {
	out_ << "<none>";
      }
      out_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag << "Anode t4 : ";
      if (has_anode_R4()) {
	out_ << _anode_R4_;
      } else {
	out_ << "<none>";
      }
      out_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag << "Cathode t5 : ";
      if (has_cathode_R5()) {
	out_ << _cathode_R5_;
      } else {
	out_ << "<none>";
      }
      out_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag << "Cathode t6 : ";
      if (has_cathode_R6()) {
	out_ << _cathode_R6_;
      } else {
	out_ << "<none>";
      }
      out_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
	   << "Validity : " << (is_valid() ? "<yes>" : "<no>") << std::endl;

      return;
    }

  }  // end of namespace datamodel

}  // end of namespace snemo
