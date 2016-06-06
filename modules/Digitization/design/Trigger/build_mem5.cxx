
// Standard library:
#include <iostream>

// Third party :
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/logger.h>

// This project:
#include "memory.h"
#include "build_mem5.h"

int main(void)
{
  DT_LOG_NOTICE(datatools::logger::PRIO_ALWAYS, "Hello, SuperNEMO!");

  //snemo::trigger::mem5_type mem5;
  snemo::trigger::mem5_type mem5_reverse_logic;
  snemo::trigger::mem5_builder m5b;
  //m5b.build(mem5);
  m5b.build_reverse_logic(mem5_reverse_logic);
  //mem5.store_to_file("config/mem5.conf");
  mem5_reverse_logic.store_to_file("config/mem5_reverse.conf");

  DT_LOG_NOTICE(datatools::logger::PRIO_ALWAYS, "Bye bye!");
  return 0;
}
