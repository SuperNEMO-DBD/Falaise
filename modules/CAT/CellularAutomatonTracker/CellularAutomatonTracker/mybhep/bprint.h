/* -*- mode: c++ -*- */

#ifndef BPRINT
#define BPRINT 
/*   
 *
 * J.J. Gomez-Cadenas, J.A. Hernando      
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */

#include <iostream>
#include <mybhep/utilities.h>

using namespace std;

namespace mybhep{

//! Base utility class for conditional print 
/*!
  - derived class implement the info method to print information
  \ingroup base
*/

  class bprint {  
  public:
    bprint(){level_ = VERBOSE;}
    virtual ~bprint(){}
      
    //! print interface 
    virtual void info(ostream& os = std::clog) const {
      os << " no information available" <<endl;
    }

    //! print the information using the << operator
    friend ostream& operator<<(ostream& os, const bprint& v) {
      v.info(os);
      return os;
    }
      
    //! print the information using the << operator
    friend ostream& operator<<(ostream& os, const bprint* v) {
      v->info(os);
      return os;
    }
      
    //! return print_level
    const prlevel& print_level() const {return level_;} 
  
    //! set print_level
    void set_print_level(prlevel& level) {level_ = level;}   

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
      
  protected:
    /// print level
    prlevel level_;
    
  };
}

#endif




