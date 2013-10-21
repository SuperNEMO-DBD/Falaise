//! \file    flsimulatemain.cc
//! \brief   Main program for flreconstruct command line application
//! \details Configure, setup and run the bayeux::dpp based pipeline
//!          reconstruction of SuperNEMO data.
//
//
// Copyright (c) 2013 by Ben Morgan <bmorgan.warwick@gmail.com>
// Copyright (c) 2013 by The University of Warwick
//
// This file is part of Falaise.
//
// Falaise is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Falaise is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Falaise.  If not, see <http://www.gnu.org/licenses/>.

// Standard Library

// Third Party
// - A

// This Project
#include "falaise/falaise.h"
#include "falaise/exitcodes.h"

//----------------------------------------------------------------------
// IMPLEMENTATION DETAILS
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//! Run reconstruction using command line args as given
falaise::exit_code do_flreconstruct(int argc, char *argv[]) {
  return falaise::EXIT_UNAVAILABLE;
}

//----------------------------------------------------------------------
// MAIN PROGRAM
//----------------------------------------------------------------------
int main(int argc, char *argv[]) {
  // - Needed...
  FALAISE_INIT();

  // - Do the reconstruction
  // Ideally, exceptions should not propagate out of this - the error
  // code should be enough, but might want a catch all...
  falaise::exit_code ret = do_flreconstruct(argc, argv);

  // - Needed...
  FALAISE_FINI();
  return ret;
}

