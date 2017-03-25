// FLVisualizeCommandLine.h - Interface for FLVisualize command line
//
// Copyright (c) 2017 by François Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2017 by Université de Caen Normandie

// Distributed under the OSI-approved BSD 3-Clause License (the "License");
// see accompanying file License.txt for details.
//
// This software is distributed WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the License for more information.

#ifndef FLVISUALIZECOMMANDLINE_H
#define FLVISUALIZECOMMANDLINE_H

// Standard library:
#include <cstdlib>
#include <iostream>

// Third party:
// - Boost
#include <boost/program_options.hpp>

// This project:
#include "FLVisualizeArgs.h"

namespace FLVisualize {

  //! Exceptions for dialog
  class FLDialogHelpRequested : public std::exception {};
  class FLDialogOptionsError : public std::exception {};

  //! Initialization flags for kernel setup
  uint32_t app_kernel_init_flags();

  //! Handle command line argument dialog
  void do_cldialog(int argc_, char *argv_[], FLVisualizeArgs& params_);

  //! Handle printing of help message to screen
  void do_help(const boost::program_options::options_description& od);

  //! Handle printing of version information to given ostream
  void do_version(std::ostream& os, bool isVerbose);


} // namespace FLVisualize

#endif // FLVISUALIZECOMMANDLINE_H

  // Local Variables: --
  // mode: c++ --
  // c-file-style: "gnu" --
  // tab-width: 2 --
  // End: --
