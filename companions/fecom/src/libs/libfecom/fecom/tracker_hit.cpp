// fecom/tracker_hit.cpp

// Ourselves:
#include <fecom/tracker_hit.hpp>

// Third party:
#include <bayeux/datatools/exception.h>

namespace fecom {

  tracker_hit::tracker_hit()
  {
    _reset_();
  }

  tracker_hit::~tracker_hit()
  {
  }

  bool tracker_hit::is_valid() const
  {
    if (cell_id >= tracker_constants::NUMBER_OF_CELLS_PER_BOARD) return false;
    return true;
  }

  void tracker_hit::reset()
  {
    _reset_();
    return;
  }

  void tracker_hit::_reset_()
  {
    cell_id = tracker_constants::NUMBER_OF_CELLS_PER_BOARD;
    reset_anodic_t0();
    reset_anodic_t1();
    reset_anodic_t2();
    reset_anodic_t3();
    reset_anodic_t4();
    reset_cathodic_t5();
    reset_cathodic_t6();
    return;
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

  bool tracker_hit::has_cathodic_t5() const
  {
    bool valid = false;
    if (cathodic_t5 != tracker_constants::INVALID_TIME) valid = true;
    return valid;
  }

  uint64_t tracker_hit::get_cathodic_t5() const
  {
    return cathodic_t5;
  }

  void tracker_hit::set_cathodic_t5(const uint64_t value_)
  {
    cathodic_t5 = value_;
    return;
  }

  void tracker_hit::reset_cathodic_t5()
  {
    cathodic_t5 = tracker_constants::INVALID_TIME;
    return;
  }

  bool tracker_hit::has_cathodic_t6() const
  {
    bool valid = false;
    if (cathodic_t6 != tracker_constants::INVALID_TIME) valid = true;
    return valid;
  }

  uint64_t tracker_hit::get_cathodic_t6() const
  {
    return cathodic_t6;
  }

  void tracker_hit::set_cathodic_t6(const uint64_t value_)
  {
    cathodic_t6 = value_;
    return;
  }

  void tracker_hit::reset_cathodic_t6()
  {
    cathodic_t6 = tracker_constants::INVALID_TIME;
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

  void tracker_hit::set_cathodic_times(const uint64_t t5_,
				       const uint64_t t6_)
  {
    set_cathodic_t5(t5_);
    set_cathodic_t6(t6_);
    return;
  }

  void tracker_hit::reset_times()
  {
    reset_anodic_t0();
    reset_anodic_t1();
    reset_anodic_t2();
    reset_anodic_t3();
    reset_anodic_t4();
    reset_cathodic_t5();
    reset_cathodic_t6();
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
         << "Cell ID                   : " << (int) cell_id << std::endl;

    out_ << indent_ << io::tag()
         << "Anodic channel ID         : " << (int) anodic_channel << std::endl;

    out_ << indent_ << io::tag()
         << "Bottom cathode channel ID : " << (int) bottom_cathode_channel << std::endl;

    out_ << indent_ << io::tag()
         << "Top cathode channel ID    : " << (int) top_cathode_channel << std::endl;

    out_ << indent_ << io::tag()
         << "Anodic time 0             : " << (int) anodic_t0 << std::endl;

    out_ << indent_ << io::tag()
         << "Anodic time 1             : " << (int) anodic_t1 << std::endl;

    out_ << indent_ << io::tag()
         << "Anodic time 2             : " << (int) anodic_t2 << std::endl;

    out_ << indent_ << io::tag()
         << "Anodic time 3             : " << (int) anodic_t3 << std::endl;

    out_ << indent_ << io::tag()
         << "Anodic time 4             : " << (int) anodic_t4 << std::endl;

    out_ << indent_ << io::tag()
         << "Cathodic time 5           : " << (int) cathodic_t5 << std::endl;

    out_ << indent_ << io::tag()
         << "Cathodic time 6           : " << (int) cathodic_t6 << std::endl;

    out_ << indent_ << io::inherit_last_tag(inherit_)
         << "Validity                  : " << std::boolalpha << is_valid() << std::endl;

    return;
  }


} // namespace fecom
