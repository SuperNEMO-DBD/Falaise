/* -*- mode: c++ -*- */
/*
 *
 * Copyright 2004
 * J.J. Gomez-Cadenas, J.A. Hernando
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

#ifndef mybhep_svc_h
#define mybhep_svc_h

#include <string>
#include <mybhep/event.h>
#include <mybhep/event_cvt.h>
#include <mybhep/converter_svc.h>

#include <mybhep/reader_txt.h>
#include <mybhep/reader_gz.h>

//#ifdef HDF5
#include <mybhep/reader_hdf5.h>
//#endif

#include <mybhep/writer_txt.h>
#include <mybhep/writer_gz.h>

//#ifdef HDF5
#include <mybhep/writer_hdf5.h>
//#endif

#include <mybhep/random_writer.h>

namespace mybhep {
//! mybhep_svc Class
/*!
This is a singleton, the genious of the lamp which can be
invoked by muttering mybhep_svc::instance() anywhere in the program.
 - It contains a pointer to the transient event class
\ingroup mybhep
*/

class mybhep_svc {
 public:
  //! constructor
  mybhep_svc();
  //! destructor

  ~mybhep_svc();

  //! returns the reference to itself (the singleton)
  static mybhep_svc& instance() { return *m_instance; }

  event& get_event() { return event_; }

  converter_svc& converter_service() { return *csvc_; }

  reader_txt& get_reader_txt() { return *rt_; }

  reader_gz& get_reader_gz() { return *rz_; }

  //#ifdef HDF5
  reader_hdf5& get_reader_hdf5() { return *rh_; }
  //#endif
  writer_hdf5& get_writer_hdf5() { return *wh_; }
  //#endif

  writer_txt& get_writer_txt() { return *wt_; }

  writer_gz& get_writer_gz() { return *wz_; }

  void clear_event();

 private:
  //! singleton, pointer to itself
  static mybhep_svc* m_instance;

  /// event
  event event_;
  /// event converter
  converter_svc* csvc_;
  /// txt reader
  reader_txt* rt_;
  /// gz reader
  reader_gz* rz_;
  /// hdf5 reader
  //#ifdef HDF5
  reader_hdf5* rh_;
  //#endif
  /// txt writer
  writer_txt* wt_;
  /// gz writer
  writer_gz* wz_;
  /// hdf5 writer
  //#ifdef HDF5
  writer_hdf5* wh_;
  //#endif
};
}  // namespace mybhep
#endif
