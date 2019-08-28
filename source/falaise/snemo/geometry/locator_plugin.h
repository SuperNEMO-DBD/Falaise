// falaise/snemo/geometry/locator_plugin.h
/* Author(s) :    Xavier Garrido <garrido@lal.in2p3.fr>
 * Creation date: 2013-07-15
 * Last modified: 2014-01-28
 *
 * License:
 *
 * Description:
 *
 *   A geometry manager plugin with embeded locators.
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_GEOMETRY_LOCATOR_PLUGIN_H
#define FALAISE_SNEMO_GEOMETRY_LOCATOR_PLUGIN_H 1

#include <memory>

// Third party:
// - Boost :
#include <boost/cstdint.hpp>
// - Bayeux/geomtools:
#include <geomtools/manager.h>
#include <geomtools/manager_macros.h>

namespace geomtools {
class i_base_locator;
}

namespace snemo {

namespace geometry {

class gg_locator;
class calo_locator;
class xcalo_locator;
class gveto_locator;

/// \brief A geometry manager plugin with embedded SuperNEMO locators.
class locator_plugin : public geomtools::manager::base_plugin {
 public:
  /// Main plugin initialization method
  virtual int initialize(const datatools::properties& config_,
                         const geomtools::manager::plugins_dict_type& plugins_,
                         const datatools::service_dict_type& services_) override;

  /// Plugin reset method
  virtual int reset() override;

  /// Check if plugin is initialized
  virtual bool is_initialized() const override;

  /// Check if geiger locator is available
  bool hasGeigerLocator() const;

  /// Check if calo locator is available
  bool hasCaloLocator() const;

  /// Check if xcalo locator is available
  bool hasXCaloLocator() const;

  /// Check if gveto locator is available
  bool hasGVetoLocator() const;

  /// Returns a non-mutable reference to the geiger locator
  const snemo::geometry::gg_locator& geigerLocator() const;

  /// Returns a non-mutable reference to the main wall locator
  const snemo::geometry::calo_locator& caloLocator() const;

  /// Returns a non-mutable reference to the X wall locator
  const snemo::geometry::xcalo_locator& xcaloLocator() const;

  /// Returns a non-mutable reference to the gamma veto locator
  const snemo::geometry::gveto_locator& gvetoLocator() const;

 protected:
  /// Internal mapping build method
  void _build_locators(const datatools::properties& config_);

 private:
  bool isInitialized_ = false;                                    //!< Initialization flag
  std::unique_ptr<snemo::geometry::gg_locator> geigerLocator_;    //!< Geiger locator
  std::unique_ptr<snemo::geometry::calo_locator> caloLocator_;    //!< Main wall locator
  std::unique_ptr<snemo::geometry::xcalo_locator> xcaloLocator_;  //!< X-wall locator
  std::unique_ptr<snemo::geometry::gveto_locator> gvetoLocator_;  //!< gamma-veto locator

  GEOMTOOLS_PLUGIN_REGISTRATION_INTERFACE(locator_plugin)
};

}  // end of namespace geometry

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_GEOMETRY_LOCATOR_PLUGIN_H
