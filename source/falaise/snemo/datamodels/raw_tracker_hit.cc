// -*- mode: c++ ; -*-
/** \file falaise/snemo/datamodels/raw_tracker_hit.cc
 */

// Ourselves
#include <falaise/snemo/datamodels/raw_tracker_hit.h>

// Third party
// - Bayeux/datatools
//#include <datatools/clhep_units.h>
//#include <datatools/utils.h>

namespace snemo {

  namespace datamodel {

    // Serial tag for datatools::serialization::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(raw_tracker_hit, "snemo::datamodel::raw_tracker_hit")

    bool raw_tracker_hit::is_valid() const
    {
      return _data_description_;
    }

    void raw_tracker_hit::set_trigger_id(uint16_t tid_)
    {
      _trigger_id_ = tid_;
      return;
    }

    uint16_t raw_tracker_hit::get_trigger_id() const
    {
      return _trigger_id_;
    }

    void raw_tracker_hit::set_channel_address(uint32_t ca_)
    {
      _channel_address_ = ca_;
      return;
    }

    uint32_t raw_tracker_hit::get_channel_address() const
    {
      return _channel_address_;
    }

    uint32_t raw_tracker_hit::get_module_id() const
    {
      uint32_t id = 0;
      id = _channel_address_ & ADDRESS_MODULE_ID;
      id >>= 12;
      return id;
    }

    uint32_t raw_tracker_hit::get_crate_id() const
    {
      uint32_t id = 0;
      id = _channel_address_ & ADDRESS_CRATE_ID;
      id >>= 10;
      return id;
    }

    uint32_t raw_tracker_hit::get_board_id() const
    {
      uint32_t id = 0;
      id = _channel_address_ & ADDRESS_BOARD_ID;
      id >>= 6;
      return id;
    }

    uint32_t raw_tracker_hit::get_channel_id() const
    {
      uint32_t id = 0;
      id = _channel_address_ & ADDRESS_CHANNEL_ID;
      return id;
    }

    void raw_tracker_hit::set_data_description(uint16_t dd_)
    {
      _data_description_ = dd_;
      return;
    }

    uint16_t raw_tracker_hit::get_data_description() const
    {
      return _data_description_;
    }

    bool raw_tracker_hit::is_mode_nemo3() const
    {
      return ! (_data_description_ & DATA_DESC_MODE);
    }

    bool raw_tracker_hit::is_mode_alternative() const
    {
      return (_data_description_ & DATA_DESC_MODE);
    }

    bool raw_tracker_hit::has_anode_t0() const
    {
      return _data_description_ & DATA_DESC_HAS_ANODE_T0;
    }

    bool raw_tracker_hit::has_anode_t1() const
    {
      return _data_description_ & DATA_DESC_HAS_ANODE_T1;
    }

    bool raw_tracker_hit::has_anode_t2() const
    {
      return _data_description_ & DATA_DESC_HAS_ANODE_T2;
    }

    bool raw_tracker_hit::has_anode_t3() const
    {
      return _data_description_ & DATA_DESC_HAS_ANODE_T3;
    }

    bool raw_tracker_hit::has_anode_t4() const
    {
      return _data_description_ & DATA_DESC_HAS_ANODE_T4;
    }

    bool raw_tracker_hit::has_cathode_t5() const
    {
      return _data_description_ & DATA_DESC_HAS_CATHODE_T5;
    }

    bool raw_tracker_hit::has_cathode_t6() const
    {
      return _data_description_ & DATA_DESC_HAS_CATHODE_T6;
    }


    void raw_tracker_hit::set_anode_t0(uint64_t t_)
    {
      _anode_t0_ = t_;
      return ;
    }

    uint64_t raw_tracker_hit::get_anode_t0() const
    {
      return _anode_t0_;
    }

    void raw_tracker_hit::set_anode_t1(uint16_t t_)
    {
      _anode_t1_ = t_;
      return ;
    }

    uint16_t raw_tracker_hit::get_anode_t1() const
    {
      return _anode_t1_;
    }

    void raw_tracker_hit::set_anode_t2(uint16_t t_)
    {
      _anode_t2_ = t_;
      return ;
    }

    uint16_t raw_tracker_hit::get_anode_t2() const
    {
      return _anode_t2_;
    }

    void raw_tracker_hit::set_anode_t3(uint16_t t_)
    {
      _anode_t3_ = t_;
      return ;
    }

    uint16_t raw_tracker_hit::get_anode_t3() const
    {
      return _anode_t3_;
    }

    void raw_tracker_hit::set_anode_t4(uint16_t t_)
    {
      _anode_t4_ = t_;
      return ;
    }

    uint16_t raw_tracker_hit::get_anode_t4() const
    {
      return _anode_t4_;
    }

    void raw_tracker_hit::set_cathode_t5(uint16_t t_)
    {
      _cathode_t5_ = t_;
      return ;
    }

    uint16_t raw_tracker_hit::get_cathode_t5() const
    {
      return _cathode_t5_;
    }

    void raw_tracker_hit::set_cathode_t6(uint16_t t_)
    {
      _cathode_t6_ = t_;
      return ;
    }

    uint16_t raw_tracker_hit::get_cathode_t6() const
    {
      return _cathode_t6_;
    }

    raw_tracker_hit::raw_tracker_hit()
    {
      _set_defaults();
      return;
    }

    raw_tracker_hit::~raw_tracker_hit()
    {
      return;
    }

    void raw_tracker_hit::reset()
    {
      _set_defaults();
      return;
    }

    void raw_tracker_hit::_set_defaults()
    {
      _trigger_id_ = 0;
      _channel_address_ = 0;
      _data_description_ = 0;
      _anode_t0_ = 0;
      _anode_t1_ = 0;
      _anode_t2_ = 0;
      _anode_t3_ = 0;
      _anode_t4_ = 0;
      _cathode_t5_ = 0;
      _cathode_t6_ = 0;
      return;
    }

    void raw_tracker_hit::tree_dump (std::ostream & out_,
                                     const std::string & title_,
                                     const std::string & indent_,
                                     bool inherit_) const
    {
      std::string indent;
      if (! indent_.empty ()) indent = indent_;
      base_hit::tree_dump (out_, title_, indent_, true);

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Trigger ID       : " << _trigger_id_ << std::endl;
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Channel address  : " << _channel_address_ << std::endl;
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Data description : " << _data_description_ << std::endl;
      if (has_anode_t0()) {
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Anode t0 : " << _anode_t0_ << std::endl;
      }
      if (has_anode_t1()) {
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Anode t1 : " << _anode_t1_ << std::endl;
      }
      if (has_anode_t2()) {
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Anode t2 : " << _anode_t2_ << std::endl;
      }
      if (has_anode_t3()) {
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Anode t3 : " << _anode_t3_ << std::endl;
      }
      if (has_anode_t4()) {
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Anode t4 : " << _anode_t4_ << std::endl;
      }
      if (has_cathode_t5()) {
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Cathode t5 : " << _cathode_t5_ << std::endl;
      }
      if (has_cathode_t6()) {
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Cathode t6 : " << _cathode_t6_ << std::endl;
      }
      out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
           << "Valid : " << is_valid() << std::endl;

      return;
    }

  } // end of namespace datamodel

} // end of namespace snemo
