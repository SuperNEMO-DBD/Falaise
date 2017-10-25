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

#ifndef __SPARTICLE_CVT_H__
#define __SPARTICLE_CVT_H__

#include <string>
#include <vector>
#include <mybhep/sparticle.h>
#include <mybhep/ibconverter.h>

namespace mybhep {

//! class sparticle_cvt
/*!
  Converter to persistency
*/

typedef sparticle sparticle;

class sparticle_cvt : public ibconverter {
 private:
  //! pointer to the sparticle
  sparticle* nsparticle_;
  //! const pointer to the sparticle
  const sparticle* sparticle_;

  //! integer for seed particle
  int iseed_;

  //! integer for subparticles
  std::vector<int> isubp_;

  //! label por particles
  std::string particle_label_;

 public:
  //! construct from a sparticle and an index
  sparticle_cvt(const sparticle& t, int inx);
  //! construct from a string
  sparticle_cvt(std::string);
  //! destructor
  virtual ~sparticle_cvt();

  //! store class in a string
  virtual std::string store();
  //! restore from a string
  virtual void restore(std::string);
  //! complete restore
  virtual void complete_restore();

  //! retrieve a constant reference to the sparticle
  const sparticle& retrieve() const;
  //! retrieve a reference to the sparticle
  sparticle& reference();

  //! instantiate a new sparticle
  sparticle* create();
};
}  // namespace mybhep
#endif
