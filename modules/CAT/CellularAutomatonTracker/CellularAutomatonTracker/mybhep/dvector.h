/* -*- mode: c++ -*- */
/*
 *
 *
 * Copyright 2003
 * J.J. Gomez-Cadenas, J.A. Hernando, A. Cervera
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#ifndef D_vector
#define D_vector

#include <vector>
#include <mybhep/bprint.h>
#include <fstream>


namespace mybhep
{
  /// adds print interface to STL vector
  /**
     \ingroup base
  */

  template<class T>
  class dvector :  public bprint, public std::vector<T> {

  public:
    dvector() : std::vector<T>(){}
    dvector(size_t n) : std::vector<T>(n){}

    /// operator *=
    dvector& operator *= (const T& a){
      for (size_t i = 0; i < this->size(); i++)
        (*this)[i] *= a;
      return *this;
    }
    /// operator /=
    dvector& operator /= (const T& a){
      for (size_t i = 0; i < this->size(); i++)
        (*this)[i] /= a;
      return *this;
    }


    bool find(T object) const {
      bool found = false;

      for (size_t i = 0; i < this->size(); i++)
        if (object == (*this)[i])
          {
            found = true;
            break;
          }
      return found;

    }

    void read(std::ifstream& file){
      size_t size;
      file >> size;
      T t;

      for (size_t i = 0; i< size; i++) {
        file >> t;
        this->push_back(t);
      }
    }

    void write(std::ofstream& file) const{
      file << this->size() << "\t";
      for (size_t i = 0; i< this->size(); i++)
        file << (*this)[i] << "\t";
    }

    virtual void info(std::ostream& s = std::clog) const{
      typename std::vector<T>::const_iterator vi;

      if (this->size() == 0)
        {
          s <<  "(  )";
        }
      else
        {
          s <<  "(";

          for (vi = this->begin(); vi< this->end()-1; vi++)
            {
              s << *vi <<",";
            }

          s << *vi << ")" ;
        }

    }
  };

  /// operator *
  template<class T>
  dvector<T> operator * (const dvector<T>& dv, const T& a){
    dvector<T> d = dv;
    return d*=a;
  }
  template<class T>
  dvector<T> operator * (const T& a, const dvector<T>& dv){
    dvector<T> d = dv;
    return d*=a;
  }
  /// operador /=
  template<class T>
  dvector<T> operator / (const dvector<T>& dv, const T& a){
    dvector<T> d = dv;
    return d/=a;
  }

}

#endif




