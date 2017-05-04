/* -*- mode: c++ -*- */
#ifndef IPOINT
#define IPOINT
/*
 *
 * Copyright (C) 2002 J.J. Gomez-Cadenas
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
#include <iostream>
#include <cmath>
#include <mybhep/error.h>
#include <mybhep/utilities.h>

/**
 * \defgroup types Irene types
*/
namespace mybhep{

//! Space point (x,y,z)
/**
 * \ingroup types
*/
  class point {
  private:
   //! x coordinate
    double x_;
    //! y coordinate
    double y_;
    //! z coordinate
    double z_;
    //! view: XZ, YZ, or XYZ
    std::string view_;

  public:
    //!Default constructor
    point(std::string view_arg="XYZ")
    {
      x_= small_neg; y_ =small_neg; z_ = small_neg;
      view_ = to_upper(view_arg);
    }
    //!Default destructor
    virtual ~point(){};
    //! constructor
    point(double x_arg, double y_arg, double z_arg, std::string view_arg="XYZ"):
      x_(x_arg),y_(y_arg),z_(z_arg),view_(to_upper(view_arg)){}

    //! set all coordinates
    void coordinates(double x_arg,double y_arg, double z_arg){
      x_=x_arg; y_=y_arg; z_=z_arg;
      view_ = "XYZ";
    }
    //! set all coordinates
    void set(double x_arg,double y_arg, double z_arg){
      coordinates(x_arg,y_arg,z_arg);
    }

    //! set  coordinates in a view
    void set(double xt,double z_arg, std::string view_arg){
      coordinates(xt,z_arg,view_arg);
    }

    //! set  coordinates in a view
    void coordinates(double xt,double z_arg, std::string view_arg){
      view_ = to_upper(view_arg);
      Assert(view_ == "XZ" || view_ == "YZ",
	     __FILE__,__LINE__,
	     internal_logic("view must be XZ,or YZ"));

      if (view_ == "XZ")
	{
	  x_=xt; y_=small_neg; z_=z_arg;
	}
      else if (view_ == "YZ")
	{
	  x_=small_neg; y_=xt; z_=z_arg;
	}
    }

    //! set x
    void set_x(double x_arg){
      x_ = x_arg;
    }

    //! set y
    void set_y(double y_arg){
      y_ = y_arg;
    }

    //! set z
    void set_z(double z_arg){
      z_ = z_arg;
    }


    //! get all coordinates
    const point& coordinates()const {return *this;}
    //!get x
    const double& x() const
    {
      return x_;
    }
    //! read x
    double& x(){
      if(view_ == "YZ") view_ = "XYZ";
      return x_;
    }

    //!get y
    const double& y() const {
      return y_;
    }
    //! read y
    double& y(){
      if(view_ == "XZ") view_ = "XYZ";
      return y_;
    }
    //!get z
    const double& z() const {return z_;}
    //! read z
    double& z(){return z_;}
    //! view
    std::string view() const {return view_;}

    // Operators
    // operator () returns/set x,y,z
    //! read v(i), i = 0,1,2 = x,y,z
    const double& operator () (size_t i) const
    {
      Assert(i < 3, __FILE__, __LINE__,
	     bad_index("index i must be < 3"));
      if (i==0) return x();
      if (i==1) return y();
      return z_;
    }

    //! write v(i), i = 0,1,2
    double& operator () (size_t i)
    {
      Assert(i < 3, __FILE__,__LINE__,
	     bad_index("index i must be < 3"));

      if (i==0) return x();
      if (i==1) return y();
      return z_;
    }

    // no protection in operator []
    // (write x,y,z even if the view not XYZ)
    const double& operator [] (size_t i) const
    {

      Assert(i < 3, __FILE__, __LINE__,
	     bad_index("index i must be < 3"));

      if (i==0) return x_;
      if (i==1) return y_;
      return z_;
    }

    //! write v(i), i = 0,1,2
    double& operator [] (size_t i)
    {

      Assert(i < 3, __FILE__,__LINE__,
	     bad_index("index i must be < 3"));

      if (i==0) return x_;
      if (i==1) return y_;
      return z_;
    }

    //! operador +=
    point& operator += (const point& p2)
    {
      point& p1= *this;

      Assert(p1.view_ == p2.view_,__FILE__,__LINE__,
	     internal_logic("error, points must have same view"));

      for (size_t i = 0; i < 3; i++)
	p1(i) += p2(i);
      return *this;
    }

    //! operador -=
    point& operator -= (const point& p2)
    {
      point& p1= *this;
      Assert(p1.view_ == p2.view_,__FILE__,__LINE__,
	     internal_logic("error, points must have same view"));

      for (size_t i = 0; i < 3; i++)
	p1(i) -= p2(i);
      return *this;
    }

    //! operador *=
    point& operator *= (double a)
    {
      point& p1= *this;

      for (size_t i = 0; i < 3; i++)
	p1(i) *= a;
      return *this;
    }

    //! operador *=
    point& operator /= (double a)
    {
      Assert(a !=0, __FILE__,__LINE__,
	     divide_by_zero("error, divide by zero"));

      point& p1= *this;

      for (size_t i = 0; i < 3; i++)
	p1(i) /= a;
      return *this;
    }

    //! distance
    double distance(const point& p2) const
    {
      const point& p1= *this;
      Assert(p1.view_ == p2.view_,__FILE__,__LINE__,
	     internal_logic("error, points must have same view"));

      if(view_ == "XZ")
	return sqrt(square(x_-p2.x()) + square(z_-p2.z()));
      else if(view_ == "YZ")
	return sqrt(square(y_-p2.y()) + square(z_-p2.z()));
      else if(view_ == "XYZ")
	{
	  return sqrt(square(x_-p2.x()) + square(y_-p2.y()) +
		      square(z_-p2.z()));
	}
      else
	Assert(1,__FILE__,__LINE__,
	       internal_logic("error, wrong view"));
      return -9999;
    }
    //! module
    double module() const
    {
      if(view_ == "XZ")
	return sqrt(x_*x_ + z_*z_);
      else if(view_ == "YZ")
	return sqrt(y_*y_ + z_*z_);
      else if(view_ == "XYZ")
	return sqrt(x_*x_ + y_*y_ + z_*z_);
      else
	Assert(1,__FILE__,__LINE__,
	       internal_logic("error, wrong view"));
      return -9999;
    }

  };

  inline std::ostream& operator << (std::ostream& s, const point& ip) {
      s << std::endl;

      s << "view =" << ip.view() << std::endl;
      if(ip.view() == "XZ")
      s << "(" << ip.x() << ","<<ip.z() <<")";
      else if(ip.view() == "YZ")
      s << "(" <<ip.y() <<","<<ip.z() <<")";
      else if(ip.view() == "XYZ")
      s << "(" << ip.x() << "," <<ip.y() <<","<<ip.z() <<")";
      return s;
    }


  // Operations with points
  // -v
  inline point operator - (const point& v1)
  {
    point v = v1;
    for (size_t i = 0; i < 3; i++)
	v(i) = -v1(i);
    return v;

  }
  // v1+v2
  inline point operator + (const point& v1, const point& v2)
  {
    point v = v1;
    v+=v2;
    return v;
  }
  //! v1-v2
  inline point operator - (const point& v1, const point& v2)
  {
    point v = v1;
    v-=v2;
    return v;
  }

  // v*d
  inline point operator * (const point& v1, double d)
  {
    point v = v1;
    v*=d;
    return v;
  }
  // v/d

  inline point operator / (const point& v1, double d)
  {
    point v = v1;
    v/=d;
    return v;
  }

  // d*v
  inline point operator * (double d, const point& v1)
  {
    point v = v1;
    v*=d;
    return v;
  }

}

#endif
