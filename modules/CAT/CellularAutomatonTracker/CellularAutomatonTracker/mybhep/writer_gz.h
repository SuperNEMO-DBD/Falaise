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

#ifndef writer_gz_h
#define writer_gz_h

#include <mybhep/sequential_writer.h>
#include <zlib.h>

using namespace std;


namespace mybhep{
//! writer_gz Class
/*!
  Reads a BHEP DST in gz format
*/


  class writer_gz : public sequential_writer
  {

  protected:
    gzFile gf_;

  public:    

    //! constructor 
    writer_gz();
    virtual ~writer_gz();

    //! protected interface
  protected:
    //! open file
    virtual void open_file(string fileName);
    //! close file
    virtual void close_file() ;
    //! write the event as a record
    virtual void write_record(string record) ;

  public:
    void fede();


  };   
}
#endif
