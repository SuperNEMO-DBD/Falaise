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
    virtual void info(std::ostream& os = std::clog) const {
      os << " no information available" <<std::endl;
    }

    //! print the information using the << operator
    friend std::ostream& operator<<(std::ostream& os, const bprint& v) {
      v.info(os);
      return os;
    }

    //! print the information using the << operator
    friend std::ostream& operator<<(std::ostream& os, const bprint* v) {
      v->info(os);
      return os;
    }

    //! return print_level
    const prlevel& print_level() const {return level_;}

    //! set print_level
    void set_print_level(prlevel& level) {level_ = level;}

    /// set the verbosity level
    void set_print_level(std::string info_repr){
      if(info_repr == "MUTE")  level_ = MUTE;
      else if(info_repr == "QUIET")  level_ = CONCISE;
      else if(info_repr == "NORMAL")  level_ = NORMAL;
      else if(info_repr == "WARNING")  level_ = WARNING;
      else if(info_repr == "DETAILED")  level_ = DETAILED;
      else if(info_repr == "VERBOSE")  level_ = VERBOSE;
      else if(info_repr == "VVERBOSE")  level_ = VVERBOSE;
      else if(info_repr == "DUMP")  level_ = DUMP;
      else
        level_ = NORMAL;
    }

  protected:
    /// print level
    prlevel level_;

  };
}

#endif
