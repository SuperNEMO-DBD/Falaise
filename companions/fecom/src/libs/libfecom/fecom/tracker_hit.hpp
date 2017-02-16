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
#include <bayeux/datatools/i_serializable.h>

// This project:
#include <fecom/tracker_constants.hpp>
#include <fecom/tracker_channel_hit.hpp>
#include <fecom/utils.hpp>

namespace fecom {

  struct tracker_hit
		: public datatools::i_serializable
  {

    tracker_hit();

    virtual ~tracker_hit();

    bool is_valid() const;

		void add_tracker_channel(const fecom::tracker_channel_hit & a_tracker_channel_);

    virtual void reset();

    virtual void tree_dump(std::ostream & out_,
                           const std::string & title_ = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const;



		/// Check if tracker hit has cell id
		bool has_cell_id() const;

    /// Set cell ID
    void set_cell_id(const uint64_t value_);

    /// Get cell ID
    uint16_t get_cell_id() const;

    /// Check if has anodic time 0
    bool has_anodic_t0() const;

    /// Set anodic time 0
    void set_anodic_t0(const uint64_t value_);

    /// Get anodic time 0
    uint64_t get_anodic_t0() const;

    /// Reset anodic time 0
    void reset_anodic_t0();

    /// Check if has anodic time 1
    bool has_anodic_t1() const;

    /// Set anodic time 1
    void set_anodic_t1(const uint64_t value_);

    /// Get anodic time 1
    uint64_t get_anodic_t1() const;

    /// Reset anodic time 1
    void reset_anodic_t1();

    /// Check if has anodic time 2
    bool has_anodic_t2() const;

    /// Set anodic time 2
    void set_anodic_t2(const uint64_t value_);

    /// Get anodic time 2
    uint64_t get_anodic_t2() const;

    /// Reset anodic time 2
    void reset_anodic_t2();

    /// Check if has anodic time 3
    bool has_anodic_t3() const;

    /// Set anodic time 3
    void set_anodic_t3(const uint64_t value_);

    /// Get anodic time 3
    uint64_t get_anodic_t3() const;

    /// Reset anodic time 3
    void reset_anodic_t3();

    /// Check if has anodic time 4
    bool has_anodic_t4() const;

    /// Set anodic time 4
    void set_anodic_t4(const uint64_t value_);

    /// Get anodic time 4
    uint64_t get_anodic_t4() const;

    /// Reset anodic time 4
    void reset_anodic_t4();

    /// Check if has cathodic time 5
    bool has_cathodic_t5() const;

    /// Set cathodic time 5
    void set_cathodic_t5(const uint64_t value_);

    /// Get cathodic time 5
    uint64_t get_cathodic_t5() const;

    /// Reset anodic time 5
    void reset_cathodic_t5();

    /// Check if has cathodic time 6
    bool has_cathodic_t6() const;

    /// Set cathodic time 6
    void set_cathodic_t6(const uint64_t value_);

    /// Get cathodic time 6
    uint64_t get_cathodic_t6() const;

    /// Reset anodic time 6
    void reset_cathodic_t6();

    /// Set all anodic times
    void set_anodic_times(const uint64_t t0_,
													const uint64_t t1_,
													const uint64_t t2_,
													const uint64_t t3_,
													const uint64_t t4_);

    /// Set all cathodic times
    void set_cathodic_times(const uint64_t t5_,
														const uint64_t t6_);

    /// Reset all times (anodic and cathodic)
    void reset_times();

  private:

    void _reset_();

  public:

		// SNDER p.30-31 for channel / cell association
    uint16_t _cell_id_; ///< Cell ID on the board [0-35]

		/* Not relevant information (ftm) because it's FEAST + CHANNEL_ID

    uint16_t anodic_channel;         ///< Anodic channel id
    uint16_t bottom_cathode_channel; ///< Bottom cathode channel id
    uint16_t top_cathode_channel;    ///< Top cathode channel id
		*/

    // DATA
    // 7 geiger timing
    uint64_t anodic_t0;   ///< Anodic time 0
    uint64_t anodic_t1;   ///< Anodic time 1
    uint64_t anodic_t2;   ///< Anodic time 2
    uint64_t anodic_t3;   ///< Anodic time 3
    uint64_t anodic_t4;   ///< Anodic time 4
    uint64_t cathodic_t5; ///< Cathodic time 5
    uint64_t cathodic_t6; ///< Cathodic time 6

		DATATOOLS_SERIALIZATION_DECLARATION()

  };

} // namespace fecom

#endif // FECOM_TRACKER_HIT_HPP

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
