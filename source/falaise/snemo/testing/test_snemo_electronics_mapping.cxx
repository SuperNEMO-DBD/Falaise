// test_snemo_electronics_mapping.cxx
/*
 * Copyright 2016 F. Mauger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public  License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

// Standard library:
#include <cstdlib>
#include <iostream>

// This project:
#include <falaise/falaise.h>
#include <falaise/snemo/electronics/mapping.h>

void test1(const geomtools::manager & mgr_, bool debug_);

int main (int argc_, char ** argv_)
{
  falaise::initialize(argc_, argv_);
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::electronics::mapping' !" << std::endl;
    bool   debug = false;
    bool   do_test1 = true;
    std::string manager_config_file;
    int iarg = 1;
    while (iarg < argc_) {
      std::string token = argv_[iarg];
      if (token[0] == '-') {
        std::string option = token;
        if ((option == "-d") || (option == "--debug")) {
          debug = true;
        } else if ((option == "-t1") || (option == "--test1")) {
          do_test1 = true;
        } else {
          std::clog << datatools::io::warning << "ignoring option '"
               << option << "'!" << std::endl;
        }
      } else {
        std::string argument = token;
        {
          if (manager_config_file.empty()) {
            manager_config_file = argument;
          } else {
            std::clog << datatools::io::warning << "ignoring argument '"
                 << argument << "'!" << std::endl;
          }
        }
      }
      iarg++;
    }

    if (manager_config_file.empty()) {
      manager_config_file = "@falaise:config/snemo/demonstrator/geometry/4.0/manager.conf";
    }
    datatools::fetch_path_with_env(manager_config_file);
    std::clog << datatools::io::notice << "Manager config. file : '"
         << manager_config_file << "'" << std::endl;

    // load properties from the configuration file:
    datatools::properties manager_config;
    datatools::properties::read_config(manager_config_file,
                                        manager_config);
    geomtools::manager my_manager;
    if(debug) my_manager.set_logging_priority(datatools::logger::PRIO_DEBUG);
    my_manager.initialize(manager_config);
    if (debug) {
      my_manager.get_factory().tree_dump(std::clog, "The SuperNEMO geometry model factory");
      my_manager.get_id_mgr().tree_dump(std::clog, "The SuperNEMO geometry ID manager");
    }

    if (do_test1) {
      test1(my_manager, debug);
    }

  } catch (std::exception & x) {
    std::cerr << "ERROR: " << x.what () << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "ERROR: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }

  falaise::terminate();
  return (error_code);
}

void test1(const geomtools::manager & gmgr_, bool debug_)
{

  snemo::electronics::mapping emap;
  emap.set_geo_manager(gmgr_);
  if (debug_) emap.set_logging(datatools::logger::PRIO_DEBUG);
  emap.set_module_number(0);
  emap.initialize();

  return;
}
