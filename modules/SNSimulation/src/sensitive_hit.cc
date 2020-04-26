// sensitive_hit.cc

// Standard library:
#include <iostream>
#include <sstream>
#include <stdexcept>

// Ourselves:
#include <mctools/g4/sensitive_hit.h>

namespace snsim {

auto sensitive_hit::get_hit_data() const -> const mctools::base_step_hit& { return _hit_data_; }

auto sensitive_hit::grab_hit_data() -> mctools::base_step_hit& { return _hit_data_; }

void sensitive_hit::reset() { _hit_data_.reset(); }

}  // namespace snsim
