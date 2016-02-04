/// \file falaise/TrackerPreClustering/interface.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-03-30
 * Last modified: 2014-02-07
 *
 * Copyright 2012-2014 F. Mauger
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
 * You should have received a copy of the GNU General Public  License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Description:
 *
 *  Description of the setup/input/output interface data for the
 *  Tracker Pre-Clustering templatized algorithm.
 *
 */

#ifndef FALAISE_TRACKERPRECLUSTERING_INTERFACE_H
#define FALAISE_TRACKERPRECLUSTERING_INTERFACE_H 1

// Standard library:
#include <string>
#include <limits>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux/datatools:
#include <datatools/logger.h>

namespace TrackerPreClustering {

  /// \brief Setup data for the TrackerPreClustering algorithm
  struct setup_data
  {
    /// Return the last error message
    const std::string & get_last_error_message() const;

    /// Set the last error message
    void set_last_error_message(const std::string & message_);

    /// Default constructor
    setup_data();

    /// Reset
    void reset();

    /// Check the setup data
    bool check() const;

    // Attributes:
    datatools::logger::priority logging; //!< Logging flag
    double cell_size;                    //!< The dimension of a cell
    double delayed_hit_cluster_time;     //!< Delayed hit cluster time
    bool   processing_prompt_hits;       //!< Flag for processing of prompt hits
    bool   processing_delayed_hits;      //!< Flag for processing of delayed hits
    bool   split_chamber;                //!< Flag to split the chamber

  protected:

    std::string _last_error_message; /// The last error message

  };

  /// \brief Input data structure
  template <class Hit>
  struct input_data
  {
    // Typedefs:
    typedef Hit                           hit_type;
    typedef std::vector<const hit_type *> hit_collection_type;

    /// Return the last error message
    const std::string & get_last_error_message() const;

    /// Set the last error message
    void set_last_error_message(const std::string & message_);

    /// Default constructor
    input_data();

    /// Reset
    void reset();

    /// Check
    bool check() const;

    // Attributes:
    hit_collection_type hits; //!< Collection of Geiger hits

  protected:

    std::string _last_error_message; //!< The last error message at check

  };

  /// \brief Output data structure
  template <class Hit>
  struct output_data
  {
    // Typedefs:
    typedef Hit                              hit_type;
    typedef std::vector<const hit_type *>    hit_collection_type;
    typedef std::vector<hit_collection_type> cluster_collection_type;

    /// Default constructor
    output_data();

    /// Reset
    void reset();

    /// Print
    void dump(std::ostream & out_) const;

    // Attributes:
    hit_collection_type     ignored_hits;     //!< Collection of ignored hits
    cluster_collection_type prompt_clusters;  //!< Collection of prompt clusters
    cluster_collection_type delayed_clusters; //!< Collection of delayed clusters

   };

} // end of namespace TrackerPreClustering

#include <TrackerPreClustering/interface.tpp>

#endif // FALAISE_TRACKERPRECLUSTERING_INTERFACE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
