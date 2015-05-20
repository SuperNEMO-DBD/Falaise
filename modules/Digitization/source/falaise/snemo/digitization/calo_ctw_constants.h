// snemo/digitization/calo_ctw_constants.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CALO_CTW_CONSTANTS_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CALO_CTW_CONSTANTS_H

namespace snemo {
  
  namespace digitization {
  
    namespace calo {
 
      namespace ctw {
	
	/// Position of each bits in the CTW bitset word (18 bits)
	enum bit_pos {
	  HTM_PC_BIT0  = 0,
	  HTM_PC_BIT1  = 1,
	  ZONING_BIT0  = 2,
	  ZONING_BIT1  = 3,
	  ZONING_BIT2  = 4,
	  ZONING_BIT3  = 5,
	  ZONING_BIT4  = 6,
	  ZONING_BIT5  = 7,
	  ZONING_BIT6  = 8,
	  ZONING_BIT7  = 9,
	  ZONING_BIT8  = 10,
	  ZONING_BIT9  = 11,
	  LTO_PC_BIT   = 12,
	  XT_PC_BIT    = 13,
	  CONTROL_BIT0 = 14,
	  CONTROL_BIT1 = 15,
	  CONTROL_BIT2 = 16,
	  CONTROL_BIT3 = 17
	};

	/// Size of each bitset in the CTW bitset word
	enum bitset_size {
	  FULL_BITSET_SIZE    = 18,
	  HTM_BITSET_SIZE     = 2,
	  ZONING_BITSET_SIZE  = 10,
	  LTO_BITSET_SIZE     = 1,
	  XT_BITSET_SIZE      = 1,
	  CONTROL_BITSET_SIZE = 4
	};
	
	enum xwall_bit_pos {
	  ZONING_XWALL_BIT0 = 3,
	  ZONING_XWALL_BIT1 = 4,
	  ZONING_XWALL_BIT2 = 5,
	  ZONING_XWALL_BIT3 = 6
	};
	  
	enum gveto_bit_pos {
	  ZONING_GVETO_BIT0 = 2,
	  ZONING_GVETO_BIT1 = 7
	};
	  
	
      } // end of namespace ctw

    } // end of namespace calo

  } // end of namespace digitization

} // end of namespace snemo

#endif /* FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_MAPPING_TP_H */

/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
