// -*- mode: c++ ; -*-
/* data_model.h
 * Author (s) :     Xavier Garrido <<garrido@lal.in2p3.fr>>
 * Creation date: 2010-08-03
 * Last modified: 2010-08-03
 *
 * Copyright (C) 2011 Xavier Garrido <garrido@lal.in2p3.fr>
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
 *  Typedef to ease the use of sncore objects
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_VISUALIZATION_IO_DATA_MODEL_H
#define FALAISE_SNEMO_VISUALIZATION_IO_DATA_MODEL_H 1

#include <falaise/snemo/datamodels/unified_digitized_data.h>
#include <falaise/snemo/datamodels/calibrated_data.h>
#include <falaise/snemo/datamodels/data_model.h>
#include <falaise/snemo/datamodels/event_header.h>
#include <falaise/snemo/datamodels/particle_track_data.h>
#include <falaise/snemo/datamodels/tracker_clustering_data.h>
#include <falaise/snemo/datamodels/tracker_trajectory_data.h>
#include <mctools/simulated_data.h>

namespace snemo {

namespace visualization {

namespace io {

// data container
typedef datatools::things event_record;

// event header
const std::string EH_LABEL = snedm::labels::event_header();

// simulated stuff
const std::string SD_LABEL = snedm::labels::simulated_data();

// unified digitized stuff
const std::string UDD_LABEL = snedm::labels::unified_digitized_data();

// calibrated stuff
const std::string CD_LABEL = snedm::labels::calibrated_data();

// tracker clustering stuff
const std::string TCD_LABEL = snedm::labels::tracker_clustering_data();

// tracker trajectory stuff
const std::string TTD_LABEL = snedm::labels::tracker_trajectory_data();

// particle track stuff
const std::string PTD_LABEL = snedm::labels::particle_track_data();

}  // end of namespace io

}  // end of namespace visualization

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_VISUALIZATION_IO_DATA_MODEL_H

// end of data_model.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
