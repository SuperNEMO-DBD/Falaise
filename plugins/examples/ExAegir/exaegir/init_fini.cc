
// Standard library:
#include <iostream>

// Third party:
#include <bayeux/bayeux.h>
#include <datatools/kernel.h>
#include <datatools/library_info.h>
#include <genbb_help/i_genbb.h>
#include <datatools/factory.h>
#include <datatools/factory_macros.h>

#include <exaegir/version.h>
#include <exaegir/resources.h>

namespace {

  static bool _local_bayeux_initialization = false;
  
  static __attribute__((constructor)) void exaegir_init()
  {
    std::cerr << "[log] [exaegir_init]: Loading the 'exaegir' plugin library (version " << exaegir::version_string() << ")..." << std::endl;

    // do something at library loading...
    
    if (not bayeux::is_initialized()) {
      std::cerr << "[log] [exaegir_init]: Forcing Bayeux initialization..." << std::endl;
      bayeux::initialize();
      _local_bayeux_initialization = true;
    } else {
      std::cerr << "[log] [exaegir_init]: Bayeux is already initialized." << std::endl;
    }

    // Now we can use the Bayeux's kernel:
    datatools::kernel & krnl = datatools::kernel::instance();
    
    datatools::library_info & lib_info_reg = krnl.grab_library_info_register();
    DT_THROW_IF(lib_info_reg.has("exaegir"), std::logic_error, "exaegir is already registered !");
    datatools::properties & exaegir_lib_infos = lib_info_reg.registration(
      "exaegir",
      "exaegir provides some additional primary event generator(s) and associated data to Falaise.",
      exaegir::version_string());

    // Register the exaegir resource path in the datatools' kernel:
    exaegir_lib_infos.store_string(datatools::library_info::keys::install_resource_dir(),
				   exaegir::get_resource_dir());

    // If the 'EXAEGIR_RESOURCE_DIR' environment variable is set, it will supersede
    // the official registered resource path above through the 'datatools::fetch_path_with_env'
    // function:   
    exaegir_lib_infos.store_string(datatools::library_info::keys::env_resource_dir(),
				   "EXAEGIR_RESOURCE_DIR");

    // Dump the Kernel's guts...
    // krnl.tree_dump(std::cerr, "Bayeux's kernel: ", "[log] ");

    // Print the list of registered event generators:
    const datatools::factory_register<genbb::i_genbb> & genbbEventGeneratorFactory
      = DATATOOLS_FACTORY_GET_SYSTEM_REGISTER(genbb::i_genbb);
    genbbEventGeneratorFactory.print(std::cerr, "[log] [exaegir_init]: ");
    
    std::cerr << "[log] [exaegir_init]: The 'exaegir' plugin library has been loaded." << std::endl;
    return;
  }

  static __attribute__((destructor)) void aegir_fini()
  {  
    std::cerr << "[log] [exaegir_fini]: Unloading the 'exaegir' plugin library..." << std::endl;
    
    // do something at library unloading...
    
    datatools::kernel & krnl = datatools::kernel::instance();
    if (krnl.has_library_info_register()) {
      // Access to the datatools kernel library info register:
      datatools::library_info & lib_info_reg = krnl.grab_library_info_register();
      // Unregistration of the 'exaegir' registered submodule from the kernel's
      // library info register:
      if (lib_info_reg.has("exaegir")) {
        lib_info_reg.unregistration("exaegir");
      }
    }

    // Special operation with Bayeux:
    if (bayeux::is_initialized() and _local_bayeux_initialization) {
      std::cerr << "[log] [exaegir_fini]: Forcing Bayeux termination..." << std::endl;
      bayeux::terminate();
      _local_bayeux_initialization = true;
    }
    
    std::cerr << "[log] [exaegir_fini]: The 'exaegir' plugin library has been unloaded." << std::endl;
    return;
  }
  
}
