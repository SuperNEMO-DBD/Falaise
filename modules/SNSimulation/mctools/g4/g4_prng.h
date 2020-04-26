/// \file mctools/g4/g4_prng.h
/* Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-02-26
 * Last modified : 2013-03-09
 *
 * Copyright (C) 2011-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Description:
 *
 *   GEANT4 PRNG based on mygsl::rng
 *
 * History:
 *
 */

#ifndef SNSIM_G4_PRNG_H
#define SNSIM_G4_PRNG_H 1

// Standard library:
#include <iostream>
#include <string>

// Third party:
// - CLHEP:
#include <CLHEP/Random/RandomEngine.h>

namespace mygsl {
class rng;
}
namespace snsim {

class g4_prng : public CLHEP::HepRandomEngine {
 public:
  static const int SEED_INVALID = -1;
  static const int SEED_TIME = 0;

  /// Constructor
  g4_prng();

  /// Constructor
  g4_prng(mygsl::rng&);

  /// Destructor
  virtual ~g4_prng();

  void set_random(mygsl::rng& rng_);

  virtual double flat();

  virtual void flatArray(const int size, double* vect);

  virtual void setSeed(long seed, int dummy_ = 0);

  virtual void setSeeds(const long* seeds, int index_ = -1);

  virtual void saveStatus(const char filename[] = "G4PRNG.conf") const;

  virtual void restoreStatus(const char filename[] = "G4PRNG.conf");

  virtual void showStatus() const;

  virtual std::string name() const;

 private:
  int _random_seed_;     /// Random seed
  mygsl::rng* _random_;  /// Handle to an external mygsl's PRNG
};

}  // namespace snsim

#endif  // SNSIM_G4_PRNG_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
