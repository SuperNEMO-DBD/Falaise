/// \file falaise/snemo/digitization/fake_trigger_algo.h
/* Author(s)     : Guillaume Oliviero <goliviero@lpccaen.in2p3.fr>
 *                 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *                 Yves Lemiere <lemiere@lpccaen.in2p3.fr>
 * Creation date : 2016-02-26
 * Last modified : 2014-02-26
 *
 * Copyright (C) 2012-2016 Guillaume Oliviero <goliviero@lpccaen.in2p3.fr>
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
 * Description:
 *
 * An algorithm class that implements a fake trigger to cut SD files.
 * The physical selection for a fake trigger is configurable but the
 * default configuration is 1 PMT hit and 3 Geiger cells hits.
 *
 * History:
 *
 */

// Third party :
// - Bayeux/mctools:
#include <mctools/simulated_data.h>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_FAKE_TRIGGER_ALGO_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_FAKE_TRIGGER_ALGO_H

namespace snemo {

namespace digitization {

/// \brief Fake trigger algorithm class to cut SD files.
class fake_trigger_algo {
 public:
  /// Default constructor :
  fake_trigger_algo();

  /// Destructor
  virtual ~fake_trigger_algo();

  /// Getting initialization flag
  bool is_initialized() const;

  /// Initialize the driver through configuration properties
  void initialize();

  /// Reset
  void reset();

  /// Get the boolean status to know if we keep a SD or not
  bool get_status_for_a_SD();

  /// Data record processing
  bool process(const mctools::simulated_data& SD_);

 protected:
  /// Fake trigger algo process
  bool _process(const mctools::simulated_data& SD_);

 private:
  // Configuration :
  bool _initialized_;      //!< Initialization flag
  bool _status_for_a_SD_;  //!< Flag to know if the fake trigger is passed or not.
};

}  // end of namespace digitization

}  // end of namespace snemo

#endif  // FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_FAKE_TRIGGER_ALGO_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
