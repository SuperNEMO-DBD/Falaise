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
      _trigger_id_ = -1;
      _lt_ct_25_ = -1;
      _ht_ct_25_ = -1;
      datatools::invalidate(_time_);
      _timestamp_ = -1;
      _baseline_ = 0;
      _peak_ = 0;
      _charge_ = 0;
      _falling_cell_ = -1;
      _falling_offset_ = 0;
      _rising_cell_ = -1;
      _rising_offset_ = 0;

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

    void sim_calo_digi_hit::set_waveform(std::vector<int16_t> & waveform_)
    {
      _waveform_ = waveform_;
      return;
    }

    bool sim_calo_digi_hit::is_lto() const
    {
      return _is_lto_;
    }

    void sim_calo_digi_hit::set_lto(const bool lto_)
    {
      _is_lto_ = lto_;
      return;
    }

    bool sim_calo_digi_hit::is_lt() const
    {
      return _is_lt_;
    }

    void sim_calo_digi_hit::set_lt(const bool lt_)
    {
      _is_lt_ = lt_;
      return;
    }

    bool sim_calo_digi_hit::is_ht() const
    {
      return _is_ht_;
    }

    void sim_calo_digi_hit::set_ht(const bool ht_)
    {
      _is_ht_ = ht_;
      return;
    }

    uint32_t sim_calo_digi_hit::get_lt_ct_25() const
    {
      return _lt_ct_25_;
    }

    void sim_calo_digi_hit::set_lt_ct_25(const uint32_t lt_ct_25_)
    {
      _lt_ct_25_ = lt_ct_25_;
      return;
    }

    uint32_t sim_calo_digi_hit::get_ht_ct_25() const
    {
      return _ht_ct_25_;
    }

    void sim_calo_digi_hit::set_ht_ct_25(const uint32_t ht_ct_25_)
    {
      _ht_ct_25_ = ht_ct_25_;
      return;
    }

    double sim_calo_digi_hit::get_time() const
    {
      return _time_;
    }

    void sim_calo_digi_hit::set_time(const double time_)
    {
      _time_ = time_;
      return;
    }

    int64_t sim_calo_digi_hit::get_timestamp() const
    {
      return _timestamp_;
    }

    void sim_calo_digi_hit::set_timestamp(const int64_t timestamp_)
    {
      _timestamp_ = timestamp_;
      return;
    }

    int16_t sim_calo_digi_hit::get_baseline() const
    {
      return _baseline_;
    }

    void sim_calo_digi_hit::set_baseline(const int16_t baseline_)
    {
      _baseline_ = baseline_;
      return;
    }

    int16_t sim_calo_digi_hit::get_peak() const
    {
      return _peak_;
    }

    void sim_calo_digi_hit::set_peak(const int16_t peak_)
    {
      _peak_ = peak_;
      return;
    }

    int16_t sim_calo_digi_hit::get_charge() const
    {
      return _charge_;
    }

    void sim_calo_digi_hit::set_charge(const int32_t charge_)
    {
      _charge_ = charge_;
      return;
    }

    bool sim_calo_digi_hit::is_charge_overflow() const
    {
      return _charge_overflow_;
    }

    void sim_calo_digi_hit::set_charge_overflow(const bool co_)
    {
      _charge_overflow_ = co_;
      return;
    }

    uint32_t sim_calo_digi_hit::get_rising_cell() const
    {
      return _rising_cell_;
    }

    void sim_calo_digi_hit::set_rising_cell(const uint32_t rising_cell_)
    {
      _rising_cell_ = rising_cell_;
      return;
    }

    uint16_t sim_calo_digi_hit::get_rising_offset() const
    {
      return _rising_offset_;
    }

    void sim_calo_digi_hit::set_rising_offset(const uint16_t rising_offset_)
    {
      _rising_offset_ = rising_offset_;
      return;
    }

    uint32_t sim_calo_digi_hit::get_falling_cell() const
    {
      return _falling_cell_;
    }

    void sim_calo_digi_hit::set_falling_cell(const uint32_t falling_cell_)
    {
      _falling_cell_ = falling_cell_;
      return;
    }

    uint16_t sim_calo_digi_hit::get_falling_offset() const
    {
      return _falling_offset_;
    }

    void sim_calo_digi_hit::set_falling_offset(const uint16_t falling_offset_)
    {
      _falling_offset_ = falling_offset_;
      return;
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

      out_ << indent << datatools::i_tree_dumpable::tag << "Electronic ID  : ";
      if (_elec_id_.is_valid())	{
	out_ << _elec_id_;
      } else {
	out_ << "<none>";
      }
      out_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
	   << "Trigger ID     : " << _trigger_id_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
	   << "Waveform       : " << (_waveform_.size() != 0 ? "<yes>" : "<no>") << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
	   << "Is LTO         : " << _is_lto_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
	   << "Is LT          : " << _is_lt_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
	   << "Is HT          : " << _is_ht_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
	   << "LT CT 25ns     : " << _lt_ct_25_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
	   << "HT CT 25ns     : " << _ht_ct_25_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
	   << "Time (ns)      : " << _time_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
	   << "Timestamp      : " << _timestamp_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
	   << "Baseline       : " << _baseline_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
	   << "Peak Value     : " << _peak_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
	   << "Charge Value   : " << _charge_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
	   << "Falling cell   : " << _falling_cell_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
	   << "Falling offset : " << _falling_offset_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
	   << "Rising cell    : " << _rising_cell_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
	   << "Rising offset  : " << _rising_offset_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
	   << "Validity : " << (is_valid() ? "<yes>" : "<no>") << std::endl;

      return;
    }

  }  // end of namespace datamodel

}  // end of namespace snemo
