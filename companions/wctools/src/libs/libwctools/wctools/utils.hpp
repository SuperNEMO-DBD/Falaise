//! \file wctools/utils.hpp
//! \brief Wavecatcher utils
//
// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>

#ifndef WCTOOLS_UTILS_HPP
#define WCTOOLS_UTILS_HPP

// Standard library:
#include <string>

namespace wctools {

//!< Expand a string with respect to environment
void expand_environ(std::string &);

}  // namespace wctools

#endif  // WCTOOLS_UTILS_HPP

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
