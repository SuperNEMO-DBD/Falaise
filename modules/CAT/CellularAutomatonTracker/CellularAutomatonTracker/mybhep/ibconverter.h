/* -*- mode: c++ -*- */
#ifndef ibconverter_H
#define ibconverter_H

/* 
 * Copyright (C) 2004 J.A. Hernando J.J. Gomez-Cadenas
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 * namespace Centella{
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 */
#include <mybhep/iconverter.h>
#include <string>

using namespace std;

namespace mybhep{
  //! Base class  for converters
  /*! 
    It propagates and expand the interface for a converter
  */

  class ibconverter : public Iconverter {

  public:
  
    //! retrieve the index of the object to be converter
    int  index() const		{ return index_; }
    //! set the index of the object to be converter
    void set_index( int index )	{ index_ = index; }

    //! retrieve the name of the object to be converter
    string  name() const		{ return name_; }
    //! set the name of the object to be converter
    void set_name( string name )	{ name_ = name; }

    //! retrieve the state of the object 
    bool  owner() const		{ return owner_; }
    //! set the state of the object 
    void set_owner( bool owner )	{ owner_ = owner; }

    //! propagate interface (store)
    virtual string store() = 0;

    virtual ~ibconverter(){
    }


    //! propagate interface (restore)
    virtual void restore(string) = 0;
    //! propagate interface (restore)
    virtual void complete_restore() = 0;

    void strip(string& s, size_t size)
    {
      s.erase(0,size);
    }


  private :

    string name_;
    int index_;
    bool owner_;

  private :
    //! no copy constructor
    ibconverter(const ibconverter& ib);
    //! no asignment
    ibconverter& operator = (const ibconverter& ib) ;

  protected:
    //! only derived classes have access to constructor
    ibconverter()
    { 
      index_ = -1;
    }

  };
}
#endif
