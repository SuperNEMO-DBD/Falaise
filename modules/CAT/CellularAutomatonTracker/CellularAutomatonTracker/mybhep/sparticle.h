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

#ifndef __ISPARTICLE_H__
#define __ISPARTICLE_H__

#include <string>
#include <vector>
#include <mybhep/particle.h>

namespace mybhep {

//! class sparticle
/*!
  represents a "super particle", that is an object larger than a particle
  used to express matching properties. For example, two particles can
  be formed from two segments in different detectors, which are then
  matched to form a new particle. This new particle contains
  a std::vector of pointers to the sub-particles that compose it plus a std::vector
  of doubles to express the quality of the match (typically a chi2).
  Thus it is a super particle.
*/

class sparticle {
  //! The seed (inside) particle
  particle* sp_;

  //! std::vector of subparticles
  std::vector<const particle*> subp_;

  //! std::vector of matching quantities
  std::vector<double> q_;

 private:
  //! do not allow  copy constructor
  sparticle(const sparticle& part);
  //! do not allow asigment
  sparticle& operator=(const sparticle&);

 public:
  //! default constructor
  sparticle();
  //! construct from a particle (seed particle)
  sparticle(const particle& p);
  //! destructor
  ~sparticle();

 public:
  //! set seed particle
  void set_seed_particle(const particle& p);
  //! add particle to super particle (and quality)
  void add_particle(const particle& p, double q);
  //! add particle to sp
  void add_particle(const particle& p);
  //! add quality
  void add_quality(double q);

  // std::vector of particles (const)
  const std::vector<const particle*>& subparticles() const { return subp_; }

  // std::vector of particles (non const)
  std::vector<const particle*>& subparticles() { return subp_; }

  // std::vector of matching quantities (match quality)
  const std::vector<double>& quality() const { return q_; }

  //! return particle inside
  particle& seed_particle() { return *sp_; }

  //! return particle inside (constant)
  const particle& seed_particle() const { return *sp_; }
};
std::ostream& operator<<(std::ostream& s, const sparticle& ip);
}  // namespace mybhep
#endif
