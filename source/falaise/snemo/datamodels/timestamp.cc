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
  return _seconds_ != INVALID_SECONDS && _picoseconds_ != INVALID_PICOSECONDS;
}


int timestamp::compare(const timestamp& ts_) const {
  DT_THROW_IF(!is_valid(), std::logic_error, "Invalid timestamp (this) !");
  DT_THROW_IF(!ts_.is_valid(), std::logic_error, "Invalid timestamp (argument) !");
  if (_seconds_ < ts_._seconds_) {
    return -1;
  }
  if (_seconds_ > ts_._seconds_) {
    return +1;
  }
  if (_picoseconds_ < ts_._picoseconds_) {
    return -1;
  }
  if (_picoseconds_ > ts_._picoseconds_) {
    return +1;
  }
  return 0;
}

int64_t timestamp::get_seconds() const { return _seconds_; }

void timestamp::set_seconds(int64_t new_value_) { _seconds_ = new_value_; }

int64_t timestamp::get_picoseconds() const { return _picoseconds_; }

void timestamp::set_picoseconds(int64_t new_value_) { _picoseconds_ = new_value_; }

double timestamp::to_real() const {
  double time = _seconds_ * CLHEP::second;
  time += _picoseconds_ * CLHEP::picosecond;
  return time;
}

bool operator==(const timestamp& ts1_, const timestamp& ts2_) { return ts1_.compare(ts2_) == 0; }

bool operator<(const timestamp& ts1_, const timestamp& ts2_) { return ts1_.compare(ts2_) == -1; }

bool operator>(const timestamp& ts1_, const timestamp& ts2_) { return ts1_.compare(ts2_) == +1; }

bool operator<=(const timestamp& ts1_, const timestamp& ts2_) { return ts1_.compare(ts2_) <= 0; }

bool operator>=(const timestamp& ts1_, const timestamp& ts2_) { return ts1_.compare(ts2_) >= 0; }

void timestamp::to_string(std::string& str_) const {
  std::ostringstream out;
  out << *this;
  str_ = out.str();
}

std::string timestamp::to_string() const {
  std::ostringstream out;
  out << *this;
  return out.str();
}

void timestamp::from_string(const std::string& str_) {
  timestamp ts;
  std::istringstream in(str_);
  in >> ts;
  DT_THROW_IF(!in, std::logic_error, "Format error !");
  *this = ts;
}

std::ostream& operator<<(std::ostream& out_, const timestamp& ts_) {
  out_ << ::IO_FORMAT_OPEN;
  if (ts_._seconds_ != timestamp::INVALID_SECONDS) {
    out_ << ts_._seconds_;
  } else {
    out_ << ::IO_FORMAT_INVALID;
  }
  out_ << ::IO_FORMAT_SEP;
  if (ts_._picoseconds_ != timestamp::INVALID_PICOSECONDS) {
    out_ << ts_._picoseconds_;
  } else {
    out_ << ::IO_FORMAT_INVALID;
  }
  out_ << ::IO_FORMAT_CLOSE;
  return out_;
}

std::istream& operator>>(std::istream& a_in, timestamp& ts_) {
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
  ts_.set_seconds(s);
  ts_.set_picoseconds(ps);
  return a_in;
}

}  // end of namespace datamodel

}  // end of namespace snemo
