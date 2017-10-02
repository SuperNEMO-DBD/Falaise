// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/timestamp.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-03-15
 * Last modified: 2014-01-27
 *
 * License:
 *
 * Description:
 *
 *   High resolution timestamp
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_DATAMODELS_TIMESTAMP_H
#define FALAISE_SNEMO_DATAMODELS_TIMESTAMP_H 1

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
#include <boost/serialization/access.hpp>
// - Bayeux/datatools :
#include <bayeux/datatools/i_serializable.h>

namespace snemo {

namespace datamodel {

/// \brief A class to handle time stamp
class timestamp : public datatools::i_serializable {
 public:
  static const int64_t INVALID_SECONDS;
  static const int64_t INVALID_PICOSECONDS;
  static const char IO_FORMAT_OPEN;
  static const char IO_FORMAT_SEP;
  static const char IO_FORMAT_CLOSE;
  static const char IO_FORMAT_INVALID;

  /// Return the number of seconds
  int64_t get_seconds() const;

  /// Set the number of seconds
  void set_seconds(int64_t);

  /// Return the number of picoseconds
  int64_t get_picoseconds() const;

  /// Set the number of picoseconds
  void set_picoseconds(int64_t);

  /// Constructor
  timestamp();

  /// Overloaded constructor
  timestamp(int64_t sec_, int64_t picosec_);

  /// Destructor
  virtual ~timestamp();

  /// Check if the timestamp object is valid
  bool is_valid() const;

  /// Invalidate the timestamp object
  void invalidate();

  /// Compare with another timestamp
  int compare(const timestamp &) const;

  /// Convert timestamp to real value (explicit time unit)
  double to_real() const;

  /// Format time stamp as string object and return it
  std::string to_string() const;

  /// Format time stamp as string object and return it as reference
  void to_string(std::string &) const;

  /// Parse time from string object and set the timestamp object accordingly
  void from_string(const std::string &);

  friend bool operator==(const timestamp &, const timestamp &);
  friend bool operator<(const timestamp &, const timestamp &);
  friend bool operator>(const timestamp &, const timestamp &);
  friend bool operator<=(const timestamp &, const timestamp &);
  friend bool operator>=(const timestamp &, const timestamp &);
  friend std::ostream &operator<<(std::ostream &, const timestamp &);
  friend std::istream &operator>>(std::istream &, timestamp &);

 private:
  int64_t _seconds_;      //!< Number of seconds
  int64_t _picoseconds_;  //!< Number of picoseconds

  DATATOOLS_SERIALIZATION_DECLARATION()
};

}  // end of namespace datamodel

}  // end of namespace snemo

#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(snemo::datamodel::timestamp, 1)

#endif  // FALAISE_SNEMO_DATAMODELS_TIMESTAMP_H
