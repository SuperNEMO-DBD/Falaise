/* -*- mode: c++ -*- */
 
/* 
 *
 * J.J. Gomez-Cadenas, J.A. Hernando, A. Cervera
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

#ifndef _ISTR_
#define _ISTR_
#include <vector>
#include <mybhep/tstore.h>
#include <mybhep/dvector.h>

namespace mybhep{
    //! a store of integers
    typedef tstore<int> istore;
    //! a store of doubles
    typedef tstore<double> dstore;
    //! a store of strings
    typedef tstore<std::string> sstore;
    //! a store of histogram definitions
    typedef dvector<double> vdouble;
    typedef dvector<int> vint;
    //! a store of vectors of string
    typedef dvector<string> vstring;
    //! a store of vectors of double
    typedef tstore<vdouble> vstore;
    //! a store of vectors of int
    typedef tstore<vint> ivstore;
    //! a store of vectors of string
    typedef tstore<vstring> svstore;
}
#endif  
