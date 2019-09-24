// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/timestamp.cc

// Ourselves:
#include <falaise/snemo/datamodels/timestamp.h>

// Standard library:
#include <limits>

// Third party:
// - Bayeux:
#include <bayeux/datatools/clhep_units.h>

namespace {
const char IO_FORMAT_OPEN = '[';
const char IO_FORMAT_SEP = ':';
const char IO_FORMAT_CLOSE = ']';
const char IO_FORMAT_INVALID = '?';
}  // namespace

namespace snemo {

namespace datamodel {

// serial tag for datatools::i_serializable interface :
DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(timestamp, "snemo::datamodel::timestamp")

const int64_t timestamp::INVALID_SECONDS = std::numeric_limits<int64_t>::min();
const int64_t timestamp::INVALID_PICOSECONDS = std::numeric_limits<int64_t>::min();

bool timestamp::is_valid() const {
  return seconds_ != INVALID_SECONDS && picoseconds_ != INVALID_PICOSECONDS;
}


int timestamp::compare(const timestamp& ts) const {
  DT_THROW_IF(!is_valid(), std::logic_error, "Invalid timestamp (this) !");
  DT_THROW_IF(!ts.is_valid(), std::logic_error, "Invalid timestamp (argument) !");
  if (seconds_ < ts.seconds_) {
    return -1;
  }
  if (seconds_ > ts.seconds_) {
    return +1;
  }
  if (picoseconds_ < ts.picoseconds_) {
    return -1;
  }
  if (picoseconds_ > ts.picoseconds_) {
    return +1;
  }
  return 0;
}

int64_t timestamp::get_seconds() const { return seconds_; }

void timestamp::set_seconds(int64_t s) { seconds_ = s; }

int64_t timestamp::get_picoseconds() const { return picoseconds_; }

void timestamp::set_picoseconds(int64_t ps) { picoseconds_ = ps; }

double timestamp::to_real() const {
  return seconds_ * CLHEP::second + picoseconds_ * CLHEP::picosecond;
}

bool operator==(const timestamp& lhs, const timestamp& rhs) { return lhs.compare(rhs) == 0; }

bool operator<(const timestamp& lhs, const timestamp& rhs) { return lhs.compare(rhs) == -1; }

bool operator>(const timestamp& lhs, const timestamp& rhs) { return lhs.compare(rhs) == +1; }

bool operator<=(const timestamp& lhs, const timestamp& rhs) { return lhs.compare(rhs) <= 0; }

bool operator>=(const timestamp& lhs, const timestamp& rhs) { return lhs.compare(rhs) >= 0; }

void timestamp::to_string(std::string& s) const {
  s = to_string();
}

std::string timestamp::to_string() const {
  std::ostringstream out;
  out << *this;
  return out.str();
}

void timestamp::from_string(const std::string& str) {
  timestamp ts;
  std::istringstream in(str);
  in >> ts;
  DT_THROW_IF(!in, std::logic_error, "Format error !");
  *this = ts;
}

std::ostream& operator<<(std::ostream& os, const timestamp& ts) {
  os << ::IO_FORMAT_OPEN;
  if (ts.seconds_ != timestamp::INVALID_SECONDS) {
    os << ts.seconds_;
  } else {
    os << ::IO_FORMAT_INVALID;
  }
  os << ::IO_FORMAT_SEP;
  if (ts.picoseconds_ != timestamp::INVALID_PICOSECONDS) {
    os << ts.picoseconds_;
  } else {
    os << ::IO_FORMAT_INVALID;
  }
  os << ::IO_FORMAT_CLOSE;
  return os;
}

std::istream& operator>>(std::istream& a_in, timestamp& ts) {
  char c = 0;
  a_in >> c;
  if (!a_in) {
    return a_in;
  }
  if (c != ::IO_FORMAT_OPEN) {
    a_in.setstate(std::ios_base::failbit);
    return a_in;
  }
  int64_t s, ps;
  c = 0;

  c = a_in.peek();
  if (c == ::IO_FORMAT_INVALID) {
    s = timestamp::INVALID_SECONDS;
    a_in.get();
  } else {
    a_in >> s;
    if (!a_in) {
      return a_in;
    }
  }

  c = 0;
  a_in >> c;
  if (c != ::IO_FORMAT_SEP) {
    a_in.setstate(std::ios_base::failbit);
    return a_in;
  }

  c = a_in.peek();
  if (c == ::IO_FORMAT_INVALID) {
    ps = timestamp::INVALID_SECONDS;
    a_in.get();
  } else {
    a_in >> ps;
    if (!a_in) {
      return a_in;
    }
  }

  c = 0;
  a_in >> c;
  if (c != ::IO_FORMAT_CLOSE) {
    a_in.setstate(std::ios_base::failbit);
    return a_in;
  }
  ts.set_seconds(s);
  ts.set_picoseconds(ps);
  return a_in;
}

}  // end of namespace datamodel

}  // end of namespace snemo
