// snemo/asb/base_signal_generator_driver.h
// Author(s): F. Mauger <mauger@lpccaen.in2p3.fr>
// Author(s): G. Oliviéro <goliviero@lpccaen.in2p3.fr>
// Date: 2016-11-01

#ifndef FALAISE_ASB_PLUGIN_SNEMO_ASB_BASE_SIGNAL_GENERATOR_DRIVER_H
#define FALAISE_ASB_PLUGIN_SNEMO_ASB_BASE_SIGNAL_GENERATOR_DRIVER_H

// Third party:
// - Bayeux/datatools:
#include <bayeux/datatools/logger.h>
#include <bayeux/datatools/i_tree_dump.h>
#include <bayeux/datatools/properties.h>
#include <bayeux/datatools/factory_macros.h>
#include <bayeux/datatools/object_configuration_description.h>
// - Bayeux/geomtools:
#include <bayeux/geomtools/manager.h>
// - Bayeux/mctools:
#include <bayeux/mctools/simulated_data.h>
#include <bayeux/mctools/signal/signal_data.h>

namespace snemo {

  namespace asb {

    //! \brief Base class for signal generator driver classes
    class base_signal_generator_driver
      : public datatools::i_tree_dumpable
    {
    public:

      /// Constructor
      base_signal_generator_driver(const std::string & id_ = "");

      /// Destructor
      virtual ~base_signal_generator_driver();

      /// Set logging priority level
      void set_logging_priority(datatools::logger::priority logging_priority_);

      /// Get logging priority
      datatools::logger::priority get_logging_priority() const;

      /// Check the algorithm ID
      bool has_id() const;

      // Set the algorithm ID
      void set_id(const std::string & id_);

      /// Return the algorithm ID
      const std::string & get_id() const;

      /// Check the signal category
      bool has_signal_category() const;

      // Set the signal category
      void set_signal_category(const std::string & category_);

      /// Return the signal category
      const std::string & get_signal_category() const;

      /// Check geometry manager
      bool has_geo_manager() const;

      /// Set the geometry manager
      void set_geo_manager(const geomtools::manager & mgr_ );

      /// Return the geometry manager
      const geomtools::manager & get_geo_manager() const;

      /// Check if the algorithm is initialized
      bool is_initialized() const;

      /// Initialize the algorithm through configuration properties
      void initialize(const datatools::properties & config_);

      /// Reset the algorithm
      void reset();

      /// Run the algorithm
      void process(const mctools::simulated_data & sim_data_,
                   mctools::signal::signal_data & sim_signal_data_);

      // Smart print
      virtual void tree_dump(std::ostream & out_ = std::clog,
                             const std::string & title_ = "",
                             const std::string & indent_ = "",
                             bool inherit_ = false) const;


    protected :

      /// Set default attribute values
      void _set_defaults();

      /// Initialize the algorithm through configuration properties
      virtual void _initialize(const datatools::properties & config_) = 0;

      /// Reset the algorithm
      virtual void _reset() = 0;

      /// Run the algorithm
      virtual void _process(const mctools::simulated_data & sim_data_,
                            mctools::signal::signal_data & sim_signal_data_) = 0;

    private:

      /// Set the initialization flag
      void _set_initialized_(bool);

    private:

      // Management:
      bool        _initialized_ = false; //!< Initialization status
      datatools::logger::priority _logging_priority_; //!< Logging priority

      // Configuration:
      std::string _id_; //!< Identifier of the algorithm
      std::string _signal_category_; //!< Identifier of the signal category
      const geomtools::manager * _geo_manager_ = nullptr; //!< Geometry manager

      // Factory stuff :
      DATATOOLS_FACTORY_SYSTEM_REGISTER_INTERFACE(base_signal_generator_driver)

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
