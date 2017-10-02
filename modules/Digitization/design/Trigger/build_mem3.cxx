
// Standard library:
#include <iostream>

// Third party :
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/logger.h>

// This project:
#include "build_mem3.h"
#include "memory.h"

int main(void) {
  DT_LOG_NOTICE(datatools::logger::PRIO_ALWAYS, "Hello, SuperNEMO!");

  snemo::trigger::mem3_type mem3;
  snemo::trigger::mem3_builder m3b;
  m3b.build(mem3);
  mem3.store_to_file("config/mem3.conf");

  DT_LOG_NOTICE(datatools::logger::PRIO_ALWAYS, "Bye bye!");
  return 0;
}
