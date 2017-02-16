//! \file fecom/channel_mapping_with_geom.hpp
//
// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2016 by Guillaume Oliviéro <goliviero@lpccaen.in2p3.fr>

#ifndef FECOM_CHANNEL_MAPPING_WITH_GEOM_HPP
#define FECOM_CHANNEL_MAPPING_WITH_GEOM_HPP

// Standard library:
#include <string>
#include <iostream>

// - Bayeux:
// - Bayeux/geomtools:
#include <bayeux/geomtools/geom_id.h>

// - Boost:
#include <boost/bimap.hpp>

namespace fecom {

  class channel_mapping_with_geom
  {

  public :

    // Left value : geometric ID, right value : electronic ID
    typedef boost::bimap< geomtools::geom_id, geomtools::geom_id > ID_bimap;
    typedef ID_bimap::value_type ID_doublet;

    channel_mapping_with_geom();

    virtual ~channel_mapping_with_geom();

    bool is_initialized() const;

    void initialize();

    void build_mapping_from_file(const std::string & filename_);

    void get_cell_layer_row_for_a_channel(const uint16_t input_feast_,
					  const uint16_t input_channel_,
					  uint16_t & layer_number_,
					  uint16_t & row_number_) const;

    void get_electronic_id_from_geometric_id(const geomtools::geom_id geometric_id_,
					     geomtools::geom_id & electronic_id_) const;

    void get_geometric_id_from_electronic_id(const geomtools::geom_id electronic_id_,
					     geomtools::geom_id & geometric_id_) const;

    /// Check if the channel is anodic
    bool is_anodic_channel(const uint16_t feast_id_,
			   const uint16_t channel_id_) const;

    /// Check if the channel is bottom cathodic
    bool is_bottom_cathodic_channel(const uint16_t feast_id_,
				    const uint16_t channel_id_) const;

    /// Check if the channel is top cathodic
    bool is_top_cathodic_channel(const uint16_t feast_id_,
				 const uint16_t channel_id_) const;

    virtual void reset();

    virtual void tree_dump(std::ostream & out_,
                           const std::string & title_ = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const;

  private :

    void _reset_();

  public :

    bool initialized;

    ID_bimap gg_bimap;

  };

} // namespace fecom

#endif // FECOM_CHANNEL_MAPPING_WITH_GEOM_HPP

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
