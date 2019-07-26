#ifndef FALAISE_SNEMO_GEOMETRY_LOCATOR_HELPERS_H
#define FALAISE_SNEMO_GEOMETRY_LOCATOR_HELPERS_H 1

#include <string>

#include <bayeux/geomtools/manager.h>

#include "falaise/snemo/geometry/locator_plugin.h"

namespace snemo {
namespace geometry {
/// Return a valid pointer to the geomtools locator for SuperNEMO
// Identified and refactored from ChargedParticleTracking
inline const snemo::geometry::locator_plugin* getSNemoLocator(const geomtools::manager& gm,
                                                              const std::string& name) {
  using PluginType = snemo::geometry::locator_plugin;
  if (name.empty()) {
    // Just find the first of the right type
    for (const auto& ip : gm.get_plugins()) {
      if (gm.is_plugin_a<PluginType>(ip.first)) {
        return &(gm.get_plugin<PluginType>(ip.first));
      }
    }
  }
  // Direct get will throw if no plugin with that name, or not of correct type
  return &(gm.get_plugin<PluginType>(name));
}
}  // namespace geometry
}  // namespace snemo

#endif