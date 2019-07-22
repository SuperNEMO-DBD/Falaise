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

#include <mybhep/base_reader.h>
#include <mybhep/error.h>
#include <mybhep/utilities.h>

using namespace std;

namespace mybhep {

//! constructor

base_reader::base_reader() : brw() { imem_ = 0; }

//! destructor
base_reader::~base_reader() {
  close();
  Assert(imem_ == 0, __FILE__, __LINE__,
         internal_logic("Memory check yields" + to_string(imem_) + " This implies a memory leak"));
}

//! close file
void base_reader::close() {
  brw::close();
  clear_event();
}

//! set file
void base_reader::reopen(std::string fileName) {
  close();
  open(fileName);
}

//! clear event
void base_reader::clear_event() {
  if (imem_ > 0) {
    if (imem_ > 1) {
      std::cerr << "!!Warning possible memory leak, imem = " << imem_ << endl;
    }
    imem_--;
    delete event_;
  }
}
}  // namespace mybhep
