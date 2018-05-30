//! \file fecom/utils.hpp
//
// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>

#ifndef FECOM_UTILS_HPP
#define FECOM_UTILS_HPP

// Standard library:
#include <string>

namespace fecom {

  namespace io {

    std::string tag();

    std::string last_tag();

    std::string skip_tag();

    std::string last_skip_tag();

    std::string inherit_last_tag(bool inherit_);

  } // namespace io

} // namespace fecom

#endif // FECOM_UTILS_HPP

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
