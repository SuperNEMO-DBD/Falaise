#ifndef FALAISE_TRACKFIT_PLUGIN_UTILITIES_H
#define FALAISE_TRACKFIT_PLUGIN_UTILITIES_H

// Falaise:
#include <falaise/snemo/datamodels/calibrated_data.h>
#include <falaise/snemo/datamodels/tracker_clustering_data.h>
#include <falaise/snemo/datamodels/tracker_trajectory_data.h>
#include <falaise/snemo/geometry/gg_locator.h>

void generate_tcd(const snemo::geometry::gg_locator& ggloc_,
                  snemo::datamodel::TrackerHitHdlCollection& gghits_,
                  snemo::datamodel::tracker_clustering_data& tcd_);

void display_event(const snemo::geometry::gg_locator& ggloc_,
                   const snemo::datamodel::TrackerHitHdlCollection& gghits_,
                   const snemo::datamodel::tracker_clustering_data& tcd_,
                   const snemo::datamodel::tracker_trajectory_data& ttd_);

#endif  // FALAISE_TRACKFIT_PLUGIN_UTILITIES_H
