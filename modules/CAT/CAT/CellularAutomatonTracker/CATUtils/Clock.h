/* -*- mode: c++ -*- */
#ifndef __cat_utils__Clock_h_
#define __cat_utils__Clock_h_ 1

#include <iostream>
#include <cmath>
#include <CATUtils/clockable.h>
#include <vector>
#include <algorithm>
#include <functional>

namespace CAT {
namespace utils {

// a Clock is a time counter
class Clock {
 public:
  // list of clockable objects
  std::vector<clockable> clockables_;

  //! Default constructor
  Clock() { clockables_.clear(); }

  //! Default destructor
  virtual ~Clock(){};

  virtual void dump(std::ostream& a_out = std::clog, const std::string& a_title = "",
                    const std::string& a_indent = "", bool a_inherit = false) {
    std::sort(clockables_.begin(), clockables_.end(), clockable::compare);

    double max = clockables_.begin()->time_;

    for (size_t i = 0; i < clockables_.size(); i++) {
      this->clockables_[i].dump(max, a_out);
    }
  }

  std::vector<clockable>& clockables() { return clockables_; }

  bool has(const std::string& name, size_t* index) {
    for (std::vector<clockable>::iterator iclock = clockables_.begin(); iclock != clockables_.end();
         ++iclock) {
      if (iclock->name() == name) {
        *index = iclock - clockables_.begin();
        return true;
      }
    }

    return false;
  }

  void start(const std::string& name, std::string mode = "once") {
    if (mode == "once") {
      size_t index = 0;
      if (has(name, &index)) {
        cout << " problem: starting a clockable " << name << " which is already there " << index
             << endl;
        clockables()[index].start();
      } else {
        clockable newc(name);
        clockables_.push_back(newc);
        size_t s = clockables().size();
        clockables()[s - 1].start();
      }
    } else if (mode == "cumulative") {
      size_t index = 0;
      if (has(name, &index))
        clockables()[index].start();
      else {
        clockable newc(name);
        clockables_.push_back(newc);
        size_t s = clockables().size();
        clockables()[s - 1].start();
      }
    } else if (mode == "restart") {
      size_t index = 0;
      if (has(name, &index)) {
        clockables()[index].restart();
      } else {
        clockable newc(name);
        clockables_.push_back(newc);
        size_t s = clockables().size();
        clockables()[s - 1].start();
      }
    }
    return;
  }

  void stop(const std::string& name) {
    size_t index;
    if (has(name, &index))
      clockables()[index].stop();
    else
      cout << " problem: can't stop clockable " << name << " which is not there " << endl;
  }

  double read(const std::string& name) {
    size_t index;
    if (has(name, &index)) return clockables()[index].read();

    cout << " problem: request time of clockable " << name << " which is not there " << endl;
    return 0;
  }

  void stop_all() {
    for (std::vector<clockable>::iterator iclock = clockables_.begin(); iclock != clockables_.end();
         ++iclock)
      iclock->stop();
  }
};

}  // end of namespace utils
}  // end of namespace CAT

#endif  // __cat_utils__Clock_h_
