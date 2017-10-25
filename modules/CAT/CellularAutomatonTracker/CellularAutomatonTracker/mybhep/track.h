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

#ifndef __ITRACK_H__
#define __ITRACK_H__

#include <string>
#include <vector>
#include <mybhep/bproperties.h>
#include <mybhep/ray.h>
#include <mybhep/clhep.h>
#include <mybhep/material.h>

namespace mybhep {
class particle;

//! class track
/*!
  represents the track left by a particle on one material
  (passive or active)
*/

class track : public bproperties {
  //! physical material (water, iron, air)
  const material* material_;

  //! pointer to its mother particle
  const particle* mother_;

  //! std::vector of rays along the trajectory
  std::vector<ray*> r_;

  //! view
  std::string view_;

 private:
  //! do not allow  copy constructor
  track(const track& part);
  //! do not allow asigment
  track& operator=(const track&);

 public:
  //! constructor
  track(const particle& mother, material* material, std::string view = "XYZ");
  //! constructor
  track(material* material, std::string view = "XYZ");
  //! destructor
  ~track();

 public:
  void add_ray(ray* r);

  // std::vector of rays (const)
  const std::vector<ray*>& rays() const { return r_; }

  // std::vector of rays (non const)
  std::vector<ray*>& rays() { return r_; }

  //! return std::vector of points along trajectory
  std::vector<const mybhep::Point3D*> x() const;
  //! return std::vector of momentum along trajectory
  std::vector<const mybhep::Vector3D*> p3() const;

  //! return material
  const material& mate() const { return *material_; };

  //! return view
  std::string view() const { return view_; }

  //! return mother particle
  const particle& mother_particle() const { return *mother_; }
  //! set mother particle
  void set_mother_particle(const particle& pmom) { mother_ = &pmom; }

  //! track length
  double track_length() const;
};

std::ostream& operator<<(std::ostream& s, const track& it);
}  // namespace mybhep
#endif
