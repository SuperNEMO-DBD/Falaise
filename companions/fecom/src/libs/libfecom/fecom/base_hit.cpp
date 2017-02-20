// fecom/base_hit.cpp

// Ourselves:
#include <fecom/base_hit.hpp>

// This project:
#include <fecom/tracker_constants.hpp>

namespace fecom {

  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(base_hit, "fecom::base_hit")

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
      // , slot_index(0xFFFF)
  {
    hit_id = 0xFFFF;
    trigger_id = 0xFFFFFFFF;
    electronic_id.invalidate();
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
    return hitmode != SIG_INVALID && has_geom_id() && electronic_id.get(tracker_constants::SLOT_INDEX) < 20;
  }

  bool base_hit::has_geom_id() const
  {
    return electronic_id.is_valid();
  }

  void base_hit::reset()
  {
    hit_id = 0xFFFF;
    trigger_id = 0xFFFFFFFF;
    hitmode = SIG_INVALID;
    return;
  }

  void base_hit::tree_dump(std::ostream & out_,
                           const std::string & title_,
                           const std::string & indent_,
                           bool /* inherit_ */) const
  {
    if (!title_.empty()) {
      out_ << indent_ << title_ << std::endl;
    }

    out_ << indent_ << io::tag()
         << "Hit ID     : [" << hit_id << "]" << std::endl;

    out_ << indent_ << io::tag()
         << "Hit mode   : '" << hitmode_to_label(hitmode) << "'" << std::endl;

    out_ << indent_ << io::tag()
         << "Electronic ID   : '" << electronic_id << "'" << std::endl;

    out_ << indent_ << io::tag()
         << "Trigger ID     : " << trigger_id << std::endl;

    return;
  }

  bool base_hit::compare::operator()(const base_hit & a, const base_hit & b)
  {
    if (a.hit_id < b.hit_id) {
      return true;
    }
    else if ((a.hit_id == b.hit_id)){
      DT_THROW(std::logic_error, "Two hits have the same hit_id, hit_a = '" << a.hit_id << "' , hit_b = '" << b.hit_id << "' !");
     return true;
    }

    else {
      return false;
    }
  }


} // namespace fecom
