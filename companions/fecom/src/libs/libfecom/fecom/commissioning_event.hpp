//! \file fecom/commissioning_event.hpp
//
// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2016 by Guillaume Oliviéro <goliviero@lpccaen.in2p3.fr>

#ifndef FECOM_COMMISSIONING_EVENT_HPP
#define FECOM_COMMISSIONING_EVENT_HPP

// Standard library:
#include <string>
#include <iostream>

// Third party:
// - Boost :
#include <boost/serialization/access.hpp>
// - Bayeux:
// - Bayeux/datatools:
#include <bayeux/datatools/i_serializable.h>
#include <bayeux/datatools/i_tree_dump.h>
#include <bayeux/datatools/event_id.h>

// This project:
#include <fecom/calo_hit.hpp>
#include <fecom/tracker_hit.hpp>
#include <fecom/tracker_channel_hit.hpp>
#include <fecom/channel_mapping.hpp>

namespace fecom {

	//! \brief Commissioning event, contain a collection of calo_hits and tracker_channel_hits and can build associated tracker hits from channels
  struct commissioning_event
		: public datatools::i_serializable
		, public datatools::i_tree_dumpable
  {
		/// Predicate to find by trigger id
		struct find_by_event_id {
			find_by_event_id(const int32_t & event_id) : find_event_id(event_id) {}

			bool operator()(const commissioning_event & com_event) {
				return com_event.get_event_id().get_event_number() == find_event_id;
			}

		private:

			int32_t find_event_id;
		};

		/// Comparison of 2 commissioning events (based on run trigger id)
		struct compare
		{
			bool operator()(const commissioning_event & a,
											const commissioning_event & b);
		};

    typedef std::set<fecom::calo_hit, fecom::base_hit::compare> calo_hit_collection;
    typedef std::set<fecom::tracker_channel_hit, fecom::base_hit::compare> tracker_channel_hit_collection;
    typedef std::vector<fecom::tracker_hit> tracker_hit_collection;

		/// Default constructor
    commissioning_event();

		/// Destructor
    virtual ~commissioning_event();

		/// Check if the commissioning event is valid
		bool is_valid() const;

		/// Set the external channel mapping (ptr)
		void set_channel_mapping(const fecom::channel_mapping & mapping_);

		/// Reference to the external channel mapping
		const fecom::channel_mapping & get_channel_mapping() const;

		/// Add a calo hit in the commissioning event
		void add_calo_hit(fecom::calo_hit & a_calo_hit_);

		/// Add a tracker channel hit in the commissioning event
		void add_tracker_channel_hit(fecom::tracker_channel_hit & a_tracker_channel_hit_);

		/// Get the datatools ID of the commissioning event
		const datatools::event_id & get_event_id() const;

		/// Grab the datatools ID of the commissioning event
		datatools::event_id & grab_event_id();

		/// Set the datatools ID of the commissioning event
		void set_event_id(const datatools::event_id & id_);

		/// Set the time start in nanosecond
		double get_time_start_ns() const;

		/// Set the time start in nanosecond
		bool has_time_start_ns() const;

		/// Set the time start in nanosecond
		void set_time_start_ns(const double time_start_);

		/// Get the calo hit collection
		const	commissioning_event::calo_hit_collection & get_calo_hit_collection() const;

		/// Check if has calorimeter hits
		bool has_calo_hits() const;

		/// Get the tracker channel hit collection
		const commissioning_event::tracker_channel_hit_collection & get_tracker_channel_hit_collection() const;

		/// Check if has tracker channel hits
		bool has_tracker_channel_hits() const;

		/// Get the tracker hit collection
		const	commissioning_event::tracker_hit_collection & get_tracker_hit_collection() const;

		/// Check if has tracker channel hits
		bool has_tracker_hits() const;

		/// Check if is only tracker
		bool is_only_tracker() const;

		/// Check if is only calo
		bool is_only_calo() const;

		/// Check if calo and tracker
		bool is_calo_tracker() const;

		/// Build a tracker hit from channels (anodic, bot cathode and top cathode max)
    void build_tracker_hit_from_channels();

		/// Reset
    virtual void reset();

		/// Print succinct contained calo and tracker hits
		virtual void print(std::ostream & out_);

		/// Smart print
    virtual void tree_dump(std::ostream & out_,
                           const std::string & title_ = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const;

		std::bitset<4> & grab_traits() { return _traits_; }

		const std::bitset<4> & get_traits() const { return _traits_; }

	public :

		// Management :
		//	mutable uint64_t _last_time_in_ns_added_; //!< Last time in ns added in the commissioning event (calo or tracker channel)
		//	mutable std::bitset<4> _traits_; //!< Traits for data quality of the commissioning event of data
		/*
			BIT 0 : Tracker only Event
			BIT 1 : UNDEFINED (ftm)
			BIT 2 : UNDEFINED (ftm)
			BIT 3 : UNDEFINED (ftm)
		*/
  private:

    void _reset_();

		// Configuration
		const fecom::channel_mapping * _my_channel_mapping_; //!< Handle to an external channel mapping

		// Datas :
		datatools::event_id _event_id_; ///< Datatools event ID
		double _time_start_ns_; ///< Time start of the commissioning event in ns
		std::bitset<4> _traits_; ///< Traits for data quality of the commissioning event of data
		calo_hit_collection _calo_hit_collection_; ///< Calo hit collection for a trigger id
		tracker_channel_hit_collection _tracker_channel_hit_collection_; ///< Tracker hit collection for a trigger id
		tracker_hit_collection _tracker_hit_collection_; ///< Tracker hit collection for a trigger id


		DATATOOLS_SERIALIZATION_DECLARATION()
  };

} // namespace fecom


#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(fecom::commissioning_event,
												"fecom::commissioning_event")

#endif // FECOM_COMMISSIONING_EVENT_HPP

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
