// -*- mode: c++; -*-
#ifndef SNEMO_TRIGGER_BUILD_MEM5_H
#define SNEMO_TRIGGER_BUILD_MEM5_H

// Standard library:
#include <iostream>

// This project:
#include "memory.h"
#include "utils.h"

namespace snemo {
  namespace trigger {

    //! \brief Building of the zone horizontal classification programmable memory 
    struct mem5_builder
    {     

      bool debug = true;

      mem5_builder();

      void build(mem5_type & mem5_) const;

      std::size_t szD_BIT_INNER;
      std::size_t szD_BIT_OUTER;
      std::size_t szC_BIT_INNER;
      std::size_t szC_BIT_OUTER;
      std::size_t szB_BIT_INNER;
      std::size_t szB_BIT_OUTER;
      std::size_t szA_BIT_INNER;
      std::size_t szA_BIT_OUTER;

      std::size_t data_BIT_RIGHT;
      std::size_t data_BIT_LEFT;
    };
    
    mem5_builder::mem5_builder()
    {
      szD_BIT_INNER = 0;
      szD_BIT_OUTER = 1;
      szC_BIT_INNER = 2;
      szC_BIT_OUTER = 3;
      szB_BIT_INNER = 4;
      szB_BIT_OUTER = 5;
      szA_BIT_INNER = 6;
      szA_BIT_OUTER = 7; 

      data_BIT_RIGHT = 0;
      data_BIT_LEFT  = 1;

      return;
    }
        
    void mem5_builder::build(mem5_type & mem5_) const
    {
      for (unsigned long addr = 0; addr < mem5_.get_number_of_addresses(); addr++) 
	{
	  mem5_type::address_type address = addr;
	  mem5_type::data_type data = 0x0;

	  // Vertical address:
	  //                   O I O I O I O I             L R
	  // vertical bitset : 7 6 5 4 3 2 1 0 ---> data : 1 0
	  //   Sliding Zone  : A A B B C C D D            AB CD
 
	  if ((address.test(szA_BIT_INNER) ||
	       address.test(szA_BIT_OUTER) ||
	       address.test(szB_BIT_INNER) ||
	       address.test(szB_BIT_OUTER))) 

// && 
// 	      (!address.test(szC_BIT_INNER) &&
// 	       !address.test(szC_BIT_OUTER) && 
// 	       !address.test(szD_BIT_INNER) && 
// 	       !address.test(szD_BIT_OUTER)))
	    {
	      data.set(data_BIT_LEFT);
	    }
	     
	  if ((address.test(szC_BIT_INNER) ||
	       address.test(szC_BIT_OUTER) ||
	       address.test(szD_BIT_INNER) ||
	       address.test(szD_BIT_OUTER)))
	      // && 
	      // (!address.test(szA_BIT_INNER) &&
	      //  !address.test(szA_BIT_OUTER) && 
	      //  !address.test(szB_BIT_INNER) && 
	      //  !address.test(szB_BIT_OUTER)))
	    {
	      data.set(data_BIT_RIGHT);
	    }

	  // Ajout du bit middle ABarre &  B || C & DBarre


	  std::string data_str = data.to_string();
	  //if (data_str == "00") std::clog << address << " => " << data << std::endl;
	  mem5_.push(address, data);
	}
	if (debug) mem5_.memory_map_display();
      return;
    }
    
  }
}

#endif // SNEMO_TRIGGER_BUILD_MEM5_H
