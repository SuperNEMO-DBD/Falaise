//! \file falaise/bounded_int.h
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
//! \brief Class representing an integral value with strict bounds
/*!
 * \tparam lower_bound Inclusive lower bound on value
 * \tparam upper_bound Inclusive upper bound on value
 *
 * Provides a convenience class for integral values with strict bounds that
 * cannot overflow. The canonical use case in Falaise is indices for tracker
 * layers and columns, [0,8] and [0,111] respectively. These bounds do not line
 * up with the range of any builtin integral type, and builtin types are
 * vunerable to overflow/underflow. Instead of having to write code that checks
 * bounds explicitly everytime we use an builtin integral :
 *
 * ```cpp
 * const int32_t NLAYER = 8;
 *
 * void example(int32_t layerIndex) {
 *   if (layer < 0 || layer > NLAYER) {
 *     throw std::out_of_range;
 *   }
 *   ... do something ...
 * }
 * ```
 *
 * bounded_int allows us to do
 *
 * ```cpp
 * using layer_index_t = bounded_int<0,8>;
 *
 * void example(layer_index_t i) {
 *   ... do something ...
 * }
 *
 * layer_index_t good{1}; // fine
 * layer_index_t bad{9};  // throws std::out_of_range
 *
 * int32_t works{2};
 * int32_t fails{-1};
 *
 * // Implicit conversion fron int32_t to layer_index_t
 * example(works); // fine
 * example(fails); // throws std::out_of_range
 * ```
 *
 * bounded_int only holds an int32_t value, so its maximum bounds are equivalent
 * to int32_t's numeric limits. Compile time assertions will be raised in the cases:
 *
 * - lower_bound is greater than upper_bound
 * - lower_bound is equal to upper_bound
 *
 * The first is enforced for consistency, the second is equivalent to a more useful
 * named constant.
 *
 * bounded_int can be converted back to int32_t and any integral type that's implicitly
 * convertible form int32_t:
 *
 * ```cpp
 * bounded_int<0,8> x{2};
 * int32_t y = x; // y = 2
 * ```
 *
 * However, there is no checking for over/uinderflow in these cases.
 */
template <int32_t lower_bound, int32_t upper_bound>
class bounded_int {
  static_assert(lower_bound != upper_bound, "bounds cannot be equal, use a named constant instead");
  static_assert(lower_bound < upper_bound, "lower_bound must be less than upper_bound");
 public:
  //! Default constructor
  /*!
   * Initializes value to lower_bound
   *
   * \tparam lower_bound Inclusive lower bound on value
   * \tparam upper_bound Inclusive upper bound on value
   */
  bounded_int() = default;

  //! Construct from a int32_t value
  /*!
   * \tparam lower_bound Inclusive lower bound on value
   * \tparam upper_bound Inclusive upper bound on value
   * \param[in] x value
   * \throw out_of_range if value is outside [lower_bound,upper_bound]
   */
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

  //! Convert back to int32_t
  operator int32_t() const { return value_; }

  //! Return lower bound
  constexpr int32_t min() const { return lower_bound; }

  //! Return upper bound
  constexpr int32_t max() const { return upper_bound; }

 private:
  int32_t value_ = lower_bound; //< underlying value
};
}  // namespace falaise

#endif  // FALAISE_BOUNDED_INT_H
