// Copyright (c) 2020 by Ben Morgan <Ben.Morgan@warwick.ac.uk>
// Copyright (c) 2020 by The University of Warwick
//
// This file is part of Falaise.
//
// Falaise is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Falaise is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Falaise.  If not, see <http://www.gnu.org/licenses/>.

#ifndef FALAISE_BOUNDED_INT_H
#define FALAISE_BOUNDED_INT_H

#include <cstdint>
#include <exception>

namespace falaise {

template <int32_t lower_bound, int32_t upper_bound>
class bounded_int {
  static_assert(lower_bound != upper_bound, "bounds cannot be equal, use a named constant instead");
  static_assert(lower_bound < upper_bound, "lower_bound must be less than upper_bound");
 public:
  bounded_int() = default;
  bounded_int(int32_t x) : value_{x} {
    if (x < lower_bound || x > upper_bound) {
      throw std::out_of_range("supplied value is out of range");
    }
  }

  ~bounded_int() = default;

  bounded_int(const bounded_int&) = default;
  bounded_int(bounded_int&& rhs) = default;

  bounded_int& operator=(const bounded_int&) = default;
  bounded_int& operator=(bounded_int&&) = default;

  operator int32_t() const { return value_; }

  constexpr int32_t min() const { return lower_bound; }

  constexpr int32_t max() const { return upper_bound; }

 private:
  int32_t value_ = lower_bound;
};
}  // namespace falaise

#endif  // FALAISE_BOUNDED_INT_H
