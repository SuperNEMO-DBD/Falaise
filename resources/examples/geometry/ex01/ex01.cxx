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
#include <datatools/exception.h>
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


/// \brief Supported geometry categories for volumes in the virtual geometry:
enum geometry_category_type {
  GCAT_INVALID    = 0, //!< Undefined geometry category
  GCAT_ANODE_WIRE = 1, //!< The drift cell anode wires
  GCAT_FIELD_WIRE = 2, //!< The drift cell field wires
  GCAT_DEFAULT    = GCAT_ANODE_WIRE //!< Default value
};

/// \brief Return a display name associated to a given geometry category type
std::string display_name(geometry_category_type gcat_);

/// An example function that extract informations from a given geometry category
/// and print them on the standard output
void ex01_inspect_category(const geomtools::manager & gmgr_, geometry_category_type gcat_);

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
    geometry_category_type geometry_category = GCAT_INVALID; // Volume category

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
          } else if (option == "--anode-wires") {
            geometry_category = GCAT_ANODE_WIRE;
        } else if (option == "--field-wires") {
            geometry_category = GCAT_FIELD_WIRE;
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

    // Set default values:
    if (manager_config_file.empty()) {
      manager_config_file = "@falaise:config/snemo/demonstrator/geometry/3.0/manager.conf";
    }

    if (geometry_category == GCAT_INVALID) {
      geometry_category = GCAT_DEFAULT;
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
    DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE,
                  "Initializing the geometry manager.... please wait!");
    the_geom_manager.initialize(manager_config);
    if (logging == datatools::logger::PRIO_DEBUG) {
      the_geom_manager.get_factory().tree_dump(std::clog, "The SuperNEMO geometry model factory");
      the_geom_manager.get_id_mgr().tree_dump(std::clog, "The SuperNEMO geometry ID manager");
    }

    ex01_inspect_category(the_geom_manager, geometry_category);

    DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE,
                  "We are done. Bye!");
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

std::string display_name(geometry_category_type gcat_)
{
  switch (gcat_) {
  case GCAT_ANODE_WIRE:
    return "anode wire";
    break;
  case GCAT_FIELD_WIRE:
    return "field wire";
    break;
  default:
    DT_THROW_IF(true, std::logic_error, "Insupported geometry category '" << gcat_ << "'!");
  }
}

void ex01_inspect_category(const geomtools::manager & geo_mgr_, geometry_category_type gcat_)
{
  DT_THROW_IF(!geo_mgr_.is_mapping_available(),
              std::logic_error,
              "Mapping is not available!");

  // Instantiate a locator for specific volumes;
  geomtools::smart_id_locator volumes_locator(geo_mgr_.get_mapping());

  // Set the rule to select which volumes to be considered:
  std::string volumes_selection_rule;
  switch (gcat_) {
  case GCAT_ANODE_WIRE:
    // Select all anode wires in the demonstrator module:
    volumes_selection_rule = "category='drift_cell_anodic_wire' module={0} side={*} layer={*} row={*}";
    break;
  case GCAT_FIELD_WIRE:
    // Select all field wires in the demonstrator module:
    volumes_selection_rule = "category='drift_cell_field_wire' module={0} side={*} layer={*} row={*} set={*} wire={*}";
    break;
  default:
    DT_THROW_IF(true, std::logic_error, "Insupported geometry category '" << gcat_ << "'!");
  }

  // Initialize the locator:
  volumes_locator.initialize(volumes_selection_rule);

  // Fetch the list of geometry informations associated to selected volumes:
  const std::list<const geomtools::geom_info *> list_of_volume_infos
    = volumes_locator.get_ginfos();

  // Fetch the number of volumes:
  unsigned int number_of_volumes = list_of_volume_infos.size();
  std::cout << "Number of selected volumes is: " << number_of_volumes << std::endl;

  // Fetch the geometry information structure associated to the first volume in the selected list:
  const geomtools::geom_info * first_volume_info = list_of_volume_infos.front();

  // Fetch the geometry logical volume associated to the first volume:
  const geomtools::logical_volume & first_volume_logical = first_volume_info->get_logical();

  // Fetch the geometry solid shape associated to the first volume:
  const geomtools::i_shape_3d & first_volume_solid = first_volume_logical.get_shape();

  // Check if the shape is a cylinder:
  double first_volume_surf;
  datatools::invalidate(first_volume_surf);
  double first_volume_vol;
  datatools::invalidate(first_volume_vol);
  if (first_volume_solid.get_shape_name() == "cylinder") {
    // Convert the geometry solid shape to a cylinder:
    const geomtools::cylinder & first_volume_cyl
      = dynamic_cast<const geomtools::cylinder &>(first_volume_solid);

    std::cout << "Volume length: " << first_volume_cyl.get_z() / CLHEP::cm << " cm" << std::endl;
    std::cout << "Volume diameter: " << first_volume_cyl.get_diameter() / CLHEP::mm << " mm" << std::endl;
    first_volume_vol = first_volume_cyl.get_volume();
    first_volume_surf = first_volume_cyl.get_surface(geomtools::cylinder::FACE_SIDE);
  } else {
    DT_THROW_IF(true, std::logic_error,
                "Volume solid has unsupported solid shape '" << first_volume_solid.get_shape_name() << "'!");
  }

  DT_THROW_IF(!datatools::is_valid(first_volume_vol), std::logic_error,
              "Cannot compute the volume of this shape!");

  DT_THROW_IF(!datatools::is_valid(first_volume_surf), std::logic_error,
              "Cannot compute the side surface of this shape!");

  std::cout << "Volume: " << first_volume_vol / CLHEP::mm3 << " mm3" << std::endl;
  std::cout << "Surface: " << first_volume_surf / CLHEP::mm2 << " mm2" << std::endl;

  // Fetch the material name associated to the first volume:
  const std::string & material_ref = first_volume_logical.get_material_ref();
  std::cout << "Volume material reference: '" << material_ref << "'" << std::endl;

  // Check if a plugin for the description of materials is available:
  DT_THROW_IF(!geo_mgr_.is_plugin_a<geomtools::materials_plugin>("materials_driver"),
              std::logic_error,
              "Material plugin is not available!");

  // Fetch the materials plugin:
  const geomtools::materials_plugin & materials_driver
    = geo_mgr_.get_plugin<geomtools::materials_plugin>("materials_driver");

  // Fetch the material manager from the plugin:
  const materials::manager & mat_mgr = materials_driver.get_manager();

  // Check if a plugin for the description of materials is available:
  DT_THROW_IF(!mat_mgr.has_material(material_ref),
              std::logic_error,
              "Material manager has no material with reference '" << material_ref << "'!");

  if (mat_mgr.is_alias(material_ref)) {
    std::cout << "Volume material name: '" << mat_mgr.alias_of(material_ref) << "'" << std::endl;
  }

  // Fetch the dictionary of materials:
  const materials::material_dict_type & materials = mat_mgr.get_materials();

  // Fetch the material associated to the volume:
  const materials::material & first_volume_material
    = materials.find(material_ref)->second.get_ref();

  // Fetch the density:
  double first_volume_density = first_volume_material.get_density();
  std::cout << "Volume material density: " << first_volume_density / (CLHEP::g/CLHEP::cm3) << " g/cm3" << std::endl;

  // Compute the total mass of the selected volumes:
  double total_volume_mass =
    number_of_volumes * first_volume_density * first_volume_vol;
  double total_volume_surf =
    number_of_volumes * first_volume_surf;

  std::cout << "Total mass for volumes with category '" << display_name(gcat_) << "' : "
            << total_volume_mass / CLHEP::kg << " kg" << std::endl;

  std::cout << "Total (side) surface for volumes with category '" << display_name(gcat_) << "' : "
            << total_volume_surf / CLHEP::m2 << " m2" << std::endl;

  return;
}
