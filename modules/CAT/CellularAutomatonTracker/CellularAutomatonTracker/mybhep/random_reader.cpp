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

#include <mybhep/random_reader.h>
#include <mybhep/event_cvt.h>
#include <mybhep/error.h>
#include <mybhep/utilities.h>

using namespace std;

namespace mybhep {

//! constructor

random_reader::random_reader() : base_reader() {
  path_ = "/DST";
  join_ = "/";

  // number of objects in file
  maxEvents_ = 0;
}

random_reader::~random_reader() { close(); }

//! set path
void random_reader::set_path(std::string path) { path_ = path; }

//! return path
std::string random_reader::path() { return path_; }

//! number of objects in file
size_t random_reader::max_events() {
  if (!maxEvents_) maxEvents_ = get_max_events();
  return maxEvents_;
}

bool random_reader::eof(size_t ievent) {
  if (!maxEvents_) maxEvents_ = get_max_events();
  if (ievent > maxEvents_) return true;

  return false;
}

//! random reading
event& random_reader::read(std::string key) {
  Assert(setFile_ == true, __FILE__, __LINE__, bad_key("reader is not open, should open it first"));

  clear_event();

  std::string record = get_record(key);
  event_cvt cvt(record);
  event_ = cvt.create();
  imem_++;
  return *event_;
}

//! read event
event& random_reader::read_event(size_t ievent) { return read(to_string(ievent)); }
}  // namespace mybhep
