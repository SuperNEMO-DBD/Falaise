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

#ifndef writer_txt_h
#define writer_txt_h

#include <mybhep/sequential_writer.h>
#include <fstream>

namespace mybhep {
//! writer_txt Class
/*!
  Reads a BHEP DST in txt format
*/

class writer_txt : public sequential_writer {
 protected:
  std::ofstream os_;

 public:
  //! constructor
  writer_txt();
  virtual ~writer_txt();
  //! protected interface
 protected:
  //! open file
  virtual void open_file(std::string fileName);
  //! close file
  virtual void close_file();
  //! write the event as a record
  virtual void write_record(std::string record);
};
}  // namespace mybhep
#endif
