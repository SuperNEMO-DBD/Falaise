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

#include <mybhep/writer_gz.h>

namespace mybhep {

using namespace std;

//! constructor
writer_gz::writer_gz() : sequential_writer() {}
//! destructor
writer_gz::~writer_gz() { close(); }

void writer_gz::open_file(std::string fileName) { gf_ = gzopen(fileName.c_str(), "wb"); }

void writer_gz::close_file() { gzclose(gf_); }

//! write the event as a record
void writer_gz::write_record(std::string record) {
  record += "\n";
  gzputs(gf_, record.c_str());
}

void writer_gz::fede() { return; }

}  // namespace mybhep
