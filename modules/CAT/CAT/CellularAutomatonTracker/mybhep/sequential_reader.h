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

#ifndef sequential_reader_h
#define sequential_reader_h

#include <mybhep/base_reader.h>

namespace mybhep {
//! Base class for a sequential reader
/*!
  Provides common functionality for sequential readers
  A sequential reader can be visualized as a reader accessing
  a tape. Its default action is to read the next event in the tape.

  Other actions that the sequential reader can take are:
  skip to event "ievent" and rewind to the beginning of the file

  The combination of this two action allows the reader to read
  any event, by either skipping to its index position. For
  a sequential reader, the operations involving skipping and rewinding
  are not efficient

  Derived class need to implement the protected interface,
  which specifies low level actions such as open_file,
  close_file, skip, rewind, etc.

  The API consists of  functions obtained from the protected interface


  open(fileName) : open files and init counters
  close():  close file, clears memory, reset counters
  reopen(fileName) : closes previous file and opens again

  read_next() : reads next event in file
  read(ievent): reads event "ievent" in file

*/

class sequential_reader : public base_reader {
 protected:
  //! points to the current event
  size_t curEvent_;
  //! max number of events set to reader
  size_t maxEvents_;
  //! offset from the beginning of the event
  std::vector<size_t> off_;

  //! constructor (only to be instantiated by derived classes)
 protected:
  sequential_reader();

  //!  public functions
 public:
  virtual ~sequential_reader();
  //! open reader
  void open(std::string fileName);

  //! close reader
  void close();

  //! set reader to a new file
  void reopen(std::string fileName);

  //! max events
  size_t max_events();

  //! end of file
  bool eof() { return end_of_file(); }

  //! read next event
  event& read_next();

  //! read event
  virtual event& read_event(size_t ievent);

  //! protected interface
 protected:
  //! get max events
  virtual size_t get_max_events() = 0;

  //! end of file
  virtual bool end_of_file() = 0;

  //! rewind to ievent
  virtual void rewind() = 0;

  //! skip to ievent
  virtual void skip(size_t ievent) = 0;

  //! get the event as a record
  virtual std::string get_record() = 0;
};
}  // namespace mybhep
#endif
