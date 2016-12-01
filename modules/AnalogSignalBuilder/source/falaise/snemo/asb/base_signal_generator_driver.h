// snemo/asb/base_signal_generator_driver.h
// Author(s): F. Mauger <mauger@lpccaen.in2p3.fr>
// Author(s): G. Oliviéro <goliviero@lpccaen.in2p3.fr>
// Date: 2016-11-01

#ifndef FALAISE_ASB_PLUGIN_SNEMO_ASB_BASE_SIGNAL_GENERATOR_DRIVER_H
#define FALAISE_ASB_PLUGIN_SNEMO_ASB_BASE_SIGNAL_GENERATOR_DRIVER_H

namespace snemo {

  namespace asb {

    //! \brief Base class for signal generator driver classes
    class base_signal_generator_driver
    {
      /// Constructor
      base_signal_generator_driver();

      /// Destructor
      virtual ~base_signal_generator_driver();

      // API: to be done

    };

  } // end of namespace asb

} // end of namespace snemo

#include <datatools/ocd_macros.h>

// Declare the OCD interface of the module
DOCD_CLASS_DECLARATION(snemo::asb::base_signal_generator_driver)

#endif // FALAISE_ASB_PLUGIN_SNEMO_ASB_BASE_SIGNAL_GENERATOR_DRIVER_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
