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

#ifndef writer_hdf5_h
#define writer_hdf5_h

#include <mybhep/random_writer.h>


#ifdef HDF5
#include <mybhep/hdf5.h>      // C++ API header file
#endif


#define CHNK_DIM 1000

namespace mybhep{

#ifndef HDF5

  class writer_hdf5 : public random_writer
  {
  public:
    void open_file(std::string s){out();}
    void close_file(){out();}
    void write_record(std::string s1,std::string s2){out();}
    void out(){
      std::cerr<<"+++ BHEP compiled without HDF5 support. ABORT !!!"<<std::endl;
      exit(1);
    }
  };

#else


//! writer_hdf5 Class
/*!
  Writes a BHEP DST in hdf5 format
*/


  class writer_hdf5 : public random_writer
  {

  protected:
    const char* wdata_[1];  // std::string to write

    hid_t       file_,gfile_;  // file and gzipped file handler
    hid_t       dataset_;      // dataset handler
    hid_t       space_;         // dataspace handler
    hid_t       type_;         // datatype handler
    hid_t       cparms_,xfer_; // parameters for plists
    hid_t       dst_;          // name of group

  public:

    //! constructor
    writer_hdf5();
    virtual ~writer_hdf5();
    //! set file
    virtual void open_file(std::string fileName);
    //! close file
    virtual void close_file() ;
    //! write record
    virtual void write_record(std::string record, std::string key) ;

  };

#endif
}
#endif
