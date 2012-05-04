/* -*- mode: c++ -*- */
#ifndef __CATAlgorithm__Clock_h
#define __CATAlgorithm__Clock_h 1

#include <CATAlgorithm/clockable.h>
#include <vector>
#include <iostream>
#include <string>

namespace CAT {

  class Clock {

    // a Clock is a time counter

  private:
    // list of clockable objects
    std::vector<clockable> clockables_;

  public:

    //!Default constructor
    Clock();

    //!Default destructor
    virtual ~Clock();

    virtual void dump (std::ostream & a_out         = std::clog,
                       const std::string & a_title  = "",
                       const std::string & a_indent = "",
                       bool a_inherit               = false) const;

    std::vector<clockable> & clockables();

    bool has(const std::string & name, size_t *index) const;

    void start(const std::string & name, const std::string & mode="once");

    void stop(const std::string & name);

    double read(const std::string & name);

    void stop_all();

  };

}

#endif // __CATAlgorithm__Clock_h
