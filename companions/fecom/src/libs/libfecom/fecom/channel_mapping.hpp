//! \file fecom/channel_mapping.hpp
//
// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2016 by Guillaume Oliviéro <goliviero@lpccaen.in2p3.fr>

#ifndef FECOM_CHANNEL_MAPPING_HPP
#define FECOM_CHANNEL_MAPPING_HPP

// Standard library:
#include <string>
#include <iostream>

// - Boost:
#include <boost/bimap.hpp>

namespace fecom {

  class channel_mapping
  {

  public :

		struct channel_triolet
		{

			// int compare (const channel_trioletgeom_id & id1_, const geom_id & id2_)

			uint16_t feast_1;
			uint16_t channel_1;
			uint16_t feast_2;
			uint16_t channel_2;
			uint16_t feast_3;
			uint16_t channel_3;
		};

    channel_mapping();

    virtual ~channel_mapping();

		void cell_to_channel_builder();

    /// Get associated channels for a given channel (same for the two FEAST)
		void get_associated_channels(const uint16_t feast_id_,
																 const uint16_t channel_id_,
																 uint16_t & associated_feast_1_,
																 uint16_t & associated_channel_1_,
																 uint16_t & associated_feast_2_,
																 uint16_t & associated_channel_2_) const;

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

		std::map<uint16_t, fecom::channel_mapping::channel_triolet> _cell_channel_mapping_;

  };

} // namespace fecom

#endif // FECOM_CHANNEL_MAPPING_HPP

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
