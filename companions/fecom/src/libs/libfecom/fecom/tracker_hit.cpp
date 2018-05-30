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
    if (!has_fecom_geom_id()
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

    if (a_tracker_channel_.timestamp_type == "R0") {
      set_anodic_timestamp_t0(a_tracker_channel_.timestamp_value);
    }

    else if (a_tracker_channel_.timestamp_type == "R1") {
      set_anodic_timestamp_t1(a_tracker_channel_.timestamp_value);
    }

    else if (a_tracker_channel_.timestamp_type == "R2") {
      set_anodic_timestamp_t2(a_tracker_channel_.timestamp_value);
    }

    else if (a_tracker_channel_.timestamp_type == "R3") {
      set_anodic_timestamp_t3(a_tracker_channel_.timestamp_value);
    }

    else if (a_tracker_channel_.timestamp_type == "R4") {
      set_anodic_timestamp_t4(a_tracker_channel_.timestamp_value);
    }

    return;
  }

  void tracker_hit::add_bot_cathodic_tracker_channel(const fecom::tracker_channel_hit & a_tracker_channel_)
  {
    DT_THROW_IF(a_tracker_channel_.electronic_id.get_type() != tracker_constants::CATHODIC_CHANNEL_TYPE,
		std::logic_error,
		"Invalid type of tracker channel !");

    // Dont know yet the name of the register in Jihanne data :
    if (a_tracker_channel_.timestamp_type == "R0") {
      set_bot_cathodic_timestamp(a_tracker_channel_.timestamp_value);
    }

    return;
  }

  void tracker_hit::add_top_cathodic_tracker_channel(const fecom::tracker_channel_hit & a_tracker_channel_)
  {
    DT_THROW_IF(a_tracker_channel_.electronic_id.get_type() != tracker_constants::CATHODIC_CHANNEL_TYPE,
		std::logic_error,
		"Invalid type of tracker channel !");

    // Dont know yet the name of the register in Jihanne data :
    if (a_tracker_channel_.timestamp_type == "R0") {
      set_top_cathodic_timestamp(a_tracker_channel_.timestamp_value);
    }

    return;
  }

  void tracker_hit::reset()
  {
    _reset_();
    return;
  }

  void tracker_hit::_reset_()
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
  double tracker_hit::get_timestamp() const
  {
    if (has_anodic_t0()) return anodic_t0_ns;
    else return -1;
  }

  bool tracker_hit::has_anodic_t0() const
  {
    bool valid = false;
    if (anodic_timestamp_t0 != tracker_constants::INVALID_TIME
	&& datatools::is_valid(anodic_t0_ns)) valid = true;
    return valid;
  }

  void tracker_hit::set_anodic_timestamp_t0(const uint64_t value_)
  {
    anodic_timestamp_t0 = value_;
    anodic_t0_ns = anodic_timestamp_t0 * tracker_constants::tracker_clock_tick() / CLHEP::nanosecond;
    return;
  }

  void tracker_hit::reset_anodic_t0()
  {
    anodic_timestamp_t0 = tracker_constants::INVALID_TIME;
    datatools::invalidate(anodic_t0_ns);
    return;
  }

  // uint64_t tracker_hit::get_anodic_t0() const
  // {
  //   return anodic_t0;
  // }

  bool tracker_hit::has_anodic_t1() const
  {
    bool valid = false;
    if (anodic_timestamp_t1 != tracker_constants::INVALID_TIME
	&& datatools::is_valid(anodic_t1_ns)) valid = true;
    return valid;
  }

  void tracker_hit::set_anodic_timestamp_t1(const uint64_t value_)
  {
    anodic_timestamp_t1 = value_;
    anodic_t1_ns = anodic_timestamp_t1 * tracker_constants::tracker_clock_tick() / CLHEP::nanosecond;
    return;
  }

  void tracker_hit::reset_anodic_t1()
  {
    anodic_timestamp_t1 = tracker_constants::INVALID_TIME;
    datatools::invalidate(anodic_t1_ns);
    return;
  }

  bool tracker_hit::has_anodic_t2() const
  {
    bool valid = false;
    if (anodic_timestamp_t2 != tracker_constants::INVALID_TIME
	&& datatools::is_valid(anodic_t2_ns)) valid = true;
    return valid;
  }

  void tracker_hit::set_anodic_timestamp_t2(const uint64_t value_)
  {
    anodic_timestamp_t2 = value_;
    anodic_t2_ns = anodic_timestamp_t2 * tracker_constants::tracker_clock_tick() / CLHEP::nanosecond;
    return;
  }

  void tracker_hit::reset_anodic_t2()
  {
    anodic_timestamp_t2 = tracker_constants::INVALID_TIME;
    datatools::invalidate(anodic_t2_ns);
    return;
  }

  bool tracker_hit::has_anodic_t3() const
  {
    bool valid = false;
    if (anodic_timestamp_t3 != tracker_constants::INVALID_TIME
	&& datatools::is_valid(anodic_t3_ns)) valid = true;
    return valid;
  }

  void tracker_hit::set_anodic_timestamp_t3(const uint64_t value_)
  {
    anodic_timestamp_t3 = value_;
    anodic_t3_ns = anodic_timestamp_t3 * tracker_constants::tracker_clock_tick() / CLHEP::nanosecond;
    return;
  }

  void tracker_hit::reset_anodic_t3()
  {
    anodic_timestamp_t3 = tracker_constants::INVALID_TIME;
    datatools::invalidate(anodic_t3_ns);
    return;
  }

  bool tracker_hit::has_anodic_t4() const
  {
    bool valid = false;
    if (anodic_timestamp_t4 != tracker_constants::INVALID_TIME
	&& datatools::is_valid(anodic_t4_ns)) valid = true;
    return valid;
  }

  void tracker_hit::set_anodic_timestamp_t4(const uint64_t value_)
  {
    anodic_timestamp_t4 = value_;
    anodic_t4_ns = anodic_timestamp_t4 * tracker_constants::tracker_clock_tick() / CLHEP::nanosecond;
    return;
  }

  void tracker_hit::reset_anodic_t4()
  {
    anodic_timestamp_t4 = tracker_constants::INVALID_TIME;
    datatools::invalidate(anodic_t4_ns);
    return;
  }

  bool tracker_hit::has_bot_cathodic_time() const
  {
    bool valid = false;
    if (bot_cathodic_timestamp != tracker_constants::INVALID_TIME
	&& datatools::is_valid(bot_cathodic_time_ns)) valid = true;
    return valid;
  }

  void tracker_hit::set_bot_cathodic_timestamp(const uint64_t value_)
  {
    bot_cathodic_timestamp = value_;
    bot_cathodic_time_ns = bot_cathodic_timestamp * tracker_constants::tracker_clock_tick() / CLHEP::nanosecond;
    return;
  }

  void tracker_hit::reset_bot_cathodic_time()
  {
    bot_cathodic_timestamp = tracker_constants::INVALID_TIME;
    datatools::invalidate(bot_cathodic_time_ns);
    return;
  }

  bool tracker_hit::has_top_cathodic_time() const
  {
    bool valid = false;
    if (top_cathodic_timestamp != tracker_constants::INVALID_TIME
	&& datatools::is_valid(top_cathodic_time_ns)) valid = true;
    return valid;
  }

  void tracker_hit::set_top_cathodic_timestamp(const uint64_t value_)
  {
    top_cathodic_timestamp = value_;
    top_cathodic_time_ns = top_cathodic_timestamp * tracker_constants::tracker_clock_tick() / CLHEP::nanosecond;
    return;
  }

  void tracker_hit::reset_top_cathodic_time()
  {
    top_cathodic_timestamp = tracker_constants::INVALID_TIME;
    datatools::invalidate(top_cathodic_time_ns);
    return;
  }

  void tracker_hit::set_anodic_timestamps(const uint64_t t0_,
					  const uint64_t t1_,
					  const uint64_t t2_,
					  const uint64_t t3_,
					  const uint64_t t4_)
  {
    set_anodic_timestamp_t0(t0_);
    set_anodic_timestamp_t1(t1_);
    set_anodic_timestamp_t2(t2_);
    set_anodic_timestamp_t3(t3_);
    set_anodic_timestamp_t4(t4_);
    return;
  }

  void tracker_hit::set_cathodic_timestamps(const uint64_t bot_time_,
					    const uint64_t top_time_)
  {
    set_bot_cathodic_timestamp(bot_time_);
    set_top_cathodic_timestamp(top_time_);
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
    this->base_hit::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << io::tag()
         << "Anodic timestamp 0        : " << anodic_timestamp_t0 << std::endl;

    out_ << indent_ << io::tag()
         << "Anodic time 0 (ns)        : " << anodic_t0_ns << std::endl;

    out_ << indent_ << io::tag()
         << "Anodic timestamp 1        : " << anodic_timestamp_t1 << std::endl;

    out_ << indent_ << io::tag()
         << "Anodic time 1 (ns)        : " << anodic_t1_ns << std::endl;

    out_ << indent_ << io::tag()
         << "Anodic timestamp 2        : " << anodic_timestamp_t2 << std::endl;

    out_ << indent_ << io::tag()
         << "Anodic time 2 (ns)        : " << anodic_t2_ns << std::endl;

    out_ << indent_ << io::tag()
         << "Anodic timestamp 3        : " << anodic_timestamp_t3 << std::endl;

    out_ << indent_ << io::tag()
         << "Anodic time 3 (ns)        : " << anodic_t3_ns << std::endl;

    out_ << indent_ << io::tag()
         << "Anodic timestamp 4        : " << anodic_timestamp_t4 << std::endl;

    out_ << indent_ << io::tag()
         << "Anodic time 4 (ns)        : " << anodic_t4_ns << std::endl;

    out_ << indent_ << io::tag()
         << "Bot cathodic timestamp    : " << bot_cathodic_timestamp << std::endl;

    out_ << indent_ << io::tag()
         << "Bot cathodic time (ns)    : " << bot_cathodic_time_ns << std::endl;

    out_ << indent_ << io::tag()
         << "Top cathodic timestamp    : " << top_cathodic_timestamp << std::endl;

    out_ << indent_ << io::tag()
         << "Top cathodic time (ns)    : " << top_cathodic_time_ns << std::endl;

    out_ << indent_ << io::inherit_last_tag(inherit_)
         << "Validity                  : " << std::boolalpha << is_valid() << std::endl;

    return;
  }


} // namespace fecom
