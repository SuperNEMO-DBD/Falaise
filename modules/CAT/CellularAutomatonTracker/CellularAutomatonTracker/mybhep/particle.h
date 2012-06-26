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


#ifndef __IPARTICLE_H__
#define __IPARTICLE_H__


#include <vector>
#include <mybhep/clhep.h>
#include <mybhep/particle_definition.h>
#include <mybhep/ray.h>
#include <mybhep/track.h>
#include <mybhep/hit.h>
#include <mybhep/mparticle.h>
#include <mybhep/store.h>



namespace mybhep {

  //! class particle
  /*!
    This class is an abstraction of the concept of particle, useful
    for high level analysis.
  */

  //! TRUTH = MC particle, DIGI = reconstructed particle
  enum ptype{TRUTH=0,DIGI=1};

  //! particle state
  /*!
   * PROJ = XZ or YZ projection
   * MPROJ = Matched XZ and YZ projections
   * TRK = Track: Projections have been fitted, p and vertex known
   * PID = Particle has been identified (PID performed)
   */
  enum pstate{PROJ=0,MPROJ,TRK,PID};


  class particle : public particle_definition{

  private:
    //! The type of particle (TRUTH or DIGI)
    ptype ptype_;

    //! The state of particle (PROJ, MPROJ...)
    pstate pstate_;

    //! Lorentz four std::vector characterizing the particle
    mybhep::Vector4D* p4_;

    //! Ray with the position and momentum at production vertex.
    /**! A ray is an object made of a position and a momentum*/
    ray* r_;

    //! Ray with the position and momentum at decay vertex.
    /**! A ray is an object made of a position and a momentum*/
    ray* rd_;

    //! specifies if particle is primary (no mother particle) or secondary
    bool primary_;

    //! pointer to its mother particle if any
    const particle* mother_;

    //! std::vector of daughter particles
    std::vector<const particle*> daughters_;

    //! specifies if the particle has a mother
    bool has_mother_;

    //! std::vector of tracks left by this particle
    /**! A track is the "shadow" left by a particle in a
     medium. It is useful to follow (MC) particles propagating
    inside a given material */
    std::vector<track*> tracks_;

    //! std::vector of mparticles or mirror particles to this particle
    /**! Class mirror particle allows the representation of a
     mirror between DIGI and TRUTH and its matching properties
    a std::vector allows to mirror more than one particle */
    std::vector<mparticle*> mp_;


    //! map of hits left by this particle ordered by detector name
    /**! A hit is an abstraction of the signal left by the
       particle in a given detector */
    std::multimap<std::string,hit*> hits_;

  private:

    //! do not allow  copy constructor
    particle(const particle& part);
    //! do not allow asigment
    particle& operator = (const particle &);
    void destroy_hits();

  public:
    //! constructor from particle type, name and initial ray
    particle(ptype type, std::string name, const ray& r);
    //! constructor from particle type and name
    particle(ptype type, std::string name);
    //! constructor from particle type and GEANT3 or PDG code
    particle(ptype type, int code, idcode scheme=GEANT3);
    //! general constructor
    particle(ptype type, std::string name, int pdg, double m,
	     double q, double l=-1);

    //! destructor
    virtual ~particle();

  public:


    //! set mother particle
    void set_mother(const particle& pmom)
    {
      mother_ = &pmom;
      primary_ = false;
      has_mother_ = true;
    }


    //! set momentum (at production vertex)
    void set_p(const mybhep::Vector3D& v);
    //! set momentum from 3 std::vector (at production vertex)
    void set_p(double px, double py, double pz);
    //! set production vertex
    void set_vertex(double x, double y, double z);
    //! set production vertex
    void set_vertex(const mybhep::Point3D& p);


    //! set momentum (at decay vertex)
    void set_decay_p(const mybhep::Vector3D& v);
    //! set momentum from 3 std::vector (at decay vertex)
    void set_decay_p(double px, double py, double pz);
    //! set decay vertex
    void set_decay_vertex(double x, double y, double z);
    //! set decay vertex
    void set_decay_vertex(const mybhep::Point3D& p);

    //! add a daughter to the particle
    void add_daughter(const particle& p)
    {
      daughters_.push_back(&p);
    }

    //! add a track to the particle
    void add_track(track* tr){
      tracks_.push_back(tr);
    }

    //! add a mirror to the particle
    void add_mirror(mparticle* mp){
      mp_.push_back(mp);
    }

    //! add a hit to the particle
    void add_hit(std::string detector, hit* ht){
      hits_.insert(make_pair(detector,ht));
    }

    //! return particle momentum (const)
    const mybhep::Vector3D& p3() const {return r_->p3();}

    //! return particle momentum (non const)
    mybhep::Vector3D& p3() {return r_->p3();}

    //! return particle momentum at decay (const)
    const mybhep::Vector3D& p3d() const {return rd_->p3();}

    //! return particle momentum at decay (non const)
    mybhep::Vector3D& p3d() {return rd_->p3();}

    //! return particle production vertex (const)
    const mybhep::Point3D& vertex() const {return r_->x();}

    //! return particle decay vertex (const)
    const mybhep::Point3D& decay_vertex() const {return rd_->x();}

    //! return ray
    ray& vertex_ray()  {return *r_;}
    //! return ray (const)
    const ray& vertex_ray()  const {return *r_;}

    //! return ray at decay
    ray& decay_vertex_ray()  {return *rd_;}
    //! return ray at decay (const)
    const ray& decay_vertex_ray()  const {return *rd_;}

    //! return particle 4 momentum (const)
    const mybhep::Vector4D& p4() const {return *p4_;}
    //! return particle 4 momentum
    mybhep::Vector4D& p4() {return *p4_;}

    //! return particle momentum (magnitude)  at production vertex
    const double p() const {return r_->p3().mag();}
    //! return px at production vertex
    const double px() const {return r_->p3().x();}
    //! return py at production vertex
    const double py() const {return r_->p3().y();}
    //! return pz at production vertex
    const double pz() const {return r_->p3().z();}
    //! return pt at production vertex
    const double pt() const {return r_->p3().perp();}

    //! return particle momentum (magnitude)  at decay vertex
    const double pd() const {return rd_->p3().mag();}
    //! return px at decay vertex
    const double pxd() const {return rd_->p3().x();}
    //! return py at decay vertex
    const double pyd() const {return rd_->p3().y();}
    //! return pz at decay vertex
    const double pzd() const {return rd_->p3().z();}
    //! return pt at decay vertex
    const double ptd() const {return rd_->p3().perp();}

    //! return energy
    double e() const {return p4_->e();}

    //! return production vertex x
    const double x() const {return vertex().x();}
    //! return production vertex y
    const double y() const {return vertex().y();}
    //! return production vertex z
    const double z() const {return vertex().z();}

    //! return decay vertex x
    const double xd() const {return decay_vertex().x();}
    //! return decay vertex y
    const double yd() const {return decay_vertex().y();}
    //! return decay vertex z
    const double zd() const {return decay_vertex().z();}

    //! return mother
    const particle&  mother() const {
      return *mother_;
    }


    //! return std::vector of daughters (const)
    const std::vector<const particle*>&  daughters() const {return daughters_;}

    //! return std::vector of daughters (const)
    std::vector<const particle*>&  daughters()  {return daughters_;}

    //! return std::vector of tracks (const)
    const std::vector<track*>&  tracks() const {return tracks_;}

    //! return std::vector of tracks
    std::vector<track*>&  tracks()  {return tracks_;}

    //! return std::vector of mparticles (const)
    const std::vector<mparticle*>&  mparticles() const {return mp_;}

    //! return std::vector of mparticles
    std::vector<mparticle*>&  mparticles()  {return mp_;}

    //! return std::vector of hits associated to a detector
    std::vector<hit*>  hits(std::string detector) const;

    //! return multimap of hits
    const std::multimap<std::string,hit*>& hit_map() const {return hits_;}

    //! return std::vector of strings associated to detector name
    std::vector<std::string>  detectors() const;

    //! description of particle
    bool primary() const { return primary_; }

    //! description of particle type
    ptype type() const { return ptype_; }

    //! description of particle state
    pstate state() const { return pstate_; }

    //! description of particle mother
    bool has_mother() const { return has_mother_; }

    //! set description of particle
    void set_primary(bool is_primary) {
      primary_ = is_primary;
      if (!has_mother_) mother_ = NULL;
    }

    //! set description of particle type
    void set_type(ptype type) {
      ptype_ = type;
    }

    //! set description of particle state
    void set_state(pstate state) {
      pstate_ = state;
    }

    //! set description of particle mother
    void set_mother(bool has_mother) {
      has_mother_ = has_mother;
      if (!has_mother_) mother_ = NULL;
    }
    //! track length
    double track_length() const  ;

  };
  std::ostream& operator << (std::ostream& s, const particle& ip);
}
#endif









