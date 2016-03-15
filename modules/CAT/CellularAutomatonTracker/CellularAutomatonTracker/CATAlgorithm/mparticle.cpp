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

// Implementación of class mparticle

#include <mybhep/error.h>
#include <mybhep/mparticle.h>
#include <mybhep/particle.h>
#include <mybhep/utilities.h>

namespace mybhep
{
  using namespace std;

  //! constructor
  mparticle::mparticle (particle& p, std::string view,
                        size_t xz, size_t yz)
  {
    mirror_ = &p;
    view_ = view;
    xz_ = xz;
    yz_ = yz;
    q_ = small_neg;

  }
  mparticle::mparticle ()

  {
    mirror_ = NULL;
    view_ = "NULL";
    xz_ = (int) small_neg;
    yz_ = (int) small_neg;
    q_ = small_neg;

  }

  ostream& operator << (ostream& out, const mparticle& ip) {
    out << std::endl;

    if (&ip.mirror() != NULL)
      {
        out << " mirror of particle is " << ip.mirror().name() << std::endl;
        out << " with 3  momentum (MeV) =" << ip.mirror().p3()/MeV << std::endl;
        out << " and energy (MeV)= " << ip.mirror().e()/MeV << std::endl;
        out << " view = " << ip.view()
            << " matches xz = " << ip.xz()
            << " matches yz = " << ip.yz()
            << " quality = " << ip.quality()
            << std::endl;
      }

    out << " List of properties "
        << "-----------------------------" << std::endl;

    typedef map<string, string>::const_iterator I;
    for(I i=ip.properties_map().begin(); i !=ip.properties_map().end(); ++i)
      {
        out << " property name = " << i->first << "   "
            << " property value = " << i->second
            << std::endl;
      }
    return out;
  }
}
