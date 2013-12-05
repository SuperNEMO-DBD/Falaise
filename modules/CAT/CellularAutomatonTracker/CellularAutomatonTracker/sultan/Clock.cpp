/* -*- mode: c++ -*- */

#include <sultan/Clock.h>
#include <algorithm>

namespace SULTAN {

  using namespace std;

  //!Default constructor
  Clock::Clock()
  {
    return;
  }

  //!Default destructor
  Clock::~Clock()
  {
    return;
  }

  void Clock::dump (ostream & a_out,
                    const std::string & a_title,
                    const std::string & a_indent,
                    bool a_inherit) const
  {
    {
      // Trick to workaround the constness of the dump method :
      // because 'this' is const, we cannot modify/sort the 'clockables_' STL collection,
      // so we use 'mutable_this' :
      Clock * mutable_this = const_cast<Clock *>(this);
      std::sort( mutable_this->clockables_.begin(), 
                 mutable_this->clockables_.end(), clockable::compare );
    }
    double max = clockables_.begin()->time_;

    for(size_t i=0; i<clockables_.size(); i++){
      this->clockables_[i].dump(max);
    }
    return;
  }


  std::vector<clockable>& Clock::clockables()
  {
    return clockables_;
  }

  bool Clock::has(const std::string & name, size_t *index) const
  {
    for(std::vector<clockable>::const_iterator iclock = clockables_.begin(); iclock != clockables_.end(); ++iclock){
      if( iclock->name() == name ){
        *index = iclock - clockables_.begin();
        return true;
      }
    }
    return false;
  }

  void Clock::start(const std::string & name, const std::string & mode)
  {
    if( mode == "once" ){
      size_t index=0;
      if( has(name, &index ) ){
        std::clog << " problem: starting a clockable " << name << " which is already there " << index << std::endl;
        clockables()[index].start();
      }
      else{
        clockable newc(name);
        clockables_.push_back(newc);
        size_t s = clockables().size();
        clockables()[s-1].start();
      }
    }
    else if( mode == "cumulative" ){
      size_t index=0;
      if( has(name, &index ) )
        clockables()[index].start();
      else{
        clockable newc(name);
        clockables_.push_back(newc);
        size_t s = clockables().size();
        clockables()[s-1].start();
      }
    }
    else if( mode == "restart" ){
      size_t index=0;
      if( has(name, &index ) ){
        clockables()[index].restart();
      }
      else{
        clockable newc(name);
        clockables_.push_back(newc);
        size_t s = clockables().size();
        clockables()[s-1].start();
      }


    }
    return;
  }

  void Clock::stop(const std::string & name)
  {
    size_t index;
    if( has(name , &index) )
      clockables()[index].stop();  
    else
      std::clog << " problem: can't stop clockable " << name << " which is not there " << std::endl;
  }

  double Clock::read(const std::string & name)
  {
    size_t index;
    if( has(name , &index) )
      return clockables()[index].read();  

    std::clog << " problem: request time of clockable " << name << " which is not there " << std::endl;
    return 0;
  }

  void Clock::stop_all()
  {
    for(std::vector<clockable>::iterator iclock = clockables_.begin(); iclock != clockables_.end(); ++iclock)
      iclock->stop();
  }

}
