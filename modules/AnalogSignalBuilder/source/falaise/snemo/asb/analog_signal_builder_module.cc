// analog_signal_builder_module.cc - Implementation of Falaise ASB analog signal builder module
//
// Copyright (c) 2016 F. Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2016 G. Oliviéro <goliviero@lpccaen.in2p3.fr>
//
// This file is part of Falaise/ASB plugin.
//
// Falaise/ASB plugin is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Falaise/ASB plugin is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Falaise/ASB plugin.  If not, see <http://www.gnu.org/licenses/>.

// Ourselves:
#include <snemo/asb/analog_signal_builder_module.h>

// Standard library:
#include <memory>

// Third party:
// - Bayeux/datatools:
#include <bayeux/datatools/handle.h>
#include <bayeux/datatools/service_manager.h>
// - Bayeux/geomtools:
#include <bayeux/geomtools/geometry_service.h>

// This project:
#include <falaise/snemo/datamodels/data_model.h>
#include <falaise/snemo/processing/services.h>

namespace snemo {

  namespace asb {

    // Registration instantiation macro :
    DPP_MODULE_REGISTRATION_IMPLEMENT(analog_signal_builder_module,
                                      "snemo::asb::analog_signal_builder_module")

    /// \brief Handler for an embedded signal generator driver
    struct analog_signal_builder_module::driver_entry
    {
    public:
      driver_entry(analog_signal_builder_module & parent_,
                   const std::string & name_,
                   const std::string & type_id_,
                   const datatools::properties & config_);
      ~driver_entry();
      bool is_driver_initialized() const;
      base_signal_generator_driver & grab_driver();
      const base_signal_generator_driver & get_driver() const;
    private:
      void _initialize_();
      void _reset_();
    private:
      analog_signal_builder_module & _parent_;
      std::string _name_;
      std::string _type_id_;
      datatools::properties _config_;
      datatools::handle<base_signal_generator_driver> _handle_;
    };

    analog_signal_builder_module::driver_entry::driver_entry(analog_signal_builder_module & parent_,
                                                             const std::string & name_,
                                                             const std::string & type_id_,
                                                             const datatools::properties & config_)
      : _parent_(parent_), _name_(name_), _type_id_(type_id_), _config_(config_)
    {
      return;
    }

    analog_signal_builder_module::driver_entry::~driver_entry()
    {
      _reset_();
      return;
    }

    bool analog_signal_builder_module::driver_entry::is_driver_initialized() const
    {
      if (_handle_.has_data() && _handle_.get().is_initialized())  return true;
      return false;
    }

    void analog_signal_builder_module::driver_entry::_initialize_()
    {
      if (!_handle_.has_data()) {
        DT_THROW_IF(_type_id_.empty(), std::logic_error, "Missing type identifier for driver '" << _name_ << "'!");
        base_signal_generator_driver::factory_register_type & sys_factory_register =
          DATATOOLS_FACTORY_GRAB_SYSTEM_REGISTER(base_signal_generator_driver);
        DT_THROW_IF(!sys_factory_register.has(_type_id_),
                    std::logic_error,
                    "Unknown signal generator driver class with id = '" << _type_id_ << "' for driver '" << _name_ << "'!");
        const base_signal_generator_driver::factory_register_type::factory_type & the_factory
          = sys_factory_register.get(_type_id_);
        _handle_.reset(the_factory());
        if (_parent_.has_geometry_manager()) {
          _handle_.grab().set_geo_manager(_parent_.get_geometry_manager());
        }
        // if (_parent_.has_database_manager()) {
        //   _handle_.grab().set_database_manager(_parent_.get_database_manager());
        // }
      }
      if (!_handle_.get().is_initialized()) {
        _handle_.grab().initialize(_config_);
      }
      return;
    }

    void analog_signal_builder_module::driver_entry::_reset_()
    {
      if (_handle_.has_data()) {
        if (_handle_.get().is_initialized()) {
          _handle_.grab().reset();
        }
        _handle_.reset();
      }
      return;
    }

    const base_signal_generator_driver &
    analog_signal_builder_module::driver_entry::get_driver() const
    {
      driver_entry * mutable_this = const_cast<driver_entry *>(this);
      base_signal_generator_driver & mutable_driver = mutable_this->grab_driver();
      return const_cast<base_signal_generator_driver &>(mutable_driver);
    }

    base_signal_generator_driver &
    analog_signal_builder_module::driver_entry::grab_driver()
    {
      _initialize_();
      return _handle_.grab();
    }

    /* ====================================== */

    void analog_signal_builder_module::_set_defaults_()
    {
      _SD_label_.clear();
      _SSD_label_.clear();
      _Geo_label_.clear();
      _Db_label_.clear();
      _abort_at_missing_input_ = true;
      _abort_at_former_output_ = false;
      _preserve_former_output_ = false;
      return;
    }

    analog_signal_builder_module::analog_signal_builder_module(datatools::logger::priority logging_priority_)
      : dpp::base_module(logging_priority_)
    {
      _geometry_manager_ = nullptr;
      _set_defaults_();
      return;
    }

    // Destructor :
    analog_signal_builder_module::~analog_signal_builder_module()
    {
      if (is_initialized()) analog_signal_builder_module::reset();
      return;
    }

    void analog_signal_builder_module::initialize(const datatools::properties  & config_,
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
        _SSD_label_ = snemo::datamodel::data_info::default_simulated_signal_data_label();
      }

      /*if (_db_manager_ == nullptr) */ {
        /// Db service:
        if (_Db_label_.empty()) {
          if (config_.has_key("Db_label")) {
            _Db_label_ = config_.fetch_string("Db_label");
          }
        }
        // Default label:
        // if (_Db_label_.empty()) {
        //   _Db_label_ = snemo::datamodel::data_info::default_database_service_label();
        // }
        // DT_THROW_IF(! service_manager_.has(_Db_label_) ||
        //             ! service_manager_.is_a<snemo::XXX::database_service>(_Db_label_),
        //             std::logic_error,
        //             "Module '" << get_name() << "' has no '" << _Db_label_ << "' service !");
        // const snemo::XXX::::database_service & Db = service_manager_.get<snemo::XXX::database_service>(_Db_label_);
        // set_db_manager(Db.get_geom_manager());
      }

      if (_geometry_manager_ == nullptr) {
        /// Geo service:
        if (_Geo_label_.empty()) {
          if (config_.has_key("Geo_label")) {
            _Geo_label_ = config_.fetch_string("Geo_label");
          }
        }
        // Default label:
        if (_Geo_label_.empty()) {
          _Geo_label_ = snemo::processing::service_info::default_geometry_service_label();
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

      _init_drivers_(config_, service_manager_);

      _set_initialized(true);
      return;
    }

    void analog_signal_builder_module::_init_drivers_(const datatools::properties & config_,
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

    void analog_signal_builder_module::reset()
    {
      DT_THROW_IF (! is_initialized(),
                   std::logic_error,
                   "Module '" << get_name() << "' is not initialized !");
      _set_initialized(false);
      _drivers_.clear();
      _geometry_manager_ = nullptr;
      // _database_manager_ = nullptr;
      _set_defaults_();
      return;
    }

    void analog_signal_builder_module::set_sd_label(const std::string & lbl_)
    {
      DT_THROW_IF(is_initialized(),
                  std::logic_error,
                  "Module '" << get_name() << "' is already initialized ! ");
      _SD_label_ = lbl_;
      return;
    }

    const std::string & analog_signal_builder_module::get_sd_label() const
    {
      return _SD_label_;
    }

    void analog_signal_builder_module::set_ssd_label(const std::string & lbl_)
    {
      DT_THROW_IF(is_initialized(),
                  std::logic_error,
                  "Module '" << get_name() << "' is already initialized ! ");
      _SSD_label_ = lbl_;
      return;
    }

    const std::string & analog_signal_builder_module::get_ssd_label() const
    {
      return _SSD_label_;
    }

    void analog_signal_builder_module::set_geo_label(const std::string & lbl_)
    {
      DT_THROW_IF(is_initialized(),
                  std::logic_error,
                  "Module '" << get_name() << "' is already initialized ! ");
      _Geo_label_ = lbl_;
      return;
    }

    const std::string & analog_signal_builder_module::get_geo_label() const
    {
      return _Geo_label_;
    }

    void analog_signal_builder_module::set_db_label(const std::string & lbl_)
    {
      DT_THROW_IF(is_initialized(),
                  std::logic_error,
                  "Module '" << get_name() << "' is already initialized ! ");
      _Db_label_ = lbl_;
      return;
    }

    const std::string & analog_signal_builder_module::get_db_label() const
    {
      return _Db_label_;
    }

    bool analog_signal_builder_module::has_database_manager() const
    {
      // return _database_manager_ != nullptr;
      return false;
    }

    // void analog_signal_builder_module::set_database_manager(const database::manager & gmgr_)
    // {
    //   DT_THROW_IF(is_initialized(),
    //               std::logic_error,
    //               "Module '" << get_name() << "' is already initialized ! ");
    //   _geometry_manager_ = &gmgr_;
    //   return;
    // }

    // const database::manager & analog_signal_builder_module::get_database_manager() const
    // {
    //   DT_THROW_IF(! is_initialized(),
    //               std::logic_error,
    //               "Module '" << get_name() << "' is not initialized ! ");
    //   return *_database_manager_;
    // }

    bool analog_signal_builder_module::has_geometry_manager() const
    {
      return _geometry_manager_ != nullptr;
    }

    void analog_signal_builder_module::set_geometry_manager(const geomtools::manager & gmgr_)
    {
      DT_THROW_IF(is_initialized(),
                  std::logic_error,
                  "Module '" << get_name() << "' is already initialized ! ");
      _geometry_manager_ = &gmgr_;
      return;
    }

    const geomtools::manager & analog_signal_builder_module::get_geometry_manager() const
    {
      DT_THROW_IF(! is_initialized(),
                  std::logic_error,
                  "Module '" << get_name() << "' is not initialized ! ");
      return *_geometry_manager_;
    }

    bool analog_signal_builder_module::is_abort_at_missing_input() const
    {
      return _abort_at_missing_input_;
    }

    void analog_signal_builder_module::set_abort_at_missing_input(bool a_)
    {
      _abort_at_missing_input_ = a_;
      return;
    }

    bool analog_signal_builder_module::is_abort_at_former_output() const
    {
      return _abort_at_former_output_;
    }

    void analog_signal_builder_module::set_abort_at_former_output(bool a_)
    {
      _abort_at_former_output_ = a_;
      return;
    }

    bool analog_signal_builder_module::is_preserve_former_output() const
    {
      return _preserve_former_output_;
    }

    void analog_signal_builder_module::set_preserve_former_output(bool p_)
    {
      _preserve_former_output_ = p_;
      return;
    }

    bool analog_signal_builder_module::has_driver(const std::string & name_) const
    {
      return _drivers_.count(name_);
    }

    void analog_signal_builder_module::add_driver(const std::string & name_,
                                                  const std::string & type_id_,
                                                  const datatools::properties & config_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Module '" << get_name() << "' is already initialized !");
      DT_THROW_IF(name_.empty(), std::logic_error,
                  "Module '" << get_name() << "' does not accept anonymous driver !");
      DT_THROW_IF(type_id_.empty(), std::logic_error,
                  "Module '" << get_name() << "' does not accept driver without type identifier !");
      driver_entry de(*this, name_, type_id_, config_);
      _drivers_.insert(std::pair<std::string, driver_entry>(name_, de));
      return;
    }

    void analog_signal_builder_module::remove_driver(const std::string & name_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Module '" << get_name() << "' is already initialized !");
      DT_THROW_IF(name_.empty(), std::logic_error,
                  "Module '" << get_name() << "' does not accept anonymous driver !");
      DT_THROW_IF(!has_driver(name_), std::logic_error,
                  "Module '" << get_name() << "' has no driver named '" << name_ << "'!");
      driver_dict_type::iterator found = _drivers_.find(name_);
      _drivers_.erase(name_);
      return;
    }

    // Processing :
    dpp::base_module::process_status
    analog_signal_builder_module::process(datatools::things & data_record_)
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
      const mctools::simulated_data & the_simulated_data
        = data_record_.get<mctools::simulated_data>(_SD_label_);

      /////////////////////////////////
      // Check simulated signal data //
      /////////////////////////////////

      // check if some 'signal_data' are available in the data model:
      mctools::signal::signal_data * ptr_signal_data = nullptr;
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
      try {
        _process_(the_simulated_data, the_signal_data);
      } catch (std::exception & error) {
        DT_LOG_ERROR(get_logging_priority(), error.what());
        return dpp::base_module::PROCESS_ERROR;
      }

      return dpp::base_module::PROCESS_SUCCESS;
    }

    void analog_signal_builder_module::_process_(const mctools::simulated_data & sim_data_,
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

  } // end of namespace asb

} // end of namespace snemo
