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

// Implementación of class track
#include <iomanip>
#include <mybhep/particle.h>
#include <mybhep/error.h>
#include <mybhep/track.h>
#include <mybhep/container_algorithm.h>
#include <mybhep/system_of_units.h>

namespace mybhep{

  using namespace std;

  // constructor
  track::track(const particle& mother, material* mate,std::string view)
  {
    mother_ = &mother;
    material_ = new material(*mate);
    view_ = view;

  }
  //! constructor
  track::track(material* mate, std::string view)
  {
    mother_ = NULL;
    material_ = new material(*mate);
    view_ = view;
  }

  //! destructor
  track::~track()
  {

    delete material_;
    container_algorithm::destroy(r_);
  }


  void track::add_ray(ray* r)
  {
    Assert(view_ == r->view(),__FILE__,__LINE__,
	   internal_logic("ray view must be the same than track view"));
    r_.push_back(r);
  }

  std::vector<const mybhep::Point3D*> track::x() const
  {
    std::vector<const mybhep::Point3D*> vx;
    for(size_t i=0; i< r_.size(); i++){

      ray& r = *r_[i];
      const mybhep::Point3D& x = r.x();
      vx.push_back(&x);
    }
    return vx;
  }

  std::vector<const mybhep::Vector3D*> track::p3() const
  {
    std::vector<const mybhep::Vector3D*> vx;
    for(size_t i=0; i< r_.size(); i++){

      ray& r = *r_[i];
      const mybhep::Vector3D& x = r.p3();
      vx.push_back(&x);
    }
    return vx;
  }

  double track::track_length() const
  {
    double track_length ;

    if(r_.size()>0){
      ray& r1 = *r_[0];
      ray& r2 = *r_[r_.size()-1];
      const mybhep::Point3D& first = r1.x();
      const mybhep::Point3D& last = r2.x();
      track_length = last.distance(first);
    }
    else
      track_length = 0;

    return track_length;
  }


  ostream& operator << (ostream& s, const track& ih) {
    s << endl;
    s  << " track info: material = " << ih.mate() << " "
       << " track_length (cm) = " << ih.track_length()/cm
       << " track_view = " << ih.view()
       << endl;

    s << " x (cm)     y (cm)      z (cm)       p (MeV) " << endl;

    for(size_t i=0; i< ih.rays().size(); i++){

      ray& r = *ih.rays()[i];
      const mybhep::Point3D& x = r.x();
      double p = r.p3().mag();

      s << std::setw(6) << x[0]/cm <<"   "
	<< std::setw(6) << x[1]/cm <<"   "
	<< std::setw(6) << x[2]/cm <<"   "
	<< std::setw(6) << p
	<<endl;
    }
      return s;
  }
}
