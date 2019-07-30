// falaise/detail/falaise_sys.cc - Implementation of Falaise system singleton
//

// Ourselves:
#include <falaise/detail/falaise_sys.h>

// Standard library
#include <cstdlib>
#include <memory>
#include <string>

// Third party:
#include <datatools/kernel.h>
#include <datatools/library_info.h>
#include <datatools/urn_db_service.h>
#include <datatools/urn_to_path_resolver_service.h>

// This project;
#include "falaise/resource.h"
#include "falaise/version.h"

namespace falaise {

namespace detail {

// static
const std::string &falaise_sys::fl_setup_db_name() {
  static const std::string _n("flSetupDb");
  return _n;
}

// static
const std::string &falaise_sys::fl_resource_resolver_name() {
  static const std::string _n("flResourceResolver");
  return _n;
}

datatools::logger::priority falaise_sys::process_logging_env() {
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  char *l = getenv("FALAISE_SYS_LOGGING");
  if (l != nullptr) {
    std::string level_label(l);
    ::datatools::logger::priority prio = ::datatools::logger::get_priority(level_label);
    if (prio != ::datatools::logger::PRIO_UNDEFINED) {
      logging = prio;
    }
  }
  return logging;
}

// static
falaise_sys *falaise_sys::_instance_ = nullptr;

falaise_sys::falaise_sys() {
  _logging_ = falaise_sys::process_logging_env();
  if (_logging_ == ::datatools::logger::PRIO_UNDEFINED) {
    DT_LOG_WARNING(::datatools::logger::PRIO_WARNING, "Ignoring invalid FALAISE_SYS_LOGGING=\""
                                                          << getenv("FALAISE_SYS_LOGGING")
                                                          << "\" environment!");
  }
  DT_LOG_TRACE_ENTERING(_logging_);
  DT_THROW_IF(falaise_sys::_instance_ != nullptr, std::logic_error,
              "Falaise system singleton is already set!");
  falaise_sys::_instance_ = this;
  DT_LOG_TRACE_EXITING(_logging_);
}

falaise_sys::~falaise_sys() {
  DT_LOG_TRACE_ENTERING(_logging_);
  if (is_initialized()) {
    shutdown();
  }
  falaise_sys::_instance_ = nullptr;
  DT_LOG_TRACE_EXITING(_logging_);
}

datatools::logger::priority falaise_sys::get_logging() const { return _logging_; }

bool falaise_sys::is_initialized() const { return _initialized_; }

void falaise_sys::initialize() {
  DT_LOG_TRACE_ENTERING(_logging_);
  DT_THROW_IF(is_initialized(), std::logic_error,
              "Falaise system singleton is already initialized!");

  // Register library informations in the Bayeux/datatools' kernel:
  _libinfo_registration_();

  // Setup services:
  DT_LOG_TRACE(_logging_, "Falaise system singleton services...");
  _services_.set_name("flservices");
  _services_.set_description("Falaise System Singleton Services");
  _services_.set_allow_dynamic_services(true);
  _services_.initialize();

  _initialize_urn_services_();

  _initialized_ = true;
  DT_LOG_TRACE_EXITING(_logging_);
}

void falaise_sys::shutdown() {
  DT_LOG_TRACE_ENTERING(_logging_);
  DT_THROW_IF(!is_initialized(), std::logic_error, "Falaise system singleton is not initialized!");
  _initialized_ = false;

  // Terminate services:
  if (_services_.is_initialized()) {
    _shutdown_urn_services_();

    DT_LOG_TRACE(_logging_, "Terminating Falaise system singleton services...");
    _services_.reset();
  }

  // Deregister library informations from the Bayeux/datatools' kernel:
  _libinfo_deregistration_();

  DT_LOG_TRACE_EXITING(_logging_);
}

datatools::service_manager &falaise_sys::grab_services() { return _services_; }

const datatools::service_manager &falaise_sys::get_services() const { return _services_; }

// static
bool falaise_sys::is_instantiated() { return _instance_ != nullptr; }

// static
falaise_sys &falaise_sys::instance() { return *_instance_; }

// static
const falaise_sys &falaise_sys::const_instance() { return *_instance_; }

// static
falaise_sys &falaise_sys::instantiate() {
  if (!falaise_sys::is_instantiated()) {
    static std::unique_ptr<falaise_sys> _flsys_handler;
    if (!_flsys_handler) {
      // Allocate the Falaise sys library singleton and initialize it:
      _flsys_handler.reset(new falaise_sys);
    }
  }
  return falaise_sys::instance();
}

void falaise_sys::_libinfo_registration_() {
  DT_LOG_TRACE_ENTERING(_logging_);

  DT_THROW_IF(!datatools::kernel::is_instantiated(), std::runtime_error,
              "The Bayeux/datatools' kernel is not instantiated !");
  datatools::kernel &krnl = datatools::kernel::instance();

  // Populate the library info register, basically dumb if we don't
  // have it so assume it exists and hope for an exception if
  // it doesn't
  datatools::library_info &lib_info_reg = krnl.grab_library_info_register();

  // Bundled submodules:
  {
    DT_LOG_TRACE(_logging_, "Registration of Falaise library in the Bayeux/datatools' kernel...");
    // Falaise itself:
    DT_THROW_IF(lib_info_reg.has("falaise"), std::logic_error, "falaise is already registered !");
    datatools::properties &falaise_lib_infos = lib_info_reg.registration(
        "falaise",
        "Falaise provides the main computational environment for the simulation,"
        "processing and analysis of data for the SuperNEMO double beta decay "
        "search experiment.",
        falaise::version::get_version());

    // Register the Falaise resource path in the datatools' kernel:
    falaise_lib_infos.store_string(datatools::library_info::keys::install_resource_dir(),
                                   falaise::get_resource_dir());

    // If the 'FALAISE_RESOURCE_DIR' environment variable is set, it will supersede
    // the official registered resource path above through the 'datatools::fetch_path_with_env'
    // function:
    falaise_lib_infos.store_string(datatools::library_info::keys::env_resource_dir(),
                                   "FALAISE_RESOURCE_DIR");

    // Register the Falaise plugin path in the datatools' kernel:
    falaise_lib_infos.store_string(datatools::library_info::keys::install_plugin_lib_dir(),
                                   falaise::get_plugin_dir());

    // If the 'FALAISE_PLUGIN_LIB_DIR' environment variable is set, it will supersed
    // the official registered plugin path above through the 'datatools::library_loader'
    // function:
    falaise_lib_infos.store_string(datatools::library_info::keys::env_plugin_lib_dir(),
                                   "FALAISE_PLUGIN_LIB_DIR");
    DT_LOG_TRACE(_logging_, "Falaise library has been registered in the Bayeux/datatools' kernel.");
  }

  DT_LOG_TRACE_EXITING(_logging_);
}

void falaise_sys::_libinfo_deregistration_() {
  DT_LOG_TRACE_ENTERING(_logging_);

  if (datatools::kernel::is_instantiated()) {
    datatools::kernel &krnl = datatools::kernel::instance();
    if (krnl.has_library_info_register()) {
      // Access to the datatools kernel library info register:
      datatools::library_info &lib_info_reg = krnl.grab_library_info_register();

      // Unregistration of all registered submodules from the kernel's
      // library info register:
      if (lib_info_reg.has("falaise")) {
        DT_LOG_TRACE(_logging_,
                     "Deregistration of the Falaise library from the Bayeux/datatools' kernel...");
        lib_info_reg.unregistration("falaise");
        DT_LOG_TRACE(_logging_,
                     "Falaise library has been deregistered from the Bayeux/datatools' kernel.");
      }
    }
  }

  DT_LOG_TRACE_EXITING(_logging_);
}

void falaise_sys::_initialize_urn_services_() {
  DT_LOG_TRACE_ENTERING(_logging_);

  // Currently, path to these are hardcoded...
  std::string urn_db_conf_file = "@falaise:urn_db.conf";
  std::string urn_resolver_conf_file = "@falaise:urn_resolver.conf";

  // Activate an URN info DB service:
  {
    datatools::urn_db_service &urnSetupDb = dynamic_cast<datatools::urn_db_service &>(
        _services_.load_no_init(fl_setup_db_name(), "datatools::urn_db_service"));
    urnSetupDb.set_logging_priority(_logging_);
    datatools::fetch_path_with_env(urn_db_conf_file);
    datatools::properties urn_db_conf;
    urn_db_conf.read_configuration(urn_db_conf_file);
    urnSetupDb.initialize_standalone(urn_db_conf);
    if (datatools::logger::is_debug(_logging_)) {
      urnSetupDb.tree_dump(std::cerr, urnSetupDb.get_name() + ": ", "[debug] ");
    }
    DT_LOG_TRACE(_logging_, "Publishing the URN info DB '"
                                << urnSetupDb.get_name() << "' to the Bayeux/datatools' kernel...");
    urnSetupDb.kernel_push();
    DT_LOG_TRACE(_logging_, "URN info DB has been plugged in the Bayeux/datatools' kernel.");
  }

  // Activate an URN resolver service:
  {
    datatools::urn_to_path_resolver_service &urnResourceResolver =
        dynamic_cast<datatools::urn_to_path_resolver_service &>(_services_.load_no_init(
            fl_resource_resolver_name(), "datatools::urn_to_path_resolver_service"));
    urnResourceResolver.set_logging_priority(_logging_);
    datatools::fetch_path_with_env(urn_resolver_conf_file);
    datatools::properties urn_resolver_conf;
    urn_resolver_conf.read_configuration(urn_resolver_conf_file);
    urnResourceResolver.initialize_standalone(urn_resolver_conf);
    if (datatools::logger::is_debug(_logging_)) {
      urnResourceResolver.tree_dump(std::cerr, urnResourceResolver.get_name() + ": ", "[debug] ");
    }
    DT_LOG_TRACE(_logging_, "Publishing the URN path resolver '"
                                << urnResourceResolver.get_name()
                                << "' to the Bayeux/datatools' kernel...");
    urnResourceResolver.kernel_push();
    DT_LOG_TRACE(_logging_, "URN path resolver has been plugged in the Bayeux/datatools' kernel.");
  }

  DT_LOG_TRACE_EXITING(_logging_);
}

void falaise_sys::_shutdown_urn_services_() {
  DT_LOG_TRACE_ENTERING(_logging_);

  // DeActivate the URN resolver:
  {
    DT_LOG_TRACE(_logging_, "Accessing URN path resolver...");
    datatools::urn_to_path_resolver_service &urnResourceResolver =
        _services_.grab<datatools::urn_to_path_resolver_service &>(fl_resource_resolver_name());
    DT_LOG_TRACE(_logging_, "Removing URN path resolver '"
                                << urnResourceResolver.get_name()
                                << "' from the  Bayeux/datatools's kernel...");
    urnResourceResolver.kernel_pop();
    DT_LOG_TRACE(_logging_,
                 "URN path resolver has been removed from the  Bayeux/datatools kernel.");
    urnResourceResolver.reset();
  }

  // DeActivate the URN info setup DB:
  {
    DT_LOG_TRACE(_logging_, "Accessing URN info setup DB...");
    datatools::urn_db_service &urnSetupDb =
        _services_.grab<datatools::urn_db_service &>(fl_setup_db_name());
    DT_LOG_TRACE(_logging_, "Removing URN info setup DB '"
                                << urnSetupDb.get_name()
                                << "' from the  Bayeux/datatools's kernel...");
    urnSetupDb.kernel_pop();
    DT_LOG_TRACE(_logging_,
                 "URN info setup DB has been removed from the  Bayeux/datatools kernel.");
    urnSetupDb.reset();
  }

  DT_LOG_TRACE_EXITING(_logging_);
}

}  // end of namespace detail

}  // namespace falaise
