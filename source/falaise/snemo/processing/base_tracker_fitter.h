/** \file falaise/snemo/processing/base_tracker_fitter.h
 * Author(s)     : Xavier Garrido <garrido@lal.in2p3.fr>
 * Creation date : 2012-10-07
 * Last modified : 2014-02-09
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
 * Description:
 *
 * History:
 */

#ifndef FALAISE_SNEMO_PROCESSING_BASE_TRACKER_FITTER_H
#define FALAISE_SNEMO_PROCESSING_BASE_TRACKER_FITTER_H 1

// Standard library:
#include <string>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux/datatools:
#include <datatools/logger.h>
#include <datatools/object_configuration_description.h>

// Forward declaration :
namespace datatools {
class properties;
}

namespace geomtools {
class manager;
}

namespace snemo {

namespace geometry {
class gg_locator;
}

namespace datamodel {
class tracker_clustering_data;
class tracker_trajectory_data;
}  // namespace datamodel

namespace processing {

/// \brief The base class for all tracker fitter algorithms
class base_tracker_fitter {
 public:
  /// Default constructor
  base_tracker_fitter(const std::string &name = "anonymous");

  /// Destructor
  virtual ~base_tracker_fitter();

  /// Set logging priority level
  void set_logging_priority(datatools::logger::priority logging_priority_);

  /// Get logging priority
  datatools::logger::priority get_logging_priority() const;

  /// Return the clusterizer ID
  const std::string &get_id() const;

  /// Return the tracker locator
  const snemo::geometry::gg_locator &get_gg_locator() const;

  /// Check the geometry manager
  bool has_geometry_manager() const;

  /// Address the geometry manager
  void set_geometry_manager(const geomtools::manager &gmgr_);

  /// Return a non-mutable reference to the geometry manager
  const geomtools::manager &get_geometry_manager() const;

  /// Check if theclusterizer is initialized
  bool is_initialized() const;

  /// Main tracker trajectory driver
  int process(const snemo::datamodel::tracker_clustering_data &clustering_,
              snemo::datamodel::tracker_trajectory_data &trajectory_);

  /// Initialize the tracker trajectory fitter through configuration properties
  virtual void initialize(const datatools::properties &setup_) = 0;

  /// Reset the tracker trajectory fitter
  virtual void reset() = 0;

  /// Smart print
  void tree_dump(std::ostream &out_ = std::clog, const std::string &title_ = "",
                 const std::string &indent_ = "", bool inherit_ = false) const;

  /// OCD support
  static void ocd_support(datatools::object_configuration_description &,
                          const std::string &prefix_ = "");

 protected:
  /// Initialize the clusterizer through configuration properties
  void _initialize(const datatools::properties &setup_);

  /// Reset the clusterizer
  void _reset();

  /// Set default attribute values
  void _set_defaults();

  /// Set the initialization flag
  void _set_initialized(bool);

  /// Specific fitting algorithm
  virtual int _process_algo(const snemo::datamodel::tracker_clustering_data &clustering_,
                            snemo::datamodel::tracker_trajectory_data &trajectory_) = 0;

  /// Post-processing operation
  virtual int _post_process(snemo::datamodel::tracker_trajectory_data &trajectory_);

 protected:
  datatools::logger::priority _logging_priority;  /// Logging priority threshold

 private:
  bool isInitialized_;                            /// Initialization status
  std::string id_;                              /// Identifier of the fitter algorithm
  const geomtools::manager *geoManager_;  /// The SuperNEMO geometry manager
  const snemo::geometry::gg_locator
      *geigerLocator_;                /// Locator dedicated to the SuperNEMO tracking chamber
  size_t maxFitsToSave_;  /// The maximum number of fits to be saved
};

}  // end of namespace processing

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_PROCESSING_BASE_TRACKER_FITTER_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
