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

#include <mybhep/random_writer.h>
#include <mybhep/event_cvt.h>
#include <mybhep/utilities.h>

using namespace std;


namespace mybhep{
    
  random_writer::random_writer() :
    brw()
  {
    path_ = "/DST";
    join_ = "/";
  }

  //! set path
  void random_writer::set_path(std::string path)
  {
    path_ = path;
  }
  
  //! return path
  std::string random_writer::path()
  {
    return path_;
  }

  //! write event
  void random_writer::write(event& evt, size_t ievent, std::string key)
  {
    event_cvt cvt(evt,ievent);
    std::string record = cvt.store();
    if (key =="")
      key = to_string(ievent);
    write_record(record,key);
  }
}
