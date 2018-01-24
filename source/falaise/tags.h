//! \file    falaise/tags.h
//! \brief   Utilities for tag management
//
// Copyright (c) 2018 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of falaise.
//
// falaise is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// falaise is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with falaise.  If not, see <http://www.gnu.org/licenses/>.

#ifndef FALAISE_TAGS_H
#define FALAISE_TAGS_H

// Standard Library:
#include <string>

namespace falaise {

  namespace tags {

    const std::string & experiment_category();

    const std::string & experimental_setup_category();

    const std::string & geometry_setup_category();

    const std::string & device_setup_category();

    const std::string & electronics_setup_category();

    const std::string & cms_setup_category();

    const std::string & simulation_setup_category();

    const std::string & reconstruction_setup_category();

    const std::string & variant_service_category();

    const std::string & services_category();

    const std::string & generic_configuration_category();

    const std::string & generic_data_category();

    const std::string & generic_image_category();

    const std::string & software_category();

    const std::string & firmware_category();

    const std::string & hardware_category();

  } // namespace tags

} // namespace falaise

#endif // FALAISE_TAGS_H
