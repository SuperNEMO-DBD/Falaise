// -*- mode: c++ ; -*-
/** \file falaise/TrackerPreClustering/event_display.tpp
 */

#ifndef FALAISE_TRACKERPRECLUSTERING_EVENT_DISPLAY_TPP
#define FALAISE_TRACKERPRECLUSTERING_EVENT_DISPLAY_TPP 1

// Third party:
// - Boost:
#include <boost/foreach.hpp>

namespace TrackerPreClustering {

  template<typename Hit>
  void event_display::display_gg_hits(std::ostream & out_,
                                      const std::vector<const Hit *> & hits_,
                                      uint32_t mode_) const
  {
    typedef Hit hit_type;
    typedef std::vector<const hit_type *> hit_collection_type;

    BOOST_FOREACH (const hit_type * hitref,
                   hits_) {
      const hit_type & hit = *hitref;
      bool plot = false;
      if ((mode_ & prompt) && hit.is_prompt ()) {
        plot = true;
      }
      if ((mode_ & delayed) && hit.is_delayed ()) {
        plot = true;
      }
      if (plot) {
        plot_gg_hit (out_,
                     hit.get_id (),
                     hit.get_r (),
                     hit.get_dr (),
                     hit.get_x (),
                     hit.get_y (),
                     hit.get_z (),
                     hit.get_dz (),
                     hit.get_delayed_time ());
      }
    }
    return;
  }

  template<class Hit>
  void event_display::display_cluster(std::ostream & out_,
                                      const std::vector<const Hit *> & cluster_,
                                      uint32_t mode_) const
  {
    typedef Hit hit_type;
    typedef std::vector<const hit_type *> hit_collection_type;

    BOOST_FOREACH (const hit_type * hitref,
                   cluster_) {
      const hit_type & hit = *hitref;
      bool plot = false;
      if ((mode_ & prompt) && hit.is_prompt ()) {
        plot = true;
      }
      if ((mode_ & delayed) && hit.is_delayed ()) {
        plot = true;
      }
      if (plot) {
        plot_clustered_item (out_,
                             hit.get_id (),
                             hit.get_x (),
                             hit.get_y (),
                             hit.get_z (),
                             hit.get_dz (),
                             hit.get_delayed_time ());
      }
    }
    return;
  }

} // end of namespace TrackerPreClustering

#endif // FALAISE_TRACKERPRECLUSTERING_EVENT_DISPLAY_TPP
