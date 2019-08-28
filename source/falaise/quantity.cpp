#include "quantity.h"

#include <utility>

namespace falaise {
quantity::quantity(double value, std::string  unit) : value_(value), unit_name(std::move(unit)) {
  if (!datatools::units::find_unit(unit_name, unit_scale, dimension_name)) {
    throw unknown_unit_error{"unit '" + unit_name + "' is unknown"};
  }
}

double quantity::value_in(std::string const& unit) const {
  double rescale{1.0};
  std::string dim{};
  datatools::units::find_unit(unit, rescale, dim);
  if (dim != dimension_name) {
    throw wrong_dimension_error("cannot convert '" + unit_name + "' to '" + unit + "'");
  }

  return value_ * unit_scale / rescale;
}
}  // namespace falaise