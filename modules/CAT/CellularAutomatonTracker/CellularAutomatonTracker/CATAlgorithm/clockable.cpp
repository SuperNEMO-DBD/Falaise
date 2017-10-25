/* -*- mode: c++ -*- */

#include <CATAlgorithm/clockable.h>

namespace CAT {

using namespace std;

bool clockable::compare(const clockable& c1, const clockable& c2) {
  return c1.time_ > c2.time_;  // notice that you have private access rights, which can be useful.
}

//! Default constructor
clockable::clockable(std::string name) {
  name_ = name;
  time_ = 0.;
  return;
}

//! Default destructor
clockable::~clockable() { return; }

void clockable::dump(double max, std::ostream& a_out, const std::string& /* a_title */,
                     const std::string& /* a_indent */, bool /* a_inherit */) const {
  a_out << "CAT::clockable::dump: time of '" << name_ << "' : " << time_ << " ms ("
        << time_ / max * 100. << " %)" << std::endl;
  return;
}

//! set name
void clockable::set_name(std::string name) {
  name_ = name;
  return;
}

//! get name
const std::string& clockable::name() const { return name_; }

//! read time
double clockable::read() {
  // stop();
  // tv_begin_ = tv_end_;
  // tz_begin_ = tz_end_;
  gettimeofday(&tv_end_, &tz_end_);
  return time_ + ((double)tv_end_.tv_sec - (double)tv_begin_.tv_sec) * 1.e+3 +
         ((double)tv_end_.tv_usec - (double)tv_begin_.tv_usec) * 1.e-3;
}

void clockable::start() {
  gettimeofday(&tv_begin_, &tz_begin_);
  return;
}

void clockable::restart() {
  time_ = 0.;
  gettimeofday(&tv_begin_, &tz_begin_);
  return;
}

void clockable::stop() {
  gettimeofday(&tv_end_, &tz_end_);
  time_ += ((double)tv_end_.tv_sec - (double)tv_begin_.tv_sec) * 1.e+3 +
           ((double)tv_end_.tv_usec - (double)tv_begin_.tv_usec) * 1.e-3;
  return;
}

}  // namespace CAT
