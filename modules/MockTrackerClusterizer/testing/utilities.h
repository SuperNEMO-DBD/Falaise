#ifndef FALAISE_MOCKTRACKERCLUSTERIZER_PLUGIN_UTILITIES_H
#define FALAISE_MOCKTRACKERCLUSTERIZER_PLUGIN_UTILITIES_H

// Falaise:
#include <falaise/snemo/geometry/gg_locator.h>
#include <falaise/snemo/datamodels/calibrated_data.h>
#include <falaise/snemo/datamodels/tracker_clustering_data.h>

void generate_gg_hits(const snemo::geometry::gg_locator & ggloc_,
                      snemo::datamodel::calibrated_data::tracker_hit_collection_type & gghits_);

void display_event(const snemo::geometry::gg_locator & ggloc_,
                   const snemo::datamodel::calibrated_data::tracker_hit_collection_type & gghits_,
                   const snemo::datamodel::tracker_clustering_data & tcd_);

#endif // FALAISE_MOCKTRACKERCLUSTERIZER_PLUGIN_UTILITIES_H
