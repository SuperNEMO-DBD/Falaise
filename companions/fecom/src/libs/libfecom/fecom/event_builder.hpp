//! \file fecom/event_builder.hpp
//
// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2016 by Guillaume Oliviéro <goliviero@lpccaen.in2p3.fr>

#ifndef FECOM_EVENT_BUILDER_HPP
#define FECOM_EVENT_BUILDER_HPP

// Standard library:
#include <string>
#include <iostream>

// - Bayeux:
#include <bayeux/datatools/utils.h>
// #include <bayeux/datatools/i_serializable.h>

// This project:
#include <fecom/commissioning_event.hpp>
#include <fecom/channel_mapping.hpp>
#include <fecom/tracker_constants.hpp>
#include <fecom/tracker_channel_hit.hpp>

namespace fecom {

  /// \brief Half Commissioning Event builder
  class event_builder
  {

  public :

    static constexpr double EVENT_BUILDING_CALO_L1_GATE_IN_NS = 100.0; // 100ns to stack calorimeters hits in a commissioning event
    static constexpr double EVENT_BUILDING_CALO_TRACKER_L2_GATE_IN_NS = 150000.0; // 150µs to build a commissioning event

    // Constructor
    event_builder(double l1_time_gate_ns_ = -1.0,
									double l2_time_gate_ns_ = -1.0);

    // Destructor
    virtual ~event_builder();

    // Set the different times from time start
    void set_times(const double time_start_);

    // Build tracker hits from 7 (max) tracker channels
    void build_tracker_hits_from_channels(const fecom::channel_mapping & channel_mapping_);

    // Reset internal CE for a new serialization
    void reset_serialization();

		// Print configuration
		void print_config(std::ofstream);

		// Configuration:
    double l1_time_gate = 0.0; //!< Time is in implicit ns
    double l2_time_gate = 0.0; //!< Time is in implicit ns

    // Working data:
    fecom::commissioning_event commissioning_event_for_serialization;
    fecom::commissioning_event working_commissioning_event;
    double time_start;     //!< Time is in implicit ns
    double calo_time_stop; //!< Time is in implicit ns
    double l2_time_stop;   //!< Time is in implicit ns
    bool   com_event_is_active     = false;
    bool   ready_for_serialization = false;
    bool   begin_by_calorimeter    = false;

  };

} // end of namespace fecom

#endif // FECOM_EVENT_BUILDER_HPP

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
