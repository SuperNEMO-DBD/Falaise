/* -*- mode: c++ -*- */
#ifndef ICLOCK
#define ICLOCK
#include <iostream>
#include <cmath>
#include <util/clockable.h>
#include <vector>
#include <algorithm>
#include <functional>


using namespace std;

  class Clock {

    // a Clock is a time counter

  private:

  public:

    // list of clockable objects
    std::vector<clockable> clockables_;


    //!Default constructor
    Clock()
    {
      clockables_.clear();
    }

    //!Default destructor
    virtual ~Clock(){};

    virtual void dump (ostream & a_out         = cout,
                            const string & a_title  = "",
                            const string & a_indent = "",
                            bool a_inherit          = false){


      std::sort( clockables_.begin(), clockables_.end(), clockable::compare );

      double max = clockables_.begin()->time_;

      for(size_t i=0; i<clockables_.size(); i++){
	this->clockables_[i].dump(max);
      }
    }


    std::vector<clockable>& clockables(){
      return clockables_;
    }

    bool has(string name, size_t *index){

      for(std::vector<clockable>::iterator iclock = clockables_.begin(); iclock != clockables_.end(); ++iclock){
	if( iclock->name() == name ){
	  *index = iclock - clockables_.begin();
	  return true;
	}
      }

      return false;
    }

    void start(string name, string mode="once"){
      if( mode == "once" ){
	size_t index=0;
	if( has(name, &index ) ){
	  cout << " problem: starting a clockable " << name << " which is already there " << index << endl;
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


    void stop(string name){
      size_t index;
      if( has(name , &index) )
	clockables()[index].stop();  
      else
	cout << " problem: can't stop clockable " << name << " which is not there " << endl;
    }

    double read(string name){
      size_t index;
      if( has(name , &index) )
	return clockables()[index].read();  

      cout << " problem: request time of clockable " << name << " which is not there " << endl;
      return 0;
    }

    void stop_all(){
      for(std::vector<clockable>::iterator iclock = clockables_.begin(); iclock != clockables_.end(); ++iclock)
	iclock->stop();
    }






  };


#endif
