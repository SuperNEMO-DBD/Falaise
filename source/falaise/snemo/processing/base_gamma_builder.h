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

#ifndef FALAISE_SNEMO_PROCESSING_BASE_GAMMA_BUILDER_H
#define FALAISE_SNEMO_PROCESSING_BASE_GAMMA_BUILDER_H 1

// Standard library:
#include <string>

// Third party:
// - Boost:
// - Bayeux/datatools:
#include <bayeux/datatools/logger.h>
#include <bayeux/datatools/object_configuration_description.h>

// Falaise:
#include <falaise/snemo/datamodels/calibrated_data.h>

// Forward declaration :
namespace datatools {
class properties;
}

namespace geomtools {
class manager;
}

namespace snemo {

namespace datamodel {
class particle_track_data;
}

namespace geometry {
class locator_plugin;
class calo_locator;
class xcalo_locator;
class gveto_locator;
}  // namespace geometry

namespace processing {

/// \brief The base class for all gamma builder algorithms
class base_gamma_builder {
 public:
  /// Typedef to calibrated calorimeter hits
  using hit_collection_type = snemo::datamodel::calibrated_data::calorimeter_hit_collection_type;

  /// Default constructor
  base_gamma_builder(const std::string &name = "anonymous");

  /// Destructor
  virtual ~base_gamma_builder();

  /// Set logging priority level
  void set_logging_priority(datatools::logger::priority logging_priority_);

  /// Get logging priority
  datatools::logger::priority get_logging_priority() const;

  /// Return the gamma builder ID
  const std::string &get_id() const;

  /// Return the main wall calorimeter locator
  const snemo::geometry::calo_locator &get_calo_locator() const;

  /// Return the X-wall calorimeter locator
  const snemo::geometry::xcalo_locator &get_xcalo_locator() const;

  /// Return the gamma veto calorimeter locator
  const snemo::geometry::gveto_locator &get_gveto_locator() const;

  /// Check the geometry manager
  bool has_geometry_manager() const;

  /// Address the geometry manager
  void set_geometry_manager(const geomtools::manager &gmgr_);

  /// Return a non-mutable reference to the geometry manager
  const geomtools::manager &get_geometry_manager() const;

  /// Check if theclusterizer is initialized
  bool is_initialized() const;

  /// Main tracker trajectory driver
  int process(const base_gamma_builder::hit_collection_type &calo_hits_,
              snemo::datamodel::particle_track_data &ptd_);

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

  /// Clear working arrays
  void _clear_working_arrays();

  /// Prepare data for processing
  virtual int _prepare_process(const base_gamma_builder::hit_collection_type &calo_hits_,
                               snemo::datamodel::particle_track_data &ptd_);

  /// Specific algorithm
  virtual int _process_algo(const base_gamma_builder::hit_collection_type &calo_hits_,
                            snemo::datamodel::particle_track_data &ptd_) = 0;

  /// Post processing
  virtual int _post_process(const base_gamma_builder::hit_collection_type &calo_hits_,
                            snemo::datamodel::particle_track_data &ptd_);

 protected:
  datatools::logger::priority _logging_priority;  //!< Logging priority threshold

 private:
  bool isInitialized_;                    //!< Initialization status
  std::string id_;                        //!< Identifier of the gamma builder algorithm
  const geomtools::manager *geoManager_;  //!< The SuperNEMO geometry manager
  const snemo::geometry::locator_plugin *geoLocator_;  //!< The SuperNEMO locator plugin

  hit_collection_type usedHits_;     //!< Hits that are used as input for any gamma algorithm
  hit_collection_type ignoredHits_;  //!< Hits that are not used as input for any gamma algorithm

  bool extrapolateFoilVertex_;       //!< Flag to enable foil vertex extrapolation
  double minFoilVertexProbability_;  //!< Minimal TOF internal probability to accept
                                     //!< foil vertex extrapolation

  bool tagAnnihilationGamma_;  //!< Flag to enable tagging of gamma from e+/e- annihilation
  double minAnnihilationGammaProbability_;  //!< Minimal TOF probability to tag
                                            //!< gamma from annihilation

  bool selectCaloHits_;                   //!< Flag to select calorimeter hits based on tags
  std::vector<std::string> caloHitTags_;  //!< Tags to use in selecting calorimeter hits
};

}  // end of namespace processing

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_PROCESSING_BASE_GAMMA_BUILDER_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
