//! \file fecom/tracker_hit.hpp
//
// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2016 by Guillaume Oliviéro <goliviero@lpccaen.in2p3.fr>

#ifndef FECOM_TRACKER_CHANNEL_HIT_HPP
#define FECOM_TRACKER_CHANNEL_HIT_HPP

// Standard library:
#include <string>
#include <iostream>

// - Bayeux:
// - Bayeux/datatools:
#include <bayeux/datatools/i_serializable.h>

// This project:
#include <fecom/base_hit.hpp>
#include <fecom/tracker_constants.hpp>

namespace fecom {

  struct tracker_channel_hit
    : public base_hit
  {

    enum channelmode_type {
      INVALID_CHANNEL  = 0,
      ANODIC_CHANNEL   = 1,
      CATHODIC_CHANNEL = 2
    };

    tracker_channel_hit();

    virtual ~tracker_channel_hit();

    bool is_valid() const;

    virtual void reset();

    virtual void tree_dump(std::ostream & out_,
                           const std::string & title_ = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const;


    /// Reset all times (anodic and cathodic)
    void reset_timestamp();

  private:

    void _reset_();

  public:

		// Management :
		bool associated = false;

    // Config :
    uint16_t feast_id; ///< FEAST (0..1)
    uint16_t channel;  ///< Channel (0..15)
    uint16_t event_id; ///< Debug counter (0..255)
    channelmode_type channel_type; ///< Channel type (anodic || cathodic)
    std::string timestamp_type;    ///< Type of the timestamp (t0..t6)

    // Data :
    uint64_t timestamp_value;      ///< Value of the timestamp

		DATATOOLS_SERIALIZATION_DECLARATION()

    friend class tracker_channel_hit_parser;
  };

} // namespace fecom

#endif // FECOM_TRACKER_CHANNEL_HIT_HPP

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
