// -*- mode: c++; -*-
#ifndef SNEMO_TRIGGER_BUILD_MEM3_H
#define SNEMO_TRIGGER_BUILD_MEM3_H

// Standard library:
#include <iostream>

// This project:
#include "memory.h"
#include "utils.h"

namespace snemo {
  namespace trigger {

    //! \brief Building of the zone vertical classification programmable memory 
    struct mem3_builder
    {
      enum classification_type {
        VVOID  = 0x0, // 00
        VINNER = 0x1, // 01
        VOUTER = 0x2, // 10
        VFULL  = 0x3  // 11
      };

      bool debug = true;

      mem3_builder();

      void build(mem3_type & mem3_) const;

      std::size_t szD_BIT_INNER;
      std::size_t szD_BIT_OUTER;
      std::size_t szC_BIT_INNER;
      std::size_t szC_BIT_OUTER;
      std::size_t szB_BIT_INNER;
      std::size_t szB_BIT_OUTER;
      std::size_t szA_BIT_INNER;
      std::size_t szA_BIT_OUTER;
      std::size_t data_BIT_INNER;
      std::size_t data_BIT_OUTER;
      
    };
    
    mem3_builder::mem3_builder()
    {
      szD_BIT_INNER = 0;
      szD_BIT_OUTER = 1;
      szC_BIT_INNER = 2;
      szC_BIT_OUTER = 3;
      szB_BIT_INNER = 4;
      szB_BIT_OUTER = 5;
      szA_BIT_INNER = 6;
      szA_BIT_OUTER = 7;      
      data_BIT_INNER = 0;
      data_BIT_OUTER = 1;
          
      return;
    }
    
    //                  O I O I O I O I
    // bitset address : 7 6 5 4 3 2 1 0
    //   Sliding Zone : A A B B C C D D     
    void mem3_builder::build(mem3_type & mem3_) const
    {
      for (unsigned long addr = 0; addr < mem3_.get_number_of_addresses(); addr++) 
	{
	  mem3_type::address_type address = addr; 

	  // Default VVOID pattern:
	  mem3_type::data_type data = VVOID;

	  if (address.test(szB_BIT_INNER) || address.test(szC_BIT_INNER)) data.set(data_BIT_INNER);
	  if (address.test(szB_BIT_OUTER) || address.test(szC_BIT_OUTER)) data.set(data_BIT_OUTER);
	  
	  if (data != VFULL) 
	    {
	      if (address.test(szD_BIT_OUTER) && !data.test(data_BIT_OUTER)) data.set(data_BIT_OUTER);
	      if (address.test(szD_BIT_INNER) && !data.test(data_BIT_INNER)) data.set(data_BIT_INNER);

	      if (address.test(szA_BIT_INNER) && !data.test(data_BIT_INNER)) data.set(data_BIT_INNER);
	      if (address.test(szA_BIT_OUTER) && !data.test(data_BIT_OUTER)) data.set(data_BIT_OUTER);

	      if (address.to_string() == "10000001") {
		data.set(data_BIT_INNER);
		data.set(data_BIT_OUTER, 0);
	      }

	      if (address.to_string() == "01000010") {
		data.set(data_BIT_INNER);
		data.set(data_BIT_OUTER, 0);
	      }
	    }
	  mem3_.push(address, data);
	}
	if (debug) mem3_.memory_map_display();
      return;
    }
    
  }
}

#endif // SNEMO_TRIGGER_BUILD_MEM3_H
