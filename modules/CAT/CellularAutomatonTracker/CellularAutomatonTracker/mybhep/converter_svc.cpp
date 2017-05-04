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


#include <mybhep/converter_svc.h>
#include <mybhep/error.h>
#include <mybhep/container_algorithm.h>
#include <mybhep/utilities.h>

using namespace std;


namespace mybhep{
  //! destructor 
  converter_svc::~converter_svc()
  {
    clear();
  }
  
  void converter_svc::complete_restore()
  {
    for (size_t i = 0; i < converters_.size(); i++)
      {
	ibconverter& ib = *converters_[i];
	ib.complete_restore();
      }
    
  }
  void converter_svc::clear()
  {
    container_algorithm::destroy(converters_); 
  }

  
  ibconverter& converter_svc::converter(int index)
  {
    int found = 0;
    size_t indx = 0;
    

    for (size_t i = 0; i < converters_.size(); i++)
      {
	ibconverter& ib = *converters_[i];

	if (index == ib.index())
	  {
	    found = 1;
	    indx = i;
	  }      
	
      }

    Assert(found,__FILE__,__LINE__,
	   bad_index(" index not found")); 
	
    return *converters_[indx];
  }

  void converter_svc::add_converter(ibconverter* ib)
  {
    converters_.push_back(ib);
  }
  
}

