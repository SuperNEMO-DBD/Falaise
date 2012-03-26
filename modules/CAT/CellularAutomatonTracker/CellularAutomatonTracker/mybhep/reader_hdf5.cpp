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


#include <mybhep/reader_hdf5.h>

#include <mybhep/error.h>
#include <mybhep/utilities.h>


namespace mybhep{


  //! reader_hdf5 Class
/*!
  Reads a BHEP DST in hdf5 format
*/

#ifdef HDF5  
        
  //! constructor 
  reader_hdf5::reader_hdf5() :
    random_reader()
  {
  }

  reader_hdf5::~reader_hdf5()
  {
    close();
  }

  //! close file
  void reader_hdf5::close_file()
  {
    if (setFile_)
      {
	H5Tclose (type_);
	H5Gclose (gfile_);
	H5Fclose (file_);
	clear_flags();
      }
    clear_event();
  }

  //! open file
  void reader_hdf5::open_file(string fileName)
  {
    const char* fname = fileName.c_str();

    // Open file
    file_ = H5Fopen(fname,H5F_ACC_RDONLY,H5P_DEFAULT);
    gfile_  = H5Gopen(file_, path_.c_str());

  /* Create a datatype to refer to (a string) */
    type_ = H5Tcopy (H5T_C_S1);
    H5Tset_size (type_,H5T_VARIABLE); // string is variable length
  }

  //! read event
  string reader_hdf5::get_record(string key)
  {
    string skey = path_ + join_ + key;

    char* rdata[DIM1];  // string to write
    dataset_ = H5Dopen(gfile_,skey.c_str());

    Assert(dataset_>=0, __FILE__,__LINE__,
	       bad_key("key in random file"+fileName_+
			      " does not exist"));	

    // transfer
    xfer_ = H5Pcreate (H5P_DATASET_XFER);
    //read
    H5Dread (dataset_, type_, H5S_ALL, H5S_ALL, xfer_, rdata);
    //close
    H5Dclose (dataset_);
    H5Pclose (xfer_);

    // get data into a string and convert to event
    string def = rdata[0];
    return def;
  }
    
        
  //! max number of events
  size_t reader_hdf5::get_max_events()
  {
    hsize_t nobj;
    H5Gget_num_objs(gfile_, &nobj);
    return (size_t) nobj;
  }

#endif

}
