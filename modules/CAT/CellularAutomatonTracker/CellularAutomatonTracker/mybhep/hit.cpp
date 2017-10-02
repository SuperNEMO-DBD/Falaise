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
namespace mybhep {

// constructor
hit::hit(const particle& mother, std::string detector) : detector_(detector) { mother_ = &mother; }

// constructor
hit::hit(std::string detector) : detector_(detector) { mother_ = NULL; }
// copy constructor
hit::hit(const hit& tr) : bproperties() {
  mother_ = &tr.mother_particle();
  detector_ = tr.detector_;
  x_ = tr.x_;
  prop_ = tr.prop_;
}
// asigment
hit& hit::operator=(const hit& tr) {
  mother_ = &tr.mother_particle();
  detector_ = tr.detector_;
  x_ = tr.x_;
  prop_ = tr.prop_;

  return *this;
}
// destructor
hit::~hit() {
  // prop_.clear();
}

// visual output
std::ostream& operator<<(std::ostream& out, const hit& ih) {
  out << std::endl;

  out << " hit info: detector = " << ih.detector() << " " << std::endl;

  out << " x (cm)    y (cm)    z (cm)    " << std::endl;

  out << std::setw(6) << ih.x()[0] / cm << "     " << std::setw(6) << ih.x()[1] / cm << "     "
      << std::setw(6) << ih.x()[2] / cm << std::endl;

  out << " data info = " << std::endl;

  typedef std::map<std::string, std::string>::const_iterator I;
  for (I i = ih.data_map().begin(); i != ih.data_map().end(); ++i) {
    out << " data name = " << i->first << " data value = " << i->second << std::endl;
  }

  return out;
}
}  // namespace mybhep
