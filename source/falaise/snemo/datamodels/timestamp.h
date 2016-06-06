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
#include <datatools/i_serializable.h>

namespace snemo {

  namespace datamodel {

    class timestamp : public datatools::i_serializable
    {
    public:

      static const int64_t INVALID_SECONDS;
      static const int64_t INVALID_PICOSECONDS;
      static const char    IO_FORMAT_OPEN;
      static const char    IO_FORMAT_SEP;
      static const char    IO_FORMAT_CLOSE;
      static const char    IO_FORMAT_INVALID;

      int64_t get_seconds () const;

      void set_seconds (int64_t);

      int64_t get_picoseconds () const;

      void set_picoseconds (int64_t);

      /// Constructor
      timestamp ();

      timestamp (int64_t a_sec, int64_t a_picosec);

      /// Destructor
      virtual ~timestamp ();

      /// Check if the timestamp object is valid
      bool is_valid () const;

      /// Invalidate the timestamp object
      void invalidate ();

      /// Compare with another timestamp
      int compare (const timestamp &) const;

      friend bool operator== (const timestamp &, const timestamp &);
      friend bool operator<  (const timestamp &, const timestamp &);
      friend bool operator>  (const timestamp &, const timestamp &);
      friend bool operator<= (const timestamp &, const timestamp &);
      friend bool operator>= (const timestamp &, const timestamp &);

      std::string to_string () const;
      void to_string (std::string & ) const;
      void from_string (const std::string &);
      friend std::ostream & operator<< (std::ostream &, const timestamp &);
      friend std::istream & operator>> (std::istream &, timestamp &);

    private:

      int64_t seconds_;     //!< Number of seconds
      int64_t picoseconds_; //!< Number of picosecond

      DATATOOLS_SERIALIZATION_DECLARATION()
    };

  } // end of namespace datamodel

} // end of namespace snemo

#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(snemo::datamodel::timestamp, 1)

#endif // FALAISE_SNEMO_DATAMODELS_TIMESTAMP_H
