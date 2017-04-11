// fecom/tracker_hit.cpp

// Ourselves:
#include <fecom/tracker_hit.hpp>

// Third party:
#include <bayeux/datatools/exception.h>

namespace fecom {

  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(tracker_hit, "fecom::tracker_hit")

  tracker_hit::tracker_hit()
  {
    _reset_();
  }

  tracker_hit::~tracker_hit()
  {
  }

  bool tracker_hit::is_valid() const
  {
    if (!has_geom_id()
	|| (!has_anodic_t0()
	    && !has_anodic_t1()
	    && !has_anodic_t2()
	    && !has_anodic_t3()
	    && !has_anodic_t4()
	    && !has_bot_cathodic_time()
	    && !has_top_cathodic_time())) return false;
    return true;
  }

  void tracker_hit::add_anodic_tracker_channel(const fecom::tracker_channel_hit & a_tracker_channel_)
  {
    DT_THROW_IF(a_tracker_channel_.electronic_id.get_type() != tracker_constants::ANODIC_CHANNEL_TYPE,
		std::logic_error,
		"Invalid type of tracker channel !");

    if (a_tracker_channel_.timestamp_type == "R0") set_anodic_t0(a_tracker_channel_.timestamp_value);
    else if (a_tracker_channel_.timestamp_type == "R1") set_anodic_t1(a_tracker_channel_.timestamp_value);
    else if (a_tracker_channel_.timestamp_type == "R2") set_anodic_t2(a_tracker_channel_.timestamp_value);
    else if (a_tracker_channel_.timestamp_type == "R3") set_anodic_t3(a_tracker_channel_.timestamp_value);
    else if (a_tracker_channel_.timestamp_type == "R4") set_anodic_t4(a_tracker_channel_.timestamp_value);

    return;
  }

  void tracker_hit::add_bot_cathodic_tracker_channel(const fecom::tracker_channel_hit & a_tracker_channel_)
  {
    DT_THROW_IF(a_tracker_channel_.electronic_id.get_type() != tracker_constants::CATHODIC_CHANNEL_TYPE,
		std::logic_error,
		"Invalid type of tracker channel !");
    // Dont know yet the name of the register in Jihanne data :
    if (a_tracker_channel_.timestamp_type == "R0") set_bot_cathodic_time(a_tracker_channel_.timestamp_value);

    return;
  }

  void tracker_hit::add_top_cathodic_tracker_channel(const fecom::tracker_channel_hit & a_tracker_channel_)
  {
    DT_THROW_IF(a_tracker_channel_.electronic_id.get_type() != tracker_constants::CATHODIC_CHANNEL_TYPE,
		std::logic_error,
		"Invalid type of tracker channel !");
    // Dont know yet the name of the register in Jihanne data :
    if (a_tracker_channel_.timestamp_type == "R1")  set_top_cathodic_time(a_tracker_channel_.timestamp_value);

    return;
  }


  void tracker_hit::reset()
  {
    _reset_();
    return;
  }

  void tracker_hit::_reset_()
  {
    cell_geometric_id.invalidate();
    reset_anodic_t0();
    reset_anodic_t1();
    reset_anodic_t2();
    reset_anodic_t3();
    reset_anodic_t4();
    reset_bot_cathodic_time();
    reset_top_cathodic_time();
    trigger_id = 0xFFFFFFFF;
    return;
  }

  bool tracker_hit::has_geom_id() const
  {
    return cell_geometric_id.is_valid();
  }

  bool tracker_hit::has_anodic_t0() const
  {
    bool valid = false;
    if (anodic_t0 != tracker_constants::INVALID_TIME) valid = true;
    return valid;
  }

  uint64_t tracker_hit::get_anodic_t0() const
  {
    return anodic_t0;
  }

  void tracker_hit::set_anodic_t0(const uint64_t value_)
  {
    anodic_t0 = value_;
    return;
  }

  void tracker_hit::reset_anodic_t0()
  {
    anodic_t0 = tracker_constants::INVALID_TIME;
    return;
  }

  bool tracker_hit::has_anodic_t1() const
  {
    bool valid = false;
    if (anodic_t1 != tracker_constants::INVALID_TIME) valid = true;
    return valid;
  }

  uint64_t tracker_hit::get_anodic_t1() const
  {
    return anodic_t1;
  }

  void tracker_hit::set_anodic_t1(const uint64_t value_)
  {
    anodic_t1 = value_;
    return;
  }

  void tracker_hit::reset_anodic_t1()
  {
    anodic_t1 = tracker_constants::INVALID_TIME;
    return;
  }

  bool tracker_hit::has_anodic_t2() const
  {
    bool valid = false;
    if (anodic_t2 != tracker_constants::INVALID_TIME) valid = true;
    return valid;
  }

  uint64_t tracker_hit::get_anodic_t2() const
  {
    return anodic_t2;
  }

  void tracker_hit::set_anodic_t2(const uint64_t value_)
  {
    anodic_t2 = value_;
    return;
  }

  void tracker_hit::reset_anodic_t2()
  {
    anodic_t2 = tracker_constants::INVALID_TIME;
    return;
  }

  bool tracker_hit::has_anodic_t3() const
  {
    bool valid = false;
    if (anodic_t3 != tracker_constants::INVALID_TIME) valid = true;
    return valid;
  }

  uint64_t tracker_hit::get_anodic_t3() const
  {
    return anodic_t3;
  }

  void tracker_hit::set_anodic_t3(const uint64_t value_)
  {
    anodic_t3 = value_;
    return;
  }

  void tracker_hit::reset_anodic_t3()
  {
    anodic_t3 = tracker_constants::INVALID_TIME;
    return;
  }

  bool tracker_hit::has_anodic_t4() const
  {
    bool valid = false;
    if (anodic_t4 != tracker_constants::INVALID_TIME) valid = true;
    return valid;
  }

  uint64_t tracker_hit::get_anodic_t4() const
  {
    return anodic_t4;
  }

  void tracker_hit::set_anodic_t4(const uint64_t value_)
  {
    anodic_t4 = value_;
    return;
  }

  void tracker_hit::reset_anodic_t4()
  {
    anodic_t4 = tracker_constants::INVALID_TIME;
    return;
  }

  bool tracker_hit::has_bot_cathodic_time() const
  {
    bool valid = false;
    if (bot_cathodic_time != tracker_constants::INVALID_TIME) valid = true;
    return valid;
  }

  uint64_t tracker_hit::get_bot_cathodic_time() const
  {
    return bot_cathodic_time;
  }

  void tracker_hit::set_bot_cathodic_time(const uint64_t value_)
  {
    bot_cathodic_time = value_;
    return;
  }

  void tracker_hit::reset_bot_cathodic_time()
  {
    bot_cathodic_time = tracker_constants::INVALID_TIME;
    return;
  }

  bool tracker_hit::has_top_cathodic_time() const
  {
    bool valid = false;
    if (top_cathodic_time != tracker_constants::INVALID_TIME) valid = true;
    return valid;
  }

  uint64_t tracker_hit::get_top_cathodic_time() const
  {
    return top_cathodic_time;
  }

  void tracker_hit::set_top_cathodic_time(const uint64_t value_)
  {
    top_cathodic_time = value_;
    return;
  }

  void tracker_hit::reset_top_cathodic_time()
  {
    top_cathodic_time = tracker_constants::INVALID_TIME;
    return;
  }

  void tracker_hit::set_anodic_times(const uint64_t t0_,
				     const uint64_t t1_,
				     const uint64_t t2_,
				     const uint64_t t3_,
				     const uint64_t t4_)
  {
    set_anodic_t0(t0_);
    set_anodic_t1(t1_);
    set_anodic_t2(t2_);
    set_anodic_t3(t3_);
    set_anodic_t4(t4_);
    return;
  }

  void tracker_hit::set_cathodic_times(const uint64_t bot_time_,
				       const uint64_t top_time_)
  {
    set_bot_cathodic_time(bot_time_);
    set_top_cathodic_time(top_time_);
    return;
  }

  void tracker_hit::reset_times()
  {
    reset_anodic_t0();
    reset_anodic_t1();
    reset_anodic_t2();
    reset_anodic_t3();
    reset_anodic_t4();
    reset_bot_cathodic_time();
    reset_top_cathodic_time();
    return;
  }

  void tracker_hit::tree_dump(std::ostream & out_,
			      const std::string & title_,
			      const std::string & indent_,
			      bool inherit_) const
  {
    if (!title_.empty()) {
      out_ << indent_ << title_ << std::endl;
    }

    out_ << indent_ << io::tag()
         << "Cell geometric ID         : " << (geomtools::geom_id) cell_geometric_id << std::endl;

    out_ << indent_ << io::tag()
         << "Anodic time 0             : " << anodic_t0 << std::endl;

    out_ << indent_ << io::tag()
         << "Anodic time 1             : " << anodic_t1 << std::endl;

    out_ << indent_ << io::tag()
         << "Anodic time 2             : " << anodic_t2 << std::endl;

    out_ << indent_ << io::tag()
         << "Anodic time 3             : " << anodic_t3 << std::endl;

    out_ << indent_ << io::tag()
         << "Anodic time 4             : " << anodic_t4 << std::endl;

    out_ << indent_ << io::tag()
         << "Bottom cathodic time      : " << bot_cathodic_time << std::endl;

    out_ << indent_ << io::tag()
         << "Top cathodic time         : " << top_cathodic_time << std::endl;

    out_ << indent_ << io::inherit_last_tag(inherit_)
         << "Validity                  : " << std::boolalpha << is_valid() << std::endl;

    return;
  }


} // namespace fecom
