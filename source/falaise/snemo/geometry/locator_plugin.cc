// falaise/snemo/geometry/locator_plugin.cc

// Ourselves:
#include <falaise/snemo/geometry/locator_plugin.h>

// Third party:
// - Bayeux/datatools :
#include <datatools/version_id.h>

// This project:
#include <falaise/snemo/geometry/calo_locator.h>
#include <falaise/snemo/geometry/gg_locator.h>
#include <falaise/snemo/geometry/gveto_locator.h>
#include <falaise/snemo/geometry/xcalo_locator.h>

namespace snemo {

namespace geometry {

GEOMTOOLS_PLUGIN_REGISTRATION_IMPLEMENT(locator_plugin, "snemo::geometry::locator_plugin")

bool locator_plugin::has_gg_locator() const { return _gg_locator_ != nullptr; }

bool locator_plugin::has_calo_locator() const { return _calo_locator_ != nullptr; }

bool locator_plugin::has_xcalo_locator() const { return _xcalo_locator_ != nullptr; }

bool locator_plugin::has_gveto_locator() const { return _gveto_locator_ != nullptr; }

const snemo::geometry::gg_locator& locator_plugin::get_gg_locator() const {
  DT_THROW_IF(!is_initialized(), std::logic_error, "Locator plugin is not initialized !");
  return *_gg_locator_;
}

const snemo::geometry::calo_locator& locator_plugin::get_calo_locator() const {
  DT_THROW_IF(!is_initialized(), std::logic_error, "Locator plugin is not initialized !");
  return *_calo_locator_;
}

const snemo::geometry::xcalo_locator& locator_plugin::get_xcalo_locator() const {
  DT_THROW_IF(!is_initialized(), std::logic_error, "Locator plugin is not initialized !");
  return *_xcalo_locator_;
}

const snemo::geometry::gveto_locator& locator_plugin::get_gveto_locator() const {
  DT_THROW_IF(!is_initialized(), std::logic_error, "Locator plugin is not initialized !");
  return *_gveto_locator_;
}

const locator_plugin::locator_dict_type& locator_plugin::get_locators() const {
  DT_THROW_IF(!is_initialized(), std::logic_error, "Locator plugin is not initialized !");
  return _locators_;
}

locator_plugin::locator_dict_type& locator_plugin::grab_locators() {
  DT_THROW_IF(!is_initialized(), std::logic_error, "Locator plugin is not initialized !");
  return _locators_;
}

locator_plugin::locator_plugin() {
  _initialized_ = false;
  _gg_locator_ = 0;
  _calo_locator_ = 0;
  _xcalo_locator_ = 0;
  _gveto_locator_ = 0;
}

locator_plugin::~locator_plugin() {
  if (is_initialized()) {
    reset();
  }
}

bool locator_plugin::is_initialized() const { return _initialized_; }

int locator_plugin::initialize(const datatools::properties& config_,
                               const geomtools::manager::plugins_dict_type& /*plugins_*/,
                               const datatools::service_dict_type& /*services_*/) {
  DT_THROW_IF(is_initialized(), std::logic_error, "Plugin is already initialized !");

  geomtools::manager::base_plugin::_basic_initialize(config_);

  DT_LOG_NOTICE(get_logging_priority(), "Building the embedded locators...");
  _build_locators(config_);

  _initialized_ = true;
  return 0;
}

int locator_plugin::reset() {
  _gg_locator_ = 0;
  _calo_locator_ = 0;
  _xcalo_locator_ = 0;
  _gveto_locator_ = 0;
  for (locator_dict_type::iterator iloc = _locators_.begin(); iloc != _locators_.end(); ++iloc) {
    geomtools::base_locator& a_locator = iloc->second.locator_handle.grab();
    a_locator.reset();
  }
  _initialized_ = false;
  return 0;
}

void locator_plugin::_build_locators(const datatools::properties& config_) {
  DT_LOG_TRACE(get_logging_priority(), "Entering...");

  DT_THROW_IF(!config_.has_key("locators.module_number"), std::logic_error,
              "Missing 'locators.module_number' key !");
  const unsigned int module_number = config_.fetch_integer("locators.module_number");

  bool do_gg = true;
  bool do_calo = true;
  bool do_xcalo = true;
  bool do_gveto = true;

  if (get_geo_manager().get_setup_label() != "snemo::demonstrator") {
    DT_THROW_IF(
        true, std::logic_error,
        "Locator plugin does not support setup '" << get_geo_manager().get_setup_label() << "' !");
    /*
    // Inhibit some locators:
    do_calo = false;
    DT_LOG_NOTICE(get_logging_priority(),
                  "Default calo locator inhibition for '"
                  << get_geo_manager().get_setup_label() << "'...");
    datatools::version_id geom_mgr_setup_vid;
    get_geo_manager().fetch_setup_version_id(geom_mgr_setup_vid);
    if (geom_mgr_setup_vid.has_major()) {
      //if (geom_mgr_setup_vid.get_major() <= 1) {
      do_xcalo = false;
      DT_LOG_NOTICE(get_logging_priority(),
                    "Default xcalo locator inhibition for '"
                    << get_geo_manager().get_setup_label() << "'...");
      do_gveto = false;
      DT_LOG_NOTICE(get_logging_priority(),
                    "Default gveto locator inhibition for '"
                    << get_geo_manager().get_setup_label() << "'...");
      //}
    }
    */
  }

  if (config_.has_key("gg.active")) {
    do_gg = config_.fetch_boolean("gg.active");
  }
  if (config_.has_key("calo.active")) {
    do_calo = config_.fetch_boolean("calo.active");
  }
  if (config_.has_key("xcalo.active")) {
    do_xcalo = config_.fetch_boolean("xcalo.active");
  }
  if (config_.has_key("gveto.active")) {
    do_gveto = config_.fetch_boolean("gveto.active");
  }

  if (do_gg) {
    DT_LOG_NOTICE(get_logging_priority(), "Activating the Geiger locator...");
    // Add a new entry for geiger locator
    const std::string name = "gg";
    locator_entry_type tmp_entry;
    tmp_entry.label = name;
    _locators_[name] = tmp_entry;
    _locators_[name].locator_handle.reset(new gg_locator(get_geo_manager(), module_number));
    _locators_[name].locator_handle.grab().initialize(config_);
    _gg_locator_ = dynamic_cast<const gg_locator*>(&_locators_[name].locator_handle.get());
  }
  if (do_calo) {
    DT_LOG_NOTICE(get_logging_priority(), "Activating the calo locator...");
    // Add a new entry for main wall locator
    const std::string name = "calo";
    locator_entry_type tmp_entry;
    tmp_entry.label = name;
    _locators_[name] = tmp_entry;
    _locators_[name].locator_handle.reset(new calo_locator(get_geo_manager(), module_number));
    _locators_[name].locator_handle.grab().initialize(config_);
    _calo_locator_ = dynamic_cast<const calo_locator*>(&_locators_[name].locator_handle.get());
  }
  if (do_xcalo) {
    DT_LOG_NOTICE(get_logging_priority(), "Activating the X-calo locator...");
    // Add a new entry for X wall locator
    const std::string name = "xcalo";
    locator_entry_type tmp_entry;
    tmp_entry.label = name;
    _locators_[name] = tmp_entry;
    _locators_[name].locator_handle.reset(new xcalo_locator(get_geo_manager(), module_number));
    _locators_[name].locator_handle.grab().initialize(config_);
    _xcalo_locator_ = dynamic_cast<const xcalo_locator*>(&_locators_[name].locator_handle.get());
  }
  if (do_gveto) {
    // Add a new entry for gamma veto locator
    DT_LOG_NOTICE(get_logging_priority(), "Activating the gamma veto locator...");
    const std::string name = "gveto";
    locator_entry_type tmp_entry;
    tmp_entry.label = name;
    _locators_[name] = tmp_entry;
    _locators_[name].locator_handle.reset(new gveto_locator(get_geo_manager(), module_number));
    _locators_[name].locator_handle.grab().initialize(config_);
    _gveto_locator_ = dynamic_cast<const gveto_locator*>(&_locators_[name].locator_handle.get());
  }

  DT_LOG_TRACE(get_logging_priority(), "Exiting.");
}

}  // end of namespace geometry

}  // namespace snemo
