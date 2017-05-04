/* -*- mode: c++ -*- */
//
/*
 *
 * Copyright (C) 2003 Irene team
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


#ifndef __HIT_CVT_H__
#define __HIT_CVT_H__

#include <string>
#include <vector>
#include <mybhep/hit.h>
#include <mybhep/ibconverter.h>


namespace mybhep{

  //! class hit_cvt
  /*!
    Converter to persistency
  */
  typedef hit hit;
  class hit_cvt : public ibconverter{

  private:
    //! pointer to the hit
    hit*  nhit_;
    //! const pointer to the hit
    const hit* hit_;
    //! pointer to mother particle
    int imother_;

  public:
    //! construct from a hit and an index
    hit_cvt( const hit& t, int inx);
    //! construct from a string
    hit_cvt( std::string );
    //! destructor
    virtual ~hit_cvt();

    //! store class in a string
    virtual std::string store();
    //! restore from a string
    virtual void restore( std::string );
    //! complete restore
    virtual void complete_restore();


    //! retrieve a constant reference to the hit
    const hit& retrieve() const;
    //! retrieve a reference to the hit
    hit& reference();

    //! instantiate a new hit
    hit* create();

  };
}
#endif









