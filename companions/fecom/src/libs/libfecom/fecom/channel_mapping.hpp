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

  class channel_mapping
  {

  public :

    // Left value : geometric ID, right value : electronic ID
    typedef boost::bimap< geomtools::geom_id, geomtools::geom_id > ID_bimap;
    typedef ID_bimap::value_type ID_doublet;

    channel_mapping();

    virtual ~channel_mapping();

    bool is_initialized() const;

    void initialize();

    void build_calo_mapping_from_file(const std::string & filename_);

    void build_tracker_mapping_from_file(const std::string & filename_);

		void get_cell_layer_row_for_a_channel(const geomtools::geom_id & electronic_id_,
																					uint16_t & layer_number_,
																					uint16_t & row_number_) const;

		void get_electronics_id_for_a_cell_layer_row(const uint16_t & layer_number_,
																								 const uint16_t & row_number_,
																								 geomtools::geom_id & electronic_anodic_id_,
																								 geomtools::geom_id & electronic_bot_cathodic_id_,
																								 geomtools::geom_id & electronic_top_cathodic_id_) const;

    void get_electronic_id_from_geometric_id(const geomtools::geom_id & geometric_id_,
																						 geomtools::geom_id & electronic_id_) const;

    void get_geometric_id_from_electronic_id(const geomtools::geom_id & electronic_id_,
																						 geomtools::geom_id & geometric_id_) const;


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

    virtual void reset();

    virtual void tree_dump(std::ostream & out_,
                           const std::string & title_ = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const;

  private :

    void _reset_();

  public :

    bool initialized;

    ID_bimap tracker_bimap;
		ID_bimap calo_bimap;

  };

} // namespace fecom

#endif // FECOM_CHANNEL_MAPPING_HPP

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
