//! \file fecom/channel_mapping.hpp
//
// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2016 by Guillaume Oliviéro <goliviero@lpccaen.in2p3.fr>

#ifndef FECOM_CHANNEL_MAPPING_HPP
#define FECOM_CHANNEL_MAPPING_HPP

// Standard library:
#include <string>
#include <iostream>

// - Bayeux:
// - Bayeux/geomtools:
#include <bayeux/geomtools/geom_id.h>

// - Boost:
#include <boost/bimap.hpp>

namespace fecom {

	/// \brief Channel mapping manager. Load the mapping from a file
	// and build some bimaps for calorimeters and trackers

  class channel_mapping
  {

  public :

    // Left value : geometric ID, right value : electronic ID
		/// Bimap between two geometric ID, Geometric mapping <-> Electronic mapping
    typedef boost::bimap< geomtools::geom_id, geomtools::geom_id > ID_bimap;

    typedef ID_bimap::value_type ID_doublet;

		/// Default constructor
    channel_mapping();

		/// Destructor
    virtual ~channel_mapping();

		/// Check if the channel mapping is initialized
    bool is_initialized() const;

		/// Initialize the channel mapping
    void initialize();

		/// Build the calorimeter mapping from a file
    void build_calo_mapping_from_file(const std::string & filename_);

		/// Build the calorimeter mapping from a file
    void build_tracker_mapping_from_file(const std::string & filename_);

		/// Get the cell layer and row for a given channel
		void get_cell_layer_row_for_a_channel(const geomtools::geom_id & electronic_id_,
																					uint16_t & layer_number_,
																					uint16_t & row_number_) const;

		/// Get the electronics ID associated to a cell (layer, row)
		void get_electronics_id_for_a_cell_layer_row(const uint16_t & layer_number_,
																								 const uint16_t & row_number_,
																								 geomtools::geom_id & electronic_anodic_id_,
																								 geomtools::geom_id & electronic_bot_cathodic_id_,
																								 geomtools::geom_id & electronic_top_cathodic_id_) const;

		/// Get an associated electronic ID from a geometric ID
    void get_electronic_id_from_geometric_id(const geomtools::geom_id & geometric_id_,
																						 geomtools::geom_id & electronic_id_) const;

		/// Get an associated geometric ID from an electronic ID
    void get_geometric_id_from_electronic_id(const geomtools::geom_id & electronic_id_,
																						 geomtools::geom_id & geometric_id_) const;

		/// Given an electronic for the tracker, return the anodic, bottom cathodic and top cathodic associated ID
    void get_associated_electronics_id(const geomtools::geom_id & input_electronic_id_,
																			 geomtools::geom_id & electronic_anodic_id_,
																			 geomtools::geom_id & electronic_bot_cathodic_id_,
																			 geomtools::geom_id & electronic_top_cathodic_id_) const;

    /// Check if the channel is anodic
    bool is_anodic_channel(const geomtools::geom_id & input_electronic_id_) const;

    /// Check if the channel is bottom cathodic
    bool is_bottom_cathodic_channel(const geomtools::geom_id & input_electronic_id_) const;

    /// Check if the channel is top cathodic
    bool is_top_cathodic_channel(const geomtools::geom_id & input_electronic_id_) const;

		/// Reset
    virtual void reset();

		/// Smart print
    virtual void tree_dump(std::ostream & out_,
                           const std::string & title_ = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const;

  private :

    void _reset_();

  public :

		// Management
    bool initialized; ///< Flag for initialization

    ID_bimap tracker_bimap; ///< Tracker GID<->EID bimap
		ID_bimap calo_bimap;    ///< Calo GID <-> EID bimap

  };

} // namespace fecom

#endif // FECOM_CHANNEL_MAPPING_HPP

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
