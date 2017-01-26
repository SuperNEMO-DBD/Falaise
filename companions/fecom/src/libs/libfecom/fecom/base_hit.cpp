// fecom/base_hit.cpp

// Ourselves:
#include <fecom/base_hit.hpp>

namespace fecom {

  // static
  std::string base_hit::hitmode_to_label(const hitmode_type hm_)
  {
    if (hm_ == SIG_CALORIMETER) {
      return "calorimeter";
    } else if (hm_ == SIG_TRACKER) {
      return "tracker";
    }
    return "";
  }

  base_hit::base_hit()
    : hitmode(SIG_INVALID)
    , slot_index(0xFFFF)
  {
    hit_id = 0xFFFF;
    trigger_id = 0xFFFFFFFF;
  }

  base_hit::base_hit(const hitmode_type hm_)
    : hitmode(hm_)
  {
  }

  base_hit::~base_hit()
  {
  }

  bool base_hit::is_valid() const
  {
    return hitmode != SIG_INVALID && slot_index < 16;
  }

  void base_hit::reset()
  {
    hit_id = 0xFFFF;
    trigger_id = 0xFFFFFFFF;
    hitmode = SIG_INVALID;
    slot_index = 0xFFFF;
    return;
  }

  void base_hit::tree_dump(std::ostream & out_,
                           const std::string & title_,
                           const std::string & indent_,
                           bool inherit_) const
  {
    if (!title_.empty()) {
      out_ << indent_ << title_ << std::endl;
    }

    out_ << indent_ << io::tag()
         << "Hit ID     : [" << hit_id << "]" << std::endl;

    out_ << indent_ << io::tag()
         << "Hit mode   : '" << hitmode_to_label(hitmode) << "'" << std::endl;

    out_ << indent_ << io::tag()
         << "Trigger ID     : " << trigger_id << std::endl;

    out_ << indent_ << io::inherit_last_tag(inherit_)
         << "Slot index : " << slot_index << std::endl;

    return;
  }

} // namespace fecom
