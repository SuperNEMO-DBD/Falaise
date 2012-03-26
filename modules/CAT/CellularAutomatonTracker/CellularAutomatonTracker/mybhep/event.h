/* -*- mode: c++ -*- */
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


#ifndef __IEVENT_H__
#define __IEVENT_H__

#include <string>
#include <vector>
#include <mybhep/bproperties.h>
#include <mybhep/particle.h>
#include <mybhep/sparticle.h>
#include <mybhep/clhep.h>

using namespace std;  

namespace mybhep{


  //! class event
  /*!
    Class event is a simple container of the particles
    and super particles found in a given event. 
    Any additional property
    of event can be described in a map of properties
    ordered by name.
  */

  class event : public bproperties {

  private:
    //! id of event
    int event_number_;

    //! vector of true particles in this event
    vector<particle*> true_particles_;

    //! vector of digitised particles in this event
    vector<particle*> digi_particles_;

    //! vector of super particles in this event
    vector<sparticle*> sparticles_;

    //! interaction vertex
    mybhep::Point3D* vertex_;


  private:
    //! do not allow copy constructor     
    event(const event&) ;

    //! do not allow assign operator
    event& operator = (const event&) ;

    bool find_daugthers(particle* p,ptype type, vector<particle*>& dau);
    int find_tree(particle* p,ptype type, vector<particle*>& daut);

    void erase_mother_memory(const particle& cpm,const particle* p, 
			     ptype type);
    bool particle_existence(particle* pr, ptype type);
    void erase_event_memory(const particle* p, ptype type);
  public:
    //! constructors
    event();
 
    //! constructor     
    event(int number);

    //! constructor     
    event(int number, const mybhep::Point3D& vertex);
    
    virtual ~event();
    
    //! set event number
    void set_event_number(int number){
      event_number_ =number;
    }
  
    //! set production vertex
    void set_vertex(double x, double y, double z);
    //! set production vertex
    void set_vertex(const mybhep::Point3D& p);

    //! add particles to the event
    /*!
      Notice that the pointer to the particle is passed to the event
      yielding memory control (event must take care of deleting
      the particles)
    */
    
    //! adds a true (normally it means MC truth) particle to event
    void add_true_particle(particle* p)
    {
      true_particles_.push_back(p);
    }

    //! adds a digi (normally it means reconstructed) particle to event
    void add_digi_particle(particle* p)
    {
      digi_particles_.push_back(p);
    }

    //! add a particle to event: ptype is either TRUTH or DIGI
    void add_particle(ptype type, particle* p);

    //! adds a super-particle to event
    void add_sparticle(sparticle* p)
    {
      sparticles_.push_back(p);
    }

    //! deletes a particle and all its descendants from event
    bool delete_particle(particle* pr, ptype type);
    //! return event number
    const int& event_number() const {
      return event_number_;
    }


    //! return vertex
    const mybhep::Point3D&  vertex() const {
      return *vertex_;
    }
 
    //! return vector of true particles (const)
    const vector<particle*>& true_particles() const {    
      return true_particles_;
    }
  
   //! return vector of particles
    vector<particle*>& true_particles() {    
      return true_particles_;
    }

    //! return vector of digi particles (const)
    const vector<particle*>& digi_particles() const {    
      return digi_particles_;
    }
  
   //! return vector of particles
    vector<particle*>& digi_particles() {    
      return digi_particles_;
    }

    //! return vector of sparticles (const)
    const vector<sparticle*>& sparticles() const {    
      return sparticles_;
    }
  
   //! return vector of sparticles
    vector<sparticle*>& sparticles() {    
      return sparticles_;
    }
 
    //! return vector of particles that pass a filter 
    /*!
      Loops over particles (TRUTH or DIGI) in the event
     */ 

    bool filter(ptype type,
		string property_name, 
		string property_value,vector<particle*>& pft) ;

    void clear();

  };
  ostream& operator << (ostream& s, const event& ie); 
}
#endif









