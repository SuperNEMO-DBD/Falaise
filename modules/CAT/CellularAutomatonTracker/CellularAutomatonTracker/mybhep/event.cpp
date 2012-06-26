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

// Implementación of class event

#include <iostream>
#include <mybhep/event.h>
#include <mybhep/bprint.h>
#include <mybhep/container_algorithm.h>
#include <mybhep/utilities.h>
#include <mybhep/error.h>


namespace mybhep{
  using namespace std;

  event::event() :
    event_number_(0)
  {
    vertex_ = new mybhep::Point3D(small_neg,small_neg,small_neg);
  }


  // constructor
  event::event(int number, const mybhep::Point3D& vertex) :
    event_number_(number)
  {
    vertex_ = new mybhep::Point3D(vertex);
  }

  // constructor
  event::event(int number) :
    event_number_(number)
  {
    vertex_ = new mybhep::Point3D(small_neg,small_neg,small_neg);
  }

  event::~event()
  {
    container_algorithm::destroy(true_particles_);
    container_algorithm::destroy(digi_particles_);
    container_algorithm::destroy(sparticles_);
    delete vertex_;
  }

  // set vertex
  void event::set_vertex(double x, double y, double z)
  {
    vertex_->set( x, y, z);
  }

  // set vertex
  void event::set_vertex(const mybhep::Point3D& p)
  {
    vertex_->set(p.x(),p.y(),p.z());
  }


  void event::add_particle(ptype type, particle* p)
  {
    Assert(type == TRUTH || type == DIGI, __FILE__,__LINE__,
           internal_logic("  unknown particle type!!!"));

    if(type == TRUTH)
      true_particles_.push_back(p);
    else
      digi_particles_.push_back(p);
  }

    bool event::filter(ptype type,
                       std::string property_name,
                       std::string property_value,vector<particle*>& pft)
    {
      std::vector<particle*> prt;

      if (type == TRUTH)
        prt = true_particles();
      else
        prt = digi_particles();

      bool found = false;

      for(size_t ip=0; ip < prt.size(); ip++)
        {
          particle* p = prt[ip];
          if (p->find_property(property_name))
            if (p->fetch_property(property_name)==property_value)
              {
                pft.push_back(p);
                found = true;
              }
        }
    return found;
    }

  void event::clear()
  {
    container_algorithm::destroy(true_particles_);
    container_algorithm::destroy(digi_particles_);
    container_algorithm::destroy(sparticles_);
    prop_.clear();
    vertex_->set(small_neg,small_neg,small_neg);
  }

  bool event::find_daugthers(particle* p,ptype type, std::vector<particle*>& dau)
  {
    std::vector<particle*> prt;

    if (type == TRUTH)
      prt = true_particles();
    else
      prt = digi_particles();

    std::vector<const particle*> cpd = p->daughters();
    if (cpd.size() == 0) return false;

    for(size_t jp=0; jp < prt.size(); jp++)
      {
        particle* pd = prt[jp];
        for(size_t kp=0; kp < cpd.size(); kp++)
          {
            if( pd == cpd[kp])
              {
                dau.push_back(pd);
              }
          }
      }

    return true;
  }

  int event::find_tree(particle* p,ptype type, std::vector<particle*>& daut)
  {
    int generation = 0;
    daut.clear();
    std::vector<particle*> dau0;
    std::vector<particle*> dau1;
    std::vector<particle*> dau;

    bool test = find_daugthers(p,type,daut);

    if (test == false) return generation;
    dau = daut;
    while (test == true)
      {
        generation+=1;
        dau1.clear();
        // loop over all particles of this generation
        for(size_t jp=0; jp < dau.size(); jp++)
          {
            particle* p= dau[jp];
            dau0.clear();

            // for each particle find descendats
            bool test2 = find_daugthers(p,type,dau0);

            // store all of them in dau1 which holds all
            // particles of a given generation
            if(test2)
              container_algorithm::append(dau0,dau1);
          }
        if (dau1.size() ==0) test=false; //no more daugthers? exit

        // copy in container daut (all particles of tree)
        container_algorithm::append(dau1,daut);
        // dau gets a copy of this generation
        dau = dau1;
      }

    return generation;
  }

  bool event::particle_existence(particle* pr, ptype type)
  {
    bool found = false;
    std::vector<particle*> prt;

    if (type == TRUTH)
    prt = true_particles();
    else
    prt = digi_particles();

    for(size_t ip=0; ip < prt.size(); ip++)
    {
      particle* p = prt[ip];
      if (pr == p)
        {
          found = true;
          break;
        }
    }
    return found;
  }

  bool event::delete_particle(particle* pr, ptype type)
  {
    bool test = particle_existence(pr, type);
    std::vector<particle*> daut;
    if (!test) return false;

    std::vector<particle*>::iterator ip;

    // find all daugthers tree
    bool test2 = find_daugthers(pr,type, daut);
    //    clog << "found "<< daut.size() << " daugthers" << endl;

    if (test2)
      {
        for(ip=daut.begin(); ip !=daut.end(); ip++)
          {
            particle* p= *ip;
            //      clog << "running through particle = " << p << endl;
            delete_particle(p,type);
          }
      }
    else
      {
        //      clog << " no daughters found for particle" << pr << endl;
        //clog << " eras mother memory now" << endl;

        const particle& cpm = pr->mother();
        if (&cpm != NULL)
          erase_mother_memory(cpm,pr,type);

        //      clog << " erase event memory now" << endl;
        erase_event_memory(pr,type);

        //      clog << "is my pointer still alive? " << endl;
        //clog << "p = " <<pr << endl;

        //      clog << " now i commit sepuku" << endl;
        delete pr;
      }

    return true;
  }

  void event::erase_event_memory(const particle* p, ptype type)
  {

    std::vector<particle*>::iterator ip;

    if (type == TRUTH)
      {
        //      clog << "event before memory erase: particles now in event= "
        //           <<true_particles_.size()<<endl;;

        for(ip=true_particles_.begin(); ip !=true_particles_.end(); ip++)
          {
            if( p == *ip)
              {
                true_particles_.erase(ip);
                break;
              }
          }
        //      clog << "event after memory erase: particles now in event= "
        //     <<true_particles_.size()<<endl;;

      }
    else
      {
        //      clog << "event before memory erase: particles now in event= "
        //     <<digi_particles_.size()<<endl;;

        for(ip=digi_particles_.begin(); ip !=digi_particles_.end(); ip++)
          {
            if( p == *ip)
              {
                digi_particles_.erase(ip);
                break;
              }
          }
        //      clog << "event after memory erase: particles now in event= "
        //     <<digi_particles_.size()<<endl;;

      }
  }


  void event::erase_mother_memory(const particle& cpm,const particle* p,
                                  ptype type)
  {
    std::vector<particle*> prt;
    if (type == TRUTH)
      prt = true_particles();
    else
      prt = digi_particles();

    for(size_t jp=0; jp < prt.size(); jp++)
      {
        particle* pd = prt[jp];
        if( pd == &cpm)
          {
            std::vector<const particle*>& cpd = pd->daughters();
            std::vector<const particle*>::iterator ip;

            for(ip=cpd.begin(); ip !=cpd.end(); ip++)
              {
                if( p == *ip)
                  {
                    cpd.erase(ip);
                    return;
                  }
              }
          }
      }
  }

  ostream& operator << (ostream& s, const event& ip) {
    s << endl;

    s << " event number= " << ip.event_number()
      << endl;

    s << " event vertex (cm)= " << ip.vertex()/cm
      << endl;

    s << " List of properties "
      << "-----------------------------" << endl;

    typedef map<string, string>::const_iterator I;
    for(I i=ip.properties_map().begin(); i !=ip.properties_map().end(); ++i)
      {
        clog << " property name = " << i->first << "   "
             << " property value = " << i->second
             << endl;
      }

    s << " Event has " << ip.true_particles().size() << " true particles"
      << endl;
    s << " Event has " << ip.digi_particles().size() << " digi particles"
      << endl;
    s << " Event has " << ip.sparticles().size() << " super particles"
      << endl;


    s << " List of TRUTH particles in the event"
      << "------------------------------------"
      << endl;

    for(size_t i=0; i< ip.true_particles().size(); i++){

      particle& p = *ip.true_particles()[i];
      s << p << endl;
    }

    s << " List of DIGI particles in the event"
      << "------------------------------------"
      << endl;

    for(size_t i=0; i< ip.digi_particles().size(); i++){

      particle& p = *ip.digi_particles()[i];
      s << p << endl;
    }

    s << " List of super particles in the event"
      << "------------------------------------"
      << endl;

    for(size_t i=0; i< ip.sparticles().size(); i++){

      sparticle& p = *ip.sparticles()[i];
      s << p << endl;
    }

    return s;
  }

}

