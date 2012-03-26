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


#include <mybhep/reader_gz.h>
#include <mybhep/error.h>
#include <mybhep/utilities.h>

namespace mybhep{
        
  //! constructor 
  reader_gz::reader_gz() :
    sequential_reader()
  {
  }
    
  reader_gz::~reader_gz()
  {
    close();
  }

  //! protected interface

  void reader_gz::open_file(string fileName)
  {
    gf_ = gzopen (fileName.c_str(),"rb");
    Assert(gf_!=0, __FILE__,__LINE__,
	   internal_logic("File "+fileName_+" does not exist!"));
  }


  void reader_gz::close_file()
  {
    gzclose (gf_);
  }

  size_t reader_gz::get_max_events()
  {
    size_t emax = 0;
    size_t len;
    rewind();
    while (!end_of_file())
      {
	string record = get_record();
	len = gztell(gf_);
	off_.push_back(len);
	emax++;
      }

    rewind();
    return emax;
  }

  bool reader_gz::end_of_file()
  {
    return gzeof(gf_);
  }

  void reader_gz::rewind()
  {
    gzrewind (gf_);
  }

  void reader_gz::skip(size_t ievent)
  {
    rewind();

    long offset = off_[ievent-1];
    gzseek(gf_,offset,SEEK_SET);

  }

  string reader_gz::get_record()
  {
    char* buff = new char[ZLEN];
    buff = gzgets (gf_, buff, ZLEN);

    if(buff == Z_NULL)
      {
	delete [] buff;
	Assert(false, __FILE__,__LINE__,
	       internal_logic("event in file"+fileName_+
			      " is corrupted: Maybe end-of-file?"));	
      }


    string record = buff;
    delete [] buff;
    return record;
  }

}
