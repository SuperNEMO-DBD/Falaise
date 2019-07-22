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

#include <mybhep/sequential_reader.h>
#include <mybhep/event_cvt.h>
#include <mybhep/error.h>
#include <mybhep/utilities.h>

using namespace std;

namespace mybhep {

//! constructor

sequential_reader::sequential_reader() : base_reader() {
  off_.push_back(0);
  curEvent_ = 0;
  maxEvents_ = 0;
}

sequential_reader::~sequential_reader() { close(); }

//! open reader
void sequential_reader::open(std::string fileName) {
  base_reader::open(fileName);
  maxEvents_ = 0;
  curEvent_ = 0;
}

void sequential_reader::close() {
  base_reader::close();
  curEvent_ = 0;
  maxEvents_ = 0;
  off_.clear();
}

//! set file
void sequential_reader::reopen(std::string fileName) {
  close();
  open(fileName);
}
//! max events
size_t sequential_reader::max_events() {
  if (!maxEvents_) maxEvents_ = get_max_events();
  return maxEvents_;
}

//! sequential reading
event& sequential_reader::read_next() {
  Assert(setFile_ == true, __FILE__, __LINE__,
         internal_logic("reader is not open, should open it first"));

  clear_event();
  curEvent_ += 1;

  std::string record = get_record();
  event_cvt cvt(record);
  event_ = cvt.create();
  imem_++;
  return *event_;
}

//! read event
event& sequential_reader::read_event(size_t ievent) {
  if (!maxEvents_) maxEvents_ = get_max_events();

  Assert(ievent <= maxEvents_, __FILE__, __LINE__,
         internal_logic("required event " + to_string(ievent) +
                        " has number above the max number of events in file " + fileName_));
  skip(ievent);

  curEvent_ = ievent - 1;
  return read_next();
}
}  // namespace mybhep
