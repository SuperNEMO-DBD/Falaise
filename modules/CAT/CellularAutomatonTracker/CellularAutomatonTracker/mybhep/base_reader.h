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

#ifndef base_reader_h
#define base_reader_h

#include <mybhep/brw.h>

using namespace std;

namespace mybhep{
//! Base class for a reader
/*!
  Provides common functionality for readers either sequential 
  or random
*/

  class base_reader : public brw
  {

  protected:
    //! memory counter
    size_t imem_;

    //! constructor (only to be instantiated by derived classes)
  protected:
    base_reader();

    //! virtual destructor
  public:
    virtual ~base_reader();
    
    /*!  Interface */
    //! read event
    virtual event& read_event(size_t ievent) = 0;

    //!  public functions
  public:
    
    //! close reader
    void close();
    
    //! set reader to a new file
    void reopen(string fileName);

    //! protected functions
  protected:

    //! clear event
    void clear_event();
    
  };   
}
#endif
