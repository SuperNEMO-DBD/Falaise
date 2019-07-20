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

// Implementación of class particle

#include <mybhep/container_algorithm.h>
#include <mybhep/particle.h>
#include <mybhep/track.h>
#include <mybhep/hit.h>

namespace mybhep {

particle::particle(ptype type, std::string nam, const ray& r) : particle_definition(nam) {
  r_ = new ray(r);
  rd_ = new ray;
  p4_ = new mybhep::Vector4D(r_->p3().x(), r_->p3().y(), r_->p3().z(),
                             sqrt(square(r_->p3().mag()) + square(mass())));

  mother_ = NULL;
  primary_ = true;
  ptype_ = type;
  has_mother_ = false;

  if (ptype_ == TRUTH)
    pstate_ = PID;
  else
    pstate_ = PROJ;
}

particle::particle(ptype type, std::string name) : particle_definition(name) {
  r_ = new ray;
  rd_ = new ray;
  p4_ = new mybhep::Vector4D(r_->p3().x(), r_->p3().y(), r_->p3().z());
  mother_ = NULL;
  primary_ = true;
  ptype_ = type;
  has_mother_ = false;

  if (ptype_ == TRUTH)
    pstate_ = PID;
  else
    pstate_ = PROJ;
}

particle::particle(ptype type, int code, idcode scheme) : particle_definition(code, scheme) {
  r_ = new ray;
  rd_ = new ray;
  p4_ = new mybhep::Vector4D(r_->p3().x(), r_->p3().y(), r_->p3().z());
  mother_ = NULL;
  primary_ = true;
  ptype_ = type;
  has_mother_ = false;

  if (ptype_ == TRUTH)
    pstate_ = PID;
  else
    pstate_ = PROJ;
}

particle::particle(ptype type, std::string name, int pdg, double mass, double charge,
                   double lifetime)
    : particle_definition(name, pdg, mass, charge, lifetime) {
  r_ = new ray;
  rd_ = new ray;
  p4_ = new mybhep::Vector4D(r_->p3().x(), r_->p3().y(), r_->p3().z());
  mother_ = NULL;
  primary_ = true;
  ptype_ = type;
  has_mother_ = false;

  if (ptype_ == TRUTH)
    pstate_ = PID;
  else
    pstate_ = PROJ;
}

//! destructor
particle::~particle() {
  delete r_;
  delete rd_;
  delete p4_;

  container_algorithm::destroy(tracks_);
  container_algorithm::destroy(mp_);
  destroy_hits();
}

void particle::destroy_hits() {
  typedef std::multimap<std::string, hit*>::const_iterator I;
  for (I i = hits_.begin(); i != hits_.end(); ++i) {
    delete i->second;
  }

  hits_.clear();
}

//! set momentum
void particle::set_p(double px, double py, double pz) {
  r_->set_p(px, py, pz);
  p4_->setPx(px);
  p4_->setPy(py);
  p4_->setPz(pz);
  p4_->setE(sqrt(square(r_->p3().mag()) + square(mass())));
}

void particle::set_p(const mybhep::Vector3D& v) {
  r_->set_p(v);
  p4_->setPx(v.x());
  p4_->setPy(v.y());
  p4_->setPz(v.z());
  p4_->setE(sqrt(square(r_->p3().mag()) + square(mass())));
}
//! set vertex
void particle::set_vertex(double x, double y, double z) { r_->set_point(x, y, z); }

//! set vertex
void particle::set_vertex(const mybhep::Point3D& p) { r_->set_point(p); }

//! set momentum
void particle::set_decay_p(double px, double py, double pz) { rd_->set_p(px, py, pz); }

void particle::set_decay_p(const mybhep::Vector3D& v) { rd_->set_p(v); }
//! set vertex
void particle::set_decay_vertex(double x, double y, double z) { rd_->set_point(x, y, z); }

//! set vertex
void particle::set_decay_vertex(const mybhep::Point3D& p) { rd_->set_point(p); }

//! return std::vector of hits associated to a detector
std::vector<hit*> particle::hits(std::string detector) const {
  std::vector<hit*> hits;
  typedef std::multimap<std::string, hit*>::const_iterator I;
  std::pair<I, I> b = hits_.equal_range(detector);
  for (I i = b.first; i != b.second; ++i) {
    hits.push_back((i->second));
  }
  return hits;
}

//! return std::vector of strings associated to detector name
std::vector<std::string> particle::detectors() const {
  std::vector<std::string> dets;
  typedef std::multimap<std::string, hit*>::const_iterator I;

  std::string old_det = " ";
  for (I i = hits_.begin(); i != hits_.end(); ++i) {
    std::string det = i->first;
    if (det != old_det) {
      dets.push_back(det);
      old_det = i->first;
    }
  }
  return dets;
}

double particle::track_length() const {
  const mybhep::Point3D& first = r_->x();
  const mybhep::Point3D& last = rd_->x();
  double track_length = last.distance(first);

  return track_length;
}

std::ostream& operator<<(std::ostream& out, const particle& ip) {
  out << std::endl;

  out << "\n***********************************************\n"
      << " particle name= " << ip.name() << " "
      << " geant3= " << ip.geant3() << " "
      << " pdg= " << ip.geant3() << " "
      << " mass (MeV)= " << ip.mass() / MeV << " charge = " << ip.charge()
      << " type (0 TRUTH, 1 DIGI) = " << ip.type()
      << " state (0 PROJ, 1 MPROJ, 2 TRK, 3 PID) = " << ip.state() << std::endl;

  out << " ++++at production vertex ++++" << std::endl;
  out << " particle 3 momentum (MeV) =" << ip.p3() / MeV << std::endl;
  out << " momentum (MeV) = " << ip.p() / MeV << std::endl;
  out << " energy (MeV)= " << ip.e() / MeV << std::endl;
  out << " vertex (cm)= " << ip.vertex() / cm << std::endl;

  out << " ++++at decay vertex ++++" << std::endl;
  out << " particle 3 momentum (MeV) =" << ip.p3d() / MeV << std::endl;
  out << " momentum (MeV) = " << ip.pd() / MeV << std::endl;
  out << " decay vertex (cm)= " << ip.decay_vertex() / cm << std::endl;

  out << " track length (cm)= " << ip.track_length() / cm << std::endl;

  if (ip.primary())
    out << " particle is primary " << std::endl;
  else {
    out << " particle is secondary" << std::endl;
    if (ip.has_mother()) {
      out << " mother of particle is " << ip.mother().name() << std::endl;
      out << " with 3  momentum (MeV) =" << ip.mother().p3() / MeV << std::endl;
      out << " and energy (MeV)= " << ip.mother().e() / MeV << std::endl;
    }
  }

  out << " List of secondary particles "
      << "-----------------------------" << std::endl;

  for (size_t i = 0; i < ip.daughters().size(); i++) {
    const particle& p = *ip.daughters()[i];
    out << " particle name= " << p.name() << " "
        << " particle mass (MeV)= " << p.mass() / MeV << " particle charge = " << p.charge()
        << std::endl;

    out << " particle 3 momentum (MeV) =" << p.p3() / MeV << std::endl;
    out << " particle momentum (MeV) = " << p.p() / MeV << std::endl;
    out << " particle energy (MeV)= " << p.e() / MeV << std::endl;
  }

  out << " List of tracks "
      << "-----------------------------" << std::endl;

  for (size_t i = 0; i < ip.tracks().size(); i++) {
    const track& p = *ip.tracks()[i];
    out << p << std::endl;
  }

  out << " List of properties "
      << "-----------------------------" << std::endl;

  {
    typedef std::map<std::string, std::string>::const_iterator I;
    for (I i = ip.properties_map().begin(); i != ip.properties_map().end(); ++i) {
      out << " property name = " << i->first << "   "
          << " property value = " << i->second << std::endl;
    }
  }

  out << " List of hits "
      << "-----------------------------" << std::endl;

  typedef std::multimap<std::string, hit*>::const_iterator I;
  for (I i = ip.hit_map().begin(); i != ip.hit_map().end(); ++i) {
    out << "detector = " << i->first << " hit = " << *(i->second) << std::endl;
  }

  out << " List of mirror particles "
      << "-----------------------------" << std::endl;

  for (size_t i = 0; i < ip.mparticles().size(); i++) {
    const mparticle& mp = *ip.mparticles()[i];
    out << mp << std::endl;
  }

  return out;
}
}  // namespace mybhep
