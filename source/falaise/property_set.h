//! \file falaise/property_set.h
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
#ifndef FALAISE_PROPERTY_SET_H
#define FALAISE_PROPERTY_SET_H

#include <ios>
#include <stdexcept>
#include <string>
#include <vector>
#include "bayeux/datatools/properties.h"
#include "boost/mpl/contains.hpp"
#include "boost/mpl/vector.hpp"

#include "falaise/path.h"
#include "falaise/quantity.h"

namespace falaise {
//! Exception thrown when requesting a key that is not in the property_set
class missing_key_error : public std::logic_error {
  using std::logic_error::logic_error;
};

//! Exception thrown when trying to put to a key already in the property_set
class existing_key_error : public std::logic_error {
  using std::logic_error::logic_error;
};

//! Exception thrown when type requested in get<T>(key) does not match that of
//! value at key
class wrong_type_error : public std::logic_error {
  using std::logic_error::logic_error;
};

//! Exception thrown when extracted value does not requirements of a predicate
//! function
class unmet_predicate_error : public std::logic_error {
  using std::logic_error::logic_error;
};

//! Class holding a set of key-value properties
/*!
 * Provides a convenient adaptor interface over datatools::properties,
 * targeted at developers of modules for Falaise needing key-value storage
 * and validation of input parameters. For example, say we program a module
 * that requires two parameters, or properties, to implement its processing:
 *
 * * ```cpp
 * class MyModule
 * {
 *   public:
 *     MyModule(falaise::property_set const& config, datatools::service_manager& services){
 *       firstParam = config.get<int>("firstParam");
 *       secondParam = config.get<double>("secondParam");
 *     }
 *
 *     falaise::processing::status process(datatools::things&) {
 *       std::cout << firstParam * secondParam << std::endl;
 *       ...
 *     }
 *
 *   private:
 *     int firstParam = 1;
 *     double secondParam = 3.14;
 * };
 *
 * Users of this module can change the `firstParam` and `secondParam` properties when
 * using it in `flreconstruct` by configuring it in their pipeline script:
 *
 * ```
 * [name="MyModule", type="MyModule"]
 * firstParam : int = 2
 * secondParam : real = 4.13
 * ```
 *
 * The module would then print "8.26" when the `process` member function is called. In particular,
 * it allows module authors to validate this user supplied input as needed to ensure correct
 * running of the module. As detailed below, this may include required vs optional values, type
 * checking, and value checking.
 *
 *
 * A property_set may be created and used directly as
 *
 * ```cpp
 * falaise::property_set ps{};
 *
 * // Store an integer at key "a"
 * ps.put("a",2);
 *
 * // Get the integer
 * auto x = ps.get<int>("a");
 *
 * // Replace the integer with a double
 * ps.put_or_replace("a",3.14);
 *
 * // Get a string, setting a default value in case key "b" doesn't exist
 * std::string s = ps.get<std::string>("b", "hello");
 * std::cout << s << std::endl; // prints "hello"
 *
 * // Assign value a variable only if key exists
 * std::string myDefault = "foo";
 *
 * // o.k....
 * myDefault = ps.get<std::string>("nonexistent", myDefault);
 *
 * // better...
 * ps.assign_if("nonexistent", myDefault);
 * std::cout << myDefault << std::endl; // prints "foo"
 *
 * ps.put("myDef", "bar");
 * ps.assign_if("myDef", myDefault);
 * std::cout << myDefault << std::endl; // prints "bar"
 * ```
 *
 * or it can be constructed from an existing instance of @ref datatools::properties
 *
 * ```cpp
 * void example(datatools::properties const& x) {
 *   falaise::property_set ps{x};
 *
 *   // ... use ps ...
 * }
 * ```
 *
 * or from a @ref datatools::properties file
 *
 * ```cpp
 * void example(std::string const& file) {
 *   falaise::property_set ps{};
 *   falaise::make_property_set(file, ps);
 *
 *   // ... use ps ...
 * }
 * ```
 *
 * or from any input stream, e.g
 *
 * ```cpp
 * void example(std::istream& is) {
 *   falaise::property_set ps{};
 *   falaise::make_property_set(is, ps);
 *   // ... use ps ...
 * }
 * ```
 *
 * Storage and retrieval of parameters is typesafe in the sense that:
 *
 * - You can only @ref put values that are of type
 *   - int
 *   - double
 *   - bool
 *   - std::string
 *   - @ref falaise::path
 *   - @ref falaise::quantity_t
 *     - Plus any type listed in @ref falaise_units
 *   - @ref falaise::property_set
 *   - std::vector<int>
 *   - std::vector<double>
 *   - std::vector<bool>
 *   - std::vector<std::string>
 * - You can only @ref get values of the above types, and a @ref wrong_type_error
 *   will be thrown if you try to retrieve a value with a different type, e.g
 *   ``` cpp
 *   ps.put("a", 2); // an integer by type inference (2. or 2.0 would be double)
 *   auto x = ps.get<double>("a"); // throws wrong_type_error
 *
 *   // Or you can check manually
 *   if (ps.is_key_to<int>("a")) {
 *     std::cout << "is int\n"; // prints "is int"
 *   }
 *   ```
 *
 * Support for @ref path and @ref quantity_t is provided to support validation
 * of input configuration scripts. These types model the `path` and `dimension`
 * decorators for properties parameters:
 *
 * ```conf
 * # this is a true filesystem path
 * foo : string as path = "${HOME}/something"
 *
 * # this is a true physical quantity with dimension "length"
 * bar : real as length = 3.14 m
 * ```
 *
 * When extracting parameters, you can ensure that that you have a true path
 * or physical quantity of the right dimension:
 *
 * ```cpp
 * void configure_me(property_set const& ps) {
 *   // throws wrong_type_error if value at "foo" is not a path
 *   auto foo = ps.get<falaise::path>("foo");
 *
 *   // throws wrong_type_error if value at bar is not a physical quantity with 'length' dimension
 *   auto bar = ps.get<falaise::length_t>("bar");
 *
 *   ...
 * }
 * ```
 *
 * A full list of available quantity types is provided in the documentation for @ref quantity_t
 *
 * Support for @ref put and @ref get of @ref property_set values is provided to
 * support @ref datatools::properties constructs of the form:
 *
 * ```conf
 * plain : string = "A plain property"
 * table.foo : string = "A string property in a 'table'"
 * table.bar : int = 42
 * ```
 *
 * The `foo` and `bar` properties an be extracted either by their full keys:
 *
 * ```cpp
 * auto foo = ps.get<std::string>("table.foo");
 * auto bar = ps.get<int>("table.bar);
 * ```
 *
 * or by getting the `table` key as a @ref property_set, then its properties
 * which will be stored using their subkey names:
 *
 * ```cpp
 * auto table = ps.get<falaise::property_set>("table");
 * auto foo = table.get<std::string>("foo");
 * auto bar = table.get<int>("bar);
 * ```
 *
 * This can be extended to further levels of nesting if required. However,
 * note that @ref datatools::properties configuration of the form:
 *
 * ```conf
 * table : string = "Not a valid table"
 * table.x : int = 11
 * table.y : int = 22
 * ```
 *
 * are not considered get-able as `property_set`s. This is due to the ambiguity
 * of the `table` key which @ref property_set regards as an atomic property
 * of type `std::string`. Trying to extract the above `table` key as a `property_set`
 * would result in a `wrong_type_error` exception:
 *
 *
 * ```cpp
 * // throws falaise::wrong_type_error
 * auto table = ps.get<falaise::property_set>("table");
 * ```
 *
 * The `table.x` and table.y` keys in the example can only be extracted
 * via these fully qualified keys. It is therefore strongly recommended that
 * you structure your configuration in the "pure table" form above for clarity
 * and ease of use.
 *
 * The default value form of @ref get or the @ref assign_if member function can be used to
 * implement optional configuration, for example
 *
 * ```cpp
 * void configure_me(property_set const& ps) {
 *   // throws missing_key_error if ps doesn't have "foo"
 *   auto requiredParam = ps.get<int>("foo");
 *
 *   // sets optionalParam to the value held at "answer", or 42 if ps doesn't have an "answer" key
 *   auto optionalParam = ps.get<int>("answer",42);
 *
 *   // In many use cases, we may already have a parameter set to its default value
 *   // e.g. class member initialization. We could write...
 *   int defaultParameter = 42;
 *   defaultParameter = ps.get<int>("answer", defaultParameter);
 *
 *   // ... but assign_if allows the clearer form
 *   ps.assign_if("answer", defaultParameter);
 *
 *   ...
 * }
 * ```
 *
 * Limited validation of extracted values is available through the @ref get_if and @ref assign_if
 * interfaces. These forms allow a value meeting the [UnaryPredicate](https://en.cppreference.com/w/cpp/named_req/Predicate)
 * concept to be supplied that can validate the value as needed. Falaise does not supply
 * any checks "out the box", but they are easy to write as lambdas, free functions, or
 * function objects. For example:
 *
 * ```cpp
 * // Use a lambda as simple example, but any callable taking value and returning bool works
 * auto int_predicate = [](const int x) {
 *   // Return true if the value is between 0 and 8
 *   return (x >= 0) && (x <= 8);
 * }
 *
 * falaise::property_set ps;
 * ps.put("good", 5);
 * ps.put("bad", -4);
 *
 * // Required key forms
 * auto good = ps.get_if<int>("good", int_predicate); // fine, good = 5
 * auto bad = ps.get_if<int>("bad", int_predicate); // throws falaise::unmet_predicate_error exception
 *
 * // Optional key forms
 * auto goodDefault = ps.get_if<int>("nokey", 2, int_predicate); // fine
 * auto badDefault = ps.get_if<int>("good", 293, int_predicate); // throws, default value must meet constraint!
 * auto badValue = ps.get_if<int>("bad", 6, int_predicate); // throws, default is good, value is bad!
 *
 * // Assign_if forms
 * int myParam = 2;
 * ps.assign_if("nokey", myParam, int_predicate); // fine, myParam is 2
 * ps.assign_if("good", myParam, int_predicate); //fine, myParam is now 5
 * ps.assign_if("bad", myParam, int_predicate); //throws, myParam is still 5
 * int myParam = 84632;
 * ps.assign_if("anykey", myParam, int_predicate); //throws, myParam must meet constraint!
 * ```
 *
 * Combined, these various ways of using property_set allow simple and reliable usage
 * for configuration and configuration validation in Falaise modules.
 *
 * \sa path
 * \sa quantity
 * \sa datatools::properties
 * \sa datatools::units
 */
class property_set {
 public:
  //! Default constructor
  property_set() = default;

  //! Construct from an existing datatools::properties
  /*!
   *  \param[in] ps properties
   */
  property_set(datatools::properties const& ps);

  // - Observers
  //! Returns true if no key-value pairs are held
  bool is_empty() const;

  //! Returns a vector of all keys in the property_set
  std::vector<std::string> get_names() const;

  //! Returns true if the property_set stores a pair with the supplied key
  /*!
   * A nested key, e.g. `foo.bar`, may be supplied.
   *
   * \param[in] key name of key to check for existence
   */
  bool has_key(std::string const& key) const;

  //! Returns true if the supplied key holds a value of type T
  /*!
   * \tparam T type of value to be returned
   * \param[in] key key for value to find
   * \returns true if key exists and holds value of type T
   */
  template <typename T>
  bool is_key_to(std::string const& key) const;

  //! Returns true if the key's value is a property/atom
  /*!
   * Examples of property/atom definitions are:
   *
   * ```
   * x : int = 1
   * y : string = "a b c"
   * ```
   *
   * A nested key, e.g. `foo.bar`, may be supplied.
   *
   * \param[in] key name of the key to check
   */
  bool is_key_to_property(std::string const& key) const;

  //! Returns true if the keys's value is a sequence (array) of values
  /*!
   * Examples of sequences are:
   *
   * ```
   * x : int[4] = 1, 2, 3, 4
   * y : real[2] = 3.14, 4.13
   * ```
   *
   * A nested key, e.g. `foo.bar`, may be supplied.
   *
   * \param[in] key name of the key to check
   */
  bool is_key_to_sequence(std::string const& key) const;

  //! Returns true if the key's value is a @ref property_set
  /*!
   * A nested key, e.g. `foo.bar`, may be supplied.
   *
   * A key is only considered to have a @ref property_set value if
   * it only exists as a prefix to a set of nested keys, e.g.
   *
   * ```
   * pure.foo : int = 1
   * pure.bar : int = 2
   * pure.nested.x : int = 11
   * pure.nested.y : int = 22
   * ```
   *
   * Keys `pure` and `pure.nested` are considered as @ref property_set values.
   * In mixed properties/values such as
   *
   * ```
   * bad : int = 1
   * bad.foo : int = 2
   * ```
   *
   * `bad` is not considered as a @ref property_set.
   *
   * \param[in] key name of the key to check
   */
  bool is_key_to_property_set(std::string const& key) const;

  //! Returns a string representation of the property_set
  std::string to_string() const;

  // - Retrievers
  //! Return the value of type T held at supplied key
  /*!
   * \tparam T type of value to be returned
   * \param[in] key key for value to find
   * \returns value held at key
   * \throw missing_key_error if key is not found
   * \throw wrong_type_error if value at key is not of type T
   */
  template <typename T>
  T get(std::string const& key) const;

  //! Return the value of type T held at supplied key if it meets a constraint
  /*!
   * \tparam T type of value to be returned
   * \tparam UnaryPredicate callable taking a value of T and returning bool
   * \param[in] key key for value to find
   * \param[in] p predicate to validate value with
   * \returns value held at key
   * \throw missing_key_error if key is not found
   * \throw wrong_type_error if value at key is not of type T
   * \throw unmet_predicate_error id p(value) returns false
   */
  template <typename T, typename UnaryPredicate>
  T get_if(std::string const& key, UnaryPredicate p) const;


  //! Return the value of type T associated with key, or a default if the key is
  //!  not present
  /*!
   * \tparam T type of value to be returned
   * \param[in] key key for value to find
   * \param[in] default_value value to return if key is not found
   * \throw wrong_type_error if key is found and associated value is not type T
   */
  template <typename T>
  T get(std::string const& key, T const& default_value) const;

  //! Return the value of type T held at supplied key, or a default value, if both meet a constraint
  /*!
   * \tparam T type of value to be returned
   * \tparam UnaryPredicate callable taking a value of T and returning bool
   * \param[in] key key for value to find
   * \param[in] default_value value to return if key is not found
   * \param[in] p predicate to validate values with
   * \returns value held at key
   * \throw missing_key_error if key is not found
   * \throw wrong_type_error if value at key is not of type T
   * \throw unmet_predicate_error if p returns false for default or extracted value
   */
  template <typename T, typename UnaryPredicate>
  T get_if(std::string const& key, T const& default_value, UnaryPredicate p) const;

  //! Assign the value held at a key to an input parameter of type T if the key exists
  //! and holds T
  /*!
   * \tparam T type of value to be assigned
   * \param[in] key key for value to find
   * \param[in,out] parameter reference to variable to assign value
   * \throw wrong_type_error if key is found and associated value is not type T
   * \post parameter has value stored at key if key exists and of matching type, unmodified
   *       otherwise
   */
  template <typename T>
  void assign_if(std::string const& key, T& parameter) const;

  //! Assign the value held at a key to an input parameter of type T if the key exists
  //! , holds T, and meets a constraint
  /*!
   * \tparam T type of value to be assigned
   * \tparam UnaryPredicate callable taking a value of T and returning bool
   * \param[in] key key for value to find
   * \param[in,out] parameter reference to variable to assign value
   * \param[in] p predicate to validate values with
   * \throw wrong_type_error if key is found and associated value is not type T
   * \post parameter has value stored at key if key exists, of matching type, and meets contraint, unmodified
   *       otherwise
   */
   template <typename T, typename UnaryPredicate>
  void assign_if(std::string const& key, T& parameter, UnaryPredicate p) const;

  //! Convert back to datatools::properties
  operator datatools::properties() const;

  // - Inserters
  //! Insert key-value pair in property_set
  /*!
   * Copies of the key and value are stored
   * \tparam T type of value to store
   * \param[in] key key to store value at
   * \param[in] value value to store
   * \throws wrong_type_error if value's type is not storable
   * \throws existing_key_error if key is already stored
   */
  template <typename T>
  void put(std::string const& key, T const& value);

  //! Insert key-value pair in property_set, replacing value if key exists
  /*!
   * Copies of the key and value are stored. For existing keys the value
   * will be overwritten, and the types of the old and new value do not need
   * to match.
   * \tparam T type of value to store
   * \param[in] key key to store value at
   * \param[in] value value to store
   * \throws wrong_type_error if value's type is not storable
   */
  template <typename T>
  void put_or_replace(std::string const& key, T const& value);

  // - Deleters:
  //! Erase the name-value pair stored at key
  /*!
   * \param[in] key key for pair to erase
   * \returns true if the exists and was erased, false if the key does not exist
   */
  bool erase(std::string const& key);

 private:
  //! List of types that property_set can hold
  using types_ = boost::mpl::vector<int, double, bool, std::string, falaise::path,
                                    falaise::quantity, std::vector<int>, std::vector<double>,
                                    std::vector<bool>, std::vector<std::string>>;
  template <typename T>
  struct can_hold_ {
    typedef typename boost::mpl::contains<types_, T>::type type;
  };

  template <typename T>
  struct can_hold_<falaise::quantity_t<T>> {
    typedef std::true_type type;
  };

  //! Compile-time check that T can be held
  /*
   * can_hold_t_<T>::value : true if T can be held, false otherwise
   */
  template <typename T>
  using can_hold_t_ = typename can_hold_<T>::type;

  //! Return true if value held at key has type T
  /*
   * Assert that T be a holdable type before dispatching to the
   * implementation function checking the specific type
   */
  template <typename T>
  bool is_type_(std::string const& key) const;

  //! Return true if value at key is an int
  bool is_type_impl_(std::string const& key, int) const;

  //! Return true if value at key is a dimensionless double
  bool is_type_impl_(std::string const& key, double) const;

  //! Return true if value at key is a bool
  bool is_type_impl_(std::string const& key, bool) const;

  //! Return true if value at key is a non-path std::string
  bool is_type_impl_(std::string const& key, const std::string&) const;

  //! Return true if value at key is a falaise::path
  bool is_type_impl_(std::string const& key, const falaise::path&) const;

  //! Return true if value at key is a falaise::quantity
  bool is_type_impl_(std::string const& key, const falaise::quantity&) const;

  //! Return true if value at key is a std::vector<int>
  bool is_type_impl_(std::string const& key, const std::vector<int>&) const;

  //! Return true if value at key is a std::vector<double> (dimensionless
  //! doubles)
  bool is_type_impl_(std::string const& key, const std::vector<double>&) const;

  //! Return true if value at key is a std::vector<bool>
  bool is_type_impl_(std::string const& key, const std::vector<bool>&) const;

  //! Return true if value at key is a std::vector<std::string>
  bool is_type_impl_(std::string const& key, const std::vector<std::string>&) const;

  //! Insert key-value pair
  /*!
   * Dispatches to specializations or overloads for T
   */
  template <typename T>
  void put_impl_(std::string const& key, T const& value);

  //! Set result to value held at key
  /*!
   * Dispatches to specializations or overloads for T
   */
  template <typename T>
  void get_impl_(std::string const& key, T& result) const;

  //! Overload of put_impl_ for @ref falaise::quantity_t
  template <typename T>
  void put_impl_(std::string const& key, falaise::quantity_t<T> const& value);

  //! Overload of get_impl_ for @ref falaise::quantity_t
  template <typename T>
  void get_impl_(std::string const& key, falaise::quantity_t<T>& result) const;

  datatools::properties ps_;  //< underlying set of properties
};
}  // namespace falaise

// - Definitions for template functions
namespace falaise {
template <typename T>
bool property_set::is_key_to(std::string const& key) const {
  return is_type_<T>(key);
}

template <typename T>
T property_set::get(std::string const& key) const {
  if (!ps_.has_key(key)) {
    throw missing_key_error("property_set does not hold a key '" + key + "'");
  }
  if (!is_type_<T>(key)) {
    throw wrong_type_error("value at '" + key + "' is not of requested type");
  }

  T result;
  get_impl_(key, result);
  return result;
}

// Specialization of get for @ref property_set
template <>
inline property_set property_set::get(std::string const& key) const {
  if (!is_key_to_property_set(key)) {
    throw wrong_type_error("key '" + key + "' is not a pure property_set");
  }
  datatools::properties tmp;
  ps_.export_and_rename_starting_with(tmp, key + ".", "");
  return property_set{tmp};
}

template <typename T, typename UnaryPredicate>
T property_set::get_if(std::string const& key, UnaryPredicate p) const {
  T result = get<T>(key);
  if (p(result)) {
    return result;
  }
  throw unmet_predicate_error("value at '" + key + "' failed to meet supplied constraint");
}

template <typename T>
T property_set::get(std::string const& key, T const& default_value) const {
  T result{default_value};
  if (ps_.has_key(key)) {
    if (!is_type_<T>(key)) {
      throw wrong_type_error("value at '" + key + "' is not of requested type");
    }
    get_impl_(key, result);
  }
  return result;
}

template <typename T, typename UnaryPredicate>
T property_set::get_if(std::string const& key, T const& default_value, UnaryPredicate p) const {
  // Precondition, because parameter itself may be invalid
  if (!p(default_value)) {
    throw unmet_predicate_error("supplied default_value failed to meet supplied constraint");
  }
  T result = get<T>(key, default_value);
  if (p(result)) {
    return result;
  }
  throw unmet_predicate_error("value at '" + key + "' failed to meet supplied constraint");
}

// Specialization of get for @ref property_set
template <>
inline property_set property_set::get(std::string const& key,
                                      property_set const& default_value) const {
  if (!is_key_to_property_set(key)) {
    return default_value;
  }
  return get<property_set>(key);
}

template <typename T>
void property_set::assign_if(std::string const& key, T& parameter) const {
  if (ps_.has_key(key)) {
    parameter = get<T>(key);
  }
}

template <typename T, typename UnaryPredicate>
void property_set::assign_if(std::string const& key, T& parameter, UnaryPredicate p) const {
  // Precondition, because parameter itself may be invalid
  if (!p(parameter)) {
    throw unmet_predicate_error("supplied input parameter to assign_if failed to meet supplied constraint");
  }

  // We don't can't non-predicate assign_if because that immediately modifies the parameter
  if (ps_.has_key(key)) {
    auto tmp = get<T>(key);
    // Postcondition
    if (p(tmp)) {
      parameter = tmp;
      return;
    }
    throw unmet_predicate_error("value at '" + key + "' failed to meet supplied constraint");
  }
}

template <typename T>
void property_set::put(std::string const& key, T const& value) {
  static_assert(can_hold_t_<T>::value, "property_set cannot hold values of type T");
  // Check directly to use our clearer exception type
  if (ps_.has_key(key)) {
    throw existing_key_error{"property_set already contains key " + key};
  }
  put_impl_(key, value);
}

// Specialization of put for @ref property_set
template <>
inline void property_set::put(std::string const& key, property_set const& value) {
  // Check both key/table existence
  if (ps_.has_key(key) || is_key_to_property_set(key)) {
    throw existing_key_error{"property_set already contains key " + key};
  }
  // Have to resort to the low level interface to put data in
  for (auto& subkey : (value.ps_).keys()) {
    ps_.store(key + "." + subkey, (value.ps_).get(subkey));
  }
}

template <typename T>
void property_set::put_or_replace(std::string const& key, T const& value) {
  // Cannot change type of existing data, so must erase/re-store
  erase(key);
  put(key, value);
}

// Specialization of put_or_replace for @ref property_set
template <>
inline void property_set::put_or_replace(std::string const& key, property_set const& value) {
  // Cannot change type of existing data, so must erase/re-store all keys/subkeys
  erase(key);
  ps_.erase_all_starting_with(key + ".");
  put(key, value);
}

template <typename T>
bool property_set::is_type_(std::string const& key) const {
  static_assert(can_hold_t_<T>::value, "property_set cannot hold values of type T");
  if (ps_.has_key(key)) {
    return is_type_impl_(key, T{});
  }
  // Absence of key is false (clearly cannot be T)
  return false;
}

// Generic put_impl_
template <typename T>
void property_set::put_impl_(std::string const& key, T const& value) {
  ps_.store(key, value);
}

//! Private specialization of put_impl_ for @ref path
template <>
inline void property_set::put_impl_(std::string const& key, falaise::path const& value) {
  ps_.store_path(key, value);
}

//! Private specialization of put_impl_ for @ref quantity
template <>
inline void property_set::put_impl_(std::string const& key, falaise::quantity const& value) {
  ps_.store_with_explicit_unit(key, value());
  ps_.set_unit_symbol(key, value.unit());
}

//! Private overload of put_impl_ for @ref quantity_t
template <typename T>
void property_set::put_impl_(std::string const& key, falaise::quantity_t<T> const& value) {
  ps_.store_with_explicit_unit(key, value());
  ps_.set_unit_symbol(key, value.unit());
}

// Generic get_impl_
template <typename T>
void property_set::get_impl_(std::string const& key, T& result) const {
  ps_.fetch(key, result);
}

// Private specialization of get_impl_ for @ref path type
template <>
inline void property_set::get_impl_(std::string const& key, falaise::path& result) const {
  result = falaise::path{ps_.fetch_path(key)};
}

// Private specialization of get_impl_ for @ref units::quantity type
template <>
inline void property_set::get_impl_(std::string const& key, falaise::quantity& result) const {
  // Fetch with explicit unit gives value in CLHEP scale, so must
  // divide by the scaling factor for the symbol
  double rescaledValue =
      ps_.fetch_real_with_explicit_unit(key) / datatools::units::get_unit(ps_.get_unit_symbol(key));
  result = {rescaledValue, ps_.get_unit_symbol(key)};
}

// Overload for explicitly dimensioned quantities
template <typename T>
void property_set::get_impl_(std::string const& key, falaise::quantity_t<T>& result) const {
  // fetch with explicit unit gives value in CLHEP scale, so must
  // divide it by the scaling factor for the symbol
  double rescaledValue =
      ps_.fetch_real_with_explicit_unit(key) / datatools::units::get_unit(ps_.get_unit_symbol(key));
  result = {rescaledValue, ps_.get_unit_symbol(key)};
}

//! Construct a property_set from an input datatools::properties file
/*!
 * \param filename File from which to read data
 * \param ps property_set to fill with data
 */
void make_property_set(const std::string& filename, property_set& ps);

//! Construct a property_set from a std::istream
/*!
 * \param is stream from which to read data
 * \param ps  property_set to fill with data
 */
void make_property_set(std::istream& is, property_set& ps);

}  // namespace falaise

#endif  // FALAISE_PROPERTY_SET_H
