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
      bool debug = true;

      mem4_builder();

      void build(mem4_type & mem4_) const;

    };
    
    mem4_builder::mem4_builder()
    {     
      return;
    }
   
    void mem4_builder::build(mem4_type & mem4_) const
    {
      for (unsigned long addr = 0; addr < mem4_.get_number_of_addresses(); addr++) 
	{
	  mem4_type::address_type address = addr;
	  mem4_type::data_type data = false;

	  if (address.any()) data = true;
	  
	  mem4_.push(address, data);
	}
	if (debug) mem4_.memory_map_display();
      return;
    }
    
  }
}

#endif // SNEMO_TRIGGER_BUILD_MEM4_H
