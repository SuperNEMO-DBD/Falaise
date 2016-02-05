/** \file falaise/snemo/reconstruction/gamma_clustering_driver.h
 * Author(s)     : Xavier Garrido <garrido@lal.in2p3.fr>
 * Creation date : 2012-10-07
 * Last modified : 2014-02-09
 *
 * Copyright (C) 2012-2014 Xavier Garrido <garrido@lal.in2p3.fr>
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
 *   A driver class that wraps the GammaClustering algorithm.
 *
 * History:
 *
 */

#ifndef FALAISE_GAMMA_CLUSTERING_PLUGIN_SNEMO_RECONSTRUCTION_GAMMA_CLUSTERING_DRIVER_H
#define FALAISE_GAMMA_CLUSTERING_PLUGIN_SNEMO_RECONSTRUCTION_GAMMA_CLUSTERING_DRIVER_H 1

// Standard library:
#include <string>

// This project:
#include <falaise/snemo/datamodels/calibrated_calorimeter_hit.h>
#include <falaise/snemo/datamodels/calibrated_data.h>
#include <falaise/snemo/processing/base_gamma_builder.h>

namespace snemo {

  namespace reconstruction {

    /// Driver for the gamma clustering algorithms
    class gamma_clustering_driver : public ::snemo::processing::base_gamma_builder
    {
    public:
      /// Typedef for list of geom ids
      typedef std::vector<geomtools::geom_id> gid_list_type;

      /// Typedef for time ordered calorimeter hits aka. gamma cluster
      typedef std::map<double, const snemo::datamodel::calibrated_data::calorimeter_hit_handle_type> cluster_type;

      /// Typedef for collection of clusters
      typedef std::vector<cluster_type> cluster_collection_type;

      /// Dedicated algorithm id
      static const std::string & gamma_clustering_id();

      /// Constructor
      gamma_clustering_driver();

      /// Destructor
      virtual ~gamma_clustering_driver();

      /// Initialize the gamma tracker through configuration properties
      virtual void initialize(const datatools::properties & setup_);

      /// Reset the clusterizer
      virtual void reset();

      /// OCD support:
      static void init_ocd(datatools::object_configuration_description & ocd_);

    protected:

      /// Give default values to specific class members.
      void _set_defaults ();

      /// Special method to process and generate particle track data
      virtual int _process_algo(const base_gamma_builder::hit_collection_type & calo_hits_,
                                snemo::datamodel::particle_track_data & ptd_);

      /// Get calorimeter neighbours given teh current calorimeter hit
      virtual void _get_geometrical_neighbours(const snemo::datamodel::calibrated_calorimeter_hit & hit_,
                                               const snemo::datamodel::calibrated_data::calorimeter_hit_collection_type & hits_,
                                               cluster_type & cluster_,
                                               gid_list_type & registered_calos_) const;

      /// Split calorimeter cluster given a cluster time range value
      virtual void _get_time_neighbours(cluster_type & cluster_, cluster_collection_type & clusters_) const;

      /// Associate clusters given Time-Of-Flight calculation
      virtual void _get_tof_association(const cluster_collection_type & from_clusters_,
                                        cluster_collection_type & to_clusters_) const;

      /// Return Time-Of-Flight probability between 2 calorimeter hits
      virtual double _get_tof_probability(const snemo::datamodel::calibrated_calorimeter_hit & head_end_calo_hit_,
                                          const snemo::datamodel::calibrated_calorimeter_hit & tail_begin_calo_hit_) const;

      /// Check if 2 calorimeter hits belong to the same wall
      virtual bool _are_on_same_wall(const snemo::datamodel::calibrated_calorimeter_hit & head_end_calo_hit_,
                                     const snemo::datamodel::calibrated_calorimeter_hit & tail_begin_calo_hit_) const;

    private:
      double _cluster_time_range_;     //!< The time condition for clustering
      std::string _cluster_grid_mask_; //!< The spatial condition for clustering
      double _min_prob_;               //!< The minimal probability required between clusters
      double _sigma_time_good_calo_;   //!< The minimal time resolution to consider calorimeter hit
    };

  }  // end of namespace reconstruction

}  // end of namespace snemo


// Declare the OCD interface of the module
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(snemo::reconstruction::gamma_clustering_driver)

#endif // FALAISE_GAMMA_CLUSTERING_PLUGIN_SNEMO_RECONSTRUCTION_GAMMA_CLUSTERING_DRIVER_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
