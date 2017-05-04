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


#ifndef MATERIAL_HH
#define MATERIAL_HH

#include <vector>
#include <string>
#include <iostream>
#include <mybhep/system_of_units.h>


namespace mybhep{
  //! represents a physical material
  class material
  {

  public:

    //! Constructor to create a material from scratch.

  public:

    material(const std::string& name, double density=-1, double rad_length =-1,
	     double inter_length=-1);

    virtual ~material(){};

    //! no default constructor
    material();
    //! copy constructor
    material(const material&);
    //! asignment
    const material& operator=(const material&);

  public:

    //! name
    std::string name() const {return name_;};
    //! density
    double density() const {return density_;};

    //! Radiation length:
    double  rad_length() const {return rad_len_;};

    // Nuclear interaction length:
    double inter_length()  const {return nuc_ilen_;};

  private:

    //
    // Basic data members ( To define a material)
    //

    std::string         name_;                 // Material name
    double         density_;              // Material density
    double  rad_len_;                      // Radiation length
    double  nuc_ilen_;                // Nuclear interaction length
  };
  std::ostream& operator << (std::ostream& s, const material& mat);
}
#endif
