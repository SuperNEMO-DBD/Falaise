// Ourselves:
#include <falaise/version.h>
#include <falaise/snemo/simulation/config.h>

namespace snemo {

  namespace simulation {

    const std::string & default_simulation_tag()
    {
      static const std::string _simuUrn(FALAISE_DEFAULT_SIMULATION_TAG);
      return _simuUrn;
    }
  
  } // end of namespace simulation

} // end of namespace snemo
