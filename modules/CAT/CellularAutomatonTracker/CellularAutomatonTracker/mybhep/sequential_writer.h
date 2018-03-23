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

#ifndef sequential_writer_h
#define sequential_writer_h

#include <string>
#include <mybhep/brw.h>

namespace mybhep {

//! Base class for a sequential writer
/*!
  Provides common functionality for sequential writers
  A sequential reader can be visualized as a writer accessing
  a tape. Its default action is to write the next event to the tape.

  Derived class need to implement the protected interface,
  which specifies low level actions such as open_file,
  close_file, etc.

  The API consists of  functions obtained from the protected interface


  open(fileName) : open files and init counters
  close():  close file, clears memory, reset counters
  reopen(fileName) : closes previous file and opens again

  write(event,ievent): writes event "ievent" to file

*/

class sequential_writer : public brw {
 protected:
  sequential_writer();

 public:
  //! write event
  void write(event& evt, size_t ievent);

  //! protected interface
 protected:
  //! write the event as a record
  virtual void write_record(std::string record) = 0;
};
}  // namespace mybhep
#endif
