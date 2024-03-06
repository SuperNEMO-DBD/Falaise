// Ourselves
#include "FLpCDToCpCDParams.h"

namespace FLpCDToCpCD {

  // static
  FLpCDToCpCDParams FLpCDToCpCDParams::makeDefault()
  {
    FLpCDToCpCDParams params;

    // Application specific parameters:
    params.logLevel = datatools::logger::PRIO_ERROR;
    params.userProfile = "normal";
    params.numberOfEvents = 0; // 0 == no limit on event loop
    params.moduloEvents = 0; // 0 == no print
    params.preserveUnclusteredEvents = true;
    params.preserveUnclusteredHits = true;
    
    // Experimental setup:
    params.experimentalSetupUrn = ""; // "urn:snemo:demonstrator:setup:2.0"

    // pCDToCpCD setup:
    // params.pcdtocpcdSetupUrn = ""; // "urn:snemo:demonstrator:pcd2cpcd:1.0" snemo::processing::default_pcd2cpcd_tag()
    // params.pcdtocpcdPipelineUrn = ""; // "urn:snemo:demonstrator:pcd2cpcd:1.0:pipeline:official-1.0"
    // params.pcdtocpcdPipelineConfig = "";
 
    // Variants support:
    params.variantConfigUrn = "";
    params.variantProfileUrn = "";
    params.variantSubsystemParams.config_filename = "";
    params.variantSubsystemParams.profile_load = "";

    // Service support:
    params.servicesSubsystemConfigUrn = "";
    params.servicesSubsystemConfig = "";

    // I/O:
    params.inputMetadataFile = "";
    params.inputFile = "";
    params.outputMetadataFile = "";
    params.outputFile = "";
    params.inputMetadata.reset();
    params.inputMetadata.set_key_label("name");
    params.inputMetadata.set_meta_label("type");

    // Additional services:

    // Plugins management:
    params.userLibConfig.reset();
    params.userLibConfig.set_key_label("name");
    params.userLibConfig.set_meta_label("filename");

    // // Pipeline processing:
    // params.modulesConfig.reset();
    // params.modulesConfig.set_key_label("name");
    // params.modulesConfig.set_meta_label("type");

    return params;
  }

  void FLpCDToCpCDParams::print(std::ostream & out_) const
  {
    static const std::string tag("|-- ");
    static const std::string last_tag("`-- ");
    out_ << "FLpCDToCpCD setup parameters: " << std::endl;
    out_ << tag
         << "logLevel                       = " << datatools::logger::get_priority_label(this->logLevel)
         << std::endl;
    out_ << tag << "processSetupUrn       = '" << processSetupUrn << "'" << std::endl;
    out_ << tag << "processConfig         = '" << processConfig << "'" << std::endl;
    out_ << tag << "userProfile             = '" << userProfile << "'" << std::endl;
    out_ << tag << "numberOfEvents         = " << numberOfEvents << std::endl;
    out_ << tag << "moduloEvents           = " << moduloEvents << std::endl;
    out_ << tag << "experimentalSetupUrn    = '" << experimentalSetupUrn  << "'"<< std::endl;
    out_ << tag << "variantConfigUrn        = '" << variantConfigUrn << "'" << std::endl;
    out_ << tag << "variantProfileUrn       = '" << variantProfileUrn << "'" << std::endl;
    out_ << tag << "variantSubsystemParams  = '" << variantSubsystemParams.config_filename << "'"
         << std::endl;
    out_ << tag << "servicesSubsystemConfigUrn   = '" << servicesSubsystemConfigUrn << "'" << std::endl;
    out_ << tag << "servicesSubsystemConfig      = '" << servicesSubsystemConfig << "'" << std::endl;
    out_ << tag << "inputMetadataFile            = '" << inputMetadataFile << "'" << std::endl;
    out_ << tag << "inputFile                    = '" << inputFile << "'" << std::endl;
    out_ << tag << "outputMetadataFile           = '" << outputMetadataFile << "'" << std::endl;
    out_ << last_tag << "outputFile                   = '" << outputFile << "'" << std::endl;
    return;
  }

} // namespace FLpCDToCpCD
