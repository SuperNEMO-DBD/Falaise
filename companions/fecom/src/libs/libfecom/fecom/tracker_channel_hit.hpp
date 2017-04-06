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

	//! \brief Commissioning tracker channel hit parser
  struct tracker_channel_hit
    : public base_hit
  {
		/// Predicate to find a tracker channel hit by channel
		struct find_by_channel {
			find_by_channel(const uint16_t & feast_id_,
											const uint16_t & channel_id_) : feast_id(feast_id_),
																											channel_id(channel_id_) {}

			bool operator()(const tracker_channel_hit & tracker_channel_hit_) {
				uint16_t input_feast_id = tracker_channel_hit_.electronic_id.get(tracker_constants::FEAST_INDEX);
				uint16_t input_channel_id = tracker_channel_hit_.electronic_id.get(tracker_constants::CHANNEL_INDEX);
				bool finded = false;
				if (feast_id == input_feast_id && channel_id == input_channel_id) finded = true;
				else finded = false;
				return finded;
			}

			uint16_t feast_id;
			uint16_t channel_id;
		};

		/// Predicate to find a tracker channel hit by timestamp
		struct find_by_timestamp {
			find_by_timestamp(const uint16_t & feast_id_,
												const uint16_t & channel_id_,
												const std::string & timestamp_type_) : feast_id(feast_id_),
																															 channel_id(channel_id_),
																															 timestamp_type(timestamp_type_) {}

			bool operator()(const tracker_channel_hit & tracker_channel_hit_) {
				uint16_t input_feast_id = tracker_channel_hit_.electronic_id.get(tracker_constants::FEAST_INDEX);
				uint16_t input_channel_id = tracker_channel_hit_.electronic_id.get(tracker_constants::CHANNEL_INDEX);
				std::string input_timestamp_type = tracker_channel_hit_.timestamp_type;
				bool finded = false;
				if (feast_id == input_feast_id
						&& channel_id == input_channel_id
						&& timestamp_type == input_timestamp_type) finded = true;
				else finded = false;
				return finded;
			}

			uint16_t feast_id;
			uint16_t channel_id;
			std::string timestamp_type;
		};

		/// Default constructor
    tracker_channel_hit();

		/// Destructor
    virtual ~tracker_channel_hit();

		/// Check if the tracker
    virtual bool is_valid() const;

		///
		virtual double get_timestamp() const;

		/// Reset
    virtual void reset();

		/// Smart print
    virtual void tree_dump(std::ostream & out_,
                           const std::string & title_ = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const;


    /// Reset all timestamp (anodic and cathodic)
    void reset_timestamp();

  private:

    void _reset_();

  public:

		// Management :
		mutable bool associated = false; ///< Flag to know if the tracker channel is already asociate in a tracker_hit

    // Config :

    std::string timestamp_type;    ///< Type of the timestamp (t0..t6)

    // Data :
    uint64_t timestamp_value;      ///< Value of the timestamp
    double timestamp_time_ns;      ///< Value of the timestamp in ns

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
