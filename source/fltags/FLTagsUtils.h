// FLTagsUtils.h - FLTags utilities
//
// Copyright (c) 2018 by François Mauger <mauger@lpccaen.in2p3.fr>

// Distributed under the OSI-approved BSD 3-Clause License (the "License");
// see accompanying file License.txt for details.
//
// This software is distributed WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the License for more information.

#ifndef FLTAGSUTILS_H
#define FLTAGSUTILS_H

// Standard Library
#include <string>

namespace FLTags {

  // Return the default name for output DOT file
  std::string default_dot_filename();

  // Return the default name for output tag list file
  std::string default_list_filename();

}  // namespace FLTags

#endif  // FLTAGSUTILS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
