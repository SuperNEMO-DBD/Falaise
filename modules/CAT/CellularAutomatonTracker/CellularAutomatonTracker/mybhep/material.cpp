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

#include <mybhep/material.h>

namespace mybhep {

material::material(const std::string& name, double density, double rad_length, double inter_length)
    : name_(name), density_(density), rad_len_(rad_length), nuc_ilen_(inter_length) {}

material::material(const material& mcopy) {
  name_ = mcopy.name();
  density_ = mcopy.density();
  rad_len_ = mcopy.rad_length();
  nuc_ilen_ = mcopy.inter_length();
}
//! asignment
const material& material::operator=(const material& mcopy) {
  name_ = mcopy.name();
  density_ = mcopy.density();
  rad_len_ = mcopy.rad_length();
  nuc_ilen_ = mcopy.inter_length();
  return *this;
}
// visual output
std::ostream& operator<<(std::ostream& out, const material& the_material) {
  out << std::endl;

  out << " material name= " << the_material.name() << " "
      << " material density (g/cm3) = " << the_material.density() / (g / cm3) << " "
      << " radiation length (cm) = " << the_material.rad_length() / cm << " "
      << " interation length (g/cm2) = " << the_material.inter_length() / (g / cm2) << std::endl;

  return out;
}

}  // namespace mybhep
