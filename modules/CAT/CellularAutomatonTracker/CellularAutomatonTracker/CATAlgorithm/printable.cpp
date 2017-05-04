/* -*- mode: c++ -*- */

#include <CATAlgorithm/printable.h>

namespace CAT {
  namespace topology{

    using namespace std;
    using namespace mybhep;
    
    printable::printable()
    {
      level_ = VERBOSE;
    }
    
    printable::~printable()
    {
    }
    
    //! print interface 
    void printable::info(std::ostream& os) const 
    {
      os << " no information available" <<endl;
    }

    //! print the information using the << operator
    ostream& operator<<(std::ostream& os, const printable& v) 
    {
      v.info(os);
      return os;
    }
    
    //! print the information using the << operator
    ostream& operator<<(std::ostream& os, const printable* v) 
    {
      v->info(os);
      return os;
    }
    
    //! return print_level
    const prlevel& printable::print_level() const 
    {
      return level_;
    } 
  
    //! set print_level
    //    void set_print_level(prlevel& level) {level_ = level;}   
    void printable::set_print_level(prlevel level) 
    {
      level_ = level;
    }   

    /// set the verbosity level 
    void printable::set_print_level(const std::string & info)
    {
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
 
    
  }
}




