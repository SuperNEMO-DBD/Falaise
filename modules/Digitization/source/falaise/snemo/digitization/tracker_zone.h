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
#include <snemo/digitization/tracker_info.h>

namespace snemo {
  
  namespace digitization {	

    struct tracker_zone
    {
			
      tracker_zone();

      void reset();

      static int start_row(int i_);

      static int stop_row(int i_);

      static int width(int i_);

      static void print_layout(std::ostream & out_);

      bool _cells_[snemo::digitization::tracker_info::NLAYERS][snemo::digitization::tracker_info::ZONE_WIDTH];
		
      //
      
      // [7 bits] of information per zone after total computing
      std::bitset<snemo::digitization::tracker_info::IO_PATTERN_SIZE>  _data_in_out_pattern_;
      std::bitset<snemo::digitization::tracker_info::LMR_PATTERN_SIZE> _data_left_mid_right_pattern_;
      std::bitset<snemo::digitization::tracker_info::NSZ_PATTERN_SIZE> _data_near_source_;
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
