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

#ifndef converter_svc_h
#define converter_svc_h

#include <string>
#include <vector>
#include <mybhep/ibconverter.h>

using namespace std;


namespace mybhep{
//! converter_svc Class
/*!
  Converter to persistency
*/

  class converter_svc 
  {

  private:
    //! vector of converters
    vector<ibconverter*> converters_;
  public:
    
    //! constructor 
    converter_svc()
    {
    }
    //! destructor 
    virtual ~converter_svc();
    
    //! set converter
    void add_converter(ibconverter* ib);

    ibconverter& converter(int index);

    void complete_restore();
    void clear();
  };   
}
#endif
