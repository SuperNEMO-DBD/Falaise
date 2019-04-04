#ifndef FALAISE_PROPERTY_SET_H
#define FALAISE_PROPERTY_SET_H

#include <exception>
#include <string>
#include <vector>
#include "bayeux/datatools/properties.h"
#include "boost/mpl/contains.hpp"
#include "boost/mpl/vector.hpp"

#include "falaise/config/path.h"
#include "falaise/config/quantity.h"

namespace falaise {
namespace config {
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

//! \brief Class holding a set of key-value properties
/*!
 * Provides a convenient adaptor interface over datatools::properties,
 * targeted at developers of modules for Falaise needing key-value storage
 * and validation of input parameters. It may be used directly as
 *
 * ```cpp
 * falaise::property_set ps{};
 *
 * // Store an integer at key "a"
 * ps.put("a",2);
 *
 * // Get the integer
 * int x = ps.get<int>("a");
 *
 * // Replace the integer with a double
 * ps.put("a",3.14);
 *
 * // Get a string, setting a default value in case key "b" doesn't exist
 * std::string s = ps.get<std::string>("b", "hello");
 * std::cout << s << std::endl; // prints "hello"
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
 * Storage and retrieval of parameters is typesafe in the sense that:
 *
 * - You can only @ref put values that are of type
 *   - int
 *   - double
 *   - bool
 *   - std::string
 *   - @ref falaise::config::path
 *   - @ref falaise::config::quantity_t
 *     - Plus any type listed in @ref falaise_units
 *   - std::vector<int>
 *   - std::vector<double>
 *   - std::vector<bool>
 *   - std::vector<std::string>
 * - You can only @ref get values of the above types, and a @ref wrong_type_error
 *   will be thrown if you try to retrieve a value with a different type, e.g
 *   ``` cpp
 *   ps.put("a", 2);
 *   double x = ps.get<double>("a"); // throws wrong_type_error
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
 *   auto foo = ps.get<falaise::config::path>("foo");
 *
 *   // throws wrong_type_error if value at bar is not a physical quantity with 'length' dimension
 *   auto bar = ps.get<falaise::config::length_t>("bar");
 *
 *   ...
 * }
 * ```
 *
 * The default value form of @ref get can be used to implement optional configuration, for
 * example
 *
 * ```cpp
 * void configure_me(property_set const& ps) {
 *   // throws missing_key_error if ps doesn't have "foo"
 *   auto required = ps.get<int>("foo");
 *
 *   // sets optional to the value held at "answer", or 42 if ps doesn't have an "answer" key
 *   auto optional = ps.get<int>("answer",42);
 *
 *   ...
 * }
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
   * \param[in] key name of key to check for existence
   */
  bool has_key(std::string const& key) const;

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
  using types_ =
      boost::mpl::vector<int, double, bool, std::string, falaise::config::path,
                         falaise::config::quantity, std::vector<int>, std::vector<double>,
                         std::vector<bool>, std::vector<std::string>>;
  template <typename T>
  struct can_hold_ {
    typedef typename boost::mpl::contains<types_, T>::type type;
  };

  template <typename T>
  struct can_hold_<falaise::config::quantity_t<T>> {
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
  bool is_type_impl_(std::string const& key, std::string) const;

  //! Return true if value at key is a falaise::path
  bool is_type_impl_(std::string const& key, falaise::config::path) const;

  //! Return true if value at key is a falaise::quantity
  bool is_type_impl_(std::string const& key, falaise::config::quantity) const;

  //! Return true if value at key is a std::vector<int>
  bool is_type_impl_(std::string const& key, std::vector<int>) const;

  //! Return true if value at key is a std::vector<double> (dimensionless
  //! doubles)
  bool is_type_impl_(std::string const& key, std::vector<double>) const;

  //! Return true if value at key is a std::vector<bool>
  bool is_type_impl_(std::string const& key, std::vector<bool>) const;

  //! Return true if value at key is a std::vector<std::string>
  bool is_type_impl_(std::string const& key, std::vector<std::string>) const;

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

  //! Overload of put_impl_ for @ref falaise::config::quantity_t
  template <typename T>
  void put_impl_(std::string const& key, falaise::config::quantity_t<T> const& value);

  //! Overload of get_impl_ for @ref falaise::config::quantity_t
  template <typename T>
  void get_impl_(std::string const& key, falaise::config::quantity_t<T>& result) const;

  datatools::properties ps_;  //< underlying set of properties
};
}  // namespace config
}  // namespace falaise

// - Definitions for template functions
namespace falaise {
namespace config {
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

template <typename T>
void property_set::put(std::string const& key, T const& value) {
  static_assert(can_hold_t_<T>::value, "property_set cannot hold values of type T");
  // Check directly to use our clearer exception type
  if (ps_.has_key(key)) {
    throw existing_key_error{"property_set already contains key " + key};
  }
  put_impl_(key, value);
}

template <typename T>
void property_set::put_or_replace(std::string const& key, T const& value) {
  // Cannot change type of already held data, so must erase/re-store
  erase(key);
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
void property_set::put_impl_(std::string const& key, falaise::config::path const& value) {
  ps_.store_path(key, value);
}

//! Private specialization of put_impl_ for @ref quantity
template <>
void property_set::put_impl_(std::string const& key, falaise::config::quantity const& value) {
  ps_.store_with_explicit_unit(key, value.value());
  ps_.set_unit_symbol(key, value.unit());
}

//! Private overload of put_impl_ for @ref quantity_t
template <typename T>
void property_set::put_impl_(std::string const& key, falaise::config::quantity_t<T> const& value) {
  ps_.store_with_explicit_unit(key, value.value());
  ps_.set_unit_symbol(key, value.unit());
}

// Generic get_impl_
template <typename T>
void property_set::get_impl_(std::string const& key, T& result) const {
  ps_.fetch(key, result);
}

// Private specialization of get_impl_ for @ref path type
template <>
void property_set::get_impl_(std::string const& key, falaise::config::path& result) const {
  result = falaise::config::path{ps_.fetch_path(key)};
}

// Private specialization of get_impl_ for @ref units::quantity type
template <>
void property_set::get_impl_(std::string const& key, falaise::config::quantity& result) const {
  result = {ps_.fetch_real_with_explicit_unit(key), ps_.get_unit_symbol(key)};
}

// Overload for explicitly dimensioned quantities
template <typename T>
void property_set::get_impl_(std::string const& key, falaise::config::quantity_t<T>& result) const {
  result = {ps_.fetch_real_with_explicit_unit(key), ps_.get_unit_symbol(key)};
}

//! Construct a property_set from an input datatools::properties file
/*!
 * \param filename File from which to read data
 * \param ps property_set to fill with data
 */
void make_property_set(const std::string& filename, property_set& ps);

}  // namespace config
}  // namespace falaise

#endif  // FALAISE_PROPERTY_SET_H
