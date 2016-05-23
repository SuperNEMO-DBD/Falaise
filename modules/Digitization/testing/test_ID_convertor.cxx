// Standard libraries :
#include <iostream>
#include <exception>
#include <cstdlib>

// Third party:
// - Bayeux/datatools:
#include <datatools/logger.h>
#include <datatools/io_factory.h>
// - Bayeux/geomtools:
#include <geomtools/manager.h>
// Falaise:
#include <falaise/falaise.h>

// This project :
#include <snemo/digitization/calo_tp.h>
#include <snemo/digitization/geiger_tp.h>
#include <snemo/digitization/ID_convertor.h>
#include <snemo/digitization/mapping.h>


int main( int /* argc_ */, char ** /* argv_ */ )
{
  falaise::initialize(argc_, argv_);
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  try { 

    std::string manager_config_file;
    
    manager_config_file = "@falaise:config/snemo/demonstrator/geometry/3.0/manager.conf";
    datatools::fetch_path_with_env (manager_config_file);
    
    datatools::properties manager_config;
    datatools::properties::read_config (manager_config_file,
					manager_config);
    geomtools::manager my_manager;
    
    manager_config.update ("build_mapping", true);
    if (manager_config.has_key ("mapping.excluded_categories"))
      {
	manager_config.erase ("mapping.excluded_categories");
      }

    // std::vector<std::string> only_categories;
    // only_categories.push_back ("module");
    // only_categories.push_back ("drift_cell_core");
    // only_categories.push_back ("xcalo_block");
    // only_categories.push_back ("gveto_block");
    // only_categories.push_back ("calorimeter_block");
    // only_categories.push_back ("calorimeter_optical_module");
    // set the 'only' property:
    // manager_config.update ("mapping.only_categories", only_categories);
    my_manager.initialize (manager_config);

    snemo::digitization::ID_convertor my_convertor;
    my_convertor.set_geo_manager(my_manager);
    my_convertor.set_module_number(0);
    my_convertor.initialize();

    geomtools::geom_id my_geom_ID_from_SD;
    my_geom_ID_from_SD.set_type(1204);
    my_geom_ID_from_SD.set_address(0,1,7,112);

   geomtools::geom_id my_electronic_ID;

    std::clog << "DEBUG : MAIN : geom_ID " << my_geom_ID_from_SD << std::endl;
    my_electronic_ID =  my_convertor.convert_GID_to_EID(my_geom_ID_from_SD);
    std::clog << "DEBUG : MAIN : elec_ID " << my_electronic_ID << std::endl;

    snemo::digitization::geiger_tp my_geiger_tp; 
    my_geiger_tp.set_header(36, my_electronic_ID, 12, 0, 0, 6);
    my_geiger_tp.tree_dump(std::clog, "my_geiger_TP (filled with electronic) : ", "INFO : ");

    my_geom_ID_from_SD.reset();    
    my_electronic_ID.reset();
   
    my_geom_ID_from_SD.set_type(1302);
    my_geom_ID_from_SD.set_address(0,0,4,11);
    std::clog << "DEBUG : MAIN : geom_ID " << my_geom_ID_from_SD << std::endl;
    my_electronic_ID =  my_convertor.convert_GID_to_EID(my_geom_ID_from_SD);
    std::clog << "DEBUG : MAIN : elec_ID " << my_electronic_ID << std::endl;
   
    snemo::digitization::calo_tp my_calo_tp; 
    my_calo_tp.set_header(25, my_electronic_ID, 20);
    my_calo_tp.tree_dump(std::clog, "my_calo_TP : ", " INFO : ");

    my_calo_tp.reset();
    my_calo_tp.tree_dump(std::clog, "my_calo_TP (reset) : ", "INFO : ");

  }

  catch (std::exception & error) {
    DT_LOG_FATAL(logging, error.what());
    error_code = EXIT_FAILURE;
  }

  catch (...) {
    DT_LOG_FATAL(logging, "Unexpected error!");
    error_code = EXIT_FAILURE;
  }

  falaise::terminate();
  return error_code;
}

