// -*- mode: c++ ; -*-
/* * \file falaise/snemo/datamodels/timestamp.cc
 */

// Ourselves
#include <falaise/snemo/datamodels/timestamp.h>

// Standard library
#include <limits>
#include <time.h>

namespace snemo {

  namespace datamodel {

    // serial tag for datatools::serialization::i_serializable interface :
    //DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(timestamp, "snemo::core::utils::timestamp")
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(timestamp, "snemo::datamodel::timestamp")

    const int64_t timestamp::INVALID_SECONDS     = std::numeric_limits<int64_t>::min ();
    const int64_t timestamp::INVALID_PICOSECONDS = std::numeric_limits<int64_t>::min ();

    const char    timestamp::IO_FORMAT_OPEN      = '[';
    const char    timestamp::IO_FORMAT_SEP       = ':';
    const char    timestamp::IO_FORMAT_CLOSE     = ']';
    const char    timestamp::IO_FORMAT_INVALID   = '?';

    bool timestamp::is_valid () const
    {
      return std::isnormal (seconds_) &&  std::isnormal (picoseconds_);
    }

    void timestamp::invalidate ()
    {
      seconds_ = INVALID_SECONDS;
      picoseconds_ = INVALID_PICOSECONDS;
      return;
    }

    int timestamp::compare (const timestamp & a_ts) const
    {
      DT_THROW_IF (! is_valid (), std::logic_error, "Invalid timestamp (this) !");
      DT_THROW_IF (! a_ts.is_valid (), std::logic_error, "Invalid timestamp (argument) !");
      if (seconds_ < a_ts.seconds_) return -1;
      if (seconds_ > a_ts.seconds_) return +1;
      if (picoseconds_ < a_ts.picoseconds_) return -1;
      if (picoseconds_ > a_ts.picoseconds_) return +1;
      return 0;
    }

    int64_t timestamp::get_seconds () const
    {
      return seconds_;
    }

    void timestamp::set_seconds (int64_t new_value)
    {
      seconds_ = new_value;
      return;
    }

    int64_t timestamp::get_picoseconds () const
    {
      return picoseconds_;
    }

    void
    timestamp::set_picoseconds (int64_t new_value)
    {
      picoseconds_ = new_value;
      return;
    }

    // ctor:
    timestamp::timestamp ()
    {
      invalidate ();
      return;
    }

    timestamp::timestamp (int64_t a_sec, int64_t a_picosec)
    {
      set_seconds (a_sec);
      set_picoseconds (a_picosec);
      return;
    }

    // dtor:
    timestamp::~timestamp ()
    {
      return;
    }

    bool operator== (const timestamp & a_ts1, const timestamp & a_ts2)
    {
      return a_ts1.compare (a_ts2) == 0;
    }

    bool operator< (const timestamp & a_ts1, const timestamp & a_ts2)
    {
      return a_ts1.compare (a_ts2) == -1;
    }

    bool operator> (const timestamp & a_ts1, const timestamp & a_ts2)
    {
      return a_ts1.compare (a_ts2) == +1;
    }

    bool operator<= (const timestamp & a_ts1, const timestamp & a_ts2)
    {
      return a_ts1.compare (a_ts2) <= 0;
    }

    bool operator>= (const timestamp & a_ts1, const timestamp & a_ts2)
    {
      return a_ts1.compare (a_ts2) >= 0;
    }

    void timestamp::to_string (std::string & a_str) const
    {
      std::ostringstream out;
      out << *this;
      a_str = out.str ();
      return;
    }

    std::string timestamp::to_string () const
    {
      std::ostringstream out;
      out << *this;
      return out.str ();
    }

    void timestamp::from_string (const std::string & a_str)
    {
      timestamp ts;
      std::istringstream in (a_str);
      in >> ts;
      DT_THROW_IF (! in, std::logic_error, "Format error !");
      *this = ts;
      return;
    }

    std::ostream & operator<< (std::ostream & a_out, const timestamp & a_ts)
    {
      a_out << timestamp::IO_FORMAT_OPEN;
      if (a_ts.seconds_ != timestamp::INVALID_SECONDS) {
        a_out << a_ts.seconds_;
      } else {
        a_out << timestamp::IO_FORMAT_INVALID;
      }
      a_out << timestamp::IO_FORMAT_SEP;
      if (a_ts.picoseconds_ != timestamp::INVALID_PICOSECONDS) {
        a_out << a_ts.picoseconds_;
      } else {
        a_out << timestamp::IO_FORMAT_INVALID;
      }
      a_out << timestamp::IO_FORMAT_CLOSE;
      return a_out;
    }

    std::istream & operator>> (std::istream & a_in, timestamp & a_ts)
    {
      char c = 0;
      a_in >> c;
      if (! a_in) return a_in;
      if (c != timestamp::IO_FORMAT_OPEN) {
        a_in.setstate (std::ios_base::failbit);
        return a_in;
      }
      int64_t s, ps;
      c = 0;

      c = a_in.peek ();
      if (c == timestamp::IO_FORMAT_INVALID) {
        s = timestamp::INVALID_SECONDS;
        a_in.get ();
      } else {
        a_in >> s;
        if (! a_in) return a_in;
      }

      c = 0;
      a_in >> c;
      if (c != timestamp::IO_FORMAT_SEP) {
        a_in.setstate (std::ios_base::failbit);
        return a_in;
      }

      c = a_in.peek ();
      if (c == timestamp::IO_FORMAT_INVALID) {
        ps = timestamp::INVALID_SECONDS;
        a_in.get ();
      } else {
        a_in >> ps;
        if (! a_in) return a_in;
      }

      c = 0;
      a_in >> c;
      if (c != timestamp::IO_FORMAT_CLOSE) {
        a_in.setstate (std::ios_base::failbit);
        return a_in;
      }
      a_ts.set_seconds (s);
      a_ts.set_picoseconds (ps);
      return a_in;
    }

  } // end of namespace datamodel

} // end of namespace snemo

// end of timestamp.cc
