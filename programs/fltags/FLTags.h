// FLTags.h - The FLTags class
//
// Copyright (c) 2018 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// Distributed under the OSI-approved BSD 3-Clause License (the "License");
// see accompanying file License.txt for details.
//
// This software is distributed WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the License for more information.

#ifndef FLTAGS_H
#define FLTAGS_H

// This Project:
#include "FLTagsArgs.h"

namespace FLTags {

  //! \brief FLTags main application
  class fltags
  {
  public:

    static const std::string & default_action();

    fltags(const FLTags::FLTagsArgs & args);

    void run();

  private:

    void _run_print_list_();

    void _run_generate_graph_();

  private:

    const FLTags::FLTagsArgs & _args_;
  };

}  // namespace FLTags

#endif  // FLTAGSARGS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
