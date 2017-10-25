
// Standard library:
#include <chrono>
#include <iostream>
#include <random>

// Third party :
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/logger.h>

// This project:
#include "event.h"
#include "memory.h"

int main(void) {
  DT_LOG_NOTICE(datatools::logger::PRIO_ALWAYS, "Hello, SuperNEMO!");

  typedef std::chrono::high_resolution_clock myclock;
  myclock::time_point beginning = myclock::now();

  // obtain a seed from a user string:
  std::string str;
  std::clog << "Please, enter a seed: ";
  std::getline(std::cin, str);
  std::seed_seq user_seed(str.begin(), str.end());

  // obtain a seed from the timer
  myclock::duration d = myclock::now() - beginning;
  unsigned time_seed = d.count();

  std::minstd_rand0 generator(user_seed);

  // std::uniform_int_distribution<int> track_side_distribution(0,1);
  // for (int i = 0; i < 10; i++) {
  //   std::clog << "r = " << track_side_distribution(generator) << std::endl;
  // }

  snemo::trigger::zone::print_layout(std::clog);
  snemo::trigger::szone::print_layout(std::clog);

  snemo::trigger::mem1_type mem1;
  mem1.load_from_file("config/mem1.conf");
  snemo::trigger::mem2_type mem2;
  mem2.load_from_file("config/mem2_pat.conf");

  snemo::trigger::event e;
  e.print_cells(std::clog);
  e.randomize_tracks(generator);
  e.print_cells(std::clog);
  e.build_zones();
  e.print_zones(std::clog);
  e.zones[0][0].print_layout(std::clog);
  e.build_szones(mem1, mem2);
  e.szones[1][14].print(std::clog);
  e.szones[1][15].print(std::clog);
  e.szones[1][16].print(std::clog);
  e.szones[1][17].print(std::clog);
  e.szones[1][18].print(std::clog);

  DT_LOG_NOTICE(datatools::logger::PRIO_ALWAYS, "Bye bye!");
  return 0;
}
