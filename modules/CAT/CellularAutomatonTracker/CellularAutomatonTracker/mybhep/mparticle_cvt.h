/* -*- mode: c++ -*- */
// 
/*   
 * 
 * Copyright (C) 2004 J.J. Gomez Cadenas
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
 */


#ifndef __MPARTICLE_CVT_H__
#define __MPARTICLE_CVT_H__

#include <string>
#include <map>
#include <vector>
#include <mybhep/mparticle.h>
#include <mybhep/ibconverter.h>


using namespace std;  

namespace mybhep{

  //! class mparticle_cvt
  /*!
    Converter to persistency
  */
  
  typedef mparticle mparticle;

  class mparticle_cvt : public ibconverter
  {

  private:
    //! pointer to the mparticle
    mparticle*   nparticle_;

    //! const pointer to the mparticle
    const mparticle*   particle_;

    //! integer for mirror mparticle
    int imirror_;

  public:
    //! construct from a mparticle and an index
    mparticle_cvt( const mparticle& p, int inx);
    //! construct from a string
    mparticle_cvt( string );
    //! destructor
    virtual ~mparticle_cvt();

    //! store class in a string
    virtual string store();
    //! restore from a string
    virtual void restore( string );
    //! complete restore
    virtual void complete_restore();

    //! retrieve a constant reference to the mparticle
    const mparticle& retrieve() const;

    //! retrieve a constant reference to the mparticle
    mparticle& reference() ;

    //! instantiate a new mparticle
    mparticle* create();
    
  
  };
}
#endif









