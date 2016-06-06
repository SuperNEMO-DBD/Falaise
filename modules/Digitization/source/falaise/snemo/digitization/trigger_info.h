// snemo/digitization/trigger_info.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_TRIGGER_INFO_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_TRIGGER_INFO_H

// Boost : 
#include <boost/cstdint.hpp>

// This project :
#include <snemo/digitization/tracker_trigger_mem_maker.h>

namespace snemo {
  
  namespace digitization {

    class trigger_info 
    {
    public :

      static const std::size_t NUMBER_OF_LAYERS_HIT_FOR_NEAR_SOURCE_BIT = 4; 
      static const std::size_t LEFT_RIGHT_ROW_SEPARATION_FOR_NEAR_SOURCE_BIT = 4; 
      static const std::size_t NSIDES  = 2;
      static const std::size_t NLAYERS = 9;
      static const std::size_t NROWS   = 113;
      static const std::size_t NZONES  = 10;
      static const std::size_t ZONE_WIDTH = 12;
      static const std::size_t NSLZONES = 31;
      static const std::size_t SLZONE_WIDTH = 8;
      static const std::size_t SLZONE_LAYER_PROJ = 9; 
      static const std::size_t SLZONE_ROW_PROJ   = 8;
      static const std::size_t SLZONE_DATA_IO_PROJ = 2; 
      static const std::size_t SLZONE_DATA_LR_PROJ   = 2;

      static const std::size_t ZONE_ADDR_IO_PATTERN_SIZE = 8;
      static const std::size_t ZONE_ADDR_LMR_PATTERN_SIZE = 6;
      static const std::size_t ZONE_ADDR_LMR_WIO_PATTERN_SIZE = 8;

      static const std::size_t DATA_IO_PATTERN_SIZE = 2;
      static const std::size_t DATA_LMR_PATTERN_SIZE = 3;
      static const std::size_t DATA_NSZ_PATTERN_SIZE = 2;
			static const std::size_t DATA_FULL_BITSET_SIZE = 7;

		};
	
	} // end of namespace digitization
	
} // end of namespace snemo

#endif /* FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_TRIGGER_INFO_H */

	/* 
	** Local Variables: --
	** mode: c++ --
	** c-file-style: "gnu" --
	** tab-width: 2 --
	** End: --
	*/
