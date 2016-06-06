// FLSimulateVariance.h - Interface for accessing FLSimulate variant parameters
//
// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2016 by SuperNEMO

// Distributed under the OSI-approved BSD 3-Clause License (the "License");
// see accompanying file License.txt for details.
//
// This software is distributed WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the License for more information.

#ifndef FLSIMULATEVARIANCE_H
#define FLSIMULATEVARIANCE_H

// Standard Library:
#include <string>
#include <vector>
#include <stdexcept>

// Third party:
// - Bayeux:
#include <bayeux/bayeux.h>
// - Bayeux/datatools:
#include <datatools/configuration/variant_repository.h>
#include <datatools/logger.h>

namespace FLSimulate {

  //! \brief Variance control service
  class VarianceControl
  {
  private:

    // Default Constructor
    VarianceControl();

  public:

    // Destructor
    ~VarianceControl();

    //! Exception for variance related error
    class VarianceException : public std::runtime_error {
    public:
      VarianceException(const std::string& msg) : std::runtime_error(msg) {}
    };

    //! Set the logging priority thresold
    void set_logging(datatools::logger::priority);

    //! Set the main configuration file for the variant repository
    void set_config(const std::string & config_);

    //! Set the optional input variant profile filename
    void set_input_profile(const std::string & ip_);

    //! Set the optional output variant profile filename
    void set_output_profile(const std::string & op_);

    //! Set the setting rules for variant parameters
    void set_settings(const std::vector<std::string> & setting_);

#if DATATOOLS_WITH_QT_GUI == 1
    //! Activate the variance GUI editor
    void set_gui(bool);
#endif // DATATOOLS_WITH_QT_GUI == 1

    //! Run startup operations at application startup
    void start();

    //! Run termination at application stop
    void stop();

    //! Check started flag
    bool is_started() const;

    //! Accessor to the variance service singleton:
    static VarianceControl & instance(bool terminate_ = false);

  private:

    // Private methods wrapped in start/stop actions:

    //----------------------------------------------------------------------
    //! Initialize the variant repository from a configuration file:
    void _do_variance_setup_();

    //----------------------------------------------------------------------
    //! Load variant parameters from a variant profile input file:
    void _do_variance_load_profile_();

    //----------------------------------------------------------------------
    //! Store variant parameters in a variant profile output file:
    void _do_variance_store_profile_();

    //----------------------------------------------------------------------
    //! Apply variant settings:
    void _do_variance_apply_settings_();

#if DATATOOLS_WITH_QT_GUI == 1
    //----------------------------------------------------------------------
    //! Launch GUI variance editor:
    void _do_variance_launch_gui_();
#endif // DATATOOLS_WITH_QT_GUI == 1

    //----------------------------------------------------------------------
    //! Export the variance repository in the Bayeux/datatools kernel.
    //! This makes all variant registries embedded in the application variance
    //! repository accessible from the system variance repository (singleton)
    //! of the Bayeux/datatools' kernel.
    //! The variance service is thus accessible from any component instantiated
    //! in the application framework.
    void _do_variance_system_export_();

    //----------------------------------------------------------------------
    //! Discard the variance repository from the system variance repository
    //! in the Bayeux/datatools' kernel.
    void _do_variance_system_discard_();

  private:

    // Configuration:
    datatools::logger::priority _logging_; //!< Logging priority
    std::string _config_;                  //!< Main variance configuration file (mandatory)
    std::string _input_profile_;           //!< Input variance profile (optional)
    std::vector<std::string> _settings_;   //!< Collection of variant parameters setting rules (optional)
    std::string _output_profile_;          //!< Output variance profile (optional)
#if DATATOOLS_WITH_QT_GUI == 1
    bool _gui_;                            //!< Qt GUI activation (optional)
#endif // DATATOOLS_WITH_QT_GUI == 1

    // Embedded working data:
    bool _started_; //!< Start flag
    datatools::configuration::variant_repository _repository_; //!< Application variance repository

  };

} // namespace FLSimulate

#endif // FLSIMULATEVARIANCE_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
