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

#ifndef reader_txt_h
#define reader_txt_h

#include <mybhep/sequential_reader.h>
#include <fstream>

using namespace std;


namespace mybhep{
//! reader_txt Class
/*!
  This is a sequentialr reader, able to 
  read a mybhep DST in txt format
*/

  class reader_txt : public sequential_reader
  {


  protected:
    ifstream is_;

  public:    
    //! constructor 
    reader_txt();
    virtual ~reader_txt();
    //! protected interface
  protected:

    //! open file
    virtual void open_file(string fileName);

    //! close file
    virtual void close_file() ;

    //! max number of events
    virtual size_t get_max_events();

    //! end of file  
    virtual bool end_of_file();     

    //! rewind to beginning
    virtual void rewind();

    //! skip to ievent
    virtual void skip(size_t ievent) ;

    //! get the event as a record
    virtual string get_record();

  };   
}
#endif
