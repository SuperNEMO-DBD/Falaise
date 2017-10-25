
// Standard library:
#include <iostream>

// Third party :
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/logger.h>

// This project:
#include "build_mem4.h"
#include "memory.h"

int main(void) {
  DT_LOG_NOTICE(datatools::logger::PRIO_ALWAYS, "Hello, SuperNEMO!");

  snemo::trigger::mem4_type mem4;
  snemo::trigger::mem4_builder m4b;
  m4b.build(mem4);
  mem4.store_to_file("config/mem4.conf");

  DT_LOG_NOTICE(datatools::logger::PRIO_ALWAYS, "Bye bye!");
  return 0;
}
