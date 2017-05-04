/* -*- mode: c++ -*- */
/*
 *
 * Copyright 2006
 * J.J. Gomez-Cadenas
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef random_reader_h
#define random_reader_h

#include <mybhep/base_reader.h>



namespace mybhep{
//! Base class for a random reader
/*!
  Provides common functionality for random readers
  A random reader can be visualized as a reader accessing
  a disk. Its default action is to read an event labeled by a key.

  The key can be (but does not need to be) an event number.
  The API separates both functions (rather than using overload functions)
  for clarity

  Derived class need to implement the protected interface,
  which specifies low level actions such as open_file,
  close_file, etc.

  The API consists of virtual functions obtained from the protected interface


  open(fileName) : open files and init counters
  close():  close file, clears memory, reset counters
  reopen(fileName) : closes previous file and opens again

  read_event(size_t ievent) : reads ievent in file
  read(std::string key): reads key in file

*/

  class random_reader : public base_reader
  {

  protected:
    //! path to root directory
    std::string path_;
    //! joins pieces of path
    std::string join_;
    //! number of objects in file
    size_t maxEvents_;

    //! constructor (only to be instantiated by derived classes)
  protected:
    random_reader();

    //!  public functions
  public:
    virtual ~random_reader() ;

    //! set path
    void set_path(std::string path);

    //! return path
    std::string path();

    //! max events
    size_t max_events() ;

    //! end of file
    bool eof(size_t ievent);

    //! read event
    virtual event& read_event(size_t ievent);

    //! read event
     event& read(std::string key);

    //! protected interface
  protected:

    //! get the event as a record
    virtual std::string get_record(std::string key) = 0;

    //! get max events
    virtual size_t get_max_events() = 0;

  };
}
#endif
