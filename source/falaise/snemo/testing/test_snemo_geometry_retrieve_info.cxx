// -*- mode: c++ ; -*-
// test_retrieve_info.cxx
/*
 * Copyright 2007-2014 F. Mauger
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
#include <string>
#include <exception>
#include <algorithm>

// Third party:
// - Bayeux:
#include <bayeux/bayeux.h>
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/utils.h>
#include <datatools/ioutils.h>
#include <datatools/clhep_units.h>
#include <datatools/library_loader.h>
// - Bayeux/geomtools:
#include <geomtools/manager.h>
#include <geomtools/box.h>

// This project:
#include <falaise/falaise.h>

int main (int argc_, char ** argv_)
{
  FALAISE_INIT_MAIN(argc_, argv_);
  using namespace std;
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'geomtools::manager' from SuperNEMO geometry !" << endl;

      bool   debug = false;
      bool   devel = false;
      bool   verbose = false;
      string manager_config_file;
      bool   dump = false;
      bool   with_mapping = true;

      int iarg = 1;
      while (iarg < argc_)
        {
          string token = argv_[iarg];

          if (token[0] == '-')
            {
              string option = token;
              if ((option == "-d") || (option == "--debug"))
                {
                  debug = true;
                }
              else if ((option == "-D") || (option == "--devel"))
                {
                  devel = true;
                }
              else if ((option == "-V") || (option == "--verbose"))
                {
                  verbose = true;
                }
              else if ((option == "-p") || (option == "--dump"))
                {
                  dump = true;
                }
              else if ((option == "-m") || (option == "--with-mapping"))
                {
                  with_mapping = true;
                }
              else if ((option == "-M") || (option == "--without-mapping"))
                {
                  with_mapping = false;
                }
              else
                {
                  clog << datatools::io::warning << "ignoring option '"
                       << option << "'!" << endl;
                }
            }
          else
            {
              string argument = token;
              {
                clog << datatools::io::warning << "ignoring argument '"
                     << argument << "'!" << endl;
              }
            }
          iarg++;
        }

      if (manager_config_file.empty ())
        {
          manager_config_file = "@falaise:config/snemo/demonstrator/geometry/3.0/manager.conf";
        }
      datatools::fetch_path_with_env (manager_config_file);
      if (devel)
        {
          clog << datatools::io::devel << "Manager config. file : '"
               << manager_config_file << "'" << endl;
        }
      // load properties from the configuration file:
      datatools::properties manager_config;
      datatools::properties::read_config (manager_config_file,
                                          manager_config);
      geomtools::manager my_manager;
      if (verbose) my_manager.set_logging_priority (datatools::logger::PRIO_NOTICE);
      if (debug)   my_manager.set_logging_priority (datatools::logger::PRIO_DEBUG);
      if (devel)   my_manager.set_logging_priority (datatools::logger::PRIO_TRACE);
      if (with_mapping)
        {
          // prepare mapping configuration with a limited set
          // of geometry categories:

          // ensure the mapping features will be built:
          manager_config.update ("build_mapping", true);

          // remove the 'exclusion' property if any:
          if (manager_config.has_key ("mapping.excluded_categories"))
            {
              manager_config.erase ("mapping.excluded_categories");
            }

          // define a set of geometry categories to be mapped
          // through the embedded mapping system:
          vector<string> only_categories;
          only_categories.push_back ("hall");
          only_categories.push_back ("module");
          only_categories.push_back ("source_pad");
          only_categories.push_back ("source_strip");
          only_categories.push_back ("drift_cell_core");
          only_categories.push_back ("xcalo_block");
          only_categories.push_back ("gveto_block");
          only_categories.push_back ("calorimeter_block");

          // set the 'only' property:
          manager_config.update ("mapping.only_categories", only_categories);
        }
      my_manager.initialize (manager_config);
      if (dump)
        {
          my_manager.get_factory ().tree_dump (clog, "The SuperNEMO geometry model factory");
          my_manager.get_id_mgr ().tree_dump (clog, "The SuperNEMO geometry ID manager");
        }
      my_manager.get_factory ().tree_dump (clog, "The SuperNEMO geometry model factory");

      /* Partial view of the manager data model :
       *
       * manager
       * `-- factory
       *     `-- models
       *         |-- "key0" -> i_model *
       *         |-- "key1" -> i_model *
       *         :
       *         |-- "keyX" -> i_model *
       *         :             `-- logical_volume *
       *                           `-- i_shape_3d * == instantiate a concrete shape class (box...)
       */
      {
        // extract the geometry model associated to the "drift cell core" :
        const geomtools::i_model * cell_core_model = 0;
        geomtools::models_col_type::const_iterator found
          = my_manager.get_factory ().get_models ().find ("drift_cell_core.model");
        if (found == my_manager.get_factory ().get_models ().end ())
          {
            throw logic_error ("You should have found the 'drift_cell_core' model here !");
          }
        clog << "Ok, You found it !" << endl;
        cell_core_model = found->second;

        // get the associated "logical volume" :
        const geomtools::logical_volume & cell_core_log
          = cell_core_model->get_logical ();
        clog << "Now, you have the logical volume..." << endl;

        // check if there is a concrete shape (it should !):
        if (! cell_core_log.has_shape ())
          {
            throw logic_error ("You should find a shape here !");
          }

        // get the 3D shape :
        const geomtools::i_shape_3d & cell_core_shape
          = cell_core_log.get_shape ();
        clog << "... then the shape..." << endl;

        // we know the concret shape class is a box, so we
        // cast it :
        const geomtools::box & cell_core_box
          = dynamic_cast<const geomtools::box &> (cell_core_shape);
        clog << "... as a box..." << endl;

        double cell_core_length = cell_core_box.get_z ();
        double cell_core_size = cell_core_box.get_x ();
        clog << "cell_core_length = " << cell_core_length  / CLHEP::cm << " cm" << endl;
        clog << "cell_core_size   = " << cell_core_size  / CLHEP::cm << " cm" << endl;
      }

    }
  catch (exception & x)
    {
      cerr << "ERROR: " << x.what () << endl;
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      cerr << "ERROR: " << "unexpected error!" << endl;
      error_code = EXIT_FAILURE;
    }
  FALAISE_FINI();
  return (error_code);
}

// end of test_retrieve_info.cxx
