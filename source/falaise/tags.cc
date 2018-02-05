// tags.cc

// Ourselves
#include "falaise/tags.h"

namespace falaise {

  namespace tags {

    const std::string & experiment_category()
    {
      static const std::string _c("experiment");
      return _c;
    }

    const std::string & experimental_setup_category()
    {
      static const std::string _c("expsetup");
      return _c;
    }

    const std::string & geometry_setup_category()
    {
      static const std::string _c("geomsetup");
      return _c;
    }

    const std::string & device_setup_category()
    {
      static const std::string _c("devsetup");
      return _c;
    }

    const std::string & electronics_setup_category()
    {
      static const std::string _c("elecsetup");
      return _c;
    }

    const std::string & cms_setup_category()
    {
      static const std::string _c("cmssetup");
      return _c;
    }

    const std::string & simulation_setup_category()
    {
      static const std::string _c("simsetup");
      return _c;
    }

    const std::string & reconstruction_setup_category()
    {
      static const std::string _c("recsetup");
      return _c;
    }

    const std::string & variant_service_category()
    {
      static const std::string _c("variants");
      return _c;
    }

    const std::string & services_category()
    {
      static const std::string _c("services");
      return _c;
    }

    const std::string & generic_configuration_category()
    {
      static const std::string _c("configuration");
      return _c;
    }

    const std::string & generic_data_category()
    {
      static const std::string _c("data");
      return _c;
    }

    const std::string & generic_image_category()
    {
      static const std::string _c("image");
      return _c;
    }

    const std::string & software_category()
    {
      static const std::string _c("software");
      return _c;
    }

    const std::string & firmware_category()
    {
      static const std::string _c("firmware");
      return _c;
    }

    const std::string & hardware_category()
    {
      static const std::string _c("hardware");
      return _c;
    }

  } // namespace tags

} // namespace falaise
