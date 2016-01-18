// -*- mode: c++; -*-
#ifndef SNEMO_TRIGGER_BUILD_MEM4_H
#define SNEMO_TRIGGER_BUILD_MEM4_H

// Standard library:
#include <iostream>

// This project:
#include "memory.h"
#include "utils.h"

namespace snemo {
  namespace trigger {

    //! \brief Building of the zone horizontal classification programmable memory 
    struct mem4_builder
    {       
      enum classification_type {
        NO_HTRACK           = 0x0, //!< No track detected           : 00
        NARROW_RIGHT_HTRACK = 0x1, //!< Right narrow track detected : 01
        NARROW_LEFT_HTRACK  = 0x2, //!< Left narrow track detected  : 10
        WIDE_HTRACK         = 0x3  //!< Wide track detected         : 11
      };   
      
      bool debug = true;

      mem4_builder();

      void build(mem4_type & mem4_) const;
      
      std::size_t szD_BIT_RIGHT;
      std::size_t szD_BIT_LEFT;
      std::size_t szC_BIT_RIGHT;
      std::size_t szC_BIT_LEFT;
      std::size_t szB_BIT_RIGHT;
      std::size_t szB_BIT_LEFT;
      std::size_t szA_BIT_RIGHT;
      std::size_t szA_BIT_LEFT;

      std::size_t ADDR_szD_BIT_LEFT;
      std::size_t ADDR_szC_BIT_RIGHT;
      std::size_t ADDR_szC_BIT_LEFT;
      std::size_t ADDR_szB_BIT_RIGHT;
      std::size_t ADDR_szB_BIT_LEFT;
      std::size_t ADDR_szA_BIT_RIGHT; 

      std::size_t DATA_BIT_MIDDLE;
      std::size_t DATA_BIT_RIGHT;
      std::size_t DATA_BIT_LEFT;
    };
    
    mem4_builder::mem4_builder()
    {   
      szD_BIT_RIGHT  = 0;
      szD_BIT_LEFT   = 1;
      szC_BIT_RIGHT  = 2;
      szC_BIT_LEFT   = 3;
      szB_BIT_RIGHT  = 4;
      szB_BIT_LEFT   = 5;
      szA_BIT_RIGHT  = 6;
      szA_BIT_LEFT   = 7; 
 
      ADDR_szD_BIT_LEFT   = 0;
      ADDR_szC_BIT_RIGHT  = 1;
      ADDR_szC_BIT_LEFT   = 2;
      ADDR_szB_BIT_RIGHT  = 3;
      ADDR_szB_BIT_LEFT   = 4;
      ADDR_szA_BIT_RIGHT  = 5;  

      DATA_BIT_RIGHT  = 0;
      DATA_BIT_MIDDLE = 1;
      DATA_BIT_LEFT   = 2;
      return;
    }

    //                   R L R L R L 
    // bitset address :  5 4 3 2 1 0
    //   Sliding Zone :  A B B C C D   
    // A6 D3 memory   :
    //                    L M R
    //    bitset data :   2 1 0

    void mem4_builder::build(mem4_type & mem4_) const
    {
      for (unsigned long addr = 0; addr < mem4_.get_number_of_addresses(); addr++) 
	{
	  mem4_type::address_type address = addr;
	  mem4_type::data_type data = 0x0;

	  // Default NO_HTRACK pattern:   
	  classification_type szA_clsf = NO_HTRACK;
	  classification_type szB_clsf = NO_HTRACK;
	  classification_type szC_clsf = NO_HTRACK;
	  classification_type szD_clsf = NO_HTRACK;
	  
	  std::string addr_str = address.to_string();  

	  if (address.test(ADDR_szA_BIT_RIGHT) || address.test(ADDR_szB_BIT_LEFT)) data.set(DATA_BIT_LEFT);
	  if (address.test(ADDR_szB_BIT_RIGHT) || address.test(ADDR_szC_BIT_LEFT)) data.set(DATA_BIT_MIDDLE);
	  if (address.test(ADDR_szC_BIT_RIGHT) || address.test(ADDR_szD_BIT_LEFT)) data.set(DATA_BIT_RIGHT);

	  // // Fetch each sliding zone horizontal classification:
	  // if      (addr_str.substr(0,2) == "01") szA_clsf = NARROW_RIGHT_HTRACK;
	  // else if (addr_str.substr(0,2) == "10") szA_clsf = NARROW_LEFT_HTRACK;
	  // else if (addr_str.substr(0,2) == "11") szA_clsf = WIDE_HTRACK;

	  // if      (addr_str.substr(2,2) == "01") szB_clsf = NARROW_RIGHT_HTRACK;
	  // else if (addr_str.substr(2,2) == "10") szB_clsf = NARROW_LEFT_HTRACK;
	  // else if (addr_str.substr(2,2) == "11") szB_clsf = WIDE_HTRACK;

	  // if      (addr_str.substr(4,2) == "01") szC_clsf = NARROW_RIGHT_HTRACK;
	  // else if (addr_str.substr(4,2) == "10") szC_clsf = NARROW_LEFT_HTRACK;
	  // else if (addr_str.substr(4,2) == "11") szC_clsf = WIDE_HTRACK;

	  // if      (addr_str.substr(6,2) == "01") szD_clsf = NARROW_RIGHT_HTRACK;
	  // else if (addr_str.substr(6,2) == "10") szD_clsf = NARROW_LEFT_HTRACK;
	  // else if (addr_str.substr(6,2) == "11") szD_clsf = WIDE_HTRACK;
	  
	  // // Fill LMR bits :
	  // if (szD_clsf == NARROW_LEFT_HTRACK || szD_clsf == WIDE_HTRACK
	  //     || szC_clsf == NARROW_RIGHT_HTRACK || szC_clsf == WIDE_HTRACK)
	  //   {
	  //     data.set(DATA_BIT_RIGHT);
	  //   }
	  
	  // if (szB_clsf == NARROW_LEFT_HTRACK || szB_clsf == WIDE_HTRACK
	  //     || szA_clsf == NARROW_RIGHT_HTRACK || szA_clsf == WIDE_HTRACK)
	  //   {
	  //     data.set(DATA_BIT_LEFT);
	  //   }

	  // if (szB_clsf == NARROW_LEFT_HTRACK || szB_clsf == WIDE_HTRACK
	  //     || szC_clsf == NARROW_LEFT_HTRACK || szC_clsf == WIDE_HTRACK)
	  //   {
	  //     data.set(DATA_BIT_MIDDLE);
	  //   }
	  mem4_.push(address, data);
	  std::string data_str = data.to_string();
	}

	if (debug) mem4_.memory_map_display();

      return;
    }
    
  }
}

#endif // SNEMO_TRIGGER_BUILD_MEM4_H
