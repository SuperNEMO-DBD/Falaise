/* -*- mode: c++ -*- */
//! error.h
/** This header file contains a simplified version
 *  of error control
 */
/*
 *
 * Copyright (C) 2004 J.J. Gomez-Cadenas
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

#ifndef __BHERROR__
#define __BHERROR__

#include <iostream>
#include <exception>
#include <stdexcept>
#include <string>

/**
 *\defgroup error Exception handling
 */

//! Global namespace for the library
namespace mybhep {

//! Assert function
/**
 *If the condition "assert" is not true raises exception e
 *\ingroup error
 */
template <class E>
inline void Assert(bool assert, E e) {
  if (!assert) {
    std::cerr << "Assertion failed";
    std::cerr << e.what() << std::endl;
    e.raise();
  }
}
//! Overloaded Assert function
/**
 *If the condition "assert" is not true raises exception e. Prints
 *file and line where error occurrs
 *\ingroup error
 */
template <class E>
inline void Assert(bool assert, const char *file, int line, E e) {
  if (!assert) {
    std::cerr << "Assertion failed in file " << file << " at line " << line << ":" << std::endl;
    std::cerr << e.what() << std::endl;
    e.raise();
  }
}

/** Exceptions **/

//! This exception is raised when an internal logic error occurs
/** For example, one tries to perform an "ilogical" operation
 *such as trying to retrieve a name that is not in a given container
 *\ingroup error
 */
struct internal_logic : public std::logic_error {
  //! Constructor
  /** Takes an std::string that will be passed to std::cerr is the exception
   * is raised
   */
  internal_logic(const std::string &s = "exception: internal logic") : std::logic_error(s) {}
  //! throws this exception or abort (if exceptions disabled)
  virtual void raise() { throw * this; }
};

//! This exception is raised when a bad argument is supplied
/** For example the user calls a given function with an invalid argument
 *\ingroup error
 */
struct bad_argument : public std::invalid_argument {
  //! Constructor
  /** Takes an std::string that will be passed to std::cerr is the exception
   * is raised
   */
  bad_argument(const std::string &s = "exception: bad argument") : std::invalid_argument(s) {}
  //! throws this exception or abort (if exceptions disabled)
  virtual void raise() { throw * this; }
};

//! This exception is raised when a given object has bad size
/** For example one tries to multiply to matrices with non-conformant
 *\ingroup error
 * dimensions */
struct bad_size : public std::domain_error {
  //! Constructor
  /** Takes an std::string that will be passed to std::cerr is the exception
   * is raised
   */
  bad_size(const std::string &s = "exception: bad size") : std::domain_error(s) {}
  //! throws this exception or abort (if exceptions disabled)
  virtual void raise() { throw * this; }
};

//! This exception is raised to signal a bad index
/** For example one tries to access a std::vector outside allowed range
 *\ingroup error
 */
struct bad_index : public std::out_of_range {
  //! Constructor
  /** Takes an std::string that will be passed to std::cerr is the exception
   * is raised
   */
  bad_index(const std::string &s = "exception: bad index") : std::out_of_range(s) {}
  //! throws this exception or abort (if exceptions disabled)
  virtual void raise() { throw * this; }
};

struct bad_key : public std::out_of_range {
  //! Constructor
  /** Takes an std::string that will be passed to std::cerr is the exception
   * is raised
   */
  bad_key(const std::string &s = "exception: bad key") : std::out_of_range(s) {}
  //! throws this exception or abort (if exceptions disabled)
  virtual void raise() { throw * this; }
};

//! This exception is raised to signal a division by zero
/**
 *\ingroup error
 */
struct divide_by_zero : public std::runtime_error {
  /// Constructor
  /** Takes an std::string that will be passed to std::cerr is the exception
   * is raised
   */
  divide_by_zero(const std::string &s = "exception: divide by zero") : std::runtime_error(s) {}
  /// throws this exception or abort (if exceptions disabled)
  virtual void raise() { throw * this; }
};

}  // namespace mybhep

#endif
