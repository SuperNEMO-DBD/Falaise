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


#ifndef ray_h
#define ray_h 1

#include <vector>
#include <string>
#include <mybhep/clhep.h>
#include <mybhep/system_of_units.h>


namespace mybhep{

  class ray
  {
    //! Describes a "track ray"
    /*! A track ray is defined by a point and a momentum. As a particle
      transverses a medium it leaves a track on it, that is a discrete
      collection of "rays", defined by a point and a momentum vector.
      We add also the energy loss in the medium
     */


public:


    //! default constructor
    ray(std::string view ="XYZ");
    //! constructor from a point and a vector
    ray(const point& x, const mybhep::Vector3D& p3, double edep=0);

    //!  copy constructor
    ray(const ray &right);

    //! Assignment operation
    ray& operator=(const ray& right);

    //! virtual destructor, safe to inherit
    virtual ~ray();


  public:
    //! returns the space point (constant)
    const point& x() const { return *x_; }
    //! sets the space point
    void set_point(const point& p);
    //! sets the space point
    void set_point(double x, double y, double z);
    void set_point(double xt, double z, std::string view);
    //! returns the momentum (constant)
    const mybhep::Vector3D& p3() const { return *p3_; }
    //! returns the momentum
    mybhep::Vector3D& p3()  { return *p3_; }
    //! sets the momentum
    void set_p(const mybhep::Vector3D& p);
    //! sets the energy deposited
    void set_edep(double  edep) {edep_ = edep;}
    //! returns the energy deposited
    double edep() const { return edep_;}
    //! sets the momentum
    void set_p(double px, double py, double pz);

    //! returns the view
    std::string view() const  { return view_; }

  private:
    //! view: XZ, YZ, or XYZ
    std::string view_;
    //! momentum of ray
    mybhep::Vector3D* p3_;
    //! Position of ray
    point* x_;

    //! energy deposited
    double edep_;
  };
  std::ostream& operator << (std::ostream& s, const ray& r);
}
#endif
