// FLSimulateVariance.cc - Implementation of the FLSimulateVariance
//
// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2016 by SuperNEMO
//
// Distributed under the OSI-approved BSD 3-Clause License (the "License");
// see accompanying file License.txt for details.
//
// This software is distributed WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the License for more information.

// - Ourselves:
#include "FLSimulateVariance.h"

// Standard library:
#include <string>
#include <fstream>

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>
#include <boost/algorithm/string.hpp>
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/configuration/io.h>
#include <datatools/kernel.h>

#if DATATOOLS_WITH_QT_GUI == 1
// Specific GUI support:
// - Qt:
#include <QStyleFactory>
#include <QApplication>
// - Bayeux/datatools:
#include <datatools/qt/interface.h>
#include <datatools/configuration/ui/variant_repository_dialog.h>
#endif // DATATOOLS_WITH_QT_GUI == 1

namespace FLSimulate {

  VarianceControl::VarianceControl()
  {
#if DATATOOLS_WITH_QT_GUI == 1
    _gui_ = false;
#endif // DATATOOLS_WITH_QT_GUI == 1
    _logging_ = datatools::logger::PRIO_FATAL;
    _started_ = false;
    return;
  }

  VarianceControl::~VarianceControl()
  {
    if (_started_) {
      stop();
    }
    return;
  }

  bool VarianceControl::is_started() const
  {
    return _started_;
  }

  void VarianceControl::set_logging(datatools::logger::priority prio_)
  {
    _logging_ = prio_;
    return;
  }

  void VarianceControl::set_config(const std::string & config_)
  {
    _config_ = config_;
    return;
  }

  void VarianceControl::set_input_profile(const std::string & ip_)
  {
    _input_profile_ = ip_;
    return;
  }

  void VarianceControl::set_output_profile(const std::string & op_)
  {
    _output_profile_ = op_;
    return;
  }

  void VarianceControl::set_settings(const std::vector<std::string> & settings_)
  {
    _settings_ = settings_;
    return;
  }

#if DATATOOLS_WITH_QT_GUI == 1
  void VarianceControl::set_gui(bool gui_)
  {
    _gui_ = gui_;
    return;
  }
#endif // DATATOOLS_WITH_QT_GUI == 1

  void VarianceControl::start()
  {
    DT_LOG_TRACE_ENTERING(_logging_); //, "Instantation of the VarianceControl singleton.")
    if (is_started()) {
      return;
    }
    // Initialize the repository:
    _do_variance_setup_();
    // Load a profile:
    _do_variance_load_profile_();
    // Apply specific parameter settings:
    _do_variance_apply_settings_();
#if DATATOOLS_WITH_QT_GUI == 1
    if (_gui_) {
      // Launch the GUI editor:
      _do_variance_launch_gui_();
    }
#endif // DATATOOLS_WITH_QT_GUI == 1
    // Store the selected profile:
    _do_variance_store_profile_();
    // Export the application variance repository to the datatools' kernel
    // in order to make the variance service accessible from
    // each software component:
    _do_variance_system_export_();
    _started_ = true;
    DT_LOG_TRACE_EXITING(_logging_);
    return;
  }

  void VarianceControl::stop()
  {
    DT_LOG_TRACE_ENTERING(_logging_);
    if (!is_started()) {
      return;
    }
    // Clean the datatools' kernel from the application variance repository:
    _started_ = false;
    _do_variance_system_discard_();
    DT_LOG_TRACE_EXITING(_logging_);
    return;
  }


  // static
  VarianceControl & VarianceControl::instance(bool /* terminate_ */)
  {
    static boost::scoped_ptr<VarianceControl> _vctrl;
    if (_vctrl.get() == 0) {
      // Instantiate the variant repository:
      _vctrl.reset(new VarianceControl);
    }
    /*
    if (terminate_) {
      if (_vctrl->is_started()) {
        _vctrl->stop();
      }
    }
    */
    return *_vctrl.get();
  }

  void VarianceControl::_do_variance_setup_()
  {
    DT_LOG_TRACE_ENTERING(_logging_);
    datatools::kernel & krnl = datatools::kernel::instance();
    if (datatools::logger::is_trace(_logging_)) {
      krnl.set_logging(_logging_);
    }
    std::string config_filename = _config_;
    datatools::fetch_path_with_env(config_filename);
    datatools::properties rep_config;
    rep_config.read_configuration(config_filename);
    DT_LOG_TRACE(_logging_, "Initializing application variant repository from '" << config_filename << "'...");
    _repository_.initialize(rep_config);
    DT_LOG_TRACE(_logging_, "Application variant repository locked = " << _repository_.is_locked());
    DT_LOG_TRACE(_logging_, "Done.");
    DT_LOG_TRACE_EXITING(_logging_);
    return;
  }

  void VarianceControl::_do_variance_load_profile_()
  {
    DT_LOG_TRACE_ENTERING(_logging_);
    std::string profile_filename = _input_profile_;
    boost::trim(profile_filename);
    if (profile_filename.empty()) return;
    datatools::fetch_path_with_env(profile_filename);
    DT_LOG_TRACE(_logging_, "Loading profile for application variant repository from '" << profile_filename << "'...");
    std::ifstream rep_file;
    rep_file.open(profile_filename.c_str());
    if (!rep_file) {
      throw VarianceException("Cannot open input variance profile '"+_input_profile_+"'");
    }
    datatools::configuration::ascii_io rep_io(datatools::configuration::ascii_io::IO_DEFAULT);
    bool was_locked = _repository_.is_locked();
    if (was_locked) {
      _repository_.unlock();
    }
    int error = rep_io.load_repository(rep_file, _repository_);
    if (was_locked) {
      _repository_.lock();
    }
    if (error) {
      throw VarianceException("Loading of variance profile '"+_input_profile_+"' failed");
    }
    DT_LOG_TRACE_EXITING(_logging_);
    return;
  }

  void VarianceControl::_do_variance_store_profile_()
  {
    DT_LOG_TRACE_ENTERING(_logging_);
    std::string profile_filename = _output_profile_;
    boost::trim(profile_filename);
    if(profile_filename.empty()) return;
    datatools::fetch_path_with_env(profile_filename);
    DT_LOG_TRACE(_logging_, "Storing profile for application variant repository from '" << profile_filename << "'...");
    std::ofstream rep_file;
    rep_file.open(profile_filename.c_str());
    if (!rep_file) {
      throw VarianceException("Cannot open output variance profile '"+_output_profile_+"'");
    }
    datatools::configuration::ascii_io rep_io(datatools::configuration::ascii_io::IO_DEFAULT);
    rep_io.store_repository(rep_file, _repository_);
    DT_LOG_TRACE_EXITING(_logging_);
    return;
  }

  void VarianceControl::_do_variance_apply_settings_()
  {
    DT_LOG_TRACE_ENTERING(_logging_);
    std::vector<std::string> settings = _settings_;
    // Re-order variant parameters:
    datatools::configuration::variant_parameter_set_comparator comp(_repository_);
    std::sort(settings.begin(), settings.end(), comp);
    bool was_locked = _repository_.is_locked();
    if (was_locked) {
      _repository_.unlock();
    }
    // Apply variant parameters:
    for (size_t iset = 0; iset < settings.size(); iset++) {
      const std::string & variant_set = settings[iset];
      DT_LOG_TRACE(_logging_, "Applying variant setting from rule '" << variant_set << "'...");
      datatools::configuration::variant_parameter_set_type vps;
      // Format is : "foo:param0/var0/key0=value"
      vps.parse(variant_set);
      datatools::command::returned_info cri =
        _repository_.cmd_set_parameter(vps.registry_key,
                                       vps.param_key,
                                       vps.param_value_str);
      if (cri.is_failure()) {
        throw VarianceException("Cannot set variant parameter : " + cri.get_error_message());
      }
    }
    if (was_locked) {
      _repository_.lock();
    }
    DT_LOG_TRACE_EXITING(_logging_);
    return;
  }

#if DATATOOLS_WITH_QT_GUI == 1
  void VarianceControl::_do_variance_launch_gui_()
  {
    DT_LOG_TRACE_ENTERING(_logging_);
    // Launch a Qt based dialog for the variant repository:
    const datatools::kernel & krnl = datatools::kernel::const_instance();
    datatools::qt::interface & iqt = datatools::qt::interface::instance(krnl.get_argc(),
                                                                        krnl.get_argv(),
                                                                        "flsimulate");
    datatools::configuration::ui::variant_repository_dialog vrep_dialog(_repository_);
    int ret = vrep_dialog.exec();
    if (ret) {
      // throw VarianceException("Variance GUI editor failed");
    }
    DT_LOG_TRACE_EXITING(_logging_);
    return;
  }
#endif // DATATOOLS_WITH_QT_GUI == 1

  void VarianceControl::_do_variance_system_export_()
  {
    DT_LOG_TRACE_ENTERING(_logging_);
    const datatools::kernel & krnl = datatools::kernel::const_instance();
    if (krnl.is_initialized() && krnl.has_variant_repository()) {
      DT_LOG_TRACE(_logging_, "Datatools' kernel system variant repository is instantiated.");
      _repository_.system_export();
    } else {
      throw VarianceException("Datatools' kernel system variant repository is not instantiated.");
    }
    DT_LOG_TRACE_EXITING(_logging_);
    return;
  }

  void VarianceControl::_do_variance_system_discard_()
  {
    DT_LOG_TRACE_ENTERING(_logging_);
    const datatools::kernel & krnl = datatools::kernel::const_instance();
    // Here there is potentially an issue with the order of termination for
    // the static system variant repository in the datatools' kernel.
    // So we first check if it is still present before to attempt to discard
    // user defined variant registries from it.
    if (krnl.is_initialized() && krnl.has_variant_repository()) {
      DT_LOG_TRACE(_logging_, "Datatools' kernel system variant repository is instantiated.");
      _repository_.system_discard();
    } else {
      // The datatools' kernel has shutdown... no need to unregister the application variant repository.
      DT_LOG_TRACE(_logging_, "Datatools' kernel system variant repository is NOT instantiated.");
    }
    DT_LOG_TRACE_EXITING(_logging_);
    return;
  }

} // namespace FLSimulate
