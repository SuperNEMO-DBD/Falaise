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


#include <mybhep/brw.h>
#include <mybhep/error.h>
#include <mybhep/utilities.h>

using namespace std;


namespace mybhep{

  //! constructor
    
  brw::brw()
  {
    clear_flags();
  }

  //! destructor
  brw::~brw()
  {
    close();
  }
 
  //! open reader
  void brw::open(string fileName)
  {
    init_flags(fileName);
    open_file(fileName);
  }

  //! close file
  void brw::close()
  {
    if (setFile_)
      {
	close_file();
	clear_flags();
      }
  }

  //! set file
  void brw::reopen(string fileName)
  {
    close();
    open(fileName);
  }

  void brw::clear_flags()
  {
    setFile_ = false;
  }

  void brw::init_flags(string fileName)
  {
    setFile_ = true;
    fileName_ = fileName;
  }
}

