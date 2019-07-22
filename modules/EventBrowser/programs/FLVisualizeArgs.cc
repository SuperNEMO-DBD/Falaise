// Ourselves
#include "FLVisualizeArgs.h"

// Standard Library
#include <memory>

// Third Party
// - Bayeux
#include <bayeux/datatools/kernel.h>
#include <bayeux/datatools/urn_query_service.h>
#include <bayeux/dpp/input_module.h>

// This project:
#include <falaise/app/metadata_utils.h>
#include <falaise/tags.h>
#include <EventBrowser/detector/detector_manager.h>
#include <EventBrowser/view/options_manager.h>
#include "FLVisualizeCommandLine.h"

namespace FLVisualize {

// static
FLVisualizeArgs FLVisualizeArgs::makeDefault() {
  FLVisualizeArgs fvArgs;

  // Application specific parameters:
  fvArgs.logLevel = datatools::logger::PRIO_FATAL;
  fvArgs.libraries.clear();
  // Experimental setup:
  fvArgs.experimentalSetupUrn = "";  // "urn:snemo:demonstrator:setup:1.0";
  // Variants support:
  fvArgs.variantConfigUrn = "";
  fvArgs.variantProfileUrn = "";
  fvArgs.variants.config_filename = "";
  fvArgs.variants.profile_load = "";
  // Service support:
  fvArgs.servicesConfigUrn = "";
  fvArgs.servicesConfig = "";
  // I/O:
  fvArgs.ignoreInputMetadata = false;
  fvArgs.inputMetadataFile = "";
  fvArgs.inputFile = "";
  fvArgs.inputMetadata.reset();
  fvArgs.inputMetadata.set_key_label("name");
  fvArgs.inputMetadata.set_meta_label("type");

  return fvArgs;
}

void FLVisualizeArgs::print(std::ostream& out_) const {
  static const std::string tag("|-- ");
  static const std::string last_tag("`-- ");
  out_ << "FLVisualize parameters: " << std::endl;
  out_ << tag << "logLevel             = " << datatools::logger::get_priority_label(this->logLevel)
       << std::endl;
  out_ << tag << "libraries            = [" << libraries.size() << ']' << std::endl;
  out_ << tag << "experimentalSetupUrn = " << experimentalSetupUrn << std::endl;
  out_ << tag << "variantConfigUrn     = " << variantConfigUrn << std::endl;
  out_ << tag << "variantConfig        = " << variants.config_filename << std::endl;
  out_ << tag << "variantProfileUrn    = " << variantProfileUrn << std::endl;
  out_ << tag << "variantProfile       = " << variants.profile_load << std::endl;
  out_ << tag << "servicesConfigUrn    = " << servicesConfigUrn << std::endl;
  out_ << tag << "servicesConfig       = " << servicesConfig << std::endl;
  out_ << tag << "ignoreInputMetadata  = " << std::boolalpha << ignoreInputMetadata << std::endl;
  out_ << tag << "inputMetadataFile    = " << inputMetadataFile << std::endl;
  out_ << tag << "inputFile            = " << inputFile << std::endl;
  out_ << last_tag << "inputMetadata        = [" << inputMetadata.size() << ']' << std::endl;
  return;
}

void do_postprocess_input_metadata(FLVisualizeArgs& flVisParameters) {
  DT_LOG_TRACE_ENTERING(flVisParameters.logLevel);

  falaise::app::metadata_collector mc;
  if (!flVisParameters.inputMetadataFile.empty()) {
    // Fetch the metadata from the companion input metadata file, if any:
    DT_LOG_NOTICE(flVisParameters.logLevel,
                  "Fetching input metadata from input metadata companion file...");
    mc.set_input_metadata_file(flVisParameters.inputMetadataFile);
    flVisParameters.inputMetadata = mc.get_metadata_from_metadata_file();
  } else if (!flVisParameters.inputFile.empty()) {
    // Fetch the metadata from the input data file:
    DT_LOG_NOTICE(flVisParameters.logLevel, "Fetching input metadata from input data file...");
    mc.set_input_data_file(flVisParameters.inputFile);
    // flVisParameters.inputMetadata.clear_meta_label();
    flVisParameters.inputMetadata = mc.get_metadata_from_data_file();
  } else {
    // No metadata is available. Do nothing.
    // Probably we should handle this path in the future.
  }
  if (datatools::logger::is_notice(flVisParameters.logLevel)) {
    flVisParameters.inputMetadata.tree_dump(std::clog, "Loaded input metadata: ", "[notice]: ");
  }

  // Input metadata of interest:
  falaise::app::metadata_input iMeta;

  // Extract input metadata of interest:
  if (!flVisParameters.inputMetadata.empty()) {
    // Try to extract informations from the metadata:
    DT_LOG_NOTICE(flVisParameters.logLevel, "Found input metadata");
    iMeta.scan(flVisParameters.inputMetadata);
    if (datatools::logger::is_notice(flVisParameters.logLevel)) {
      iMeta.print(std::cerr);
    }
  }  // End of using input metadata

  if (!iMeta.experimentalSetupUrn.empty()) {
    DT_LOG_NOTICE(flVisParameters.logLevel,
                  "Input metadata from the experimental setup identifier (URN) is '"
                      << iMeta.experimentalSetupUrn << "'.");
  } else {
    DT_LOG_NOTICE(flVisParameters.logLevel,
                  "No experimental setup identifier (URN) is set from input metadata.");
  }

  // Checks:
  {
    // Check the experimental setup identifier:
    if (!flVisParameters.experimentalSetupUrn.empty()) {
      DT_THROW_IF(!iMeta.experimentalSetupUrn.empty() &&
                      (iMeta.experimentalSetupUrn != flVisParameters.experimentalSetupUrn),
                  std::logic_error,
                  "Experimental setup URN='" << flVisParameters.experimentalSetupUrn
                                             << "' conflicts with experimental setup URN='"
                                             << iMeta.experimentalSetupUrn
                                             << "' from input metadata!");
    }
  }  // End of checks.

  // Settings:
  {
    if (flVisParameters.experimentalSetupUrn.empty()) {
      // Force the experimental setup identifier from the value found int the input metadata
      flVisParameters.experimentalSetupUrn = iMeta.experimentalSetupUrn;
      DT_LOG_NOTICE(flVisParameters.logLevel, "The experimental setup identifier (URN) is set to '"
                                                  << iMeta.experimentalSetupUrn
                                                  << "' from input metadata.");
    }
  }  // End of settings.

  DT_LOG_TRACE_EXITING(flVisParameters.logLevel);
  return;
}

// static
void FLVisualizeArgs::do_postprocess(FLVisualizeArgs& flVisParameters) {
  DT_LOG_TRACE_ENTERING(flVisParameters.logLevel);
  namespace sv = snemo::visualization;
  datatools::kernel& dtk = datatools::kernel::instance();
  const datatools::urn_query_service& dtkUrnQuery = dtk.get_urn_query();

  // Process input metadata:
  if (flVisParameters.ignoreInputMetadata) {
    DT_LOG_NOTICE(flVisParameters.logLevel, "Do not process input metadata.");
  } else {
    do_postprocess_input_metadata(flVisParameters);
  }

  // Input file:
  if (!flVisParameters.inputFile.empty()) {
    sv::view::options_manager& options_mgr = sv::view::options_manager::get_instance();
    DT_LOG_NOTICE(flVisParameters.logLevel,
                  "Add input data file '" << flVisParameters.inputFile << "'");
    options_mgr.add_input_file(flVisParameters.inputFile);
  }

  // Experiment setup:
  datatools::urn_info expSetupUrnInfo;

  // Services:
  std::string servicesConfigUrn;
  datatools::urn_info servicesConfigUrnInfo;
  std::string servicesConfigPath;

  // Geometry:
  std::string geometryConfigUrn;
  datatools::urn_info geometryConfigUrnInfo;
  std::string geometryConfigPath;

  // Variant config:
  std::string variantConfigUrn;
  datatools::urn_info variantConfigUrnInfo;
  std::string variantConfigPath;

  // Variant profile:
  std::string variantProfileUrn;
  datatools::urn_info variantProfileUrnInfo;
  std::string variantProfilePath;

  // Fetch metadata here... not implemented yet

  // Resolve tags/paths:
  if (flVisParameters.experimentalSetupUrn.empty()) {
    // Default setup:
    flVisParameters.experimentalSetupUrn = "urn:snemo:demonstrator:setup:1.0";
    DT_LOG_NOTICE(flVisParameters.logLevel, "Using default experimental setup URN='"
                                                << flVisParameters.experimentalSetupUrn << "'...");
  }

  if (!flVisParameters.experimentalSetupUrn.empty()) {
    DT_LOG_NOTICE(flVisParameters.logLevel, "Using experimental setup URN='"
                                                << flVisParameters.experimentalSetupUrn << "'...");
    // Check URN registration from the system URN query service:
    {
      std::string conf_category = falaise::tags::experimental_setup_category();
      DT_THROW_IF(!dtkUrnQuery.check_urn_info(flVisParameters.experimentalSetupUrn, conf_category),
                  std::logic_error,
                  "Cannot query URN='" << flVisParameters.experimentalSetupUrn << "'!");
    }
    expSetupUrnInfo = dtkUrnQuery.get_urn_info(flVisParameters.experimentalSetupUrn);
  }

  // Services configuration:
  if (expSetupUrnInfo.is_valid()) {
    // Automatically determine the services configuration component:
    if (expSetupUrnInfo.has_topic("services") &&
        expSetupUrnInfo.get_components_by_topic("services").size() == 1) {
      servicesConfigUrn = expSetupUrnInfo.get_component("services");
    }
    if (!flVisParameters.servicesConfigUrn.empty()) {
      DT_THROW_IF(flVisParameters.servicesConfigUrn != servicesConfigUrn, std::logic_error,
                  "Services config URN='"
                      << flVisParameters.servicesConfigUrn << "' does not match "
                      << "the automatically resolved URN='" << servicesConfigUrn
                      << "' from experimental setup '" << flVisParameters.experimentalSetupUrn
                      << "'!");
    } else {
      flVisParameters.servicesConfigUrn = servicesConfigUrn;
    }
  }
  if (!flVisParameters.servicesConfigUrn.empty()) {
    DT_LOG_NOTICE(flVisParameters.logLevel,
                  "Services configuration URN='" << flVisParameters.servicesConfigUrn << "'...");
    // Check URN registration from the system URN query service:
    {
      std::string conf_category = falaise::tags::services_category();
      DT_THROW_IF(!dtkUrnQuery.check_urn_info(servicesConfigUrn, conf_category), std::logic_error,
                  "Cannot query URN='" << servicesConfigUrn << "'!");
    }
    servicesConfigUrnInfo = dtkUrnQuery.get_urn_info(servicesConfigUrn);
    // Resolve services configuration file:
    std::string conf_services_category = "configuration";
    std::string conf_services_mime;
    DT_THROW_IF(
        !dtkUrnQuery.resolve_urn_to_path(servicesConfigUrnInfo.get_urn(), conf_services_category,
                                         conf_services_mime, servicesConfigPath),
        std::logic_error, "Cannot resolve URN='" << servicesConfigUrnInfo.get_urn() << "'!");
    if (flVisParameters.servicesConfig.empty()) {
      flVisParameters.servicesConfig = servicesConfigPath;
    } else {
      DT_THROW_IF(flVisParameters.servicesConfig != servicesConfigPath, std::logic_error,
                  "Services config path='" << flVisParameters.servicesConfig << "' does not match "
                                           << "the automatically resolved path='"
                                           << servicesConfigPath << "' from services config URN='"
                                           << flVisParameters.servicesConfigUrn << "'!");
    }
    DT_LOG_NOTICE(flVisParameters.logLevel,
                  "Services configuration path='" << flVisParameters.servicesConfig << "'...");
  }

  // Geometry configuration:
  if (servicesConfigUrnInfo.is_valid()) {
    // Automatically determine the geometry configuration component:
    if (servicesConfigUrnInfo.has_topic("geometry") &&
        servicesConfigUrnInfo.get_components_by_topic("geometry").size() == 1) {
      geometryConfigUrn = servicesConfigUrnInfo.get_component("geometry");
      // Check URN registration from the system URN query service:
      {
        std::string conf_category = falaise::tags::geometry_setup_category();
        DT_THROW_IF(!dtkUrnQuery.check_urn_info(geometryConfigUrn, conf_category), std::logic_error,
                    "Cannot query URN='" << geometryConfigUrn << "'!");
      }
      geometryConfigUrnInfo = dtkUrnQuery.get_urn_info(geometryConfigUrn);
      DT_LOG_NOTICE(flVisParameters.logLevel,
                    "Geometry configuration URN='" << geometryConfigUrn << "'...");
    }
  }

  if (geometryConfigUrnInfo.is_valid()) {
    if (datatools::logger::is_trace(flVisParameters.logLevel)) {
      geometryConfigUrnInfo.tree_dump(std::clog, "geometryConfigUrnInfo: ", "[trace]: ");
    }
    // Resolve geometry configuration file:
    std::string conf_geometry_category = "configuration";
    std::string conf_geometry_mime;
    DT_THROW_IF(
        !dtkUrnQuery.resolve_urn_to_path(geometryConfigUrnInfo.get_urn(), conf_geometry_category,
                                         conf_geometry_mime, geometryConfigPath),
        std::logic_error, "Cannot resolve URN='" << geometryConfigUrnInfo.get_urn() << "'!");
    DT_LOG_NOTICE(flVisParameters.logLevel,
                  "Resolved geometry configuration path='" << geometryConfigPath << "'...");
  }

  if (!geometryConfigPath.empty()) {
    sv::view::options_manager& options_mgr = sv::view::options_manager::get_instance();
    if (options_mgr.get_detector_config_file().empty()) {
      options_mgr.set_detector_config_file(geometryConfigPath);
    } else {
      if (options_mgr.get_detector_config_file() != geometryConfigUrn &&
          options_mgr.get_detector_config_file() != geometryConfigPath) {
        DT_THROW(std::logic_error,
                 "Geometry config path='"
                     << options_mgr.get_detector_config_file() << "' does not match "
                     << "the automatically resolved path='" << geometryConfigPath << "'!");
      }
    }
    DT_LOG_NOTICE(flVisParameters.logLevel,
                  "Geometry configuration path='" << geometryConfigPath << "'...");
  }

  // Variant configuration:
  if (expSetupUrnInfo.is_valid()) {
    // Automatically determine the variant configuration component:
    if (expSetupUrnInfo.has_topic("variants") &&
        expSetupUrnInfo.get_components_by_topic("variants").size() == 1) {
      variantConfigUrn = expSetupUrnInfo.get_component("variants");
    }
    if (!flVisParameters.variantConfigUrn.empty()) {
      DT_THROW_IF(flVisParameters.variantConfigUrn != variantConfigUrn, std::logic_error,
                  "Variant config URN='" << flVisParameters.variantConfigUrn << "' does not match "
                                         << "the automatically resolved URN='" << variantConfigUrn
                                         << "' from experimental setup '"
                                         << flVisParameters.experimentalSetupUrn << "'!");
    } else {
      flVisParameters.variantConfigUrn = variantConfigUrn;
    }
  }
  if (!flVisParameters.variantConfigUrn.empty()) {
    DT_LOG_NOTICE(flVisParameters.logLevel,
                  "Variant configuration URN='" << variantConfigUrn << "'...");
    // Check URN registration from the system URN query service:
    {
      std::string conf_category = falaise::tags::variant_service_category();
      DT_THROW_IF(!dtkUrnQuery.check_urn_info(flVisParameters.variantConfigUrn, conf_category),
                  std::logic_error,
                  "Cannot query URN='" << flVisParameters.variantConfigUrn << "'!");
    }
    variantConfigUrnInfo = dtkUrnQuery.get_urn_info(variantConfigUrn);
    DT_LOG_NOTICE(flVisParameters.logLevel, "Resolving variant configuration URN='"
                                                << flVisParameters.variantConfigUrn << "'...");
    // Resolve variant configuration file:
    std::string conf_variants_category = "configuration";
    std::string conf_variants_mime;
    DT_THROW_IF(
        !dtkUrnQuery.resolve_urn_to_path(flVisParameters.variantConfigUrn, conf_variants_category,
                                         conf_variants_mime, variantConfigPath),
        std::logic_error, "Cannot resolve URN='" << flVisParameters.variantConfigUrn << "'!");
    if (flVisParameters.variants.config_filename.empty()) {
      flVisParameters.variants.config_filename = variantConfigPath;
    } else {
      DT_THROW_IF(flVisParameters.variants.config_filename != variantConfigPath, std::logic_error,
                  "Variant config path='"
                      << flVisParameters.variants.config_filename << "' does not match "
                      << "the automatically resolved path='" << variantConfigPath
                      << "' from variant config URN='" << flVisParameters.variantConfigUrn << "'!");
    }
    DT_LOG_NOTICE(flVisParameters.logLevel, "Variant configuration path='"
                                                << flVisParameters.variants.config_filename
                                                << "'...");
  }

  // Variant profile:
  if (!flVisParameters.variants.profile_load.empty()) {
    // Force the variant profile path:
    DT_THROW_IF(!flVisParameters.variantProfileUrn.empty(), std::logic_error,
                "Required variants profile URN='"
                    << flVisParameters.variantProfileUrn << "' "
                    << "conflicts with required variants profile path='"
                    << flVisParameters.variants.profile_load << "'!");
  } else {
    DT_LOG_NOTICE(flVisParameters.logLevel, "No variant profile is set.");
    if (flVisParameters.variantProfileUrn.empty()) {
      DT_LOG_NOTICE(flVisParameters.logLevel, "No variant profile URN is set.");
      // No variant profile URN is set:
      if (variantConfigUrnInfo.is_valid()) {
        DT_LOG_NOTICE(flVisParameters.logLevel,
                      "Trying to find a default one from the current variant setup...");
        if (datatools::logger::is_notice(flVisParameters.logLevel)) {
          variantConfigUrnInfo.tree_dump(std::clog,
                                         "Variant configuration URN info: ", "[notice]: ");
        }
        // Try to find a default one from the current variant setup:
        if (variantConfigUrnInfo.has_topic("__default_profile__") &&
            variantConfigUrnInfo.get_components_by_topic("__default_profile__").size() == 1) {
          // If the simulation setup URN implies a "services" component, fetch it!
          flVisParameters.variantProfileUrn =
              variantConfigUrnInfo.get_component("__default_profile__");
          DT_LOG_NOTICE(flVisParameters.logLevel, "Using the default variant profile '"
                                                      << flVisParameters.variantProfileUrn << "'"
                                                      << " associated to variant configuration '"
                                                      << variantConfigUrnInfo.get_urn() << "'.");
        }
      }
    }
    if (!flVisParameters.variantProfileUrn.empty()) {
      // Determine the variant profile path from a blessed variant profile URN:
      std::string conf_variantsProfile_category = "configuration";
      std::string conf_variantsProfile_mime;
      DT_THROW_IF(
          !dtkUrnQuery.resolve_urn_to_path(flVisParameters.variantProfileUrn,
                                           conf_variantsProfile_category, conf_variantsProfile_mime,
                                           variantProfilePath),
          std::logic_error,
          "Cannot resolve variants profile URN='" << flVisParameters.variantProfileUrn << "'!");
      flVisParameters.variants.profile_load = variantProfilePath;
    }
  }

  if (flVisParameters.variants.config_filename.empty()) {
    DT_LOG_WARNING(flVisParameters.logLevel, "No variant configuration is provided.");
  } else {
    if (flVisParameters.variants.profile_load.empty()) {
      DT_LOG_WARNING(flVisParameters.logLevel, "No variant profile is provided.");
    } else {
      // Additional variants settings may be allowed but *must* be compatible
      // with selected variants config and optional variants profile.
    }
  }

  if (flVisParameters.servicesConfig.empty()) {
    DT_LOG_WARNING(flVisParameters.logLevel, "No services configuration is provided.");
  }

  DT_LOG_TRACE_EXITING(flVisParameters.logLevel);
  return;
}

//! Parse command line arguments to configure the simulation parameters
void do_configure(int argc_, char* argv_[], FLVisualizeArgs& flVisParameters) {
  // - Default Config
  try {
    flVisParameters.logLevel = datatools::logger::PRIO_ERROR;
  } catch (std::exception& e) {
    throw FLConfigDefaultError();
  }

  // - CL Dialog Config
  try {
    do_cldialog(argc_, argv_, flVisParameters);
  } catch (FLDialogHelpRequested& e) {
    throw FLConfigHelpHandled();
  } catch (FLDialogOptionsError& e) {
    throw FLConfigUserError();
  }

  FLVisualizeArgs::do_postprocess(flVisParameters);

  if (datatools::logger::is_notice(flVisParameters.logLevel)) {
    flVisParameters.print(std::cerr);
  }
  return;
}

}  // namespace FLVisualize
