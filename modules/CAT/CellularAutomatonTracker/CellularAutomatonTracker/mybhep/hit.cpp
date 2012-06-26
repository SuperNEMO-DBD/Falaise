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


#include <mybhep/particle.h>
#include <mybhep/hit.h>
#include <mybhep/container_algorithm.h>
#include <iomanip>
namespace mybhep{

  using namespace std;

  // constructor
  hit::hit(const particle& mother, string detector):
    detector_(detector)
  {
    mother_ = &mother;

  }

  // constructor
  hit::hit(string detector):
    detector_(detector)
  {
    mother_ = NULL;

  }
  // copy constructor
  hit::hit(const hit& tr)
  {
    mother_ = &tr.mother_particle();
    detector_ = tr.detector_;
    x_ = tr.x_;
    prop_ = tr.prop_;

  }
  // asigment
  hit& hit::operator = (const hit& tr)
  {
    mother_ = &tr.mother_particle();
    detector_ = tr.detector_;
    x_ = tr.x_;
    prop_ = tr.prop_;

    return *this;
  }
  // destructor
  hit::~hit()
  {
    // prop_.clear();
  }

  // visual output
  ostream& operator << (ostream& s, const hit& ih) {
    s << endl;

    s  << " hit info: detector = " << ih.detector() << " "
       << endl;

    s << " x (cm)    y (cm)    z (cm)    " << endl;

    s << std::setw(6) << ih.x()[0]/cm <<"     "
      << std::setw(6) << ih.x()[1]/cm <<"     "
      << std::setw(6) << ih.x()[2]/cm <<endl;

    s  << " data info = " << endl;

    typedef map<string, string>::const_iterator I;
    for(I i=ih.data_map().begin(); i !=ih.data_map().end(); ++i)
      {

	s << " data name = " << i->first
	  << " data value = " << i->second
	  <<endl;
      }

    return s;
  }
}
