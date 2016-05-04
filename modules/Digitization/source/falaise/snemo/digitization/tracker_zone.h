// snemo/digitization/tracker_zone.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_TRACKER_ZONE_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_TRACKER_ZONE_H

// Standard library : 
#include <bitset>

// Boost : 
#include <boost/cstdint.hpp>

// Ourselves : 
#include <snemo/digitization/trigger_info.h>
#include <snemo/digitization/tracker_trigger_mem_maker.h>

namespace snemo {
  
  namespace digitization {	

    struct tracker_zone
    {
			enum bit_index {
				DATA_NEAR_SOURCE_BIT_RIGHT = 0,
				DATA_NEAR_SOURCE_BIT_LEFT  = 1				
			};

			tracker_zone();

      void reset();

      static int start_row(int i_);

      static int stop_row(int i_);

      static int width(int i_);
		
      static void print_layout(std::ostream & out_);
			
      bool cells[snemo::digitization::trigger_info::NLAYERS][snemo::digitization::trigger_info::ZONE_WIDTH];
			int side;
      int zone_id;

      // Internal address computing with 4 sliding zone for a zone
			std::bitset<snemo::digitization::trigger_info::ZONE_ADDR_IO_PATTERN_SIZE>      addr_in_out_pattern;
      std::bitset<snemo::digitization::trigger_info::ZONE_ADDR_LMR_PATTERN_SIZE>     addr_left_mid_right_pattern;
			std::bitset<snemo::digitization::trigger_info::ZONE_ADDR_LMR_WIO_PATTERN_SIZE> addr_left_mid_right_with_in_out_pattern;
			
      // Output data : [7 bits] of information per zone after total computing
      std::bitset<snemo::digitization::trigger_info::DATA_IO_PATTERN_SIZE>  data_in_out_pattern;
      std::bitset<snemo::digitization::trigger_info::DATA_LMR_PATTERN_SIZE> data_left_mid_right_pattern;
      std::bitset<snemo::digitization::trigger_info::DATA_NSZ_PATTERN_SIZE> data_near_source;
    };
	
  } // end of namespace digitization
	
} // end of namespace snemo

#endif /* FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_TRACKER_ZONE_H */

/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
