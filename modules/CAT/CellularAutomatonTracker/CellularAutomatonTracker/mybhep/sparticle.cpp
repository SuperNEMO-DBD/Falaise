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

// Implementación of class sparticle

#include <mybhep/error.h>
#include <mybhep/utilities.h>
#include <mybhep/sparticle.h>


namespace mybhep{

  // using namespace std;

  // default constructor
  sparticle::sparticle ()
  {
    sp_ = new particle(DIGI,"unknown");
  }

  // constructor
  sparticle::sparticle (const particle& p)
  {
    subp_.push_back(&p);
    sp_ = new particle(p.type(),p.name());
    sp_->vertex_ray() = p.vertex_ray();
    sp_->decay_vertex_ray() = p.decay_vertex_ray();
    sp_->p4() = p.p4();
    sp_->set_mother(p.mother());
    sp_->set_primary(p.primary());

  }

  //! destructor
  sparticle::~sparticle()
  {
    delete sp_;
  }

  // set seed (inside) particle
  void sparticle::set_seed_particle(const particle& p)
  {
    subp_.push_back(&p);
    sp_-> set_type(p.type());
    sp_-> set_name(p.name());
    sp_->vertex_ray() = p.vertex_ray();
    sp_->decay_vertex_ray() = p.decay_vertex_ray();
    sp_->p4() = p.p4();
    sp_->set_mother(p.mother());
    sp_->set_primary(p.primary());

  }

  void sparticle::add_particle(const particle& p, double q)
  {
    Assert(subp_.size()>0,__FILE__,__LINE__,
           internal_logic("seed particle not set"));

    subp_.push_back(&p);
    q_.push_back(q);
  }

  void sparticle::add_particle(const particle& p)
  {
    Assert(subp_.size()>0,__FILE__,__LINE__,
           internal_logic("seed particle not set"));

    subp_.push_back(&p);
  }

  void sparticle::add_quality(double q)
  {
    q_.push_back(q);
  }

  std::ostream& operator << (std::ostream& out, const sparticle& ip) {
    out << std::endl;

    out << " sparticle info: particle = " << ip.seed_particle() << " "
        << std::endl;

    out << " List of suparticles "
        << "-----------------------------" << std::endl;

    for(size_t i=0; i< ip.subparticles().size(); i++){

      const particle& p = *ip.subparticles()[i];
      out << p << std::endl;
    }

    out << " List of matching quantities "
        << print_vector(ip.quality()) << std::endl;

    return out;
  }
}
