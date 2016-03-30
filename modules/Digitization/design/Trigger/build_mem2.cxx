
// Standard library:
#include <iostream>

// Third party :
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/logger.h>

// This project:
#include "memory.h"
#include "build_mem2.h"

int main(void)
{
  DT_LOG_NOTICE(datatools::logger::PRIO_ALWAYS, "Hello, SuperNEMO!");

  snemo::trigger::mem2_type mem2;
  snemo::trigger::mem2_builder m2b;
  m2b.mode = snemo::trigger::mem2_builder::MODE_PATTERN;
  m2b.build(mem2);
  mem2.store_to_file("config/mem2_pat.conf");

  // m2b.mode = snemo::trigger::mem2_builder::MODE_MULT;
  // m2b.build(mem2);
  // mem2.store_to_file("config/mem2_mult.conf");

  DT_LOG_NOTICE(datatools::logger::PRIO_ALWAYS, "Bye bye!");
  return 0;
}
