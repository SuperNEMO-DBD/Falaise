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

#include <mybhep/ray.h>
#include <mybhep/utilities.h>
#include <iostream>

namespace mybhep{

  using namespace std;

  ray::ray(string view)
  {
    view_ = to_upper(view);
    x_ = new point(view);
    p3_ = new mybhep::Vector3D(0,0,0);
    edep_ =0;
  }

  ray::ray(const point& x, const mybhep::Vector3D& p, double edep)
  {
    view_ = x.view();
    x_ = new point(x);
    p3_ = new mybhep::Vector3D(p);
    edep_ =edep;
  }
  ray::ray(const ray& r)
  {
    view_ = r.view();
    x_ = new point(r.x());
    p3_ = new mybhep::Vector3D(r.p3());
    edep_ =r.edep();
  }
  ray& ray::operator = (const ray& r)
  {
    delete x_;
    delete p3_;
    view_ = r.view();
    x_ = new point(r.x());
    p3_ = new mybhep::Vector3D(r.p3());
    edep_ =r.edep();
    return *this;
  }

  ray::~ray()
  {
    delete x_;
    delete p3_;
  }

  // sets the space point
  void ray::set_point(const point& p)
  {
    delete x_;
    view_ = p.view();
    x_ = new point(p);
  }
  // sets the space point
  void ray::set_point(double x, double y, double z)
  {
    delete x_;
    view_ = "XYZ";
    x_ = new point(x,y,z);

  }

  void ray::set_point(double xt, double z, string view)
  {
    delete x_;
    view_ = view;
    x_ = new point(view_);
    x_->coordinates(xt,z,view);

  }

  // sets the momentum
  void ray::set_p(const mybhep::Vector3D& p)
  {
    delete p3_;
    p3_ = new mybhep::Vector3D(p);
  }

  // sets the momentum
  void ray::set_p(double px, double py, double pz)
  {
    delete p3_;
    p3_ = new mybhep::Vector3D(px,py,pz);
  }

  ostream& operator << (ostream& s, const ray& r) {
    s << endl;

    s << " ray: view = " << r.view() << " "
      << " ray: x = " << r.x() << " "
      << " ray: p =  " << r.p3()
      << " ray: edep =  " << r.edep()
      << endl;

    return s;
  }
}
