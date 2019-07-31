// test_falaise.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

// Third party:
// - Bayeux:
#include <bayeux/datatools/kernel.h>
#include <bayeux/datatools/urn.h>
#include <bayeux/datatools/urn_query_service.h>

// This project:
#include <falaise/falaise.h>
#include <falaise/resource.h>
#include <falaise/tags.h>
#include <falaise/version.h>

int main(int argc, char** argv) {
  falaise::initialize(argc, argv);
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for the 'Falaise' library." << std::endl;

    std::clog << "Falaise version : " << falaise::version::get_version() << std::endl;
    std::clog << "Tag categories : " << std::endl;
    std::clog << " - Experiment:          : '" << falaise::tags::experiment_category() << "'"
              << std::endl;
    std::clog << " - Experimental setup   : '" << falaise::tags::experimental_setup_category()
              << "'" << std::endl;
    std::clog << " - Geometry setup       : '" << falaise::tags::geometry_setup_category() << "'"
              << std::endl;
    std::clog << " - Simulation setup     : '" << falaise::tags::simulation_setup_category() << "'"
              << std::endl;
    std::clog << " - Reconstruction setup : '" << falaise::tags::reconstruction_setup_category()
              << "'" << std::endl;
    std::clog << " - Variant service      : '" << falaise::tags::variant_service_category() << "'"
              << std::endl;
    std::clog << " - Services             : '" << falaise::tags::services_category() << "'"
              << std::endl;
    std::clog << " - Configuration        : '" << falaise::tags::generic_configuration_category()
              << "'" << std::endl;

    datatools::kernel& dtk = datatools::kernel::instance();
    const datatools::urn_query_service& dtkUrnQuery = dtk.get_urn_query();

    {
      datatools::urn_info urnInfo;
      std::string urn("urn:snemo:demonstrator");
      if (dtkUrnQuery.check_urn_info(urn, falaise::tags::experiment_category())) {
        urnInfo = dtkUrnQuery.get_urn_info(urn);
        std::clog << "The tag '" << urn << "' is registered with category '"
                  << urnInfo.get_category() << "'." << std::endl;
      }
    }

    {
      datatools::urn_info urnInfo;
      std::string urn("urn:snemo:demonstrator:geometry:4.0");
      if (dtkUrnQuery.check_urn_info(urn, falaise::tags::geometry_setup_category())) {
        urnInfo = dtkUrnQuery.get_urn_info(urn);
        std::clog << "The tag '" << urn << "' is registered with category '"
                  << urnInfo.get_category() << "'." << std::endl;
      }
    }

    {
      datatools::urn_info urnInfo;
      std::string urn("urn:snemo:demonstrator:simulation:2.1");
      if (dtkUrnQuery.check_urn_info(urn, falaise::tags::simulation_setup_category())) {
        urnInfo = dtkUrnQuery.get_urn_info(urn);
        std::clog << "The tag '" << urn << "' is registered with category '"
                  << urnInfo.get_category() << "'." << std::endl;
      }
    }

    {
      std::clog << "List of registered URNs : " << std::endl;
      std::vector<std::string> all_urns;
      std::string urn_db_regex = "(.*)";
      std::string urn_regex = "(.*)";
      std::string category_regex = "(.*)";
      if (dtkUrnQuery.find_urn_info(all_urns, urn_db_regex, urn_regex, category_regex, true)) {
        for (auto urn : all_urns) {
          std::clog << " - Tag : '" << urn << "'" << std::endl;
          datatools::urn_info urnInfo = dtkUrnQuery.get_urn_info(urn);
          urnInfo.tree_dump(std::clog, "", "    ");
        }
      }
    }

    std::clog << "The end." << std::endl;
  } catch (std::exception& x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "error: "
              << "unexpected error !" << std::endl;
    error_code = EXIT_FAILURE;
  }
  falaise::terminate();
  return (error_code);
}
