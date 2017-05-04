/* -*- mode: c++ -*- */
//
/*
 *
 * Copyright (C) 2004 J.J. Gomez-Cadenas
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


#ifndef __TRACK_CVT_H__
#define __TRACK_CVT_H__

#include <string>
#include <vector>
#include <mybhep/track.h>
#include <mybhep/ibconverter.h>



namespace mybhep{

  //! class track_cvt
  /*!
    Converter to persistency
  */
  typedef track track;

  class track_cvt : public ibconverter{

  private:
    //! pointer to the track
    track*  ntrack_;
    //! const pointer to the track
    const track* track_;
    //! pointer to mother particle
    int imother_;

  public:
    //! construct from a track and an index
    track_cvt( const track& t, int inx);
    //! construct from a string
    track_cvt( std::string );
    //! destructor
    virtual ~track_cvt();

    //! store class in a string
    virtual std::string store();
    //! restore from a string
    virtual void restore( std::string );
    //! complete restore
    virtual void complete_restore();


    //! retrieve a constant reference to the track
    const track& retrieve() const;
    //! retrieve a reference to the track
    track& reference();

    //! instantiate a new track
    track* create();

  };
}
#endif









