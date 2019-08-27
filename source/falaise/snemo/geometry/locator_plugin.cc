// falaise/snemo/geometry/locator_plugin.cc

// Ourselves:
#include <falaise/snemo/geometry/locator_plugin.h>

// Third party:
// - Bayeux/datatools :
#include <datatools/version_id.h>

// This project:
#include <falaise/property_set.h>
#include <falaise/snemo/geometry/calo_locator.h>
#include <falaise/snemo/geometry/gg_locator.h>
#include <falaise/snemo/geometry/gveto_locator.h>
#include <falaise/snemo/geometry/xcalo_locator.h>

namespace snemo {

namespace geometry {

GEOMTOOLS_PLUGIN_REGISTRATION_IMPLEMENT(locator_plugin, "snemo::geometry::locator_plugin")

bool locator_plugin::hasGeigerLocator() const { return geigerLocator_ != nullptr; }

bool locator_plugin::hasCaloLocator() const { return caloLocator_ != nullptr; }

bool locator_plugin::hasXCaloLocator() const { return xcaloLocator_ != nullptr; }

bool locator_plugin::hasGVetoLocator() const { return gvetoLocator_ != nullptr; }

const snemo::geometry::gg_locator& locator_plugin::geigerLocator() const {
  DT_THROW_IF(!hasGeigerLocator(), std::logic_error, "No GeigerLocator available");
  return *geigerLocator_;
}

const snemo::geometry::calo_locator& locator_plugin::caloLocator() const {
  DT_THROW_IF(!hasCaloLocator(), std::logic_error, "No CaloLocator available");
  return *caloLocator_;
}

const snemo::geometry::xcalo_locator& locator_plugin::xcaloLocator() const {
  DT_THROW_IF(!hasXCaloLocator(), std::logic_error, "No XCaloLocator available");
  return *xcaloLocator_;
}

const snemo::geometry::gveto_locator& locator_plugin::gvetoLocator() const {
  DT_THROW_IF(!hasGVetoLocator(), std::logic_error, "Locator plugin is not initialized !");
  return *gvetoLocator_;
}

bool locator_plugin::is_initialized() const { return isInitialized_; }

int locator_plugin::initialize(const datatools::properties& config_,
                               const geomtools::manager::plugins_dict_type& /*plugins_*/,
                               const datatools::service_dict_type& /*services_*/) {
  reset();
  geomtools::manager::base_plugin::_basic_initialize(config_);
  _build_locators(config_);
  isInitialized_ = true;
  return 0;
}

int locator_plugin::reset() {
  geigerLocator_.reset();
  caloLocator_.reset();
  xcaloLocator_.reset();
  gvetoLocator_.reset();
  isInitialized_ = false;
  return 0;
}

void locator_plugin::_build_locators(const datatools::properties& config_) {
  // NB: base_plugin supports conditions on manager setup!
  if (get_geo_manager().get_setup_label() != "snemo::demonstrator") {
    DT_THROW_IF(
        true, std::logic_error,
        "Locator plugin does not support setup '" << get_geo_manager().get_setup_label() << "' !");
  }

  falaise::property_set ps{config_};

  auto module_number = ps.get<int>("locators.module_number");
  DT_THROW_IF(module_number < 0, std::logic_error, "Negative value for locators.module_number");
  auto do_gg = ps.get<bool>("gg.active", true);
  auto do_calo = ps.get<bool>("calo.active", true);
  auto do_xcalo = ps.get<bool>("xcalo.active", true);
  auto do_gveto = ps.get<bool>("gveto.active", true);

  if (do_gg) {
    geigerLocator_.reset(new gg_locator(module_number, get_geo_manager(), ps));
  }
  if (do_calo) {
    caloLocator_.reset(new calo_locator(module_number, get_geo_manager(), ps));
  }
  if (do_xcalo) {
    xcaloLocator_.reset(new xcalo_locator(module_number, get_geo_manager(), ps));
  }
  if (do_gveto) {
    gvetoLocator_.reset(new gveto_locator(module_number, get_geo_manager(), ps));
  }
}

}  // end of namespace geometry

}  // namespace snemo
