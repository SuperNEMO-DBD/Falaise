//! \file fecom/channel_mapping.hpp
//
// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2016 by Guillaume Oliviéro <goliviero@lpccaen.in2p3.fr>

#ifndef FECOM_CHANNEL_MAPPING_HPP
#define FECOM_CHANNEL_MAPPING_HPP

// Standard library:
#include <string>
#include <iostream>


namespace fecom {

  class channel_mapping
  {

  public :

    channel_mapping();

    virtual ~channel_mapping();

    /// Get associated channels for a given channel (same for the two FEAST)
    void get_associated_channels(const uint16_t channel_id_,
																 uint16_t & associated_channel_1_,
																 uint16_t & associated_channel_2_);

    /// Check if the channel is anodic
    bool is_anodic_channel(const uint16_t channel_id_);

    /// Check if the channel is bottom cathodic
    bool is_bottom_cathodic_channel(const uint16_t channel_id_);

    /// Check if the channel is top cathodic
    bool is_top_cathodic_channel(const uint16_t channel_id_);

    virtual void reset();

    virtual void tree_dump(std::ostream & out_,
                           const std::string & title_ = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const;

	private :

    void _reset_();

  };

} // namespace fecom

#endif // FECOM_CHANNEL_MAPPING_HPP

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
