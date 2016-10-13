// snemo/digitization/sim_signal_module.cc

// Ourselves:
#include <falaise/snemo/digitization/sim_signal_module.h>

// Standard library:
#include <stdexcept>
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <bayeux/datatools/service_manager.h>
// - Bayeux/geomtools:
#include <bayeux/geomtools/geometry_service.h>

// This project:
#include <falaise/snemo/datamodels/data_model.h>
#include <falaise/snemo/processing/services.h>

namespace snemo {

  namespace digitization {

    // Registration instantiation macro :
    DPP_MODULE_REGISTRATION_IMPLEMENT(sim_signal_module,
                                      "snemo::digitization::sim_signal_module")

    void sim_signal_module::_set_defaults()
    {
      _SD_label_.clear();
      _SSD_label_.clear();
      return;
    }

    // Constructor :
    sim_signal_module::sim_signal_module(datatools::logger::priority logging_priority_)
      : dpp::base_module(logging_priority_)
    {
      _set_defaults();
      return;
    }

    // Destructor :
    sim_signal_module::~sim_signal_module()
    {
      if (is_initialized()) sim_signal_module::reset();
      return;
    }

    void sim_signal_module::initialize(const datatools::properties  & config_,
                                       datatools::service_manager   & service_manager_,
                                       dpp::module_handle_dict_type & /* module_dict_ */)
    {
      DT_THROW_IF (is_initialized(),
                   std::logic_error,
                   "Module '" << get_name() << "' is already initialized ! ");

      dpp::base_module::_common_initialize(config_);

      /// Input SD bank:
      if (_SD_label_.empty()) {
        if (config_.has_key("SD_label")) {
          _SD_label_ = config_.fetch_string("SD_label");
        }
      }
      // Default label:
      if (_SD_label_.empty()) {
        _SD_label_  = snemo::datamodel::data_info::default_simulated_data_label();
      }

      /// Output SSD bank:
      if (_SSD_label_.empty()) {
        if (config_.has_key("SSD_label")) {
          _SSD_label_ = config_.fetch_string("SSD_label");
        }
      }
      // Default label:
      if (_SSD_label_.empty()) {
        _SSD_label_ = snemo::processing::service_info::default_geometry_service_label();
      }

      if (_geometry_manager_ == nullptr) {
        /// Output Geo service:
        if (_Geo_label_.empty()) {
          if (config_.has_key("Geo_label")) {
            _Geo_label_ = config_.fetch_string("Geo_label");
          }
        }
        // Default label:
        if (_Geo_label_.empty()) {
          _Geo_label_ = snemo::datamodel::data_info::default_simulated_signal_data_label();
        }
        DT_THROW_IF(! service_manager_.has(_Geo_label_) ||
                    ! service_manager_.is_a<geomtools::geometry_service>(_Geo_label_),
                    std::logic_error,
                    "Module '" << get_name() << "' has no '" << _Geo_label_ << "' service !");
        const geomtools::geometry_service & Geo = service_manager_.get<geomtools::geometry_service>(_Geo_label_);
        set_geometry_manager(Geo.get_geom_manager());
      }
      DT_THROW_IF(_geometry_manager_ == nullptr, std::logic_error, "Missing geometry manager !");

      if (config_.has_key("abort_at_missing_input")) {
        set_abort_at_missing_input(config_.fetch_boolean("abort_at_missing_input"));
      }

      if (config_.has_key("abort_at_former_output")) {
        set_abort_at_former_output(config_.fetch_boolean("abort_at_former_output"));
      }

      if (config_.has_key("preserve_former_output")) {
        set_preserve_former_output(config_.fetch_boolean("preserve_former_output"));
      }

      _init_drivers(config_, service_manager_);

      _set_initialized(true);
      return;
    }

    void sim_signal_module::driver_entry::initialize()
    {
      if (handle.get() == nullptr) {
        DT_THROW_IF(type_id.empty(), std::logic_error, "Missing type identifier for driver '" << name << "'!");
        base_signal_generator_driver::factory_register_type & sys_factory_register =
          DATATOOLS_FACTORY_GRAB_SYSTEM_REGISTER(base_signal_generator_driver);
        DT_THROW_IF(!sys_factory_register.has(type_id),
                    std::logic_error,
                    "Unknown signal generator driver class with id = '" << type_id << "' for driver '" << name << "'!");
        const base_signal_generator_driver::factory_register_type::factory_type & the_factory
          = sys_factory_register.get(type_id);
        handle.reset(the_factory());
        if (parent != nullptr && parent->has_geometry_manager()) {
          handle->set_geo_manager(parent->get_geometry_manager());
        }
      }
      if (!handle->is_initialized()) {
        handle->initialize(config);
      }
      return;
    }

    void sim_signal_module::driver_entry::reset()
    {
      if (handle.get() != nullptr) {
        if (handle->is_initialized()) {
          handle->reset();
        }
        handle.reset();
      }
      config.reset();
      type_id.clear();
      name.clear();
      parent = nullptr;
      return;
    }

    ::snemo::digitization::base_signal_generator_driver & sim_signal_module::driver_entry::grab_driver()
    {
      initialize();
      return *handle.get();
    }

    void sim_signal_module::_init_drivers(const datatools::properties & config_,
                                          datatools::service_manager  & service_manager_)
    {
      // List of signal generator drivers:
      std::set<std::string> driver_names;
      if (config_.has_key("drivers")) {
        config_.fetch("drivers", driver_names);
      }
      DT_THROW_IF(driver_names.size() == 0, std::logic_error,
                  "No signal generator driver is provided!");
      // Loop on signal generator drivers:
      for (auto driver_name : driver_names) {
        std::string key_prefix = "driver." + driver_name + ".";
        std::string type_id_key = key_prefix + "type_id";
        std::string config_key_prefix = key_prefix + "config.";
        DT_THROW_IF(!config_.has_key(type_id_key),
                    std::logic_error,
                    "Missing type ID for signal generator driver '" << driver_name << "'!");
        std::string driver_type_id = config_.fetch_string(type_id_key);
        DT_THROW_IF(driver_type_id.empty(),
                    std::logic_error,
                    "Empty type ID for signal generator driver '" << driver_name << "'!");
        datatools::properties driver_config;
        config_.export_and_rename_starting_with(driver_config, config_key_prefix, "");
        add_driver(driver_name, driver_type_id, driver_config);
      }

      return;
    }

    void sim_signal_module::reset()
    {
      DT_THROW_IF (! is_initialized(),
                   std::logic_error,
                   "Module '" << get_name() << "' is not initialized !");
      _set_initialized(false);
      // Reset the clusterizer driver :
      for (auto drv : _drivers_) {
        if (drv.second.handle.get()->is_initialized()) {
          drv.second.handle.get()->reset();
        }
        drv.second.handle.reset();
      }
      _drivers_.clear();
      _geometry_manager_ = nullptr;
      _set_defaults();
      return;
    }

    void sim_signal_module::set_sd_label(const std::string & lbl_)
    {
      DT_THROW_IF(is_initialized(),
                  std::logic_error,
                  "Module '" << get_name() << "' is already initialized ! ");
      _SD_label_ = lbl_;
      return;
    }

    const std::string & sim_signal_module::get_sd_label() const
    {
      return _SD_label_;
    }

    void sim_signal_module::set_ssd_label(const std::string & lbl_)
    {
      DT_THROW_IF(is_initialized(),
                  std::logic_error,
                  "Module '" << get_name() << "' is already initialized ! ");
      _SSD_label_ = lbl_;
      return;
    }

    const std::string & sim_signal_module::get_ssd_label() const
    {
      return _SSD_label_;
    }

    void sim_signal_module::set_geo_label(const std::string & lbl_)
    {
      DT_THROW_IF(is_initialized(),
                  std::logic_error,
                  "Module '" << get_name() << "' is already initialized ! ");
      _Geo_label_ = lbl_;
      return;
    }

    const std::string & sim_signal_module::get_geo_label() const
    {
      return _Geo_label_;
    }

    bool sim_signal_module::has_geometry_manager() const
    {
      return _geometry_manager_ != nullptr;
    }

    void sim_signal_module::set_geometry_manager(const geomtools::manager & gmgr_)
    {
      DT_THROW_IF(is_initialized(),
                  std::logic_error,
                  "Module '" << get_name() << "' is already initialized ! ");
      _geometry_manager_ = &gmgr_;
      return;
    }

    const geomtools::manager & sim_signal_module::get_geometry_manager() const
    {
      DT_THROW_IF(! is_initialized(),
                  std::logic_error,
                  "Module '" << get_name() << "' is not initialized ! ");
      return *_geometry_manager_;
    }

    bool sim_signal_module::is_abort_at_missing_input() const
    {
      return _abort_at_missing_input_;
    }

    void sim_signal_module::set_abort_at_missing_input(bool a_)
    {
      _abort_at_missing_input_ = a_;
      return;
    }

    bool sim_signal_module::is_abort_at_former_output() const
    {
      return _abort_at_former_output_;
    }

    void sim_signal_module::set_abort_at_former_output(bool a_)
    {
      _abort_at_former_output_ = a_;
      return;
    }

    bool sim_signal_module::is_preserve_former_output() const
    {
      return _preserve_former_output_;
    }

    void sim_signal_module::set_preserve_former_output(bool p_)
    {
      _preserve_former_output_ = p_;
      return;
    }

    bool sim_signal_module::has_driver(const std::string & name_) const
    {
      return _drivers_.count(name_) == 1;
    }

    void sim_signal_module::add_driver(const std::string & name_,
                                       const std::string & type_id_,
                                       const datatools::properties & config_)
    {
      DT_THROW_IF(! is_initialized(), std::logic_error,
                  "Module '" << get_name() << "' is not initialized !");
      DT_THROW_IF(name_.empty(), std::logic_error,
                  "Module '" << get_name() << "' does not accept anonymous driver !");
      DT_THROW_IF(type_id_.empty(), std::logic_error,
                  "Module '" << get_name() << "' does not accept driver without type identifier !");
      driver_entry de;
      de.parent = this;
      de.name = name_;
      de.type_id = type_id_;
      de.config = config_;
      _drivers_[name_] = de;
      return;
    }

    // Processing :
    dpp::base_module::process_status
    sim_signal_module::process(datatools::things & data_record_)
    {
      DT_THROW_IF (! is_initialized(), std::logic_error,
                   "Module '" << get_name() << "' is not initialized !");


      //////////////////////////
      // Check simulated data //
      //////////////////////////

      // Check if some 'simulated_data' are available in the data model:
      if (! data_record_.has(_SD_label_)) {
        DT_THROW_IF (is_abort_at_missing_input(), std::logic_error,
                     "Missing simulated data to be processed !");
        // leave the data unchanged.
        return dpp::base_module::PROCESS_ERROR;
      }
      // grab the 'simulated_data' entry from the data model :
      mctools::simulated_data & the_simulated_data
        = data_record_.grab<mctools::simulated_data>(_SD_label_);

      /////////////////////////////////
      // Check simulated signal data //
      /////////////////////////////////

      // check if some 'signal_data' are available in the data model:
      mctools::signal::signal_data * ptr_signal_data = 0;
      if (! data_record_.has(_SSD_label_)) {
        ptr_signal_data = &(data_record_.add<mctools::signal::signal_data>(_SSD_label_));
      } else {
        ptr_signal_data = &(data_record_.grab<mctools::signal::signal_data>(_SSD_label_));
      }
      mctools::signal::signal_data & the_signal_data = *ptr_signal_data;
      {
        std::vector<std::string> signal_categories;
        the_signal_data.build_list_of_categories(signal_categories);
        if (signal_categories.size())  {
          DT_THROW_IF (is_abort_at_former_output(), std::logic_error,
                       "Already has processed simulated signal data !");
          if (! is_preserve_former_output()) {
            the_signal_data.reset();
          }
        }
      }

      /********************
       * Process the data *
       ********************/

      // Main processing method :
      _process(the_simulated_data, the_signal_data);

      return dpp::base_module::PROCESS_SUCCESS;
    }

    void sim_signal_module::_process(const mctools::simulated_data & sim_data_,
                                     mctools::signal::signal_data & sim_signal_data_)
    {
      // Loop on embedded signal generator drivers:
      for (driver_dict_type::iterator idriver = _drivers_.begin();
           idriver != _drivers_.end();
           idriver++) {
        driver_entry & de = idriver->second;
        base_signal_generator_driver & sgd = de.grab_driver();
        sgd.process(sim_data_, sim_signal_data_);
      }
      return;
    }

  } // end of namespace digitization

} // end of namespace snemo
