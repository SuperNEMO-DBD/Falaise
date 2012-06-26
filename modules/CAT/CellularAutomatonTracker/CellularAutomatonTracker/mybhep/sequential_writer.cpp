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

#include <mybhep/sequential_writer.h>
#include <mybhep/event_cvt.h>
#include <mybhep/error.h>

using namespace std;


namespace mybhep{
    
  sequential_writer::sequential_writer() :
    brw()
  {
  }

  //! write event
  void sequential_writer::write(event& evt, size_t ievent)
  {
    Assert(setFile_ ==true, __FILE__,__LINE__,
	   internal_logic("writer is not open, should open it first"));
    
    event_cvt cvt(evt,ievent);
    std::string record = cvt.store();
    write_record(record);
  }
  

}
