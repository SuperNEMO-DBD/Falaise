/* g4_prng.cc
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
 */

// Ourselves:
#include <snsim/g4_prng.h>

// Third party:
// - Bayeux/datatools:
#include <bayeux/datatools/logger.h>
// - Bayeux/mygsl:
#include <bayeux/mygsl/rng.h>

namespace snsim {

const int g4_prng::SEED_INVALID;
const int g4_prng::SEED_TIME;

void g4_prng::set_random(mygsl::rng& rng_) { _random_ = &rng_; }

g4_prng::g4_prng() {
  _random_seed_ = SEED_INVALID;
  _random_ = nullptr;
}

g4_prng::g4_prng(mygsl::rng& rng_) {
  _random_seed_ = SEED_INVALID;
  _random_ = &rng_;
}

g4_prng::~g4_prng() = default;

auto g4_prng::flat() -> double { return _random_->uniform(); }

void g4_prng::flatArray(const int size_, double* vect_) {
  for (int i = 0; i < size_; i++) {
    vect_[i] = _random_->uniform();
  }
}

void g4_prng::setSeed(long seed_, int /*dummy_*/) {
  _random_seed_ = seed_;
  _random_->set_seed(seed_);
}

void g4_prng::setSeeds(const long* seeds_, int index_) {
  if (index_ != 0) {
    DT_LOG_WARNING(datatools::logger::PRIO_WARNING, "Ignoring index value !");
  }
  _random_->set_seed(seeds_[0]);
}

void g4_prng::saveStatus(const char filename_[]) const { _random_->store(std::string(filename_)); }

void g4_prng::restoreStatus(const char filename_[]) { _random_->load(std::string(filename_)); }

void g4_prng::showStatus() const {
  _random_->to_stream(std::clog);
  std::clog << std::endl;
}

auto g4_prng::name() const -> std::string { return "snsim::g4_prng"; }

}  // namespace snsim

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
