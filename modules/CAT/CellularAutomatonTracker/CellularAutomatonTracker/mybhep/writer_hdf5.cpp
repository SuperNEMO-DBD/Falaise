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

#include <time.h>
#include <mybhep/writer_hdf5.h>

namespace mybhep{

#ifdef HDF5
        
  //! constructor 
  writer_hdf5::writer_hdf5() :
    random_writer()
  {
  }
  //! destructor
  writer_hdf5::~writer_hdf5()
  {
    close();
  }

  //! set file
  void writer_hdf5::open_file(string fileName)
  {

    hsize_t       dims1[] = {1};
    hsize_t	  maxdims1[1] = {H5S_UNLIMITED};
    hsize_t	  chkdims1[1] = {CHNK_DIM};

    const char* fname = fileName.c_str();
    
    file_ = H5Fcreate (fname, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    dst_  = H5Gcreate(file_, path_.c_str(), 0);

    /* Create dataspace for datasets (a vector of unlimited dimensions)*/
    space_ = H5Screate_simple (1, dims1, maxdims1);

  /* Create a datatype to refer to (a string) */
    type_ = H5Tcopy (H5T_C_S1);
    H5Tset_size (type_,H5T_VARIABLE); // string is variable length

    // write data gzipped. This requires chunking
    cparms_ = H5Pcreate (H5P_DATASET_CREATE);  
    H5Pset_chunk ( cparms_, 1, chkdims1);
    H5Pset_deflate( cparms_, 6); 
  }


  //! close file
  void writer_hdf5::close_file()
  {
    H5Pclose (cparms_);
    H5Tclose (type_);
    H5Sclose (space_);
    H5Gclose(dst_);
    H5Fclose (file_);
  }
 
  //! write event
  void writer_hdf5::write_record(string record, string key)
  {
    wdata_[0] = record.c_str();

    /* Create a dataset */
    string skey = path_ + join_ + key;

    // write data gzipped. This requires chunking

    dataset_ = H5Dcreate (dst_, skey.c_str(), type_, space_,cparms_);

    /* Write dataset to disk */
     H5Dwrite (dataset_, type_, H5S_ALL, H5S_ALL, H5P_DEFAULT, wdata_);

     /* Close Dataset */
     H5Dclose (dataset_);

  }

#endif

}
