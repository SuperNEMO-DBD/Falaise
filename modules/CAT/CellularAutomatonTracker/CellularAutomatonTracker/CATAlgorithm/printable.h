/* -*- mode: c++ -*- */

#ifndef __CATAlgorithm__PRINTABLE
#define __CATAlgorithm__PRINTABLE


#include <iostream>
#include <mybhep/utilities.h>

namespace CAT {
namespace topology{

using namespace std;
using namespace mybhep;

//! Base utility class for conditional print 
/*!
  - derived class implement the info method to print information
  \ingroup base
*/

  class printable {  
  public:
    printable(){level_ = VERBOSE;}
    virtual ~printable(){}
      
    //! print interface 
    virtual void info(ostream& os = std::clog) const {
      os << " no information available" <<endl;
    }

    //! print the information using the << operator
    friend ostream& operator<<(ostream& os, const printable& v) {
      v.info(os);
      return os;
    }
      
    //! print the information using the << operator
    friend ostream& operator<<(ostream& os, const printable* v) {
      v->info(os);
      return os;
    }
      
    //! return print_level
    const prlevel& print_level() const {return level_;} 
  
    //! set print_level
    //    void set_print_level(prlevel& level) {level_ = level;}   
    void set_print_level(prlevel level) {level_ = level;}   

    /// set the verbosity level 
    void set_print_level(string info){
      if(info == "MUTE")  level_ = MUTE;
      else if(info == "QUIET")  level_ = CONCISE;
      else if(info == "NORMAL")  level_ = NORMAL;
      else if(info == "WARNING")  level_ = WARNING;
      else if(info == "DETAILED")  level_ = DETAILED;
      else if(info == "VERBOSE")  level_ = VERBOSE;
      else if(info == "VVERBOSE")  level_ = VVERBOSE;
      else if(info == "DUMP")  level_ = DUMP;
      else
        level_ = NORMAL;
    }
      

    /// print level
    prlevel level_;

  protected:

    
  };
}
}

#endif




