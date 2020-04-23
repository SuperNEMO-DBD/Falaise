// sensitive_hit.cc

// Standard library:
#include <stdexcept>
#include <sstream>
#include <iostream>

// Ourselves:
#include <mctools/g4/sensitive_hit.h>

namespace mctools {

  namespace g4 {

    const base_step_hit & sensitive_hit::get_hit_data() const
    {
      return _hit_data_;
    }

    base_step_hit & sensitive_hit::grab_hit_data()
    {
      return _hit_data_;
    }

    void sensitive_hit::reset()
    {
      _hit_data_.reset();
      return;
    }

  } // end of namespace g4

} // end of namespace mctools
