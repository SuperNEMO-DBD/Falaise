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

		struct tracker_board_channel_id
		{

			enum channel_type {
				INVALID_CHANNEL  = 0,
				ANODIC_CHANNEL   = 1,
				BOTTOM_CATHODIC_CHANNEL = 2,
				TOP_CATHODIC_CHANNEL = 3
			};

			// struct find_by_channel {
			// 	find_by_channel(const uint16_t & feast_id_,
			// 									const uint16_t & channel_id_) : feast_id(feast_id_),
			// 																									channel_id(channel_id_) {}

			// 	bool operator()(const tracker_board_channel_id & tracker_board_channel_id_) {
			// 		uint16_t input_feast_id = tracker_board_channel_id_.feast_id;
			// 		uint16_t input_channel_id = tracker_board_channel_id_.channel_id;
			// 		// std::clog << "DEBUG : TRACKER_CHANNEL_HIT::FIND_BY_CHANNEL : input_feast = " << input_feast_id
			// 		//				<< "input_channel = " << input_channel_id << std::endl;
			// 		bool finded = false;
			// 		if (feast_id == input_feast_id && channel_id == input_channel_id) finded = true;
			// 		else finded = false;
			// 		return finded;
			// 	}

			// 	uint16_t feast_id;
			// 	uint16_t channel_id;
			// };

			static int compare(const tracker_board_channel_id & tbc1_, const tracker_board_channel_id & tbc2_)
			{
				if (tbc1_.feast_id < tbc2_.feast_id) return -1;
				else if (tbc1_.feast_id > tbc2_.feast_id) return 1;
				else if (tbc1_.channel_id < tbc2_.channel_id) return -1;
				else if (tbc1_.channel_id > tbc2_.channel_id) return 1;
				else return 0;
			}

			bool operator<(const tracker_board_channel_id & tbc_) const
			{
				return tracker_board_channel_id::compare (*this, tbc_) < 0;
			}

			bool operator>(const tracker_board_channel_id & tbc_) const
			{
				return tracker_board_channel_id::compare (*this, tbc_) > 0;
			}

			bool operator==(const tracker_board_channel_id & tbc_) const
			{
				return tracker_board_channel_id::compare (*this, tbc_) == 0;
			}

			channel_type channel_type;
			uint16_t feast_id;
			uint16_t channel_id;
		};

		struct channel_triplet
		{
			tracker_board_channel_id anodic_channel;
			tracker_board_channel_id bottom_cathode_channel;
			tracker_board_channel_id top_cathode_channel;
		};

    channel_mapping();

    virtual ~channel_mapping();

		bool is_initialized() const;

		void initialize();

		void get_associated_channels(const uint16_t input_feast_,
																 const uint16_t input_channel_,
																 uint16_t & associated_feast_1_,
																 uint16_t & associated_channel_1_,
																 uint16_t & associated_feast_2_,
																 uint16_t & associated_channel_2_) const;

    /// Get associated channels for a given channel (same for the two FEAST)
		void get_associated_channels_with_types(const uint16_t input_feast_,
																						const uint16_t input_channel_,
																						uint16_t & associated_anodic_feast_,
																						uint16_t & associated_anodic_channel_,
																						uint16_t & associated_bottom_cathodic_feast_,
																						uint16_t & associated_bottom_cathodic_channel_,
																						uint16_t & associated_top_cathodic_feast_,
																						uint16_t & associated_top_cathodic_channel_) const;

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

	protected :

		void _build_channel_triplet_collection();

		void _cell_to_channel_mapping();


	private :

    void _reset_();

	public :

		bool initialized;

		std::vector<fecom::channel_mapping::channel_triplet> _channel_triplet_collection_;
		std::map<uint16_t, uint16_t> _cell_channel_mapping_;

  };

} // namespace fecom

#endif // FECOM_CHANNEL_MAPPING_HPP

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
