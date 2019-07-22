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

#ifndef __IHIT_H__
#define __IHIT_H__

#include <string>
#include <vector>
#include <mybhep/bproperties.h>
#include <mybhep/clhep.h>

namespace mybhep {
class particle;

//! class hit
/*!
  represents the hit left by a particle on one detector
*/

class hit : public bproperties {
 private:
  //! detector name
  std::string detector_;

  //! pointer to its mother particle
  const particle* mother_;

  //! space point associated to hit
  mybhep::Point3D x_;

 public:
  //! constructor
  hit(const particle& mother, std::string det_name);
  //! constructor
  hit(std::string det_name);
  //! copy constructor
  hit(const hit& tr);
  //! asigment
  hit& operator=(const hit&);
  //! destructor
  ~hit();

 public:
  //! set the hit space point
  void set_point(const mybhep::Point3D& x_arg) { x_ = x_arg; }
  //! set the hit data
  void add_data(std::string name, std::string value) { prop_.store(name, value); }
  //! find some data name
  bool find_data_name(std::string name) const { return prop_.find(name); }

  //! return the value of some data
  std::string data(std::string name) {
    std::string pr = prop_.fetch(name);
    return pr;
  }

  //! return data in a map of std::string string
  const std::map<std::string, std::string>& data_map() const { return prop_.store_map(); }

  //! return data names
  const std::vector<std::string> data_names() const { return prop_.names(); }

  //! return data values
  const std::vector<std::string> data_values() { return prop_.items(); }

  //! return space point
  const mybhep::Point3D& x() const { return x_; }

  //! return detector name
  std::string detector() const { return detector_; };

  //! return mother particle
  const particle& mother_particle() const { return *mother_; }

  //! set mother particle
  void set_mother_particle(const particle& pmom) { mother_ = &pmom; }
};
std::ostream& operator<<(std::ostream& s, const hit& ih);
}  // namespace mybhep
#endif
