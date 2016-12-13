// snemo/asb/calo_signal_generator_driver.h
// Author(s): F. Mauger <mauger@lpccaen.in2p3.fr>
// Author(s): G. Oliviéro <goliviero@lpccaen.in2p3.fr>
// Date: 2016-12-02

#ifndef FALAISE_ASB_PLUGIN_SNEMO_ASB_CALO_SIGNAL_GENERATOR_DRIVER_H
#define FALAISE_ASB_PLUGIN_SNEMO_ASB_CALO_SIGNAL_GENERATOR_DRIVER_H

// Standard library:
#include <string>
#include <memory>

// Third party:
// - Boost:
#include <boost/noncopyable.hpp>

// This project:
#include <snemo/asb/base_signal_generator_driver.h>

namespace snemo {

  namespace asb {

    /// \brief Calorimeter signal generator driver
    class calo_signal_generator_driver
      : public base_signal_generator_driver
      , private boost::noncopyable
    {
    public:

      /// \brief Driver mode
      enum mode_type {
        MODE_INVALID  = 0, ///< Invalid mode
        MODE_TRIANGLE = 1 ///< Simplified mode with triangular shape signals
        // MODE_XXX = 2, ///< XXX mode with realistic shape signals
      };

      /// Constructor
      calo_signal_generator_driver(const std::string & id_ = "calo");

      /// Constructor
      calo_signal_generator_driver(const mode_type mode_, const std::string & id_ = "calo");

      /// Destructor
      virtual ~calo_signal_generator_driver();

      /// Set the driver mode
      void set_mode(const mode_type);

      /// Return the driver mode
      mode_type get_mode() const;
			
    protected :

      /// Initialize the algorithm through configuration properties
      virtual void _initialize(const datatools::properties & config_);

      /// Reset the algorithm
      virtual void _reset();
			
			/// Convert the calo energy hit into amplitude
			double _convert_energy_to_amplitude(const double energy_);

      /// Run the algorithm
			void _process(const mctools::simulated_data & sim_data_,
										mctools::signal::signal_data & sim_signal_data_);

    private:

			/// Run the triangle mode process
      void _process_triangle_mode_(const mctools::simulated_data & sim_data_,
                                  mctools::signal::signal_data & sim_signal_data_);

    private:

      mode_type _mode_ = MODE_INVALID; //!< Mode type for calo signals
			
      struct pimpl_type; //!< Private implementation structure
      std::unique_ptr<pimpl_type> _pimpl_; //!< Pointer to a private implementation structure
			
			
    };

  } // end of namespace asb

} // end of namespace snemo

#include <datatools/ocd_macros.h>

// Declare the OCD interface of the module
DOCD_CLASS_DECLARATION(snemo::asb::calo_signal_generator_driver)

#endif // FALAISE_ASB_PLUGIN_SNEMO_ASB_CALO_SIGNAL_GENERATOR_DRIVER_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
