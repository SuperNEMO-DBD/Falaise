// test_falaise.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

// Third party:
// - Bayeux:
#include <bayeux/datatools/kernel.h>
#include <bayeux/datatools/library_info.h>
#include <bayeux/datatools/urn_query_service.h>
#include <bayeux/datatools/urn_db_service.h>

// This project:
#include <falaise/falaise.h>
#include <falaise/resource.h>
#include <falaise/version.h>
#include <falaise/detail/falaise_sys.h>

int main(int /* argc_ */, char** /* argv_ */) {
  falaise::initialize();
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for the 'Falaise' library." << std::endl;

    std::clog << "Falaise version       : " << falaise::version::get_version() << std::endl;
    std::clog << "Falaise resource dir  : '" << falaise::get_resource_dir() << "'" << std::endl;
    std::clog << "Falaise resource file : '" << falaise::get_resource("README.rst") << "'"
              << std::endl;

    {
      if (datatools::kernel::is_instantiated()) {
        datatools::kernel & krnl = datatools::kernel::instance();
        if (krnl.has_library_info_register()) {
          const datatools::library_info & lib_info_reg = krnl.get_library_info_register();
          lib_info_reg.tree_dump(std::clog, "Kernel library info register: ");
          std::clog << std::endl;
        }
        // Only with Bayeux from 3.3.0 because the 'datatools::urn_query_service::get_db' method
        // is not available before.
        // if (krnl.get_urn_query().has_db(falaise::detail::falaise_sys::fl_setup_db_name())) {
        //   const datatools::urn_db_service & flUrnDb =
        //     krnl.get_urn_query().get_db(falaise::detail::falaise_sys::fl_setup_db_name());
        //   flUrnDb.tree_dump(std::clog, "Falaise library's URN Database Service: ");
        //   std::clog << std::endl;
        // }
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
