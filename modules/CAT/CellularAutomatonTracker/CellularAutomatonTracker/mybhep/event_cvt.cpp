/* -*- mode: c++ -*- */
/*
 *
 * Copyright 2004
 * J.J. Gomez-Cadenas, J.A. Hernando
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

//#include "config.h"
#include <cstdio>
#include <cstdlib>

// #ifndef HAVE_SSTREAM
// #include <strstream>
// using std::strstream;
// //#include <strstream.h>
// #else
#include <sstream>
// #endif

#include <mybhep/event_cvt.h>
#include <mybhep/particle_cvt.h>
#include <mybhep/sparticle_cvt.h>
#include <mybhep/mybhep_svc.h>
#include <mybhep/container_algorithm.h>
#include <mybhep/error.h>
#include <mybhep/utilities.h>

namespace mybhep {

using namespace std;

//! constructor
event_cvt::event_cvt(const event& evt, int indx) : ibconverter() {
  set_index(indx);
  set_name("__event__");
  set_owner(false);
  event_ = &evt;
  nevent_ = NULL;
  particle_label_ = "__particle__";
  sparticle_label_ = "__sparticle__";
}

event_cvt::event_cvt(std::string def) : ibconverter() {
  particle_label_ = "__particle__";
  sparticle_label_ = "__sparticle__";
  restore(def);
  complete_restore();
}

event_cvt::~event_cvt() {
  if (owner()) delete nevent_;
}

// serialize object
std::string event_cvt::store() {
  std::string tmp;

  // store class name and index

  tmp = name();
  to_string(tmp, index());

  // vertex position
  const mybhep::Point3D& xd = event_->vertex();

  std::string tmp2;
  {
    ostringstream tmp2_oss;
    tmp2_oss.precision(8);
    tmp2_oss << xd[0] << ' ' << xd[1] << ' ' << xd[2];
    tmp2 = tmp2_oss.str();
  }
  /*
  std::string tmp2 = to_string_precision(xd[0],"8") + " " +
    to_string_precision(xd[1],"8") + " " +
    to_string_precision(xd[2],"8");
  */
  to_string(tmp, tmp2);

  const map<string, string>& dmap = event_->properties_map();
  to_string(tmp, dmap.size());

  typedef map<string, string>::const_iterator I;
  for (I i = dmap.begin(); i != dmap.end(); ++i) {
    std::string tmp3 = to_string(i->first) + " " + to_string(i->second);
    to_string(tmp, tmp2);
  }

  // true particle std::vector
  const std::vector<particle*>& pv = event_->true_particles();

  to_string(tmp, pv.size());

  // loop over all particle and write particle indexes
  for (size_t ipar = 0; ipar < pv.size(); ipar++) {
    particle& p = *pv[ipar];
    to_string(tmp, (long int)(&p));
  }

  // digi particle std::vector
  const std::vector<particle*>& pv2 = event_->digi_particles();

  to_string(tmp, pv2.size());

  // loop over all particle and write particle indexes
  for (size_t ipar = 0; ipar < pv2.size(); ipar++) {
    particle& p = *pv2[ipar];
    to_string(tmp, (long int)(&p));
  }

  // sparticle std::vector
  const std::vector<sparticle*>& pv3 = event_->sparticles();

  to_string(tmp, pv3.size());

  // loop over all sparticle and write sparticle indexes
  for (size_t ipar = 0; ipar < pv3.size(); ipar++) {
    sparticle& p = *pv3[ipar];
    to_string(tmp, (long int)(&p));
  }

  // loop over all true particles and write particle information

  for (size_t ipar = 0; ipar < pv.size(); ipar++) {
    particle& p = *pv[ipar];
    long int ip = (long int)(&p);
    particle_cvt pcv(p, ip);
    std::string sp = pcv.store();

    to_string(tmp, sp);
  }

  // loop over all digi particles and write particle information

  for (size_t ipar = 0; ipar < pv2.size(); ipar++) {
    particle& p = *pv2[ipar];
    long int ip = (long int)(&p);
    particle_cvt pcv(p, ip);
    std::string sp = pcv.store();

    to_string(tmp, sp);
  }

  // loop over all sparticles and write sparticle information

  for (size_t ipar = 0; ipar < pv3.size(); ipar++) {
    sparticle& p = *pv3[ipar];
    long int ip = (long int)(&p);
    sparticle_cvt pcv(p, ip);
    std::string sp = pcv.store();

    to_string(tmp, sp);
  }

  return tmp;
}

// restore event
void event_cvt::restore(std::string str) {
  // #ifndef HAVE_SSTREAM
  //     istrstream istr (s.c_str());
  // #else
  std::istringstream istr(str);
  // #endif

  // get converter service
  converter_svc& csvc = mybhep_svc::instance().converter_service();

  std::string name_par;
  istr >> name_par;

  int indx;
  istr >> indx;

  set_index(indx);
  set_name(name_par);

  // create new event
  nevent_ = new event(indx);
  set_owner(true);

  // read x,y,z of vertex and set to event
  double x, y, z;

  istr >> x >> y >> z;
  nevent_->set_vertex(x, y, z);

  size_t nprop;
  istr >> nprop;

  for (size_t i = 0; i < nprop; ++i) {
    std::string a_name;
    std::string a_value;
    istr >> a_name >> a_value;
    nevent_->add_property(a_name, a_value);
  }

  size_t true_size, digi_size, spar_size;
  istr >> true_size;

  // restore particle index for true
  for (size_t ipar = 0; ipar < true_size; ipar++) {
    int a_indx;

    // particle index
    istr >> a_indx;
    particles_true_.push_back(a_indx);
  }

  istr >> digi_size;

  // restore particle index for digi
  for (size_t ipar = 0; ipar < digi_size; ipar++) {
    int a_indx;

    // particle index
    istr >> a_indx;
    particles_digi_.push_back(a_indx);
  }

  istr >> spar_size;

  // restore particle index for spar
  for (size_t ipar = 0; ipar < spar_size; ipar++) {
    int a_indx;

    // particle index
    istr >> a_indx;
    particles_spar_.push_back(a_indx);
  }

  // Read information about particle

  strip(str, str.find(particle_label_));

  for (size_t ipar = 0; ipar < true_size; ipar++) {
    // create a particle converter
    // invoke restore method
    // register to converter service

    particle_cvt* pcv = new particle_cvt(str);

    csvc.add_converter(pcv);

    strip(str, particle_label_.size());
    strip(str, str.find(particle_label_));
  }

  for (size_t ipar = 0; ipar < digi_size; ipar++) {
    // create a particle converter
    // invoke restore method
    // register to converter service

    particle_cvt* pcv = new particle_cvt(str);
    csvc.add_converter(pcv);

    if (ipar < digi_size - 1) {
      strip(str, particle_label_.size());
      strip(str, str.find(particle_label_));
    }
  }
  strip(str, str.find(sparticle_label_));

  for (size_t ipar = 0; ipar < spar_size; ipar++) {
    // create a particle converter
    // invoke restore method
    // register to converter service

    sparticle_cvt* pcv = new sparticle_cvt(str);
    csvc.add_converter(pcv);

    strip(str, sparticle_label_.size());
    strip(str, str.find(sparticle_label_));
  }
}

void event_cvt::complete_restore() {
  // get converter service
  converter_svc& csvc = mybhep_svc::instance().converter_service();

  // complete restoration of all event properties
  // (classes owned by event)
  csvc.complete_restore();

  for (size_t ipar = 0; ipar < particles_true_.size(); ipar++) {
    int index_par = particles_true_[ipar];
    try {
      particle_cvt& pcvt = *(dynamic_cast<particle_cvt*>(&csvc.converter(index_par)));

      particle* pp = pcvt.create();

      nevent_->add_true_particle(pp);
    } catch (bad_index&) {
      std::cerr << "could not resolve a pointer to a true particle" << endl;
      std::cerr << "event is likely corrupted" << endl;
      throw;
    }
  }

  for (size_t ipar = 0; ipar < particles_digi_.size(); ipar++) {
    int index_par = particles_digi_[ipar];

    try {
      particle_cvt& pcvt = *(dynamic_cast<particle_cvt*>(&csvc.converter(index_par)));

      particle* pp = pcvt.create();

      nevent_->add_digi_particle(pp);
    } catch (bad_index&) {
      std::cerr << "could not resolve a pointer to a digi particle" << endl;
      std::cerr << "event is likely corrupted" << endl;
      throw;
    }
  }

  for (size_t ipar = 0; ipar < particles_spar_.size(); ipar++) {
    int index_par = particles_spar_[ipar];
    try {
      sparticle_cvt& pcvt = *(dynamic_cast<sparticle_cvt*>(&csvc.converter(index_par)));

      sparticle* pp = pcvt.create();

      nevent_->add_sparticle(pp);
    } catch (bad_index&) {
      std::cerr << "could not resolve a pointer to a super particle" << endl;
      std::cerr << "event is likely corrupted" << endl;
      throw;
    }
  }
}

const event& event_cvt::retrieve() const { return *event_; }

//! A new instance of event
event* event_cvt::create() {
  Assert(nevent_, __FILE__, __LINE__, internal_logic("  null pointer!!!"));

  set_owner(false);
  converter_svc& csvc = mybhep_svc::instance().converter_service();
  csvc.clear();

  return nevent_;
}
}  // namespace mybhep
