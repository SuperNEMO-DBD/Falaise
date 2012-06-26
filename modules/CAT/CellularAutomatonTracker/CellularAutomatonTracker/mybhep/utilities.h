/* -*- mode: c++ -*- */

/*
 *
 * Copyright (C) 2002 J.J. Gomez-Cadenas, J.A. Hernando
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

#ifndef __UTIL__
#define __UTIL__

#include <string>
#include <vector>
#include <sstream>
#include <cmath>
#include <cstdlib>

namespace mybhep{

  enum prlevel{MUTE,CONCISE,NORMAL,WARNING,DETAILED,VERBOSE,VVERBOSE,DUMP};

  inline prlevel get_info_level(std::string info)
  {
    if(info == "MUTE")  return MUTE;
    if(info == "CONCISE")  return CONCISE;
    if(info == "NORMAL")  return NORMAL;
    if(info == "WARNING")  return WARNING;
    if(info == "DETAILED")  return DETAILED;
    if(info == "VERBOSE")  return VERBOSE;
    if(info == "VVERBOSE")  return VVERBOSE;
    if(info == "DUMP")  return DUMP;
    else return NORMAL;
  }

  static const double small_neg = -999999;
  static const double plus_infinity = 9999999;
  static const size_t default_integer = 9999999;
  static const double default_min = 9999999.;
  static const double default_max = -9999999.;

  //! to string
  /**
   *\ingroup util
   */
  template <class T>
  inline std::string to_string(T i)
  {
    std::ostringstream ostr;
    ostr << i ;
    return ostr.str();
  }

  template <class T>
  inline std::string to_string_precision(T i, char* sprecision = "8")
  {
    std::ostringstream ostr;
    int precision=atoi(sprecision);
    if (precision<=0) precision=8;
    ostr.precision(precision);
    ostr << i ;
    return ostr.str();
  }


  template <class T>
  inline void to_string(std::string& s, T i, std::string glue=" ")
  {
    std::ostringstream ostr;
    ostr << i ;
    s+=glue;
    s+=ostr.str();
  }

  inline int int_from_string(std::string s)
  {
    std::istringstream istr(s.c_str());
    int q;
    istr >> q;
    return q;
  }

  inline double double_from_string(std::string s)
  {
    std::istringstream istr(s.c_str());

    double q;
    istr >> q;
    return q;
  }

  inline float float_from_string(std::string s)
  {
    std::istringstream istr(s.c_str());

    float q;
    istr >> q;
    return q;
  }

  inline long long_from_string(std::string s)
  {
    std::istringstream istr(s.c_str());

    long q;
    istr >> q;
    return q;
  }

  inline size_t size_t_from_string(std::string s)
  {
    std::istringstream istr(s.c_str());

    size_t q;
    istr >> q;
    return q;
  }


  template <class T>
  inline std::string vector_to_string(const std::vector<T> v)
  {
    std::string tmp  =to_string(v[0]);
    for (size_t i=1; i<v.size(); i++)
      {
        std::string tmp2 = to_string(v[i]);
	to_string(tmp,tmp2,"_");
      }
    to_string(tmp," ");
    return tmp;
  }


    template <class T>
    inline void T_from_string(std::string s, T& q)
    {
      std::istringstream istr(s.c_str());
      istr >> q;
    }

    template <class T>
    inline void vector_from_string(std::string s, std::vector<T>& v)
    {
      T q;

      bool test = true;
      while(test)
        {
          int i2= s.find("_");
          std::string ele=s.substr(0,i2);

          if (i2 <0) test = false;

          T_from_string(ele,q);
          v.push_back(q);
          s.erase(0,i2+1);
        }
    }



    //! change to lower case
    /**
     *\ingroup util
     */

  inline std::string to_lower(const std::string& s)
  {
    std::string s2(s);
    for (size_t i = 0; i < s.length(); ++i)
      s2[i] = tolower(s[i]);
    return s2;
  }

  //! change to upper case
  /**
   *\ingroup util
   */

  inline std::string to_upper(const std::string& s)
  {
    std::string s2(s);
    for (size_t i = 0; i < s.length(); ++i)
      s2[i] = toupper(s[i]);
    return s2;
  }

  template <class T>
  inline std::string print_vector(const std::vector<T>& vt)
  {
    typename std::vector<T>::const_iterator vi;

    std::ostringstream s;

    if (vt.size() == 0)
      {
	s <<  "(  )";
      }
    else
      {
	s <<  "(";

	for (vi = vt.begin(); vi< vt.end()-1; vi++)
	  {
	    s << *vi <<",";
	  }

	s << *vi << ")" ;
      }

    return s.str();
  }

  //! compare two strings (no case sensitive)
  /**
   *\ingroup util
   */

  inline bool compare_string_nocase(const std::string& s, const std::string& s2)
  {
    // return false if not the same size
    if (s.size() != s2.size()) return false;
    // otherwise iterate
    bool caso = true;

    for (size_t i = 0; i < s.length(); i++){
      if (toupper(s[i]) != toupper(s2[i]) ){
	caso = false;
	break;
      }
    }
    return caso;
  }


  //! compare two strings (case sensitive)
  /**
   *\ingroup util
   */
  inline bool compare_string(const std::string& s, const std::string& s2)
  {
    // Strings must have the same size
    if (s.size() != s2.size()) return false;

    // otherwise iterate
    bool caso = true;

    for (size_t i = 0; i < s.length(); i++){

      if (s[i] != s2[i] ){
	caso = false;
	break;
      }
    }
    return caso;
  }

  //! return a const char* from a string
  /**
   *\ingroup util
   */
   inline const char* skin(std::string str){

     std::ostringstream s;

     std::string::const_iterator p = str.begin()+1;
     while (p < str.end()-1){
       s << *p ;
       p++;
     }

     return s.str().c_str();

  }

/// square
/**
 *\ingroup util
 */
  template <class T>
  inline T square(T a)
  {
      return a*a;
  }

  template <class T>
  inline T cube(T a)
  {
      return a*a*a;
  }


  inline void fix_angles(double *a1, double *a2){
    if( fabs(*a1 - *a2) > acos(-1.) )
      {
	if( *a1 < *a2 )
	  *a1 = *a1 + 2.*acos(-1.);
	else
	  *a2 = *a2 + 2.*acos(-1.);
      }

    return;
  }

  inline double round_to(double value, int to = 1)
  //round up a double type to show n decimals
  {

    double places = pow(10.0, to);

    return round(value * places) / places;

  }

}
#endif






