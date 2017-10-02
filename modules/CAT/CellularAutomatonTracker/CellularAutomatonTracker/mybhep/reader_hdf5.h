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

#ifndef reader_hdf5_h
#define reader_hdf5_h

#include <mybhep/random_reader.h>

#ifdef HDF5
#include <mybhep/hdf5.h>
#endif

#define DIM1 1
#define RANK 1

namespace mybhep {

#ifndef HDF5

class reader_hdf5 : public random_reader {
 public:
  void open_file(std::string /*s*/) { out(); }
  void close_file() { out(); }
  std::string get_record(std::string /*s1*/) {
    out();
    return "dummy";
  }
  size_t get_max_events() {
    out();
    return 0;
  }
  void out() {
    std::cerr << "+++ BHEP compiled without HDF5 support. ABORT!!!" << std::endl;
    exit(1);
  }
};

#else

class reader_hdf5 : public random_reader {
 protected:
  hid_t file_, gfile_;   // file and gzipped file handler
  hid_t dataset_;        // dataset handler
  hid_t space_;          // dataspace handler
  hid_t type_;           // datatype handler
  hid_t cparms_, xfer_;  // parameters for plists

  std::string dclass_;  // data class

 public:
  //! constructor
  reader_hdf5();
  virtual ~reader_hdf5();

  ////+++ DO the following functions to be public???

  //! open file
  virtual void open_file(std::string fileName);
  //! close file
  virtual void close_file();

 protected:
  //! get the event as a record
  virtual std::string get_record(std::string key);

  // get number of objects in file
  virtual size_t get_max_events();
};
#endif
}
#endif
