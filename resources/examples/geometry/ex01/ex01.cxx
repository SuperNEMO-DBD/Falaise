// ex01.cxx
/*
 * Copyright 2015 F. Mauger <mauger@lpccaen.in2p3.fr>
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
#include <datatools/logger.h>
#include <datatools/properties.h>
#include <datatools/utils.h>
#include <datatools/ioutils.h>
#include <datatools/clhep_units.h>
#include <datatools/library_loader.h>
// - Bayeux/geomtools:
#include <geomtools/manager.h>
#include <geomtools/smart_id_locator.h>
#include <geomtools/materials_plugin.h>
#include <geomtools/cylinder.h>

// This project:
#include <falaise/falaise.h>

// An example function:
void ex01_task0(const geomtools::manager & gmgr_);

// void ex01_task1(const geomtools::manager & gmgr_);

int main (int argc_, char ** argv_)
{
  FALAISE_INIT_MAIN(argc_, argv_);
  datatools::logger::priority logging = datatools::logger::PRIO_WARNING;
  int error_code = EXIT_SUCCESS;
  try {
    DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE,
                  "Welcome to Falaise geometry ex01 example program !");

    std::string manager_config_file;              // The path to the geometry configuration file
    bool        force_mapping            = true;  // Flag to force geometry mapping
    bool        with_excluded_categories = false; // Flag to inhibit some geometry categories
    bool        with_only_categories     = false; // Flag to enable only specific geometry categories

    {
      // Command line arguments parsing:
      int iarg = 1;
      while (iarg < argc_) {
        std::string token = argv_[iarg];

        if (token[0] == '-') {
          std::string option = token;
          if ((option == "-d") || (option == "--debug")) {
            logging = datatools::logger::PRIO_DEBUG;
          } else if ((option == "-m") || (option == "--force-mapping")) {
            force_mapping = true;
          } else if ((option == "-M") || (option == "--no-force-mapping")) {
            force_mapping = false;
          } else if ((option == "-x") || (option == "--with-excluded-categories")) {
            with_excluded_categories = true;
          } else if ((option == "-X") || (option == "--without-excluded-categories")) {
            with_excluded_categories = false;
          } else if ((option == "-o") || (option == "--with-only-categories")) {
            with_only_categories = true;
          } else if ((option == "-O") || (option == "--without-only-categories")) {
            with_only_categories = false;
          } else {
            DT_LOG_WARNING(logging, "Ignoring option '" << option << "'!");
          }
        } else {
          std::string argument = token;
          DT_LOG_WARNING(logging, "Ignoring argument '" << argument << "'!");
        }
        iarg++;
      }
      // End of command line arguments parsing.
    }

    if (manager_config_file.empty()) {
      manager_config_file = "@falaise:config/snemo/demonstrator/geometry/3.0/manager.conf";
    }
    // Resolve the path to the geometry configuration file:
    datatools::fetch_path_with_env(manager_config_file);
    DT_LOG_DEBUG(logging, "Manager config. file : '" << manager_config_file << "'");

    // Load configuration parameters as a properties container from the configuration file:
    datatools::properties manager_config;
    datatools::properties::read_config(manager_config_file, manager_config);

    // Instantiate the geometry manager:
    geomtools::manager the_geom_manager;
    the_geom_manager.set_logging_priority(logging);

    // Force mapping in case it is not activated from the configuration parameters:
    if (force_mapping) {
      // Ensure the mapping features will be built:
      manager_config.update("build_mapping", true);
    }

    //
    if (manager_config.has_flag("build_mapping")) {

      if (with_only_categories || ! with_excluded_categories) {
        // Remove the 'exclusion' property if any:
        if (manager_config.has_key("mapping.excluded_categories")) {
          manager_config.erase("mapping.excluded_categories");
        }
      }

      if (! with_only_categories) {
        // Define a set of geometry categories to be mapped
        // through the embedded mapping system:
        std::vector<std::string> only_categories;
        only_categories.push_back("hall");
        only_categories.push_back("module");
        only_categories.push_back("source_pad");
        only_categories.push_back("source_strip");
        only_categories.push_back("drift_cell_core");
        only_categories.push_back("xcalo_block");
        only_categories.push_back("gveto_block");
        only_categories.push_back("calorimeter_block");
        only_categories.push_back("drift_cell_field_wire");
        only_categories.push_back("drift_cell_anodic_wire");
        // Set the 'only' property:
        manager_config.update("mapping.only_categories", only_categories);
      }

    }

    // Initialize the geometry manager:
    the_geom_manager.initialize(manager_config);
    if (logging == datatools::logger::PRIO_DEBUG) {
      the_geom_manager.get_factory().tree_dump(std::clog, "The SuperNEMO geometry model factory");
      the_geom_manager.get_id_mgr().tree_dump(std::clog, "The SuperNEMO geometry ID manager");
    }

    ex01_task0(the_geom_manager);

  }
  catch (std::exception & x) {
    DT_LOG_FATAL(datatools::logger::PRIO_FATAL, x.what());
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    DT_LOG_FATAL(datatools::logger::PRIO_FATAL, "Unexpected error!");
    error_code = EXIT_FAILURE;
  }
  FALAISE_FINI();
  return (error_code);
}

void ex01_task0(const geomtools::manager & geo_mgr_)
{
  DT_THROW_IF(!geo_mgr_.is_mapping_available(),
              std::logic_error,
              "Mapping is not available!");

  // Instantiate a locator for anode wires;
  geomtools::smart_id_locator anode_wires_locator(geo_mgr_.get_mapping());

  // Set the rule to select which anode wires to be considered:
  std::string anode_wires_selection_rule
    = "category='drift_cell_anodic_wire' module={0} side={*} layer={*} row={*}";

  // Set the rule to select which field wires to be considered:
  std::string field_wires_selection_rule
    = "category='drift_cell_field_wire' module={0} side={*} layer={*} row={*} set={*} wire={*}";

  // Initialize the locator:
  anode_wires_locator.initialize(anode_wires_selection_rule);
  // anode_wires_locator.initialize(field_wires_selection_rule);

  // Fetch the list of geometry informations associated to selected anode wires:
  const std::list<const geomtools::geom_info *> list_of_anode_wires_infos
    = anode_wires_locator.get_ginfos();

  // Fetch the number of anode wires:
  unsigned int number_of_anode_wires = list_of_anode_wires_infos.size();
  std::clog << "Number of anode wires is: " << number_of_anode_wires << std::endl;

  // Fetch the geometry information structure associated to the first anode wire:
  const geomtools::geom_info * first_anode_wire_info = list_of_anode_wires_infos.front();

  // Fetch the geometry logical volume associated to the first anode wire:
  const geomtools::logical_volume & first_anode_wire_logical = first_anode_wire_info->get_logical();

  // Fetch the geometry solid shape associated to the first anode wire:
  const geomtools::i_shape_3d & first_anode_wire_solid = first_anode_wire_logical.get_shape();

  // Check if the shape is a cylinder:
  DT_THROW_IF(first_anode_wire_solid.get_shape_name() != "cylinder",
              std::logic_error,
              "Anode wire solid is not a cylinder!");

  // Convert the geometry solid shape to a cylinder:
  const geomtools::cylinder & first_anode_wire_cyl
    = dynamic_cast<const geomtools::cylinder &>(first_anode_wire_solid);

  std::clog << "Anode wire length: " << first_anode_wire_cyl.get_z() / CLHEP::cm << " cm" << std::endl;
  std::clog << "Anode wire diameter: " << first_anode_wire_cyl.get_diameter() / CLHEP::mm << " mm" << std::endl;


  double first_anode_wire_vol = first_anode_wire_cyl.get_volume();
  std::clog << "Anode wire volume: " << first_anode_wire_vol / CLHEP::mm3 << " mm3" << std::endl;

  // Fetch the material name associated to the first anode wire:
  const std::string & material_ref = first_anode_wire_logical.get_material_ref();
  std::clog << "Anode wire material name: '" << material_ref << "'" << std::endl;

  // Check if a plugin for the description of materials is available:
  DT_THROW_IF(!geo_mgr_.is_plugin_a<geomtools::materials_plugin>("materials_driver"),
              std::logic_error,
              "Material pluggin is not available!");

  // Fetch the materials plugin:
  const geomtools::materials_plugin & materials_driver
    = geo_mgr_.get_plugin<geomtools::materials_plugin>("materials_driver");

  // Fetch the material manager from the plugin:
  const materials::manager & mat_mgr = materials_driver.get_manager();

  // Check if a plugin for the description of materials is available:
  DT_THROW_IF(!mat_mgr.has_material(material_ref),
              std::logic_error,
              "Material manager has no material with name '" << material_ref << "'!");

  // Fetch the dictionary of materials:
  const materials::material_dict_type & materials = mat_mgr.get_materials();

  // Fetch the material associated to the anode wire:
  const materials::material & first_anode_wire_material
    = materials.find(material_ref)->second.get_ref();

  // Fetch the density:
  double first_anode_wire_density = first_anode_wire_material.get_density();
  std::clog << "Anode wire material density: " << first_anode_wire_density / (CLHEP::g/CLHEP::cm3) << " g/cm3" << std::endl;

  // Compute the toal mass of anode wires:
  double total_anode_wire_mass =
    number_of_anode_wires * first_anode_wire_density * first_anode_wire_vol;

  std::clog << "Total mass for anode wires: "
            << total_anode_wire_mass / CLHEP::kg << " kg" << std::endl;

  return;
}
