#ifndef FALAISE_PATH_H
#define FALAISE_PATH_H

#include <exception>
#include <ostream>
#include <string>

namespace falaise {
namespace config {
//! Exception for paths that cannot be resolved by datatools::utils
class invalid_path_error : public std::logic_error {
  using std::logic_error::logic_error;
};

//! Class representing a filesystem path as held by a @ref property_set
/*!
 * Filesystem paths can be defined in datatools::properties syntax as:
 *
 * ```ini
 * mypath : string as path = "/some/path"
 * ```
 *
 * Parsing of the properties file will expand any mount points or environment
 * variables so that fetching the path will return a `std::string` containing
 * the resolved absolute path.
 *
 * @ref path provides a simple type to distinguish raw `std::string` from
 * explicit paths allowing users to validate that a @ref property_set value
 * is a true path. It is nothing more than a simple holder of the std::string for
 * the absolute path and may be used as:
 * 
 * ```cpp
 * void configure_me(property_set const& ps) {
 *   auto p = ps.get<falaise::config::path>("mypath"); // throws if "mypath" is not a path value
 *   std::cout << p << std::endl; // prints absolute path
 * }
 * ```
 *
 * A @ref path can be constructed from any string whose value is a valid path
 * as understood by @ref datatools::fetch_path_with_env. Examples are:
 *
 * - An absolute path, e.g. `auto x = falaise::config::path{"/foo/bar/baz.txt"};`
 * - A relative path, e.g. `auto x = falaise::config::path{"bar/baz.txt"};`
 *
 * The string may also include environment variables, e.g. `$HOME` or `${HOME}`,
 * which will be expanded if they exist, otherwise a @ref invalid_path_error
 * is thrown. When running in a program that provides the @ref datatools::kernel
 * service, paths can also be supplied as:
 *
 * - A resource path, e.g. `@falaise:foo/bar.txt`
 * - A @ref datatools::urn, e.g. `urn:subject:object`
 *
 * Note that in all cases, @ref path *does not* check that a file exists at the location
 * specified by the path. If you require more specific filesystem functionality, you
 * should use the value of path in a full library such as
 * [boost::filesystem](https://www.boost.org/doc/libs/1_69_0/libs/filesystem/doc/index.htm) or
 * C++17's [std::filesystem](https://en.cppreference.com/w/cpp/filesystem) if available.
 *
 * \sa property_set
 * \sa datatools::fetch_path_with_env
 */
class path {
 public:
  //! Default constructor
  path() = default;

  //! Construct path from a std::string
  /*!
   * Not declared explicit as we allow conversions for compatibility with
   * existing use of filesystem paths as std::string.
   *
   * \param[in] p
   * \throws invalid_path_error if p is not a valid path string
   */
  path(std::string const& p);

  //! Conversion to std::string
  operator std::string() const { return value; }

  //! Equality operator
  /*!
   * \returns true if the two operands are the same
   */
  bool operator==(path const& other) const { return value == other.value; }

  //! Inequality operator
  /*!
   * \returns true if the two operands are not the same
   */
  bool operator!=(path const& other) const { return !(*this == other); }

  //! Equality operator for path against a string
  /*!
   * \returns true if the value of the path is equal to the string
   */
  bool operator==(std::string const& other) const { return value == other; }

  //! Inequality operator for path against a string
  /*!
   * \returns true if the value of the path is not equal to the string
   */
  bool operator!=(std::string const& other) const { return !(*this == other); }

 private:
  std::string value;  //< absolute path value
};

//! Output path to an ostream
inline std::ostream& operator<<(std::ostream& os, path const& p) {
  os << std::string{p};
  return os;
}

}  // namespace config
}  // namespace falaise

#endif /* FALAISE_PATH_H */