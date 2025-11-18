#include <memory.h>

#include <falaise/falaise.h>

// Used by the module:
#include <dpp/base_module.h>
#include <falaise/snemo/services/geometry.h>
#include <falaise/snemo/services/service_handle.h>
#include <falaise/snemo/geometry/calo_locator.h>

/// A dummy module for SuperNEMO pipelines (should be in a header file 'snemo_dummy_module.hpp'):
class snemo_dummy_module
  : public dpp::base_module
{
public:
  
  /// Constructor
  snemo_dummy_module(datatools::logger::priority = datatools::logger::PRIO_FATAL);
  
  /// Destructor
  ~snemo_dummy_module() override;

  /// Initialization
  void initialize(const datatools::properties & config_,         // Configuration of this module
		  datatools::service_manager & service_manager_, // Access to a service manager
		  dpp::module_handle_dict_type & module_dict_    // Access to a collection of other modules (if any)
		  ) override;

  /// Reset
  void reset() override;

  /// Data record processing
  process_status process(datatools::things & event_record_) override;

private:
  
  snemo::service_handle<snemo::geometry_svc> _geometryManagerHdl_; ///< Handle to a geometry manager
  std::unique_ptr<snemo::geometry::calo_locator> _caloLocatorPtr_; ///< Handle to the calo locator
  
  // Macro to automate the registration of the module :
  DPP_MODULE_REGISTRATION_INTERFACE(snemo_dummy_module)
};


int main(void)
{
  falaise::initialize();
  int errorCode = EXIT_SUCCESS;
  std::clog << "Hello, snemo!\n";
 
  // Configuration of the service manager:
  datatools::properties serviceMgrConfig;
  serviceMgrConfig.store("logging.property", "debug");
  serviceMgrConfig.store("name", "SNServices");
  serviceMgrConfig.store("description", "SuperNEMO Demonstrator Services Manager for data processing");
  // Only the geometry service is setup here
  // but more could be added...
  std::vector<std::string> servicesConfigPaths = {
    "@falaise:snemo/demonstrator/geometry/5.0/GeometryServices.conf" 
  };
  serviceMgrConfig.store_paths("services.configuration_files", servicesConfigPaths);

  // Instantiate the service manager:
  datatools::service_manager serviceMgr;

  // Initialize the service manager:
  serviceMgr.initialize(serviceMgrConfig); 
  serviceMgr.tree_dump(std::clog, "Service manager:");

  {
    std::clog << "Entering the data processing pipeline scope...\n";

    // Configuration of the dummy module:
    datatools::properties dummyModuleConfig;
    dummyModuleConfig.store("name", "DummyModule");
    dummyModuleConfig.store("description", "A superNEMO example module for data processing");
    dummyModuleConfig.store("logging.priority", "debug");

    // Instantiate the dummy module:
    snemo_dummy_module dummyModule;

    // Initialize the dummy module only from its configuration and the service manager:
    dummyModule.initialize_with_service(dummyModuleConfig, serviceMgr);
    dummyModule.tree_dump(std::clog, "A dummy module:");
 
    {
      std::clog << "Entering the event processing loop part...\n";
      for (auto iEvent = 0u; iEvent < 3u; iEvent++) {
	std::clog << "\nProcessing event #" << iEvent << "...\n";
	datatools::things event;
	auto status = dummyModule.process(event);
	if (status == dpp::base_module::PROCESS_SUCCESS) {
	  std::clog << "Event #" << iEvent << " was successfully processed.\n";
	}
	
	std::clog << "Event #" << iEvent << " has now " << event.size() << " dummy banks:\n";
	event.tree_dump(std::clog, "", "    ");
	
      }
    }

    // Terminate the dummy module
    dummyModule.reset();
  }
  
  // Terminate the service manager
  serviceMgr.reset(); 
  
  std::clog << "Bye.\n";
  falaise::terminate();
  return errorCode;
}

//////////////////////////////////////////////
// Implementation of the dummy module class //
//////////////////////////////////////////////

// Note: should be in a source file 'snemo_dummy_module.cpp'

// Registration instantiation macro :
DPP_MODULE_REGISTRATION_IMPLEMENT(snemo_dummy_module, "snemo_dummy_module")

snemo_dummy_module::snemo_dummy_module(datatools::logger::priority verbosity_)
  : dpp::base_module(verbosity_)
{
  return;
}
  
snemo_dummy_module::~snemo_dummy_module()
{
  if (is_initialized()) {
    snemo_dummy_module::reset();
  }
  return;
}

void snemo_dummy_module::initialize(const datatools::properties & config_,
				    datatools::service_manager & service_manager_,
				    dpp::module_handle_dict_type & /* module_dict_ */)
{
  dpp::base_module::_common_initialize(config_);

  // Access to the geometry manager from the service manager:
  _geometryManagerHdl_ = snemo::service_handle<snemo::geometry_svc>{service_manager_};

  // Instantiate a calorimeter locator for module #0:
  std::uint32_t moduleNumber{0u};
  // For the main walls, the front part of the scin blocks (21 mm thickness) is registered.
  // This is useful for analyzing electron hits.
  // For gamma ray hits, we could be more interested by the back part of the scin blocks,
  // which have a larger volume for Compton interaction.
  std::uint32_t blockPart = snemo::geometry::calo_locator::BLOCK_FRONT_PART;
  datatools::properties caloLocatorConfig;
  _caloLocatorPtr_ =
    std::make_unique<snemo::geometry::calo_locator>(moduleNumber,
						    *(_geometryManagerHdl_.operator->()),
						    caloLocatorConfig,
						    blockPart);
  if (datatools::logger::is_debug(get_logging_priority())) {
    _caloLocatorPtr_->tree_dump(std::cerr, "Calorimeter locator:", "[debug] ");
  }
  
  // Tag the module as initialized :
  _set_initialized(true);
  return;
}

void snemo_dummy_module::reset()
{
  // Tag the module as uninitialized :
  _set_initialized(false);
  _caloLocatorPtr_.reset();
  return;
}

snemo_dummy_module::process_status
snemo_dummy_module::process(datatools::things & event_record_)
{
  // Add a dummy bank of properties:
  auto & newBank1 = event_record_.add<datatools::properties>("Bank1");
  newBank1.store("test", true);
  newBank1.store("description", "a dummy bank of properties");
  newBank1.store("count", 42);
  newBank1.store("value", 1.0);

  // Add another dummy bank of properties:
  auto & newBank2 = event_record_.add<datatools::properties>("Bank2");
  newBank2.store("test_again", true);
  newBank2.store("description", "another dummy bank of properties");
  newBank2.store("count", 10000);
  std::vector<double> values{ 1.0, 2.0, 3.0 };
  newBank2.store("values", values);

  // Use the calo locator with dummy geomids:
  geomtools::geom_id caloBlockId(1302, 0, 0, 0, 0, 0);
  if (_caloLocatorPtr_->isCaloBlock(caloBlockId)) {
    std::clog << "GeomID " << caloBlockId << " is a calo block ID\n";
    geomtools::vector_3d blockPos = _caloLocatorPtr_->getBlockPosition(caloBlockId);
    std::clog << "  Block position = " << blockPos << '\n';    
    geomtools::vector_3d blockWinPos
      = _caloLocatorPtr_->getBlockWindowPosition(_caloLocatorPtr_->getSideAddress(caloBlockId),
						 _caloLocatorPtr_->getColumnAddress(caloBlockId),
						 _caloLocatorPtr_->getRowAddress(caloBlockId));
    std::clog << "  Block window position = " << blockWinPos << '\n';    
  }
  if (not _caloLocatorPtr_->isCaloOM(caloBlockId)) {
    std::clog << "GeomID " << caloBlockId << " is a not calo OM ID\n";
  }
 
  geomtools::geom_id caloOmId(1301, 0, 0, 0, 0);
  if (_caloLocatorPtr_->isCaloOM(caloOmId)) {
    std::clog << "GeomID " << caloOmId << " is a calo OM ID\n";
  }
  if (_caloLocatorPtr_->isCaloBlock(caloOmId)) {
    std::clog << "GeomID " << caloOmId << " is not a calo block ID\n";
  }
  
  return dpp::base_module::PROCESS_SUCCESS;
}
