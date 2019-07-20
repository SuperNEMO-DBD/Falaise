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

//#include "config.h"
// #ifndef HAVE_SSTREAM
// #include <strstream>
// //#include <strstream.h>
// #else
#include <sstream>
//#endif

#include <mybhep/particle_cvt.h>
#include <mybhep/track_cvt.h>
#include <mybhep/hit_cvt.h>
#include <mybhep/mparticle_cvt.h>
#include <mybhep/mybhep_svc.h>

#include <mybhep/container_algorithm.h>
#include <mybhep/error.h>
#include <mybhep/utilities.h>

namespace mybhep {

using namespace std;

// FIXME label consistency!
particle_cvt::particle_cvt(const particle& par, int index_arg) : ibconverter() {
  set_index(index_arg);
  set_name("__particle__");
  set_owner(false);
  particle_ = &par;
  nparticle_ = NULL;
  track_label_ = "__track__";
  mirror_label_ = "__mirror__";
  hit_label_ = "__hit__";
  hit_size_label_ = "__hit_size__";
}

particle_cvt::particle_cvt(std::string def) : ibconverter() {
  track_label_ = "__track__";
  mirror_label_ = "__mirror__";
  hit_label_ = "__hit__";
  hit_size_label_ = "__hit_size__";
  restore(def);
}

particle_cvt::~particle_cvt() {
  if (owner()) delete nparticle_;
}

// serialize object
std::string particle_cvt::store() {
  std::string tmp;

  tmp = name();
  to_string(tmp, index());

  to_string(tmp, particle_->name());
  to_string(tmp, particle_->primary());
  to_string(tmp, particle_->type());
  to_string(tmp, particle_->state());

  // vertex ray
  const ray& r = particle_->vertex_ray();

  // vertex position
  const mybhep::Point3D& x = r.x();

  std::string tmp2 = to_string(x[0]) + " " + to_string(x[1]) + " " + to_string(x[2]);
  to_string(tmp, tmp2);

  // vertex p
  const mybhep::Vector3D& p3 = r.p3();

  tmp2 = to_string(p3[0]) + " " + to_string(p3[1]) + " " + to_string(p3[2]);
  to_string(tmp, tmp2);

  // decay vertex ray
  const ray& rd = particle_->decay_vertex_ray();

  // decay vertex position
  const mybhep::Point3D& xd = rd.x();

  tmp2 = to_string(xd[0]) + " " + to_string(xd[1]) + " " + to_string(xd[2]);
  to_string(tmp, tmp2);

  // decay vertex p
  const mybhep::Vector3D& p3d = rd.p3();

  tmp2 = to_string(p3d[0]) + " " + to_string(p3d[1]) + " " + to_string(p3d[2]);
  to_string(tmp, tmp2);

  const map<string, string>& dmap = particle_->properties_map();

  to_string(tmp, dmap.size());

  typedef map<string, string>::const_iterator I;
  for (I i = dmap.begin(); i != dmap.end(); ++i) {
    tmp2 = to_string(i->first) + " " + to_string(i->second);
    to_string(tmp, tmp2);
  }

  // indexes of objects in class
  if (particle_->primary() || !particle_->has_mother())
    to_string(tmp, -1);
  else
    to_string(tmp, (long int)&particle_->mother());

  const std::vector<const particle*>& dau = particle_->daughters();
  to_string(tmp, dau.size());

  for (size_t idau = 0; idau < dau.size(); idau++) {
    const particle& pd = *dau[idau];
    to_string(tmp, (long int)&pd);
  }

  const std::vector<mparticle*>& mp = particle_->mparticles();
  to_string(tmp, mp.size());

  for (size_t idau = 0; idau < mp.size(); idau++) {
    const mparticle& pd = *mp[idau];
    to_string(tmp, (long int)&pd);
  }

  const std::vector<track*>& trk = particle_->tracks();
  to_string(tmp, trk.size());

  for (size_t itrk = 0; itrk < trk.size(); itrk++) {
    const track& td = *trk[itrk];
    to_string(tmp, (long int)&td);
  }

  const std::vector<string> dets = particle_->detectors();
  to_string(tmp, dets.size());

  for (size_t idet = 0; idet < dets.size(); idet++) {
    std::string det = dets[idet];

    const std::vector<hit*> hts = particle_->hits(det);
    to_string(tmp, det);
    to_string(tmp, hts.size());

    for (size_t i = 0; i < hts.size(); i++) {
      const hit& ht = *hts[i];
      to_string(tmp, (long int)&ht);
    }
  }

  // loop over all mirrors and write information

  for (size_t imr = 0; imr < mp.size(); imr++) {
    const mparticle& mpt = *mp[imr];
    int it = (long int)(&mpt);

    mparticle_cvt mpcv(mpt, it);

    std::string st = mpcv.store();
    to_string(tmp, st);
  }

  // loop over all tracks and write track information

  for (size_t itrk = 0; itrk < trk.size(); itrk++) {
    const track& tr = *trk[itrk];
    int it = (long int)(&tr);

    track_cvt trcv(tr, it);

    std::string st = trcv.store();
    to_string(tmp, st);
  }

  // loop over all hits and write hit information

  for (size_t idet = 0; idet < dets.size(); idet++) {
    std::string det = dets[idet];
    const std::vector<hit*> hts = particle_->hits(det);
    to_string(tmp, hit_size_label_);
    to_string(tmp, hts.size());

    for (size_t i = 0; i < hts.size(); i++) {
      const hit& ht = *hts[i];
      int it = (long int)&ht;
      hit_cvt htcv(ht, it);
      std::string sh = htcv.store();
      to_string(tmp, sh);
    }
  }

  //     clog << "this is the std::string stored in particle "
  //       << tmp << std::endl ;
  return tmp;
}

void particle_cvt::restore(std::string def) {
  // #ifndef HAVE_SSTREAM
  //     istrstream istr (def.c_str());
  // #else
  std::istringstream istr(def.c_str());
  // #endif

  // get converter service
  converter_svc& csvc = mybhep_svc::instance().converter_service();

  std::string name_par;
  istr >> name_par;

  int indx;
  istr >> indx;

  set_index(indx);
  set_name(name_par);

  std::string particle_name;

  // name
  istr >> particle_name;

  // primary
  int primary;
  istr >> primary;

  // type
  int type_par;
  istr >> type_par;

  // FIXME! Commented out to avoid crashes, must figure out
  // why type wrong
  //     Assert(type==0 || type==1, __FILE__,__LINE__,
  //         internal_logic("  unknown particle type!!!"));

  // state
  int state_par;
  istr >> state_par;

  // FIXME! Commented out to avoid crashes, must figure out
  // why state wrong

  //     Assert(state==0 || state==1 || state==2 || state==3,
  //         __FILE__,__LINE__,
  //         internal_logic("  unknown particle state!!!"));

  ptype pt;

  if (type_par == 0)
    pt = TRUTH;
  else
    pt = DIGI;

  pstate pst;

  if (state_par == 0)
    pst = PROJ;
  else if (state_par == 1)
    pst = MPROJ;
  else if (state_par == 2)
    pst = TRK;
  else
    pst = PID;

  // create new particle
  nparticle_ = new particle(pt, particle_name);
  set_owner(true);

  nparticle_->set_primary(primary);
  nparticle_->set_type(pt);
  nparticle_->set_state(pst);

  // read x,y,z of vertex ray and set to particle
  double x, y, z;

  istr >> x >> y >> z;
  nparticle_->set_vertex(x, y, z);

  // read px,py,pz of vertex ray and set to particle
  double px, py, pz;

  istr >> px >> py >> pz;

  nparticle_->set_p(px, py, pz);

  // read x,y,z of decay vertex ray and set to particle

  istr >> x >> y >> z;
  nparticle_->set_decay_vertex(x, y, z);

  // read px,py,pz of decay vertex ray and set to particle

  istr >> px >> py >> pz;

  nparticle_->set_decay_p(px, py, pz);

  size_t ndata;
  istr >> ndata;

  for (size_t i = 0; i < ndata; ++i) {
    std::string a_name;
    std::string a_value;
    istr >> a_name >> a_value;
    nparticle_->add_property(a_name, a_value);
  }

  // indexes of objects in class
  istr >> imother_;

  if (imother_ > 0) nparticle_->set_mother(true);

  size_t ndau;
  istr >> ndau;

  for (size_t idau = 0; idau < ndau; idau++) {
    int jdau;
    istr >> jdau;
    idaughters_.push_back(jdau);
  }

  size_t nmir;
  istr >> nmir;

  for (size_t imir = 0; imir < nmir; imir++) {
    int jmir;
    istr >> jmir;
    imirror_.push_back(jmir);
  }

  size_t ntrk;
  istr >> ntrk;

  for (size_t itrk = 0; itrk < ntrk; itrk++) {
    int jtrk;
    istr >> jtrk;
    tracks_.push_back(jtrk);
  }

  size_t ndets;
  istr >> ndets;

  for (size_t idet = 0; idet < ndets; idet++) {
    std::string det_name;
    size_t nhits;
    istr >> det_name;
    istr >> nhits;

    idets_.push_back(det_name);

    for (size_t ihit = 0; ihit < nhits; ihit++) {
      int jhit;
      istr >> jhit;

      hits_.insert(make_pair(det_name, jhit));
    }
  }

  // restore mparticles

  for (size_t imir = 0; imir < nmir; imir++) {
    strip(def, def.find(mirror_label_));

    // create a mparticle converter
    // invoke restore method
    // register to converter service

    mparticle_cvt* tcv = new mparticle_cvt(def);

    csvc.add_converter(tcv);

    strip(def, mirror_label_.size());
  }

  // restore tracks

  for (size_t itrk = 0; itrk < ntrk; itrk++) {
    strip(def, def.find(track_label_));

    // create a track converter
    // invoke restore method
    // register to converter service

    track_cvt* tcv = new track_cvt(def);

    csvc.add_converter(tcv);

    strip(def, track_label_.size());
  }

  // restore hits

  for (size_t idet = 0; idet < ndets; idet++) {
    std::string det;
    size_t nhits;

    strip(def, def.find(hit_size_label_));

    nhits = number_of_hits(def);

    for (size_t ihit = 0; ihit < nhits; ihit++) {
      // create a hit converter
      // invoke restore method
      // register to converter service

      hit_cvt* tcv = new hit_cvt(def);

      csvc.add_converter(tcv);

      strip(def, hit_label_.size());
      if (ihit < nhits - 1) strip(def, def.find(hit_label_));
    }
  }
}

void particle_cvt::complete_restore() {
  // get converter service

  converter_svc& csvc = mybhep_svc::instance().converter_service();

  // restore event pointers
  if (nparticle_->has_mother()) {
    try {
      particle_cvt& pcvt = *(dynamic_cast<particle_cvt*>(&csvc.converter(imother_)));

      particle& pp = pcvt.reference();
      nparticle_->set_mother(pp);

    } catch (bad_index&) {
      nparticle_->set_mother(false);
    }
  }

  for (size_t ipar = 0; ipar < idaughters_.size(); ipar++) {
    int index_par = idaughters_[ipar];
    try {
      particle_cvt& pcvt = *(dynamic_cast<particle_cvt*>(&csvc.converter(index_par)));

      particle& pp = pcvt.reference();
      nparticle_->add_daughter(pp);
    } catch (bad_index&) {
      ;
    }
  }

  for (size_t itrk = 0; itrk < imirror_.size(); itrk++) {
    int index_par = imirror_[itrk];
    try {
      mparticle_cvt& tcvt = *(dynamic_cast<mparticle_cvt*>(&csvc.converter(index_par)));

      mparticle* tr = tcvt.create();
      nparticle_->add_mirror(tr);
    } catch (bad_index&) {
      ;
    }
  }

  for (size_t itrk = 0; itrk < tracks_.size(); itrk++) {
    int index_par = tracks_[itrk];
    try {
      track_cvt& tcvt = *(dynamic_cast<track_cvt*>(&csvc.converter(index_par)));

      track* tr = tcvt.create();
      nparticle_->add_track(tr);
    } catch (bad_index&) {
      ;
    }
  }

  for (size_t idet = 0; idet < idets_.size(); idet++) {
    std::string detector = idets_[idet];

    std::vector<int> hits;
    typedef multimap<string, int>::const_iterator I;
    pair<I, I> b = hits_.equal_range(detector);

    for (I i = b.first; i != b.second; ++i) {
      int index_par = i->second;
      try {
        hit_cvt& tcvt = *(dynamic_cast<hit_cvt*>(&csvc.converter(index_par)));

        hit* tr = tcvt.create();
        nparticle_->add_hit(detector, tr);

      } catch (bad_index&) {
        ;
      }
    }
  }
}

const particle& particle_cvt::retrieve() const { return *particle_; }

particle& particle_cvt::reference() { return *nparticle_; }

particle* particle_cvt::create() {
  Assert(nparticle_, __FILE__, __LINE__, internal_logic("  null pointer!!!"));

  set_owner(false);
  return nparticle_;
}

size_t particle_cvt::number_of_hits(string& def) {
  // #ifndef HAVE_SSTREAM
  //     istrstream istr (def.c_str());
  // #else
  std::istringstream istr(def.c_str());
  // #endif

  size_t nhits;
  std::string label;

  istr >> label;
  istr >> nhits;

  //    clog << "in number_of_hits, label ="  << label << endl;
  // clog << "in number_of_hits, nhits ="  << nhits << endl;
  strip(def, def.find(hit_label_));

  // clog << "in number_of_hits, def ="  << def << endl;
  return nhits;
}
}  // namespace mybhep
