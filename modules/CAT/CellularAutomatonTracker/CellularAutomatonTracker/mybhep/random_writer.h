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

#ifndef random_writer_h
#define random_writer_h

#include <mybhep/brw.h>

using namespace std;


namespace mybhep{

//! Base class for a random writer
/*!
  Provides common functionality for random writers
  A random reader can be visualized as a writer accessing
  a disk. Its default action is to write an event labeled with a key
  to a random place in the disk. The event is later recovered by the
  same key.

  Derived class need to implement the protected interface, 
  which specifies low level actions such as open_file, 
  close_file, etc.

  The API consists of virtual functions obtained from the protected interface
  

  open(fileName) : open files and init counters
  close():  close file, clears memory, reset counters
  reopen(fileName) : closes previous file and opens again

  write(event,key): writes event labeled with key "key" to file

*/


  class random_writer : public brw
  {

  protected:
    //! path to root directory
    string path_;
    //! joins pieces of path
    string join_;

  protected:
    
    random_writer();

    //! public functions
  public:

    //! set path
    void set_path(string path);

    //! return path
    string path();

    //! write event
    void write(event& evt, size_t ievent, string key="");

    //! protected interface
  protected:

    //! write the event as a record
    virtual void write_record(string record, string key) = 0;

    //! protected functions
  };   
}
#endif
