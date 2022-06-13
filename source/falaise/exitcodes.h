//! \file    exitcodes.h
//! \brief   Provide commonly used exit status codes
//! \details Applications need a way to communicate exit status to
//!          their caller. Provide a common set of exit statuses
//!          based on the 4.3BSD sysexits.h
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

#ifndef FALAISE_EXITCODES_H
#define FALAISE_EXITCODES_H

namespace falaise {
//! enumerator holding exit code values
enum exit_code {
  EXIT_OK = 0,           ///< Successful termination
  EXIT_ERROR = 1,        ///< Failed termination
  EXIT_USAGE = 64,       ///< Usage error, e.g. incorrect command line
  EXIT_UNAVAILABLE = 69  ///< Service unavailable
};
}  // namespace falaise

#endif  // FALAISE_EXITCODES_H
