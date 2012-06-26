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


#ifndef __IMPARTICLE_H__
#define __IMPARTICLE_H__

#include <string>
#include <vector>
#include <iomanip>
#include <mybhep/bproperties.h>



namespace mybhep{
  class particle;
  //! class mparticle
  /*!
    represents a "mirror particle", that is a particle and the
    quantities that describe it mirror properties. This class is used to
    describe matching between TRUTH and DIGI particles.
    Tipically, one matches TRUTH and DIGI particles counting the number
    of common "hits" per view, thus those quantities are explicitely
    represented in the class. Also one describes explicitely a
    generalized matching quantity, which can be used to describe the
    quality of the TRUTH-DIGI match. Eventually one may want to use other
    quantities, which are described via a store.
  */

  class mparticle : public bproperties {

    //! The  mirror particle
    particle* mirror_;
    //! matching view (0 = XZ, 1=YZ, 2 = XYZ)
    std::string view_ ;
    //! number of matching hits in view XZ
    int xz_;
    //! number of matching hits in view YZ
    int yz_;
    //! matching quality
    double q_;

  private:

    //! do not allow  copy constructor
    mparticle(const mparticle& part);
    //! do not allow asigment
    mparticle& operator = (const mparticle &);

  public:
    //! default constructor
    mparticle ();
    //! construct from a particle (seed particle)
    mparticle (particle& p, std::string view="NULL", size_t xz=0, size_t yz=0);
    //! destructor
    ~mparticle() {};

  public:
    //! set mirror
    void set_mirror(particle& p)
    {
      mirror_ = &p;
    }
    //! get mirror
    const particle&  mirror() const {return *mirror_ ;}
    particle&  mirror()  {return *mirror_ ;}

    //! set view
    void set_view(std::string view) {view_ = view;}
    //! get view
    std::string view() const {return view_ ;}
    //! set number of matches in xz
    void set_xz(size_t xz) {xz_ = xz;}
    //! get number of matches in xz
    size_t xz() const {return xz_ ;}
    //! set number of matches in yz
    void set_yz(size_t yz) {yz_ = yz;}
    //! get number of matches in yz
    size_t yz() const {return yz_ ;}
    //! set quality
    void set_quality(double q) {q_ = q;}
    //! get quality
    double quality() const {return q_ ;}


  };
  std::ostream& operator << (std::ostream& s, const mparticle& ip);
}

#endif
