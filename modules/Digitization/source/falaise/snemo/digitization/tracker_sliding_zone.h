// snemo/digitization/tracker_sliding_zone.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_TRACKER_SLIDING_ZONE_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_TRACKER_SLIDING_ZONE_H

// Standard library : 
#include <bitset>

// Boost : 
#include <boost/cstdint.hpp>

// Ourselves : 
#include <snemo/digitization/tracker_info.h>
#include <snemo/digitization/tracker_trigger_mem_maker_new_strategy.h>

namespace snemo {
  
  namespace digitization {	
	
    struct tracker_sliding_zone
    {
      enum bit_index {
				LAYER_PROJ_BIT_0 = 0,
				LAYER_PROJ_BIT_1 = 1,
				LAYER_PROJ_BIT_2 = 2,
				LAYER_PROJ_BIT_3 = 3,
				LAYER_PROJ_BIT_4 = 4,
				LAYER_PROJ_BIT_5 = 5,
				LAYER_PROJ_BIT_6 = 6,
				LAYER_PROJ_BIT_7 = 7,
				LAYER_PROJ_BIT_8 = 8,

				ROW_PROJ_BIT_0 = 0,
				ROW_PROJ_BIT_1 = 1,
				ROW_PROJ_BIT_2 = 2,
				ROW_PROJ_BIT_3 = 3,
				ROW_PROJ_BIT_4 = 4,
				ROW_PROJ_BIT_5 = 5,
				ROW_PROJ_BIT_6 = 6,
				ROW_PROJ_BIT_7 = 7,

				DATA_LAYER_PROJ_BIT_INNER = 0,
				DATA_LAYER_PROJ_BIT_OUTER = 1,

				DATA_ROW_PROJ_BIT_RIGHT = 0,
				DATA_ROW_PROJ_BIT_LEFT = 1,

				DATA_NEAR_SOURCE_BIT_RIGHT = 0,
				DATA_NEAR_SOURCE_BIT_LEFT = 1
      };
      
      
      tracker_sliding_zone();

      void reset();

      static int start_row(int i_);

      static int stop_row(int i_);
 
      static int width(int i_);

      void compute_lr_proj();
      
      void build_pattern(tracker_trigger_mem_maker_new_strategy::mem1_type & mem1_, tracker_trigger_mem_maker_new_strategy::mem2_type & mem2_);

      static void print_layout(std::ostream & out_);

      void print(std::ostream & out_) const;

      int side = -1;
      int szone_id = -1;
      bool cells[snemo::digitization::tracker_info::NLAYERS][snemo::digitization::tracker_info::SLZONE_WIDTH];
      
      std::bitset<snemo::digitization::tracker_info::SLZONE_LAYER_PROJ> addr_layer_proj;
      std::bitset<snemo::digitization::tracker_info::SLZONE_ROW_PROJ>   addr_row_proj;

      std::bitset<snemo::digitization::tracker_info::SLZONE_DATA_LAYER_PROJ> data_layer_proj;
      std::bitset<snemo::digitization::tracker_info::SLZONE_DATA_ROW_PROJ>   data_row_proj;
      std::bitset<snemo::digitization::tracker_info::NSZ_PATTERN_SIZE>       data_near_source;
       
    };
	
  } // end of namespace digitization
	
} // end of namespace snemo

#endif /* FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_TRACKER_SLIDING_ZONE_H */

/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
