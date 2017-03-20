// Ourselves
#include "FLReconstructParams.h"

namespace FLReconstruct {

  // static
  FLReconstructParams FLReconstructParams::makeDefault()
  {
    FLReconstructParams params;

    // Application specific parameters:
    params.logLevel     = datatools::logger::PRIO_ERROR;
    params.userProfile  = "normal";
    params.numberOfEvents = 0; // 0 == no limit on event loop
    params.moduloEvents = 0;

    // Experimental setup:
    params.experimentalSetupUrn = "";  // "urn:snemo:demonstrator:setup:1.0";

    // Reconstruction setup:
    params.reconstructionSetupUrn = "";  // "urn:snemo:demonstrator:reconstruction:1.0";
    params.reconstructionSetupConfig = "";
    params.reconstructionSetupModule = "pipeline";

    // Variants support:
    params.variantConfigUrn  = "";
    params.variantProfileUrn = "";
    params.variantSubsystemParams.config_filename  = "";

    // Service support:
    params.servicesSubsystemConfigUrn = "";
    params.servicesSubsystemConfig = "";

    // I/O:
    params.inputMetadataFile  = "";
    params.inputFile          = "";
    params.outputMetadataFile = "";
    params.embeddedMetadata   = false;
    params.outputFile         = "";
    params.inputMetadata.reset();
    params.inputMetadata.set_key_label("name");
    params.inputMetadata.set_meta_label("type");

    // Additional informations:
    params.dataType              = "";
    params.dataSubtype           = "";
    params.requiredInputBanks.clear();
    params.expectedOutputBanks.clear();

    // Additional services:

    // Plugins management:
    params.userLibConfig.reset();
    params.userLibConfig.set_key_label("name");
    params.userLibConfig.set_meta_label("filename");

    // Pipeline processing:
    params.modulesConfig.reset();
    params.modulesConfig.set_key_label("name");
    params.modulesConfig.set_meta_label("type");

    return params;
  }

  void FLReconstructParams::print(std::ostream & out_) const
  {
    static const std::string tag("|-- ");
    static const std::string last_tag("`-- ");
    out_ << "FLReconstruct setup parameters: " << std::endl;
    out_ << tag << "logLevel                   = " << datatools::logger::get_priority_label(this->logLevel) << std::endl;
    out_ << tag << "userProfile                = " << userProfile << std::endl;
    out_ << tag << "numberOfEvents             = " << numberOfEvents << std::endl;
    out_ << tag << "moduloEvents               = " << moduloEvents << std::endl;
    out_ << tag << "experimentalSetupUrn       = " << experimentalSetupUrn << std::endl;
    out_ << tag << "reconstructionSetupUrn     = " << reconstructionSetupUrn << std::endl;
    out_ << tag << "reconstructionSetupConfig  = " << reconstructionSetupConfig << std::endl;
    out_ << tag << "reconstructionSetupModule  = " << reconstructionSetupModule << std::endl;
    out_ << tag << "variantConfigUrn           = " << variantConfigUrn << std::endl;
    out_ << tag << "variantProfileUrn          = " << variantProfileUrn << std::endl;
    out_ << tag << "variantSubsystemParams     = " << variantSubsystemParams.config_filename << std::endl;
    out_ << tag << "servicesSubsystemConfigUrn = " << servicesSubsystemConfigUrn << std::endl;
    out_ << tag << "servicesSubsystemConfig    = " << servicesSubsystemConfig << std::endl;
    out_ << tag << "inputMetadataFile          = " << inputMetadataFile << std::endl;
    out_ << tag << "inputFile                  = " << inputFile << std::endl;
    out_ << tag << "outputMetadataFile         = " << outputMetadataFile << std::endl;
    out_ << tag << "embeddedMetadata           = " << std::boolalpha << embeddedMetadata << std::endl;
    out_ << tag
         << "outputFile                 = " << outputFile << std::endl;
    out_ << tag
         << "dataType                   = " << dataType << std::endl;
    out_ << last_tag
         << "dataSubtype                = " << dataSubtype << std::endl;
    return;
  }

} // namespace FLReconstruct
