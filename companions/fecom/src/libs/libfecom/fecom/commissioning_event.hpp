//! \file fecom/commissioning_event.hpp
//
// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2016 by Guillaume Oliviéro <goliviero@lpccaen.in2p3.fr>

#ifndef FECOM_COMMISSIONING_EVENT_HPP
#define FECOM_COMMISSIONING_EVENT_HPP

// Standard library:
#include <string>
#include <iostream>

// - Bayeux:
// - Bayeux/datatools:
#include <bayeux/datatools/i_serializable.h>

// This project:
#include <fecom/calo_hit.hpp>
#include <fecom/tracker_hit.hpp>
#include <fecom/tracker_channel_hit.hpp>
#include <fecom/channel_mapping.hpp>

namespace fecom {

  struct commissioning_event
		: public datatools::i_serializable
  {
		struct find_by_trigger_id {
			find_by_trigger_id(const uint32_t & trigger_id) : find_trigger_id(trigger_id) {}

			bool operator()(const commissioning_event & com_event) {
				return com_event.get_trigger_id() == find_trigger_id;
			}

		private:

			uint32_t find_trigger_id;
		};

		struct compare
		{
			bool operator()(const commissioning_event & a,
											const commissioning_event & b);
		};

    typedef std::set<fecom::calo_hit, fecom::base_hit::compare> calo_hit_collection;
    typedef std::set<fecom::tracker_channel_hit, fecom::base_hit::compare> tracker_channel_hit_collection;
    typedef std::vector<fecom::tracker_hit> tracker_hit_collection;

    commissioning_event();

    virtual ~commissioning_event();

		bool is_valid() const;

		void set_channel_mapping(const fecom::channel_mapping & mapping_);

		const fecom::channel_mapping & get_channel_mapping() const;

		void add_calo_hit(fecom::calo_hit & a_calo_hit_);

		void add_tracker_channel_hit(fecom::tracker_channel_hit & a_tracker_channel_hit_);

		uint32_t get_trigger_id() const;

		void set_trigger_id(const uint32_t trigger_id_);

		const commissioning_event::calo_hit_collection & get_calo_hit_collection() const;

		const commissioning_event::tracker_channel_hit_collection & get_tracker_channel_hit_collection() const;

		const commissioning_event::tracker_hit_collection & get_tracker_hit_collection() const;

    void build_tracker_hit_from_channels();

    virtual void reset();

    virtual void tree_dump(std::ostream & out_,
                           const std::string & title_ = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const;

  private:

    void _reset_();

		// Configuration
		const fecom::channel_mapping * _my_channel_mapping_;

		// Datas :
    uint32_t _trigger_id_; ///< Hit trigger ID
		calo_hit_collection _calo_hit_collection_; ///< Calo hit collection for a trigger id
		tracker_channel_hit_collection _tracker_channel_hit_collection_; ///< Tracker hit collection for a trigger id

		tracker_hit_collection _tracker_hit_collection_; ///< Tracker hit collection for a trigger id


		DATATOOLS_SERIALIZATION_DECLARATION()
  };

} // namespace fecom

#endif // FECOM_COMMISSIONING_EVENT_HPP

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
