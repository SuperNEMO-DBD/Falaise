/* -*- mode: c++ -*- */
#ifndef iconverter_H
#define iconverter_H

/*
 * Copyright (C) 2003 J.A. Hernando J.J. Gomez-Cadenas
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 * namespace Centella{
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 */
#include <string>


namespace mybhep{
  //! Interfase for converters
  /*!
    It defines the interface of a converter, that is, an object
    that saves and retrieves a transient class to persistency
  */

  class Iconverter {
  public:

    virtual ~Iconverter(){}
    //! save
    virtual std::string store() = 0;

    //! load
    virtual void restore(std::string) = 0;

    //! load
    virtual void complete_restore() = 0;
  };
}
#endif
