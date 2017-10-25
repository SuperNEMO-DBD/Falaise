/* -*- mode: c++ -*- */
/*
 *
 * Copyright 2004
 * J.J. Gomez-Cadenas, J.A. Hernando
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

#ifndef event_cvt_h
#define event_cvt_h

#include <string>
#include <mybhep/event.h>
#include <mybhep/ibconverter.h>

namespace mybhep {
//! event_cvt Class
/*!
  Converter to persistency
*/

class particle_cvt;

class event_cvt : public ibconverter {
 private:
  //! pointer to the event
  event* nevent_;
  //! const pointer to the event
  const event* event_;
  //! std::vector of particle converters for true particles
  std::vector<int> particles_true_;

  //! std::vector of particle converters for digi particles
  std::vector<int> particles_digi_;

  //! std::vector of particle converters for sparticles
  std::vector<int> particles_spar_;

  //! label for particles
  std::string particle_label_;
  std::string sparticle_label_;

  //    char* buf_;
 public:
  //! constructor
  event_cvt(const event& evt, int ind);
  //! constructor from a string
  event_cvt(std::string def);
  //! destructor
  virtual ~event_cvt();

  //! store event
  virtual std::string store();
  //! restore event
  virtual void restore(std::string def);

  //! restore event
  virtual void complete_restore();

  //! constant reference to the event
  const event& retrieve() const;
  //! A new instance of event
  event* create();
};
}  // namespace mybhep
#endif
