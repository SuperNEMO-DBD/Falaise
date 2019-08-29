// -*- mode: c++ ; -*-
/** \file falaise/snemo/reconstruction/gamma_tracking_module.h
 * Author(s) :    Xavier Garrido <garrido@lal.in2p3.fr>
 * Creation date: 2012-10-07
 * Last modified: 2014-02-28
 *
 * Copyright (C) 2011-2014 Xavier Garrido <garrido@lal.in2p3.fr>
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
 *
 * Description:
 *
 *   Module for gamma tracking
 *
 * History:
 *
 */

#include "falaise/snemo/datamodels/data_model.h"
#include "falaise/snemo/datamodels/event.h"
#include "falaise/snemo/datamodels/particle_track_data.h"
#include "falaise/snemo/processing/module.h"

#include "falaise/snemo/services/geometry.h"
#include "falaise/snemo/services/service_handle.h"

#include "GammaTracking/gamma_tracking_driver.h"

namespace snemo {

namespace reconstruction {

/// \brief The data processing module for the gamma tracking
class gamma_tracking_module {
 public:
  /// Default Constructor
  gamma_tracking_module() = default;

  /// Construction with configuration
  gamma_tracking_module(const falaise::property_set& ps, datatools::service_manager& services);

  /// Data record processing
  falaise::processing::status process(datatools::things& event);

 private:
  snemo::service_handle<snemo::geometry_svc> geoSVC_;  //!< The geometry manager
  std::string PTD_tag_;                                //!< The label of the input/output data bank
  snemo::reconstruction::gamma_tracking_driver algo_;  //!< Handle fitter algorithm
};

gamma_tracking_module::gamma_tracking_module(const falaise::property_set& ps,
                                             datatools::service_manager& services)
    : geoSVC_{services},
      PTD_tag_{ps.get<std::string>("PTD_label", snedm::labels::particle_track_data())} {
  algo_.set_geometry_manager(*(geoSVC_.operator->()));
  algo_.initialize(ps);
}

falaise::processing::status gamma_tracking_module::process(datatools::things& event) {
  auto& ptd = snedm::getOrAddToEvent<snemo::datamodel::particle_track_data>(PTD_tag_, event);

  algo_.process(ptd.isolatedCalorimeters(), ptd);
  return falaise::processing::status::PROCESS_SUCCESS;
}

}  // end of namespace reconstruction

}  // end of namespace snemo

FALAISE_REGISTER_MODULE(snemo::reconstruction::gamma_tracking_module)
