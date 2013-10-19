// sngeometry -- examples/ex01

#include <iostream>
#include <cstdlib>
#include <string>
#include <exception>

#include <datatools/properties.h>
#include <datatools/utils.h>

#include <geomtools/manager.h>

int main (void)
{
  int error_code = EXIT_SUCCESS;

  try
    {
      // Set the configuration file :
      std::string geo_config_file
        = "${SNGEOMETRY_DATA_DIR}/resources/setups/snemo/config_3.0/manager.conf";
      datatools::fetch_path_with_env (geo_config_file);
      datatools::properties geo_config;
      datatools::properties::read_config (geo_config_file,
                                                 geo_config);
      // Setup the manager :
      geomtools::manager GeoMgr;
      GeoMgr.set_logging_priority (datatools::logger::PRIO_DEBUG);
      GeoMgr.set_mapping_requested (true);
      GeoMgr.initialize (geo_config);

      // Print the setup ID :
      std::clog << "Geometry setup label   : '" << GeoMgr.get_setup_label () << "'"
                << std::endl;
      std::clog << "Geometry setup version : '" << GeoMgr.get_setup_version () << "'"
                << std::endl;

      // Read-only access to internal services :
      const geomtools::id_mgr & GidMgr = GeoMgr.get_id_mgr ();
      const geomtools::model_factory & GeoFactory = GeoMgr.get_factory ();
      const geomtools::mapping & GeoMapping = GeoMgr.get_mapping ();

      // Extract GID informations for the 'calorimeter_block' category :
      // Here, the geometry categories are defined in the file :
      // '${SNGEOMETRY_DATA_DIR}/resources/setups/snemo/config_2.0/categories.lis'
      int gid_calo_type
        = GidMgr.get_category_info ("calorimeter_block").get_type ();
      int gid_calo_module_index
        = GidMgr.get_category_info ("calorimeter_block").get_subaddress_index ("module");
      int gid_calo_side_index
        = GidMgr.get_category_info ("calorimeter_block").get_subaddress_index ("side");
      int gid_calo_column_index
        = GidMgr.get_category_info ("calorimeter_block").get_subaddress_index ("column");
      int gid_calo_row_index
        = GidMgr.get_category_info ("calorimeter_block").get_subaddress_index ("row");

      // Subpart 'part' of the scintillator block exists only in 'snemo 2.0' :
      int gid_calo_part_index
        = GidMgr.get_category_info ("calorimeter_block").get_subaddress_index ("part");

      {
        // Build a plain Gid (for a calo block) :
        geomtools::geom_id Gid0;
        Gid0.set_type (gid_calo_type);
        Gid0.set (gid_calo_module_index, 0);
        Gid0.set (gid_calo_side_index, 1);
        Gid0.set (gid_calo_column_index, 13);
        Gid0.set (gid_calo_row_index, 9);
        Gid0.set (gid_calo_part_index, 0);
        std::clog << "Calo block Gid #0 = " << Gid0 << std::endl;
      }

      {
        // Build a Gid from the Gid manager (for a calo block) :
        geomtools::geom_id Gid1;
        GidMgr.make_id ("calorimeter_block", Gid1);
        GidMgr.set (Gid1, "module", 0);
        GidMgr.set (Gid1, "side", 0);
        GidMgr.set (Gid1, "column", 8);
        GidMgr.set (Gid1, "row", 4);
        GidMgr.set (Gid1, "part", 0);
        std::clog << "Calo block Gid #1 = " << Gid1 << std::endl;
      }


      std::clog << "The end." << std::endl;
    }
  catch (std::exception & x)
    {
      std::cerr << "ERROR: " << x.what () << std::endl;
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      std::cerr << "ERROR: " << "unexpected error !" << std::endl;
      error_code = EXIT_FAILURE;
    }

  return error_code;
}

// end sngeometry -- examples/ex01
