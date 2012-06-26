/* -*- mode: c++ -*- */

#ifndef __CATAlgorithm__printable_h
#define __CATAlgorithm__printable_h 1


#include <iostream>
#include <mybhep/utilities.h>

namespace CAT {
  namespace topology{


    //! Base utility class for conditional print
    /*!
      - derived class implement the info method to print information
      \ingroup base
    */

    class printable {

    public:

      /// print level
      mybhep::prlevel level_;

    public:

      printable();

      virtual ~printable();

      //! print interface
      virtual void info(std::ostream& os = std::clog) const ;

      //! print the information using the << operator
      friend std::ostream& operator<<(std::ostream& os, const printable& v) ;

      //! print the information using the << operator
      friend std::ostream& operator<<(std::ostream& os, const printable* v) ;

      //! return print_level
      const mybhep::prlevel& print_level() const ;

      //! set print_level
      //    void set_print_level(prlevel& level) {level_ = level;}
      void set_print_level(mybhep::prlevel level) ;

      /// set the verbosity level
      void set_print_level(const std::string & info);


    };
  }
}

#endif // __CATAlgorithm__printable_h




