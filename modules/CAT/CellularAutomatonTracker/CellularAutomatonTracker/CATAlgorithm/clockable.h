/* -*- mode: c++ -*- */

#ifndef __CATAlgorithm__clockable_h
#define __CATAlgorithm__clockable_h 1

#include <iostream>
#include <string>
#include <sys/time.h> 

namespace CAT {

  class clockable {

    // a clockable is a time counter

    struct timeval tv_begin_, tv_end_;
    struct timezone tz_begin_, tz_end_;
    
  public:

    std::string name_;

    // time in milliseconds
    double time_;
    
    //!Default constructor
    clockable(std::string name="default");

    //!Default destructor
    virtual ~clockable();

    virtual void dump (double max = 1.,
                       std::ostream & a_out         = std::clog,
                       const std::string & a_title  = "",
                       const std::string & a_indent = "",
                       bool a_inherit          = false) const;

    //! set name
    void set_name(std::string name);

    //! get name
    const std::string & name() const;

    //! read time
    double read();

    void start();

    void restart();

    void stop();

    static bool compare(const clockable& c1, const clockable& c2);

  };
}

#endif // __CATAlgorithm__clockable_h
