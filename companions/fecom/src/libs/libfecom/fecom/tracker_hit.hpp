//! \file fecom/tracker_hit.hpp
//
// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2016 by Guillaume Oliviéro <goliviero@lpccaen.in2p3.fr>

#ifndef FECOM_TRACKER_HIT_HPP
#define FECOM_TRACKER_HIT_HPP

// Standard library:
#include <string>
#include <iostream>

// - Bayeux:
#include <bayeux/datatools/utils.h>
// #include <bayeux/datatools/i_serializable.h>

// This project:
#include <fecom/base_hit.hpp>
#include <fecom/tracker_constants.hpp>
#include <fecom/tracker_channel_hit.hpp>

namespace fecom {

	//! \brief commissioning tracker hit, inherit from base
  struct tracker_hit
    : public base_hit
  {
		/// Default constructor
    tracker_hit();

		/// Destructor
    virtual ~tracker_hit();

		/// Check if the tracker hit is valid
    bool is_valid() const;

		/// Add an anodic tracker channel hit
		void add_anodic_tracker_channel(const fecom::tracker_channel_hit & a_tracker_channel_);

		/// Add a bottom cathodic tracker channel hit
		void add_bot_cathodic_tracker_channel(const fecom::tracker_channel_hit & a_tracker_channel_);

		/// Add a top cathodic tracker channel hit
		void add_top_cathodic_tracker_channel(const fecom::tracker_channel_hit & a_tracker_channel_);

		/// Reset
    virtual void reset();

		/// Smart print
    virtual void tree_dump(std::ostream & out_,
                           const std::string & title_ = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const;

		/// Get hit timestamp (based on anode R0)
		double get_timestamp() const;

		/// Check if tracker hit has geom id
		// bool has_geom_id() const;

    /// Check if has anodic time 0
    bool has_anodic_t0() const;

    /// Set anodic time 0
    void set_anodic_timestamp_t0(const uint64_t value_);

    // /// Get anodic time 0
    // uint64_t get_anodic_t0() const;

    /// Reset anodic time 0
    void reset_anodic_t0();

    /// Check if has anodic time 1
    bool has_anodic_t1() const;

    /// Set anodic time 1
    void set_anodic_timestamp_t1(const uint64_t value_);

    // /// Get anodic time 1
    // uint64_t get_anodic_t1() const;

    /// Reset anodic time 1
    void reset_anodic_t1();

    /// Check if has anodic time 2
    bool has_anodic_t2() const;

    /// Set anodic time 2
    void set_anodic_timestamp_t2(const uint64_t value_);

    // /// Get anodic time 2
    // uint64_t get_anodic_t2() const;

    /// Reset anodic time 2
    void reset_anodic_t2();

    /// Check if has anodic time 3
    bool has_anodic_t3() const;

    /// Set anodic time 3
    void set_anodic_timestamp_t3(const uint64_t value_);

    // /// Get anodic time 3
    // uint64_t get_anodic_t3() const;

    /// Reset anodic time 3
    void reset_anodic_t3();

    /// Check if has anodic time 4
    bool has_anodic_t4() const;

    /// Set anodic time 4
    void set_anodic_timestamp_t4(const uint64_t value_);

    // /// Get anodic time 4
    // uint64_t get_anodic_t4() const;

    /// Reset anodic time 4
    void reset_anodic_t4();

    /// Check if has cathodic time 5
    bool has_bot_cathodic_time() const;

    /// Set cathodic time 5
    void set_bot_cathodic_timestamp(const uint64_t value_);

    // /// Get cathodic time 5
    // uint64_t get_bot_cathodic_time() const;

    /// Reset anodic time 5
    void reset_bot_cathodic_time();

    /// Check if has cathodic time 6
    bool has_top_cathodic_time() const;

    /// Set cathodic time 6
    void set_top_cathodic_timestamp(const uint64_t value_);

    // /// Get cathodic time 6
    // uint64_t get_top_cathodic_time() const;

    /// Reset anodic time 6
    void reset_top_cathodic_time();

    /// Set all anodic times
    void set_anodic_timestamps(const uint64_t t0_,
															 const uint64_t t1_,
															 const uint64_t t2_,
															 const uint64_t t3_,
															 const uint64_t t4_);

    /// Set all cathodic times
    void set_cathodic_timestamps(const uint64_t bot_time_,
																 const uint64_t top_time_);

    /// Reset all times (anodic and cathodic)
    void reset_times();

  private:

		void _reset_();

  public:

    // DATA
    // 7 geiger timing
    uint64_t anodic_timestamp_t0; ///< Anodic timestamp R0
    uint64_t anodic_timestamp_t1; ///< Anodic timestamp R1
    uint64_t anodic_timestamp_t2; ///< Anodic timestamp R2
    uint64_t anodic_timestamp_t3; ///< Anodic timestamp R3
    uint64_t anodic_timestamp_t4; ///< Anodic timestamp R4
    uint64_t bot_cathodic_timestamp; ///< Bottom cathodic timestamp R0
    uint64_t top_cathodic_timestamp; ///< Top cathodic timestamp R0

    double anodic_t0_ns; ///< Anodic time 0 in implicit ns
		double anodic_t1_ns; ///< Anodic time 1 in implicit ns
		double anodic_t2_ns; ///< Anodic time 2 in implicit ns
		double anodic_t3_ns; ///< Anodic time 3 in implicit ns
		double anodic_t4_ns; ///< Anodic time 4 in implicit ns
		double bot_cathodic_time_ns; ///< Bottom cathodic time implicit ns
		double top_cathodic_time_ns; ///< Top cathodic time in implicit ns

		DATATOOLS_SERIALIZATION_DECLARATION()

  };

} // namespace fecom

#endif // FECOM_TRACKER_HIT_HPP

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
