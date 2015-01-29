// Ourselves:
#include "event.h"

namespace gt {

  const event::calorimeter_collection_type & event::get_calorimeters() const
  {
    return _calos_;
  }

  event::calorimeter_collection_type & event::grab_calorimeters()
  {
    return _calos_;
  }

  event::event()
  {
    return;
  }

  event::~event()
  {
    return;
  }

  bool operator<(const event::calorimeter_hit & lhs_, const event::calorimeter_hit & rhs_)
  {
    return lhs_.time < rhs_.time;
  }

  std::ostream & operator<<(std::ostream & out_, const event::calorimeter_hit & hit_)
  {
    out_ << "  `- Calorimeter '" << hit_.label << "'" << std::endl;
    out_ << "    |- (x,y,z)      = (" << hit_.position.x() << ","
         << hit_.position.y() << "," << hit_.position.z() << ")" << std::endl;
    out_ << "    |- energy       = " << hit_.energy << std::endl;
    out_ << "    |- sigma_energy = " << hit_.sigma_energy << std::endl;
    out_ << "    |- time         = " << hit_.time << std::endl;
    out_ << "    `- sigma_time   = " << hit_.sigma_time << std::endl;
    return out_;
  }

  std::ostream & operator<<(std::ostream & out_, const event & event_)
  {
    const event::calorimeter_collection_type & the_calos = event_.get_calorimeters();
    out_ << "Triggered calorimeters[" << the_calos.size() << "]" << std::endl;
    for (event::calorimeter_collection_type::const_iterator
           icalo = the_calos.begin(); icalo != the_calos.end(); ++icalo) {
      out_ << icalo->second;
    }
    return out_;
  }
}
