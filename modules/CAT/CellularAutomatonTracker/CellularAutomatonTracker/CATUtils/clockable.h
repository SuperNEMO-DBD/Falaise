/* -*- mode: c++ -*- */
#ifndef ICLOCKABLE
#define ICLOCKABLE
#include <iostream>
#include <cmath>
#include <sys/time.h> 
#include <vector>
#include <algorithm>
#include <functional>


using namespace std;
using namespace mybhep;

  class clockable {

    // a clockable is a time counter


  public:

    static bool compare(const clockable& c1, const clockable& c2) {
      return c1.time_ > c2.time_; //notice that you have private access rights, which can be useful.
    };


    struct timeval tv_begin_, tv_end_;
    struct timezone tz_begin_, tz_end_;
    
  public:

    string name_;

    // time in milliseconds
    double time_;
    
    //!Default constructor
    clockable(string name="default")
    {
      name_ = name;
      time_ = 0.;
    }

    //!Default destructor
    virtual ~clockable(){};

    virtual void dump (double max = 1.,
		       ostream & a_out         = cout,
		       const string & a_title  = "",
		       const string & a_indent = "",
		       bool a_inherit          = false) const{

      cout << " time of " << name_ << " : " << time_ << " ms (" << time_/max*100. << " \%)" << endl;

    }


    //! set name
    void set_name(string name)
      {
	name_ = name;
      }

    //! get name
    string name(){
      return name_;
    }

    //! read time
    double read(){
      stop();
      tv_begin_ = tv_end_;
      tz_begin_ = tz_end_;
      return time_;
    }

    void start(){
      gettimeofday(&tv_begin_, &tz_begin_);
    }

    void restart(){
      time_ = 0.;
      gettimeofday(&tv_begin_, &tz_begin_);
    }


    void stop(){
      gettimeofday(&tv_end_, &tz_end_);
      time_ += ((double)tv_end_.tv_sec-(double)tv_begin_.tv_sec)*1.e+3+((double)tv_end_.tv_usec-(double)tv_begin_.tv_usec)*1.e-3;
    }



  };


#endif
