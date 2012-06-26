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

#ifndef brw_h
#define brw_h

#include <string>
#include <vector>
#include <mybhep/event.h>


namespace mybhep{
//! Base class for a reader/writer
/*!
  Provides common functionality for readers and writers
*/

  class brw
  {

  protected:
    //! true if a file has been set to reader
    bool setFile_;
    //! file name set to reader
    std::string fileName_;
    //! pointer to event
    event* event_;

    //! constructor (only to be instantiated by derived classes)
  protected:
    brw();

    //! virtual destructor
  public:
    virtual ~brw();

    //!  public functions
  public:

    //! open reader
    void open(std::string fileName);

    //! close reader
    void close();

    //! set reader to a new file
    void reopen(std::string fileName);

    //! protected interface
  protected:

    //! open file for reading
    virtual void open_file(std::string fileName) = 0;

    //! close file
    virtual void close_file() = 0;


    //! protected functions
  protected:

    //! clear internal flags
    void clear_flags();
    //! init internal flags
    void init_flags(std::string fileName);

  };
}
#endif
