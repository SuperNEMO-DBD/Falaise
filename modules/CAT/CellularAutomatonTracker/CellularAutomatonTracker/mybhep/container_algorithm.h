/* -*- mode: c++ -*- */
/*
 *
 * Copyright 2003
 * A. Cervera, J.J. Gomez-Cadenas, J.A. Hernando
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

#ifndef ETool_h
#define ETool_h 1


#include <vector>
#include <string>
#include <functional>


namespace mybhep{

  template <class T> class unary : public std::unary_function<T,bool> {
  public:
    virtual bool operator() (const T) const {return true;}
  };

  template <class T> class binary : public std::binary_function<T,T,bool> {
  public:
    virtual bool operator() (const T, const T) const
    {return true;}
  };

  class container_algorithm
  {
  public:

    template <class T>
    static void destroy(std::vector<T>& ini)
    {
      typename std::vector<T>::const_iterator it;
      for (it = ini.begin(); it != ini.end(); it++)
        {
          delete *it;
        }
      ini.clear();
    }

    template <class T>
    static void copy(const std::vector<T>& ini, std::vector<T>& target)
    {
      target.clear();
      typename std::vector<T>::const_iterator it;
      for (it = ini.begin(); it != ini.end(); it++)
        {
          target.push_back(*it);
        }
    }

    template <class T>
    static void append(const std::vector<T>& ini, std::vector<T>& target)
    {
      typename std::vector<T>::const_iterator it;
      for (it = ini.begin(); it != ini.end(); it++)
        {
          target.push_back(*it);
        }
    }

  };

}


#endif
