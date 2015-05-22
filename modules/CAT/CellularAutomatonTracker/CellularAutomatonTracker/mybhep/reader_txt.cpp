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


#include <mybhep/reader_txt.h>
#include <mybhep/error.h>
#include <mybhep/utilities.h>

namespace mybhep{

  using namespace std;

  //! constructor
  reader_txt::reader_txt() :
    sequential_reader()
  {
  }
  reader_txt::~reader_txt()
  {
    close();
  }

  //! protected interface

  void reader_txt::open_file(std::string fileName)
  {
    is_.open(fileName.c_str(), ios::in);
    Assert(is_.is_open(), __FILE__,__LINE__,
	   internal_logic("File "+fileName_+" does not exist!"));
  }


  void reader_txt::close_file()
  {
    is_.close();
  }

  size_t reader_txt::get_max_events()
  {
    size_t emax = 0;
    size_t len;
    rewind();
    while (!end_of_file())
      {
	std::string record = get_record();
	len = is_.tellg();
	off_.push_back(len);
	emax++;
      }

    rewind();
    return emax;
  }

  bool reader_txt::end_of_file()
  {
    bool eof=false;
    int peek = is_.peek();
    if (peek < 0) eof=true;
    return eof;
  }

  void reader_txt::rewind()
  {
    is_.clear();

    is_.seekg(0,ios::beg);
  }

  void reader_txt::skip(size_t ievent)
  {
    rewind();

    long offset = off_[ievent-1];
    is_.seekg(offset,ios::beg);

  }

  std::string reader_txt::get_record()
  {
    std::string record;
    // bool test;
    // test = getline(is_, record);
    Assert(!getline(is_, record), __FILE__,__LINE__,
	   internal_logic("event in file"+fileName_+
			  " is corrupted: Probably gone over end-of-file"));
    return record;
  }

}
