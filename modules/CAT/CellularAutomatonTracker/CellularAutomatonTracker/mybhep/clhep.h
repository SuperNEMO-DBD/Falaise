/* -*- mode: c++ -*- */

/*   
 * 
 * Copyright (C) 2003 Irene team
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


#ifndef __CL_H__
#define __CL_H__
#include <CLHEP/Vector/ThreeVector.h>
#include <CLHEP/Vector/LorentzVector.h>
#include <CLHEP/Random/RandGauss.h>
#include <CLHEP/Random/RanluxEngine.h>
#include <mybhep/point.h>

namespace mybhep {
  typedef CLHEP::Hep3Vector Vector3D;
  typedef CLHEP::HepLorentzVector Vector4D;
  typedef mybhep::point Point3D;
}

#endif
