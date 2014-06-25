/** \file falaise/snemo/datamodels/tracker_clustering_solution.cc */

// Standard library:
#include <set>

// Ourselves
#include <falaise/snemo/datamodels/tracker_clustering_solution.h>

namespace snemo {

  namespace datamodel {

    bool tracker_clustering_solution::has_solution_id() const
    {
      return _solution_id_ >= 0;
    }

    int tracker_clustering_solution::get_solution_id() const
    {
      return _solution_id_;
    }

    void tracker_clustering_solution::set_solution_id(int32_t solution_id_)
    {
      if (solution_id_ >= 0) {
        _solution_id_ = solution_id_;
      } else {
        invalidate_solution_id();
      }
      return;
    }

    void tracker_clustering_solution::invalidate_solution_id()
    {
      _solution_id_ = -1;
      return;
    }

    datatools::properties & tracker_clustering_solution::grab_auxiliaries()
    {
      return _auxiliaries_;
    }

    const datatools::properties & tracker_clustering_solution::get_auxiliaries() const
    {
      return _auxiliaries_;
    }

    tracker_clustering_solution::hit_collection_type &
    tracker_clustering_solution::grab_unclustered_hits()
    {
      return _unclustered_hits_;
    }

    const tracker_clustering_solution::hit_collection_type &
    tracker_clustering_solution::get_unclustered_hits() const
    {
      return _unclustered_hits_;
    }

    tracker_clustering_solution::cluster_col_type &
    tracker_clustering_solution::grab_clusters()
    {
      return _clusters_;
    }

    const tracker_clustering_solution::cluster_col_type &
    tracker_clustering_solution::get_clusters() const
    {
      return _clusters_;
    }

    // tracker_clustering_solution::cluster_col_type &
    // tracker_clustering_solution::grab_delayed_clusters()
    // {
    //   return _delayed_clusters_;
    // }

    // const tracker_clustering_solution::cluster_col_type &
    // tracker_clustering_solution::get_delayed_clusters() const
    // {
    //   return _delayed_clusters_;
    // }

    void tracker_clustering_solution::reset()
    {
      this->clear();
      return;
    }

    void tracker_clustering_solution::clear()
    {
      reset_hit_belonging();
      _clusters_.clear();
      // _delayed_clusters_.clear();
      _unclustered_hits_.clear();
      invalidate_solution_id();
      _auxiliaries_.clear();
      return;
    }

    tracker_clustering_solution::tracker_clustering_solution()
    {
      _solution_id_ = -1;
      return;
    }

    tracker_clustering_solution::~tracker_clustering_solution()
    {
      return;
    }

    void tracker_clustering_solution::tree_dump(std::ostream & out_,
                                                 const std::string & title_,
                                                 const std::string & indent_,
                                                 bool inherit_) const
    {
      std::string indent;
      if(! indent_.empty()) {
        indent = indent_;
      }
      if( ! title_.empty() ) {
        out_ << indent << title_ << std::endl;
      }

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Solution ID  : " << _solution_id_ << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Cluster(s)   : " <<  get_clusters().size() << std::endl;
      for (int i = 0; i < (int) get_clusters().size(); i++) {
        const tracker_cluster & tc = get_clusters()[i].get();
        std::ostringstream indent2;
        out_ << indent << datatools::i_tree_dumpable::skip_tag;
        indent2 << indent << datatools::i_tree_dumpable::skip_tag;
        if (i == (int) get_clusters().size() - 1) {
          out_ << datatools::i_tree_dumpable::last_tag;
          indent2 << datatools::i_tree_dumpable::last_skip_tag;
        } else {
          out_ << datatools::i_tree_dumpable::tag;
          indent2 << datatools::i_tree_dumpable::skip_tag;
        }
        out_ << "Cluster #" << i << " : " << std::endl;
        tc.tree_dump(out_, "", indent2.str());
      }

      {
        int hit_index = 0;
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Unclustered hit(s) : " << _unclustered_hits_.size() << std::endl;
        for (hit_collection_type::const_iterator i = _unclustered_hits_.begin();
             i != _unclustered_hits_.end();
             i++) {
          out_ << indent << datatools::i_tree_dumpable::skip_tag;
          hit_collection_type::const_iterator j = i;
          j++;
          if (j == _unclustered_hits_.end()) {
            out_ << datatools::i_tree_dumpable::last_tag;
          } else {
            out_ << datatools::i_tree_dumpable::tag;
          }
          out_ << "Hit #" << hit_index << " : Id=" << i->get().get_hit_id()
               << " GID=" << i->get().get_geom_id();
          out_ << std::endl;
          hit_index++;
        }
      }

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Hits belonging : " << _hit_belonging_.size() << std::endl;

      out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "Auxiliaries : ";
      if(_auxiliaries_.size() == 0)  {
        out_ << "<empty>";
      }
      out_ << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::inherit_skip_tag(inherit_) ;
        _auxiliaries_.tree_dump(out_, "", indent_oss.str());
      }

      return;
    }

    bool tracker_clustering_solution::hit_belongs_to_cluster(const calibrated_tracker_hit & hit_,
                                                              const tracker_cluster & cluster_) const
    {
      return hit_belongs_to_cluster(hit_.get_hit_id(), cluster_.get_cluster_id());
    }

    bool tracker_clustering_solution::hit_is_clustered(const calibrated_tracker_hit & hit_) const
    {
      return hit_is_clustered(hit_.get_hit_id());
    }

    bool tracker_clustering_solution::hit_is_clustered(int32_t hit_id_) const
    {
      if(_hit_belonging_.size() > 0) {
        hit_belonging_col_type::const_iterator found = _hit_belonging_.find(hit_id_);
        return found != _hit_belonging_.end() &&  found->second.size() > 0;
      }
      for(hit_collection_type::const_iterator ihit = _unclustered_hits_.begin();
           ihit != _unclustered_hits_.end();
           ihit++) {
        if(! ihit->has_data()) continue;
        if(ihit->get().get_hit_id() == hit_id_) return false;
      }
      return true;
    }

    bool tracker_clustering_solution::hit_belongs_to_several_clusters(const calibrated_tracker_hit & hit_) const
    {
      return hit_belongs_to_several_clusters(hit_.get_hit_id());
    }

    bool tracker_clustering_solution::hit_belongs_to_several_clusters(int32_t hit_id_) const
    {
      // If available, use the' hit_belonging' collection :
      if(_hit_belonging_.size() > 0) {
        hit_belonging_col_type::const_iterator found = _hit_belonging_.find(hit_id_);
        return found != _hit_belonging_.end() &&  found->second.size() > 1;
      }
      // Shortcut :
      if(_clusters_.size() < 2) return false;
      // Traverse all clusters to count belongings for this hit :
      int cluster_counter = 0;
      for(cluster_col_type::const_iterator icluster = _clusters_.begin();
           icluster != _clusters_.end();
           icluster++) {
        const cluster_handle_type & the_cluster_handle = *icluster;
        if(! the_cluster_handle.has_data()) continue;
        const tracker_cluster & the_cluster = the_cluster_handle.get();
        const calibrated_tracker_hit::collection_type & the_hits = the_cluster.get_hits();
        for(calibrated_tracker_hit::collection_type::const_iterator ihit =
               the_hits.begin();
             ihit != the_hits.end();
             ihit++) {
          const calibrated_tracker_hit::handle_type & the_hit_handle = *ihit;
          if(! the_hit_handle.has_data()) continue;
          const calibrated_tracker_hit & the_hit = the_hit_handle.get();
          if( the_hit.get_hit_id() == hit_id_) {
            cluster_counter++;
            break;
          }
        }
      }
      return cluster_counter > 1;
    }

    bool tracker_clustering_solution::hit_belongs_to_cluster(int32_t hit_id_, int32_t cluster_id_) const
    {
      // If available, use the' hit_belonging' collection :
      if(_hit_belonging_.size() > 0) {
        hit_belonging_col_type::const_iterator found_iter = _hit_belonging_.find(hit_id_);
        if(found_iter == _hit_belonging_.end()) {
          return false;
        }
        const cluster_col_type & hosting_clusters = found_iter->second;
        for(cluster_col_type::const_iterator icluster
               = hosting_clusters.begin();
             icluster != hosting_clusters.end();
             icluster++) {
          const cluster_handle_type & the_cluster_handle = *icluster;
          if(! the_cluster_handle.has_data()) {
            continue;
          }
          const tracker_cluster & the_cluster = the_cluster_handle.get();
          if(the_cluster.get_cluster_id() == cluster_id_) {
            return true;
          }
        }
      } else {
        // Shortcut :
        if(_clusters_.size() < 1) return false;
        // Traverse all clusters to detect belongings for this hit :
        for(cluster_col_type::const_iterator icluster = _clusters_.begin();
             icluster != _clusters_.end();
             icluster++) {
          const cluster_handle_type & the_cluster_handle = *icluster;
          if(! the_cluster_handle.has_data()) {
            continue;
          }
          const tracker_cluster & the_cluster = the_cluster_handle.get();
          if(the_cluster.get_cluster_id() != cluster_id_) {
            continue;
          }
          const calibrated_tracker_hit::collection_type & the_hits = the_cluster.get_hits();
          // Traverse all hits in the current cluster :
          for(calibrated_tracker_hit::collection_type::const_iterator ihit =
                 the_hits.begin();
               ihit != the_hits.end();
               ihit++)  {
            const calibrated_tracker_hit::handle_type & the_hit_handle = *ihit;
            if(! the_hit_handle.has_data()) {
              continue;
            }
            const calibrated_tracker_hit & the_hit = the_hit_handle.get();
            if( the_hit.get_hit_id() == hit_id_) {
              return true;
            }
          }
        }
      }
      return false;
    }

    const tracker_clustering_solution::hit_belonging_col_type &
    tracker_clustering_solution::get_hit_belonging() const
    {
      return _hit_belonging_;
    }

    void tracker_clustering_solution::reset_hit_belonging()
    {
      _hit_belonging_.clear();
      return;
    }

    bool tracker_clustering_solution::has_hit_belonging() const
    {
      return _hit_belonging_.size() > 0;
    }

    // static
    void tracker_clustering_solution::compute_hit_belonging_from_solution(const tracker_clustering_solution & tcs_,
                                                                          hit_belonging_col_type &hbc_)
    {
      hbc_.clear();
      for (cluster_col_type::const_iterator icluster
             = tcs_._clusters_.begin();
           icluster != tcs_._clusters_.end();
           icluster++) {
        const cluster_handle_type & the_cluster_handle = *icluster;
        if (! the_cluster_handle.has_data()) continue;
        const tracker_cluster & the_cluster = the_cluster_handle.get();
        const calibrated_tracker_hit::collection_type & the_hits = the_cluster.get_hits();
        for (calibrated_tracker_hit::collection_type::const_iterator ihit =
               the_hits.begin();
             ihit != the_hits.end();
             ihit++) {
          const calibrated_tracker_hit::handle_type & the_hit_handle = *ihit;
          if (! the_hit_handle.has_data()) continue;
          const calibrated_tracker_hit & the_hit = the_hit_handle.get();
          int32_t hit_id = the_hit.get_hit_id();
          if (hbc_.find(hit_id) == hbc_.end()) {
            {
              cluster_col_type dummy_col;
              hbc_[hit_id] = dummy_col;
            }
            hbc_[hit_id].push_back(the_cluster_handle);
          }
        }

      }
      return;
    }

    void tracker_clustering_solution::compute_hit_belonging()
    {
      compute_hit_belonging_from_solution(*this, _hit_belonging_);
      return;
    }

    // static
    int tracker_clustering_solution::copy_one_solution_in_one(const tracker_clustering_solution & source_,
                                                              tracker_clustering_solution & target_)
    {
      // Preallocate the total number of clusters from both solutions:
      target_.grab_clusters().reserve(target_.get_clusters().size()
                                      + source_.get_clusters().size());
      // Preallocate the total number of unclustered hits from both solutions:
      target_.grab_unclustered_hits().reserve(target_.get_unclustered_hits().size()
                                              + source_.get_unclustered_hits().size());
      // Search for the maximum cluster Id from the target:
      int max_cluster_id = -1;
      for (int icluster_target = 0;
           icluster_target  < (int) target_.get_clusters().size();
           icluster_target++) {
        const tracker_cluster & a_cluster = target_.get_clusters().at(icluster_target).get();
        int cluster_id = a_cluster.get_cluster_id();
        if (cluster_id > max_cluster_id) {
          max_cluster_id = cluster_id;
        }
      }

      // Extract clusters from the solution:
      for (int icluster_source = 0; icluster_source < (int) source_.get_clusters().size(); icluster_source++) {
        // Pickup cluster from the solution:
        const tracker_cluster & a_cluster = source_.get_clusters().at(icluster_source).get();
        // Create a new cluster:
        tracker_cluster::handle_type hcl(new tracker_cluster);
        tracker_cluster & cl = hcl.grab();
        // Copy the original cluster into the new one:
        cl = a_cluster;
        // But give it an unique Id:
        cl.set_cluster_id(max_cluster_id + icluster_source + 1); //target_.grab_clusters().size());
        // // Populate it with hits:
        // cl.grab_hits().reserve(a_cluster.get_hits().size());
        // for (int iclustered_hit = 0; iclustered_hit < (int) a_cluster.get_hits().size(); iclustered_hit++) {
        //   cl.grab_hits().push_back(a_cluster.get_hits().at(iclustered_hit));
        //   int hit_id = a_cluster.get_hits().at(iclustered_hit).get().get_hit_id();
        // }
        // Store this cluster in the solution:
        target_.grab_clusters().push_back(hcl);
      }
      // Extract unclustered hits from the solution:
      for (int iunclustered_hit = 0; iunclustered_hit < (int) target_.get_unclustered_hits().size(); iunclustered_hit++) {
        target_.grab_unclustered_hits().push_back(target_.get_unclustered_hits().at(iunclustered_hit));
        //int uhit_id = target_.get_unclustered_hits().at(iunclustered_hit).get().get_hit_id();
      }

      return 0;
    }

    // static
    int tracker_clustering_solution::merge_two_solutions_in_ones(const tracker_clustering_solution & source0_,
                                                                 const tracker_clustering_solution & source1_,
                                                                 tracker_clustering_solution & target_)
    {
      const tracker_clustering_solution & sol0 = source0_;
      const tracker_clustering_solution & sol1 = source1_;
      // Preallocate the total number of clusters from both solutions:
      target_.grab_clusters().reserve(target_.get_clusters().size()
                                      + sol0.get_clusters().size()
                                      + sol1.get_clusters().size());
      // Preallocate the total number of unclustered hits from both solutions:
      target_.grab_unclustered_hits().reserve(target_.get_unclustered_hits().size()
                                              + sol0.get_unclustered_hits().size()
                                              + sol1.get_unclustered_hits().size());
      // Search for the maximum cluster Id from the target:
      int max_cluster_id = -1;
      for (int icluster_target = 0;
           icluster_target  < (int) target_.get_clusters().size();
           icluster_target++) {
        const tracker_cluster & a_cluster = target_.get_clusters().at(icluster_target).get();
        int cluster_id = a_cluster.get_cluster_id();
        if (cluster_id > max_cluster_id) {
          max_cluster_id = cluster_id;
        }
      }
      static const unsigned int NSOURCES = 2;
      std::set<int> check_hits[NSOURCES];
      for (int source = 0; source < (int) NSOURCES; source++) {
        const tracker_clustering_solution * psol = 0;
        if (source == 0) {
          psol = &sol0;
        } else {
          psol = &sol1;
        }
        const tracker_clustering_solution & rsol = *psol;
        // Extract clusters from the solution:
        for (int icluster_source = 0; icluster_source < (int) rsol.get_clusters().size(); icluster_source++) {
          // Pickup a cluster from the solution:
          const tracker_cluster & a_cluster = rsol.get_clusters().at(icluster_source).get();
          // Create a new cluster:
          tracker_cluster::handle_type hcl(new tracker_cluster);
          tracker_cluster & cl = hcl.grab();
          // Copy the original cluster into the new one:
          cl = a_cluster;
          // But give it an unique Id:
          cl.set_cluster_id(max_cluster_id + icluster_source + 1); //target_.grab_clusters().size());
          // Record the hit Ids in the check set for this source:
          for (int iclustered_hit = 0; iclustered_hit < (int) cl.get_hits().size(); iclustered_hit++) {
            int hit_id = cl.get_hits().at(iclustered_hit).get().get_hit_id();
            check_hits[source].insert(hit_id);
          }
          // Store this cluster in the solution:
          target_.grab_clusters().push_back(hcl);
        }
        // Extract unclustered hits from the solution:
        for (int iunclustered_hit = 0; iunclustered_hit < (int) rsol.get_unclustered_hits().size(); iunclustered_hit++) {
          target_.grab_unclustered_hits().push_back(rsol.get_unclustered_hits().at(iunclustered_hit));
          int uhit_id = rsol.get_unclustered_hits().at(iunclustered_hit).get().get_hit_id();
          check_hits[source].insert(uhit_id);
        }
      }

      for (std::set<int>::const_iterator ihit_id = check_hits[0].begin();
           ihit_id != check_hits[0].end();
           ihit_id++) {
        int hit_id = *ihit_id;
        if (check_hits[1].count(hit_id) == 1) {
          DT_THROW_IF(true,
                      std::logic_error,
                      "Tracker hit with Id " << hit_id
                      << " in first solution is already taken into account by the second clustering solution !"
                      << "Both source clustering solutions should be related to independant sets of tracker hits !");
        }
      }
      return 0;
    }

    // serial tag for datatools::serialization::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(tracker_clustering_solution,
                                                      "snemo::datamodel::tracker_clustering_solution")

  } // end of namespace datamodel

} // end of namespace snemo
